
/// @file BlifParserImpl.cc
/// @brief BlibParserImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BlifParserImpl.h"
#include "ym/BlifCover.h"
#include "ym/BlifHandler.h"
#include "ym/BlifParser.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/ClibCellPin.h"
#include "ym/FileIDO.h"
#include "ym/MsgMgr.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BlifParser
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifParser::BlifParser() :
  mImpl{new BlifParserImpl()}
{
}

// @brief デストラクタ
BlifParser::~BlifParser()
{
}

// @brief 読み込みを行う．
// @param[in] filename ファイル名
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
bool
BlifParser::read(const string& filename)
{
  bool stat = mImpl->read(filename, ClibCellLibrary());
  return stat;
}

// @brief 読み込みを行う(セルライブラリ付き)．
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
bool
BlifParser::read(const string& filename,
		 const ClibCellLibrary& cell_library)
{
  bool stat = mImpl->read(filename, cell_library);
  return stat;
}


BEGIN_NONAMESPACE

// 文字列を 0/1 に変換する
// それ以外の文字なら -1 を返す．
int
str_to_01(const StrBuff& str)
{
  const char* strptr = str.c_str();
  if ( strptr[1] != '\0' ) {
    return -1;
  }
  if ( strptr[0] == '0' ) {
    return 0;
  }
  if ( strptr[0] == '1' ) {
    return 1;
  }
  return -1;
}

