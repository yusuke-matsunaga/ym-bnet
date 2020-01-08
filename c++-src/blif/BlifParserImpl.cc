
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
  auto strptr = str.c_str();
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
  InputFileObj in{fin, {filename}};
  mScanner = unique_ptr<BlifScanner>{new BlifScanner(in)};
  mIdHash.clear();
  mUngetToken = BlifToken::_EOF;

  mOidArray.clear();

  // 一つの .inputs/.outputs 文中のトークンの数
  int n_token{0};

  // エラー箇所
  FileRegion error_loc;

  // .model 文の位置
  FileRegion model_loc;

  // .names 文に現れる識別子番号のリスト
  vector<int> names_id_list;

  // .names 文の最後の識別子の定義場所
  FileRegion names_loc;

  // .names 文のキューブ数
  int cube_num{0};

  // .names 文のキューブパタンを表す文字列
  string ipat_str;

  // .names 文のキューブパタンの出力部分
  // 複数行ある場合も同一のはずなので１文字で十分
  char opat_char{'\0'};

  // .gate 文のセル番号
  int gate_id{-1};

  // .gate 文に現れる識別子番号のハッシュ表
  unordered_map<int, int> pin_id_hash;

  // .end 文の位置
  FileRegion end_loc;

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
    FileRegion loc;
    BlifToken tk= get_token(loc);
    if ( tk == BlifToken::NL ) {
      // 読み飛ばす
      goto ST_INIT;
    }
    if ( tk == BlifToken::MODEL ) {
      // .model 文の開始
      model_loc = loc;
      goto ST_MODEL;
    }
    // それ以外はエラー
    MsgMgr::put_msg(__FILE__, __LINE__,
		    loc,
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
    auto name{mScanner->cur_string()};
    for ( auto handler: mHandlerList ) {
      if ( !handler->model(model_loc, loc, name) ) {
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
    FileRegion loc;
    BlifToken tk= get_token(loc);
    switch (tk) {
    case BlifToken::NL:
      goto ST_NEUTRAL;

    case BlifToken::_EOF:
      error_loc = loc;
      goto ST_AFTER_EOF;

    case BlifToken::MODEL:
      MsgMgr::put_msg(__FILE__, __LINE__,
		      loc,
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
      names_id_list.clear();
      cube_num = 0;
      ipat_str = "";
      opat_char = '-'; // 未定義
      goto ST_NAMES;

    case BlifToken::GATE:
      if ( mCellLibrary.cell_num() == 0 ) {
	MsgMgr::put_msg(__FILE__, __LINE__,
			loc,
			MsgType::Error,
			"NOCELL01",
			"No cell-library is specified.");
	goto ST_ERROR_EXIT;
      }
      goto ST_GATE;

    case BlifToken::LATCH:
      goto ST_LATCH;

    case BlifToken::END:
      end_loc = loc;
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
      error_loc = loc;
      goto ST_SYNTAX_ERROR;
    }
    ASSERT_NOT_REACHED;
  }

 ST_INPUTS:
  {
    FileRegion loc;
    BlifToken tk= get_token(loc);
    if ( tk == BlifToken::STRING ) {
      auto name{mScanner->cur_string()};
      int id = find_id(name);
      if ( is_defined(id) ) {
	FileRegion def_loc{id2loc(id)};
	ostringstream buf;
	buf << name << ": Defined more than once. Previous definition is "
	    << def_loc << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Error,
			"MLTDEF01", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
      if ( is_output(id) ) {
	FileRegion def_loc{id2loc(id)};
	ostringstream buf;
	buf << name << ": Defined as both input and output."
	    << " Previous difinition is " << def_loc << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Warning,
			"MLTDEF02", buf.str().c_str());
      }
      set_input(id, loc);
      for ( auto handler: mHandlerList ) {
	if ( !handler->inputs_elem(id, name) ) {
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
      int id = find_id(name);
      if ( is_output(id) ) {
	FileRegion def_loc{id2loc(id)};
	ostringstream buf;
	buf << name << ": Defined more than once. Previous definition is at "
	    << def_loc;
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Error,
			"MLTDEF03", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
      if ( is_input(id) ) {
	FileRegion def_loc{id2loc(id)};
	ostringstream buf;
	buf << name << ": Defined as both input and output. "
	    << "Previous definition is at "
	    << def_loc << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, loc,
			MsgType::Warning,
			"MLTDEF02", buf.str().c_str());
      }
      set_output(id);
      mOidArray.push_back(id);
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
      auto name{mScanner->cur_string()};
      int id = find_id(name);
      names_id_list.push_back(id);
      names_loc = loc;
      goto ST_NAMES;
    }
    if ( tk == BlifToken::NL ) {
      int n = names_id_list.size();
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
      auto tmp_str{mScanner->cur_string()};
      char ochar = tmp_str[0];
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
      if ( opat_char == '-' ) {
	opat_char = ochar;
      }
      else if ( opat_char != ochar ) {
	MsgMgr::put_msg(__FILE__, __LINE__, loc1,
			MsgType::Error,
			"SYN10",
			"Outpat pattern mismatch.");
	goto ST_ERROR_EXIT;
      }
      if ( get_token(loc1) == BlifToken::NL ) {
	++ cube_num;
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
      auto tmp_str{mScanner->cur_string()};
      int n = tmp_str.size();
      if ( n != names_id_list.size() - 1 ) {
	MsgMgr::put_msg(__FILE__, __LINE__, loc1,
			MsgType::Error,
			"SYN12",
			"Input pattern does not fit "
			"with the number of fanins.");
	goto ST_ERROR_EXIT;
      }
      for ( char c: tmp_str ) {
	if ( c == '1' ) {
	  ipat_str += '1';
	}
	else if ( c == '0' ) {
	  ipat_str += '0';
	}
	else if ( c == '-' ) {
	  ipat_str += '-';
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
	auto tmp_str{mScanner->cur_string()};
	char ochar = tmp_str[0];
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
	if ( opat_char == '-' ) {
	  opat_char = ochar;
	}
	else if ( opat_char != ochar ) {
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
	++ cube_num;
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
    int n = names_id_list.size();
    int ni = n - 1;
    int id = names_id_list[ni];
    if ( is_defined(id) ) {
      // 二重定義
      FileRegion def_loc{id2loc(id)};
      ostringstream buf;
      buf << id2str(id) << ": Defined more than once. "
	  << "Previsous Definition is at " << def_loc << ".";
      MsgMgr::put_msg(__FILE__, __LINE__, names_loc,
		      MsgType::Error,
		      "MLTDEF01", buf.str());
      goto ST_ERROR_EXIT;
    }
    set_defined(id, names_loc);
    int oid = id;
    auto& cover = mCoverMgr.pat2cover(ni, cube_num, ipat_str, opat_char);
    int cover_id = cover.id();

    // ハンドラを呼び出す．
    names_id_list.pop_back();
    for ( auto handler: mHandlerList ) {
      if ( !handler->names(oid, id2str(id), names_id_list, cover_id) ) {
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
    auto name{mScanner->cur_string()};
    gate_id = mCellLibrary.cell_id(name);
    if ( gate_id == -1 ) {
      ostringstream buf;
      buf << name << ": No such cell.";
      MsgMgr::put_msg(__FILE__, __LINE__, loc,
		      MsgType::Error,
		      "NOCELL02", buf.str());
      goto ST_ERROR_EXIT;
    }
    auto& cell{mCellLibrary.cell(gate_id)};
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
    pin_id_hash.clear();
    n_token = 0;
    goto ST_GATE1;
  }

 ST_GATE1:
  { // (str '=' str)* nl
    FileRegion loc1;
    BlifToken tk= get_token(loc1);
    if ( tk == BlifToken::STRING ) {
      auto pin_name{mScanner->cur_string()};
      auto& cell{mCellLibrary.cell(gate_id)};
      int pin_id = cell.pin_id(pin_name);
      if ( pin_id == -1 ) {
	ostringstream buf;
	buf << pin_name << ": No such pin.";
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
      int id2 = find_id(name2);
      const ClibCellPin& pin = cell.pin(pin_id);
      if ( pin.is_output() ) {
	if ( is_defined(id2) ) {
	  // 二重定義
	  FileRegion def_loc{id2loc(id2)};
	  ostringstream buf;
	  buf << name2 << ": Defined more than once. "
	      << "Previous definition is at " << def_loc << ".";
	  MsgMgr::put_msg(__FILE__, __LINE__, loc2,
			  MsgType::Error,
			  "MLTDEF01", buf.str());
	  goto ST_ERROR_EXIT;
	}
	set_defined(id2, loc2);
      }
      if ( pin_id_hash.count(pin.pin_id()) > 0 ) {
	ostringstream buf;
	buf << name2 << ": Appears more than once.";
	MsgMgr::put_msg(__FILE__, __LINE__, loc2,
			MsgType::Error,
			"MLTDEF02", buf.str());
	goto ST_ERROR_EXIT;
      }
      pin_id_hash.emplace(pin.pin_id(), id2);
      ++ n_token;
      goto ST_GATE1;
    }
    else if ( tk == BlifToken::NL ) {
      if ( n_token == 0 ) {
	error_loc = loc1;
	goto ST_GATE_SYNERROR;
      }
      auto& cell{mCellLibrary.cell(gate_id)};
#warning "TODO: 要確認：0番目のピンが出力ピンって決め打ちしていいの？"
      auto& opin{cell.output(0)};
      int oid = pin_id_hash.at(opin.pin_id());
      int ni = cell.input_num();
      vector<int> id_array(ni);
      for ( int i: Range(ni) ) {
	auto& ipin = cell.input(i);
	id_array[i] = pin_id_hash.at(ipin.pin_id());
      }
      for ( auto handler: mHandlerList ) {
	if ( !handler->gate(oid, id2str(oid), id_array, gate_id) ) {
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
      int id1 = find_id(name1);

      FileRegion loc3;
      tk = get_token(loc3);
      if ( tk != BlifToken::STRING ) {
	error_loc = loc3;
	goto ST_LATCH_SYNERROR;
      }
      auto name2{mScanner->cur_string()};
      int id2 = find_id(name2);
      if ( is_defined(id2) ) {
	// 二重定義
	FileRegion def_loc{id2loc(id2)};
	ostringstream buf;
	buf << name2 << ": Defined more than once. "
	    << "Previsous Definition is at " << def_loc << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, loc3,
			MsgType::Error,
			"MLTDEF01", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
      set_defined(id2, loc3);

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
	if ( !handler->latch(id2, name2, id1, loc4, rval) ) {
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
    MsgMgr::put_msg(__FILE__, __LINE__, error_loc,
		    MsgType::Warning,
		    "SYN05",
		    "unexpected EOF. '.end' is assumed.");
    for ( auto handler: mHandlerList ) {
      if ( !handler->end(error_loc) ) {
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
    for ( auto& id_cell: mCellArray ) {
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
      auto& id_cell = mCellArray[oid];
      for ( auto handler: mHandlerList ) {
	if ( !handler->outputs_elem(oid, id_cell.name()) ) {
	  stat = false;
	}
      }
    }
    if ( !stat ) {
      goto ST_ERROR_EXIT;
    }

    for ( auto handler: mHandlerList ) {
      if ( !handler->end(end_loc) ) {
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

// @brief name に対応する識別子番号を返す．
// @param[in] name 名前
//
// 未登録の場合には新たに作る．
int
BlifParserImpl::find_id(const string& name)
{
  if ( mIdHash.find(name) == mIdHash.end() ) {
    // 未定義だった．
    // 新しく作る．
    int id = mCellArray.size();
    mCellArray.push_back({name});
    mIdHash.emplace(name, id);
    return id;
  }
  else {
    return mIdHash.at(name);
  }
}

// @brief 対応する識別子がすでに定義済みか調べる．
// @param[in] id 識別子番号
// @retval true 定義済み
// @retval false 未定義
bool
BlifParserImpl::is_defined(int id) const
{
  ASSERT_COND( 0 <= id && id < mCellArray.size() );
  return mCellArray[id].is_defined();
}

// @brief 対応する識別子が入力用か調べる．
// @param[in] id 識別子番号
// @retval true 入力
// @retval false 入力以外
bool
BlifParserImpl::is_input(int id) const
{
  ASSERT_COND( 0 <= id && id < mCellArray.size() );
  return mCellArray[id].is_input();
}

// @brief 対応する識別子が出力用か調べる．
// @param[in] id 識別子番号
// @retval true 出力
// @retval false 出力以外
bool
BlifParserImpl::is_output(int id) const
{
  ASSERT_COND( 0 <= id && id < mCellArray.size() );
  return mCellArray[id].is_output();
}

// @brief 対応する識別子に定義済みの印をつける．
// @param[in] id 識別子番号
// @param[in] loc 定義している場所．
void
BlifParserImpl::set_defined(int id,
			    const FileRegion& loc)
{
  ASSERT_COND( 0 <= id && id < mCellArray.size() );
  mCellArray[id].set_defined(loc);
}

// @brief 対応する識別子に入力用の印を付ける．
// @param[in] id 識別子番号
// @param[in] loc 定義している場所．
void
BlifParserImpl::set_input(int id,
			  const FileRegion& loc)
{
  ASSERT_COND( 0 <= id && id < mCellArray.size() );
  mCellArray[id].set_input(loc);
}

// @brief 対応する識別子に出力用の印を付ける．
// @param[in] id 識別子番号
void
BlifParserImpl::set_output(int id)
{
  ASSERT_COND( 0 <= id && id < mCellArray.size() );
  mCellArray[id].set_output();
}

// @brief スキャナーを削除する．
void
BlifParserImpl::delete_scanner()
{
  unique_ptr<BlifScanner> dummy;
  mScanner.swap(dummy);
}

END_NAMESPACE_YM_BNET
