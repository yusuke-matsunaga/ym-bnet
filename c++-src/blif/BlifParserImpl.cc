
/// @file BlifParserImpl.cc
/// @brief BlibParserImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BlifParserImpl.h"
#include "BlifIdCell.h"
#include "ym/BlifCover.h"
#include "ym/BlifHandler.h"
#include "ym/BlifParser.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/ClibCellPin.h"
#include "ym/FileIDO.h"
#include "ym/MsgMgr.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BlifParser
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifParser::BlifParser() :
  mImpl(new BlifParserImpl())
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
  mScanner = nullptr;
}

// @brief デストラクタ
BlifParserImpl::~BlifParserImpl()
{
  delete mScanner;
}

// @brief 読み込みを行う．
bool
BlifParserImpl::read(const string& filename,
		     const ClibCellLibrary& cell_library)
{
  // ファイルをオープンする．
  FileIDO ido;
  if ( !ido.open(filename) ) {
    // エラー
    ostringstream buf;
    buf << filename << " : No such file.";
    MsgMgr::put_msg(__FILE__, __LINE__, FileRegion(),
		    MsgType::Failure, "BLIF_PARSER", buf.str());
    return false;
  }

  mCellLibrary = cell_library;

  // 初期化を行う．
  mScanner = new BlifScanner(ido);
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
      mNameArray.clear();
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
      const char* name = mScanner->cur_string();
      BlifIdCell* cell = mIdHash.find(name, true);
      if ( cell->is_defined() ) {
	ostringstream buf;
	buf << name << ": Defined more than once. Previous definition is "
	    << cell->def_loc() << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Error,
			"MLTDEF01", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
      if ( cell->is_output() ) {
	ostringstream buf;
	buf << name << ": Defined as both input and output."
	    << " Previous difinition is " << cell->loc() << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Warning,
			"MLTDEF02", buf.str().c_str());
      }
      cell->set_loc(loc);
      cell->set_defined();
      cell->set_input();
      for ( auto handler: mHandlerList ) {
	if ( !handler->inputs_elem(cell->id(), cell->str()) ) {
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
      const char* name = mScanner->cur_string();
      BlifIdCell* cell = mIdHash.find(name, true);
      if ( cell->is_output() ) {
	ostringstream buf;
	buf << name << ": Defined more than once. Previous definition is "
	    << cell->loc();
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Error,
			"MLTDEF03", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
      if ( cell->is_input() ) {
	ostringstream buf;
	buf << name << ": Defined as both input and output. "
	    << "Previous definition is "
	    << cell->loc() << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Warning,
			"MLTDEF02", buf.str().c_str());
      }
      cell->set_loc(loc);
      cell->set_output();
      mOidArray.push_back(cell->id());
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
      const char* name = mScanner->cur_string();
      BlifIdCell* cell = mIdHash.find(name, true);
      cell->set_loc(loc);
      mNameArray.push_back(cell);
      goto ST_NAMES;
    }
    if ( tk == BlifToken::NL ) {
      int n = mNameArray.size();
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
      if ( mName1.size() != mNameArray.size() - 1 ) {
	MsgMgr::put_msg(__FILE__, __LINE__, loc1,
			MsgType::Error,
			"SYN12",
			"Input pattern does not fit "
			"with the number of fanins.");
	goto ST_ERROR_EXIT;
      }
      int n = mName1.size();
      mCoverPat.reserve(mCoverPat.size() + n);
      for ( int i = 0; i < n; ++ i ) {
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
    int n = mNameArray.size();
    int ni = n - 1;
    BlifIdCell* cell = mNameArray[ni];
    if ( cell->is_defined() ) {
      // 二重定義
      ostringstream buf;
      buf << cell->str() << ": Defined more than once. "
	  << "Previsous Definition is " << cell->def_loc() << ".";
      MsgMgr::put_msg(__FILE__, __LINE__, cell->loc(),
		      MsgType::Error,
		      "MLTDEF01", buf.str());
      goto ST_ERROR_EXIT;
    }
    cell->set_defined();
    int oid = cell->id();
    mIdArray.clear();
    for ( int i = 0; i < ni ; ++ i ) {
      mIdArray.push_back(mNameArray[i]->id());
    }
    string ipat_str = mCoverPat.c_str();
    const BlifCover& cover = mCoverMgr.pat2cover(ni, mNc, ipat_str, mOpatChar);
    int cover_id = cover.id();

    // ハンドラを呼び出す．
    for ( auto handler: mHandlerList ) {
      if ( !handler->names(oid, cell->str(), mIdArray, cover_id) ) {
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
    const char* name = mScanner->cur_string();
    mCell = mCellLibrary.cell(name);
    if ( mCell == nullptr ) {
      ostringstream buf;
      buf << name << ": No such cell.";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error,
		      "NOCELL02", buf.str());
      goto ST_ERROR_EXIT;
    }
    if ( !mCell->is_logic() ) {
      ostringstream buf;
      buf << name << " : Not a logic cell.";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error, "BNetBlifReader", buf.str());
      return false;
    }
    if ( mCell->output_num() != 1 ) {
      ostringstream buf;
      buf << name << " : Not a single output cell.";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error, "BNetBlifReader", buf.str());
      return false;
    }
    if ( mCell->has_tristate(0) ) {
      ostringstream buf;
      buf << name << " : Is a tri-state cell.";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error, "BNetBlifReader", buf.str());
      return false;
    }
    if ( mCell->inout_num() > 0 ) {
      ostringstream buf;
      buf << name << " : Has inout pins.";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error, "BNetBlifReader", buf.str());
      return false;
    }
    mNameArray.clear();
    mNameArray.resize(mCell->pin_num(), nullptr);
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
      const ClibCellPin* pin = mCell->pin(name1);
      if ( pin == nullptr ) {
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
      const char* name2 = mScanner->cur_string();
      BlifIdCell* cell = mIdHash.find(name2, true);
      cell->set_loc(loc2);

      if ( pin->is_output() ) {
	if ( cell->is_defined() ) {
	  // 二重定義
	  ostringstream buf;
	  buf << cell->str() << ": Defined more than once. "
	      << "Previous definition is " << cell->def_loc() << ".";
	  MsgMgr::put_msg(__FILE__, __LINE__, cell->loc(),
			  MsgType::Error,
			  "MLTDEF01", buf.str());
	  goto ST_ERROR_EXIT;
	}
	cell->set_defined();
      }
      if ( mNameArray[pin->pin_id()] != nullptr ) {
	ostringstream buf;
	buf << name2 << ": Appears more than once.";
	MsgMgr::put_msg(__FILE__, __LINE__, loc2,
			MsgType::Error,
			"MLTDEF02", buf.str());
	goto ST_ERROR_EXIT;
      }
      mNameArray[pin->pin_id()] = cell;
      ++ n_token;
      goto ST_GATE1;
    }
    else if ( tk == BlifToken::NL ) {
      if ( n_token == 0 ) {
	error_loc = loc1;
	goto ST_GATE_SYNERROR;
      }
      const ClibCellPin* opin = mCell->output(0);
      BlifIdCell* oid = mNameArray[opin->pin_id()];
      int onode_id = oid->id();
      int ni = mCell->input_num();
      mIdArray.clear();
      for ( int i = 0; i < ni; ++ i ) {
	const ClibCellPin* ipin = mCell->input(i);
	int inode_id = mNameArray[ipin->pin_id()]->id();
	mIdArray.push_back(inode_id);
      }
      for ( auto handler: mHandlerList ) {
	if ( !handler->gate(onode_id, oid->str(), mIdArray, mCell) ) {
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
      const char* name1 = mScanner->cur_string();
      BlifIdCell* cell1 = mIdHash.find(name1, true);
      cell1->set_loc(loc2);

      FileRegion loc3;
      tk = get_token(loc3);
      if ( tk != BlifToken::STRING ) {
	error_loc = loc3;
	goto ST_LATCH_SYNERROR;
      }
      const char* name2 = mScanner->cur_string();
      BlifIdCell* cell2 = mIdHash.find(name2, true);
      cell2->set_loc(loc3);

      if ( cell2->is_defined() ) {
	// 二重定義
	ostringstream buf;
	buf << cell2->str() << ": Defined more than once. "
	    << "Previsous Definition is " << cell2->def_loc() << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, cell2->loc(),
			MsgType::Error,
			"MLTDEF01", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
      cell2->set_defined();

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
	if ( !handler->latch(cell2->id(), cell2->str(), cell1->id(),
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
    int n = mIdHash.num();
    for ( int i = 0; i < n; ++ i ) {
      BlifIdCell* cell = mIdHash.cell(i);
      if ( !cell->is_defined() ) {
	ostringstream buf;
	buf << cell->str() << ": Undefined.";
	MsgMgr::put_msg(__FILE__, __LINE__, cell->loc(),
			MsgType::Error,
			"UNDEF01", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
    }

    for ( auto oid: mOidArray ) {
      BlifIdCell* cell = mIdHash.cell(oid);
      for ( auto handler: mHandlerList ) {
	if ( !handler->outputs_elem(cell->id(), cell->str()) ) {
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
  delete mScanner;
  mScanner = nullptr;

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
  delete mScanner;
  mScanner = nullptr;

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

END_NAMESPACE_YM_BNET