// カバーのシグネチャ文字列を作る．
string
make_signature(int ni,
	       const StrBuff& ipat_str,
	       char opat_char)
{
  ostringstream buf;
  buf << ni << ":"
      << opat_char << ":"
      << ipat_str;
  return buf.str();
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス BlifParserImpl
//////////////////////////////////////////////////////////////////////


// @brief コンストラクタ
BlifParserImpl::BlifParserImpl()
{
}

// @brief デストラクタ
BlifParserImpl::~BlifParserImpl()
{
}

// @brief 読み込みを行う．
bool
BlifParserImpl::read(const string& filename,
		     const ClibCellLibrary& cell_library)
{
  // ファイルをオープンする．
  ifstream fin{filename};
  if ( !fin ) {
    // エラー
    ostringstream buf;
    buf << filename << " : No such file.";
    MsgMgr::put_msg(__FILE__, __LINE__, FileRegion(),
		    MsgType::Failure, "BLIF_PARSER", buf.str());
    return false;
  }

  mCellLibrary = cell_library;

  // 初期化を行う．
  mScanner = unique_ptr<BlifScanner>{new BlifScanner(fin, {filename})};
  mIdHash.clear();
  mUngetToken = BlifToken::_EOF;

  mOidArray.clear();

  // 一つの .inputs/.outputs 文中のトークンの数
  int n_token = 0;

  FileRegion error_loc;

  bool stat = true;
  for ( auto handler: mHandlerList ) {
    if ( !handler->init() ) {
      stat = false;
    }
  }
  if ( !stat ) {
    goto ST_ERROR_EXIT;
  }

  if ( mCellLibrary.cell_num() > 0 ) {
    // セルライブラリの設定
    for ( auto handler: mHandlerList ) {
      handler->set_cell_library(mCellLibrary);
    }
  }

  // ハードコーディングした状態遷移

 ST_INIT:
  // 読込開始
  {
    BlifToken tk= get_token(mLoc1);
    if ( tk == BlifToken::NL ) {
      // 読み飛ばす
      goto ST_INIT;
    }
    if ( tk == BlifToken::MODEL ) {
      // .model 文の開始
      goto ST_MODEL;
    }
    // それ以外はエラー
    MsgMgr::put_msg(__FILE__, __LINE__,
		    mLoc1,
		    MsgType::Error,
		    "SYN01",
		    "No '.model' statement.");
    goto ST_ERROR_EXIT;
  }

 ST_MODEL:
  // .model 文の処理
  {
    FileRegion loc;
    BlifToken tk= get_token(loc);
    if ( tk != BlifToken::STRING ) {
      MsgMgr::put_msg(__FILE__, __LINE__,
		      loc,
		      MsgType::Error,
		      "SYN02",
		      "String expected after '.model'.");
      goto ST_ERROR_EXIT;
    }
    const char* name = mScanner->cur_string();
    for ( auto handler: mHandlerList ) {
      if ( !handler->model(mLoc1, loc, name) ) {
	stat = false;
      }
    }
    if ( !stat ) {
      goto ST_ERROR_EXIT;
    }

    if ( get_token(loc) != BlifToken::NL ) {
      MsgMgr::put_msg(__FILE__, __LINE__,
		      loc,
		      MsgType::Error,
		      "SYN03",
		      "Newline expected.");
      goto ST_ERROR_EXIT;
    }

    goto ST_NEUTRAL;
  }

 ST_NEUTRAL:
  // 本体の処理
  {
    BlifToken tk= get_token(mLoc1);
    switch (tk) {
    case BlifToken::NL:
      goto ST_NEUTRAL;

    case BlifToken::_EOF:
      goto ST_AFTER_EOF;

    case BlifToken::MODEL:
      MsgMgr::put_msg(__FILE__, __LINE__,
		      mLoc1,
		      MsgType::Error,
		      "SYN04",
		      "Multiple '.model' statements.");
      goto ST_ERROR_EXIT;

    case BlifToken::INPUTS:
      n_token = 0;
      goto ST_INPUTS;

    case BlifToken::OUTPUTS:
      n_token = 0;
      goto ST_OUTPUTS;

    case BlifToken::NAMES:
      mCurIdArray.clear();
      mNc = 0;
      mCoverPat.clear();
      mOpatChar = '-';
      goto ST_NAMES;

    case BlifToken::GATE:
      if ( mCellLibrary.cell_num() == 0 ) {
	MsgMgr::put_msg(__FILE__, __LINE__,
			mLoc1,
			MsgType::Error,
			"NOCELL01",
			"No cell-library is specified.");
	goto ST_ERROR_EXIT;
      }
      goto ST_GATE;

    case BlifToken::LATCH:
      goto ST_LATCH;

    case BlifToken::END:
      goto ST_AFTER_END;

    case BlifToken::EXDC:
      goto ST_EXDC;

    case BlifToken::WIRE_LOAD_SLOPE:
      goto ST_DUMMY_READ1;

    case BlifToken::WIRE:
      goto ST_DUMMY_READ1;

    case BlifToken::INPUT_ARRIVAL:
      goto ST_DUMMY_READ1;

    case BlifToken::DEFAULT_INPUT_ARRIVAL:
      goto ST_DUMMY_READ1;

    case BlifToken::OUTPUT_REQUIRED:
      goto ST_DUMMY_READ1;

    case BlifToken::DEFAULT_OUTPUT_REQUIRED:
      goto ST_DUMMY_READ1;

    case BlifToken::INPUT_DRIVE:
      goto ST_DUMMY_READ1;

    case BlifToken::DEFAULT_INPUT_DRIVE:
      goto ST_DUMMY_READ1;

    case BlifToken::OUTPUT_LOAD:
      goto ST_DUMMY_READ1;

    case BlifToken::DEFAULT_OUTPUT_LOAD:
      goto ST_DUMMY_READ1;

    default:
      break;
    }
    error_loc = mLoc1;
    goto ST_SYNTAX_ERROR;
  }

 ST_INPUTS:
  {
    FileRegion loc;
    BlifToken tk= get_token(loc);
    if ( tk == BlifToken::STRING ) {
      auto name{mScanner->cur_string()};
      auto id_cell = find_id(name);
      if ( id_cell->is_defined() ) {
	ostringstream buf;
	buf << name << ": Defined more than once. Previous definition is "
	    << id_cell->loc() << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Error,
			"MLTDEF01", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
      if ( id_cell->is_output() ) {
	ostringstream buf;
	buf << name << ": Defined as both input and output."
	    << " Previous difinition is " << id_cell->loc() << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Warning,
			"MLTDEF02", buf.str().c_str());
      }
      id_cell->set_input(loc);
      for ( auto handler: mHandlerList ) {
	if ( !handler->inputs_elem(id_cell->id(), id_cell->name()) ) {
	  stat = false;
	}
      }
      if ( !stat ) {
	goto ST_ERROR_EXIT;
      }
      ++ n_token;
      goto ST_INPUTS;
    }
    if ( tk == BlifToken::NL ) {
      if ( n_token == 0 ) {
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Warning,
			"SYN07", "Empty '.inputs' statement. Ignored.");
      }
      goto ST_NEUTRAL;
    }
    error_loc = loc;
    goto ST_SYNTAX_ERROR;
  }

 ST_OUTPUTS:
  {
    FileRegion loc;
    BlifToken tk= get_token(loc);
    if ( tk == BlifToken::STRING ) {
      auto name{mScanner->cur_string()};
      auto id_cell = find_id(name);
      if ( id_cell->is_output() ) {
	ostringstream buf;
	buf << name << ": Defined more than once. Previous definition is "
	    << id_cell->loc();
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Error,
			"MLTDEF03", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
      if ( id_cell->is_input() ) {
	ostringstream buf;
	buf << name << ": Defined as both input and output. "
	    << "Previous definition is "
	    << id_cell->loc() << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Warning,
			"MLTDEF02", buf.str().c_str());
      }
      id_cell->set_output();
      mOidArray.push_back(id_cell->id());
      if ( !stat ) {
	goto ST_ERROR_EXIT;
      }
      ++ n_token;
      goto ST_OUTPUTS;
    }
    if ( tk == BlifToken::NL ) {
      if ( n_token == 0 ) {
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Warning,
			"SYN08", "Empty '.outputs' statement. Ignored.");
      }
      goto ST_NEUTRAL;
    }
    error_loc = loc;
    goto ST_SYNTAX_ERROR;
  }

 ST_NAMES:
  { // str* nl
    FileRegion loc;
    BlifToken tk= get_token(loc);
    if ( tk == BlifToken::STRING ) {
      auto name = mScanner->cur_string();
      auto id_cell = find_id(name);
      mCurIdArray.push_back({id_cell, loc});
      goto ST_NAMES;
    }
    if ( tk == BlifToken::NL ) {
      int n = mCurIdArray.size();
      if ( n == 0 ) {
	// 名前が1つもない場合
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Error,
			"SYN09",
			"Empty '.names' statement.");
	goto ST_ERROR_EXIT;
      }
      else if ( n == 1 ) {
	// 名前が1つしかない場合
	goto ST_NAMES0;
      }
      else {
	// 通常の場合
	goto ST_NAMES1;
      }
    }
    error_loc = loc;
    goto ST_SYNTAX_ERROR;
  }

 ST_NAMES0:
  { // str nl
    FileRegion loc1;
    BlifToken tk= get_token(loc1);
    if ( tk == BlifToken::STRING ) {
      mName1 = mScanner->cur_string();
      char ochar = mName1[0];
      switch ( ochar ) {
      case '0':	break;
      case '1': break;
      default:
	MsgMgr::put_msg(__FILE__, __LINE__, loc1,
			MsgType::Error,
			"SYN15",
			"Illegal character in output cube.");
	goto ST_ERROR_EXIT;
      }
      if ( mOpatChar == '-' ) {
	mOpatChar = ochar;
      }
      else if ( mOpatChar != ochar ) {
	MsgMgr::put_msg(__FILE__, __LINE__, loc1,
			MsgType::Error,
			"SYN10",
			"Outpat pattern mismatch.");
	goto ST_ERROR_EXIT;
      }
      if ( get_token(loc1) == BlifToken::NL ) {
	++ mNc;
	goto ST_NAMES0;
      }
      MsgMgr::put_msg(__FILE__, __LINE__, loc1,
		      MsgType::Error,
		      "SYN14",
		      "Newline is expected.");
      goto ST_ERROR_EXIT;
    }
    else if ( tk == BlifToken::NL ) {
      // 空行はスキップ
      goto ST_NAMES0;
    }
    else {
      unget_token(tk, loc1);
      goto ST_NAMES_END;
    }
  }

 ST_NAMES1:
  { // str str nl
    FileRegion loc1;
    BlifToken tk= get_token(loc1);
    if ( tk == BlifToken::STRING ) {
      mName1 = mScanner->cur_string();
      if ( mName1.size() != mCurIdArray.size() - 1 ) {
	MsgMgr::put_msg(__FILE__, __LINE__, loc1,
			MsgType::Error,
			"SYN12",
			"Input pattern does not fit "
			"with the number of fanins.");
	goto ST_ERROR_EXIT;
      }
      int n = mName1.size();
      mCoverPat.reserve(mCoverPat.size() + n);
      for ( int i: Range(n) ) {
	char c = mName1[i];
	if ( c == '1' ) {
	  mCoverPat.put_char('1');
	}
	else if ( c == '0' ) {
	  mCoverPat.put_char('0');
	}
	else if ( c == '-' ) {
	  mCoverPat.put_char('-');
	}
	else {
	  MsgMgr::put_msg(__FILE__, __LINE__, loc1,
			  MsgType::Error,
			  "SYN11",
			  "Illegal character in input cube.");
	  goto ST_ERROR_EXIT;
	}
      }
      FileRegion loc2;
      tk = get_token(loc2);
      if ( tk == BlifToken::STRING ) {
	char ochar = mScanner->cur_string()[0];
	switch ( ochar ) {
	case '0': break;
	case '1': break;
	default:
	  MsgMgr::put_msg(__FILE__, __LINE__, loc1,
			  MsgType::Error,
			  "SYN15",
			  "Illegal character in output cube.");
	  goto ST_ERROR_EXIT;
	}
	if ( mOpatChar == '-' ) {
	  mOpatChar = ochar;
	}
	else if ( mOpatChar != ochar ) {
	  MsgMgr::put_msg(__FILE__, __LINE__, loc2,
			  MsgType::Error, "SYN10",
			  "Outpat pattern mismatch.");
	  goto ST_ERROR_EXIT;
	}
	if ( get_token(loc2) != BlifToken::NL ) {
	  MsgMgr::put_msg(__FILE__, __LINE__, loc2,
			  MsgType::Error, "SYN14",
			  "Newline is expected.");
	  goto ST_ERROR_EXIT;
	}
	++ mNc;
	goto ST_NAMES1;
      }
      MsgMgr::put_msg(__FILE__, __LINE__, loc1,
		      MsgType::Error, "SYN13",
		      "No output cube.");
      goto ST_ERROR_EXIT;
    }
    else if ( tk == BlifToken::NL ) {
      // 空行はスキップ
      goto ST_NAMES1;
    }
    else {
      unget_token(tk, loc1);
      goto ST_NAMES_END;
    }
  }

 ST_NAMES_END:
  {
    int n = mCurIdArray.size();
    int ni = n - 1;
    IdCell* id_cell;
    FileRegion loc;
    tie(id_cell, loc) = mCurIdArray[ni];
    if ( id_cell->is_defined() ) {
      // 二重定義
      ostringstream buf;
      buf << id_cell->name() << ": Defined more than once. "
	  << "Previsous Definition is " << id_cell->loc() << ".";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error,
		      "MLTDEF01", buf.str());
      goto ST_ERROR_EXIT;
    }
    id_cell->set_defined(loc);
    int oid = id_cell->id();
    string ipat_str = mCoverPat.c_str();
    const BlifCover& cover = mCoverMgr.pat2cover(ni, mNc, ipat_str, mOpatChar);
    int cover_id = cover.id();

    // ハンドラを呼び出す．
    vector<int> id_array(ni);
    for ( int i: Range(ni) ) {
      id_array[i] = mCurIdArray[i].first->id();
    }
    for ( auto handler: mHandlerList ) {
      if ( !handler->names(oid, id_cell->name(), id_array, cover_id) ) {
	stat = false;
      }
    }
    if ( !stat ) {
      goto ST_ERROR_EXIT;
    }
    goto ST_NEUTRAL;
  }

 ST_GATE:
  { // str -> ST_GATE1
    FileRegion loc;
    BlifToken tk= get_token(loc);
    if ( tk != BlifToken::STRING ) {
      error_loc = loc;
      goto ST_GATE_SYNERROR;
    }
    auto name = mScanner->cur_string();
    mCellId = mCellLibrary.cell_id(name);
    if ( mCellId == -1 ) {
      ostringstream buf;
      buf << name << ": No such cell.";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error,
		      "NOCELL02", buf.str());
      goto ST_ERROR_EXIT;
    }
    const ClibCell& cell = mCellLibrary.cell(mCellId);
    if ( !cell.is_logic() ) {
      ostringstream buf;
      buf << name << " : Not a logic cell.";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error, "BNetBlifReader", buf.str());
      return false;
    }
    if ( cell.output_num() != 1 ) {
      ostringstream buf;
      buf << name << " : Not a single output cell.";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error, "BNetBlifReader", buf.str());
      return false;
    }
    if ( cell.has_tristate(0) ) {
      ostringstream buf;
      buf << name << " : Is a tri-state cell.";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error, "BNetBlifReader", buf.str());
      return false;
    }
    if ( cell.inout_num() > 0 ) {
      ostringstream buf;
      buf << name << " : Has inout pins.";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error, "BNetBlifReader", buf.str());
      return false;
    }
    mCurIdArray.clear();
    mCurIdArray.resize(cell.pin_num(), {nullptr, FileRegion()});
    n_token = 0;
    goto ST_GATE1;
  }

 ST_GATE1:
  { // (str '=' str)* nl
    FileRegion loc1;
    BlifToken tk= get_token(loc1);
    if ( tk == BlifToken::STRING ) {
      mName1 = mScanner->cur_string();
      const char* name1 = mName1.c_str();
      const ClibCell& cell = mCellLibrary.cell(mCellId);
      int pin_id = cell.pin_id(name1);
      if ( pin_id == -1 ) {
	ostringstream buf;
	buf << name1 << ": No such pin.";
	MsgMgr::put_msg(__FILE__, __LINE__, loc1,
			MsgType::Error,
			"NOPIN01", buf.str());
	goto ST_ERROR_EXIT;
      }
      FileRegion loc2;
      tk = get_token(loc2);
      if ( tk != BlifToken::EQ ) {
	error_loc = loc2;
	goto ST_GATE_SYNERROR;
      }
      tk = get_token(loc2);
      if ( tk != BlifToken::STRING ) {
	error_loc = loc2;
	goto ST_GATE_SYNERROR;
      }
      auto name2{mScanner->cur_string()};
      auto id_cell = find_id(name2);
      const ClibCellPin& pin = cell.pin(pin_id);
      if ( pin.is_output() ) {
	if ( id_cell->is_defined() ) {
	  // 二重定義
	  ostringstream buf;
	  buf << id_cell->name() << ": Defined more than once. "
	      << "Previous definition is " << id_cell->loc() << ".";
	  MsgMgr::put_msg(__FILE__, __LINE__, loc2,
			  MsgType::Error,
			  "MLTDEF01", buf.str());
	  goto ST_ERROR_EXIT;
	}
	id_cell->set_defined(loc2);
      }
      if ( mCurIdArray[pin.pin_id()].first != nullptr ) {
	ostringstream buf;
	buf << name2 << ": Appears more than once.";
	MsgMgr::put_msg(__FILE__, __LINE__, loc2,
			MsgType::Error,
			"MLTDEF02", buf.str());
	goto ST_ERROR_EXIT;
      }
      mCurIdArray[pin.pin_id()] = make_pair(id_cell, loc2);
      ++ n_token;
      goto ST_GATE1;
    }
    else if ( tk == BlifToken::NL ) {
      if ( n_token == 0 ) {
	error_loc = loc1;
	goto ST_GATE_SYNERROR;
      }
      const ClibCell& cell = mCellLibrary.cell(mCellId);
      const ClibCellPin& opin = cell.output(0);
      IdCell* oid_cell;
      tie(oid_cell, ignore) = mCurIdArray[opin.pin_id()];
      int onode_id = oid_cell->id();
      int ni = cell.input_num();
      vector<int> id_array(ni);
      for ( int i: Range(ni) ) {
	auto& ipin = cell.input(i);
	id_array[i] = mCurIdArray[ipin.pin_id()].first->id();
      }
      for ( auto handler: mHandlerList ) {
	if ( !handler->gate(onode_id, oid_cell->name(), id_array, mCellId) ) {
	  stat = false;
	}
      }
      if ( !stat ) {
	goto ST_ERROR_EXIT;
      }
      goto ST_NEUTRAL;
    }
  }

 ST_GATE_SYNERROR:
  MsgMgr::put_msg(__FILE__, __LINE__, error_loc,
		  MsgType::Error,
		  "SYN16", "Syntax error in '.gate' statement.");
  goto ST_ERROR_EXIT;

 ST_LATCH:
  {
    FileRegion loc2;
    BlifToken tk= get_token(loc2);
    if ( tk == BlifToken::STRING ) {
      auto name1{mScanner->cur_string()};
      auto id_cell1 = find_id(name1);

      FileRegion loc3;
      tk = get_token(loc3);
      if ( tk != BlifToken::STRING ) {
	error_loc = loc3;
	goto ST_LATCH_SYNERROR;
      }
      auto name2{mScanner->cur_string()};
      auto id_cell2 = find_id(name2);

      if ( id_cell2->is_defined() ) {
	// 二重定義
	ostringstream buf;
	buf << id_cell2->name() << ": Defined more than once. "
	    << "Previsous Definition is " << id_cell2->loc() << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, loc3,
			MsgType::Error,
			"MLTDEF01", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
      id_cell2->set_defined(loc3);

      FileRegion loc4;
      tk = get_token(loc4);
      char rval = ' ';
      if ( tk == BlifToken::STRING ) {
	rval = mScanner->cur_string()[0];
	if ( rval != '0' && rval != '1' ) {
	  MsgMgr::put_msg(__FILE__, __LINE__, loc4,
			  MsgType::Error,
			  "SYN18",
			  "Illegal character for reset value.");
	  goto ST_ERROR_EXIT;
	}
	tk = get_token(loc4);
      }
      if ( tk != BlifToken::NL ) {
	error_loc = loc4;
	goto ST_LATCH_SYNERROR;
      }

      for ( auto handler: mHandlerList ) {
	if ( !handler->latch(id_cell2->id(), id_cell2->name(), id_cell1->id(),
			     loc4, rval) ) {
	  stat = false;
	}
      }
      if ( !stat ) {
	goto ST_ERROR_EXIT;
      }
      goto ST_NEUTRAL;
    }
    else {
      error_loc = loc2;
      goto ST_LATCH_SYNERROR;
    }
  }

 ST_LATCH_SYNERROR:
  MsgMgr::put_msg(__FILE__, __LINE__, error_loc,
		  MsgType::Error,
		  "SYN17", "Syntax error in '.latch' statement.");
  goto ST_ERROR_EXIT;

 ST_AFTER_END:
  {
    FileRegion loc;
    BlifToken tk= get_token(loc);
    if ( tk == BlifToken::_EOF ) {
      goto ST_NORMAL_EXIT;
    }
    if ( tk != BlifToken::NL ) {
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Warning,
		      "SYN06",
		      "Statement after '.end' is ignored.");
    }
    goto ST_AFTER_END;
  }

 ST_EXDC:
  {
    FileRegion loc;
    BlifToken tk= get_token(loc);
    if ( tk == BlifToken::END ) {
      goto ST_NEUTRAL;
    }
    goto ST_EXDC;
  }

 ST_DUMMY_READ1:
  {
    FileRegion loc;
    BlifToken tk= get_token(loc);
    if ( tk == BlifToken::NL ) {
      goto ST_NEUTRAL;
    }
    goto ST_DUMMY_READ1;
  }

 ST_AFTER_EOF:
  {
    MsgMgr::put_msg(__FILE__, __LINE__, mLoc1,
		    MsgType::Warning,
		    "SYN05",
		    "unexpected EOF. '.end' is assumed.");
    for ( auto handler: mHandlerList ) {
      if ( !handler->end(mLoc1) ) {
	stat = false;
      }
    }
    if ( !stat ) {
      goto ST_ERROR_EXIT;
    }
    goto ST_NORMAL_EXIT;
  }

 ST_NORMAL_EXIT:
  {
    for ( auto& id_cell: mIdArray ) {
      if ( !id_cell.is_defined() ) {
	ostringstream buf;
	buf << id_cell.name() << ": Undefined.";
	MsgMgr::put_msg(__FILE__, __LINE__, id_cell.loc(),
			MsgType::Error,
			"UNDEF01", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
    }

    for ( auto oid: mOidArray ) {
      auto& id_cell = mIdArray[oid];
      for ( auto handler: mHandlerList ) {
	if ( !handler->outputs_elem(id_cell.id(), id_cell.name()) ) {
	  stat = false;
	}
      }
    }
    if ( !stat ) {
      goto ST_ERROR_EXIT;
    }

    for ( auto handler: mHandlerList ) {
      if ( !handler->end(mLoc1) ) {
	stat = false;
      }
    }
    if ( !stat ) {
      goto ST_ERROR_EXIT;
    }
  }

  for ( auto handler: mHandlerList ) {
    handler->normal_exit();
  }
  delete_scanner();

  return true;

 ST_SYNTAX_ERROR:
  MsgMgr::put_msg(__FILE__, __LINE__, error_loc,
		  MsgType::Error,
		  "SYN00",
		  "Syntax error.");
  // わざと次の行につづく
  // goto ST_ERROR_EXIT;

 ST_ERROR_EXIT:
  for ( auto handler: mHandlerList ) {
    handler->error_exit();
  }
  delete_scanner();

  return false;
}

// @brief イベントハンドラの登録
void
BlifParserImpl::add_handler(BlifHandler* handler)
{
  mHandlerList.push_back(handler);
}

// @brief トークンを一つ読み出す．
// @param[out] loc トークンの位置を格納する変数
BlifToken
BlifParserImpl::get_token(FileRegion& loc)
{
  if ( mUngetToken == BlifToken::_EOF ) {
    return mScanner->read_token(loc);
  }
  // トークンバッファに値がある場合にはそれを返す．
  BlifToken tk = mUngetToken;
  mUngetToken = BlifToken::_EOF;
  loc = mUngetTokenLoc;
  return tk;
}

// @brief name に対応する IdCell を取り出す．
// @param[in] name 名前
// @param[in] create true なら未登録の場合に新規に追加する．
// @return 対応する IdCell を返す．
//
// 未登録の場合には新たに作る．
BlifParserImpl::IdCell*
BlifParserImpl::find_id(const char* name)
{
  if ( mIdHash.find(name) == mIdHash.end() ) {
    // 未定義だった．
    // 新しく作る．
    int id = mIdArray.size();
    mIdArray.push_back({id, name});
    auto cell = &mIdArray[id];
    mIdHash.emplace(cell->name(), cell);
    return cell;
  }
  else {
    return mIdHash.at(name);
  }
}

// @brief スキャナーを削除する．
void
BlifParserImpl::delete_scanner()
{
  unique_ptr<BlifScanner> dummy;
  mScanner.swap(dummy);
}

END_NAMESPACE_YM_BNET
