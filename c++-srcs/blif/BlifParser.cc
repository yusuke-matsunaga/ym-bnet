
/// @file BlifParser.cc
/// @brief BlifParser の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "BlifParser.h"
#include "ym/BlifCover.h"
#include "ym/ClibCell.h"
#include "ym/ClibPin.h"
#include "ym/MsgMgr.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
// クラス BlifParser
//////////////////////////////////////////////////////////////////////

// @brief 読み込みを行う．
bool
BlifParser::read(
  const string& filename,
  const ClibCellLibrary& cell_library,
  ModelImpl* model
)
{
  // blif ファイル読み込みの状態遷移
  //
  // init:    .model <モデル名> NL     -> neutral
  //          otherwise                -> error
  //
  // neutral: .inputs <入力名> ... NL  -> neutral
  //          .outputs <出力名> ... NL -> neutral
  //          .names <ネット名> ... NL -> names
  //          .gate <ピン名>=<ネット名> ... NL -> neutral
  //          .latch <ネット名> <ネット名> [<リセット値>] NL -> neutral
  //
  // names:   <入力キューブ> <出力キューブ> -> names
  //          otherwise                     -> neutral

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

  BlifScanner scanner{fin, {filename}};
  CoverMgr cover_mgr{model};

  // 初期化を行う．
  mScanner = &scanner;
  mCoverMgr = &cover_mgr;
  mModel = model;

  mCellLibrary = cell_library;

  // エラー箇所
  FileRegion error_loc;

  // .end 文の位置
  FileRegion end_loc;

  // ハードコーディングした状態遷移

  if ( !read_model() ) {
    goto ST_ERROR_EXIT;
  }

  // 本体の処理
  for ( ; ; ) {
    auto tk = cur_token();
    switch (tk) {
    case BlifToken::NL:
      // スキップ
      next_token();
      break;

    case BlifToken::_EOF:
      error_loc = cur_loc();
      goto ST_AFTER_EOF;

    case BlifToken::MODEL:
      MsgMgr::put_msg(__FILE__, __LINE__,
		      cur_loc(),
		      MsgType::Error,
		      "SYN04",
		      "Multiple '.model' statements.");
      goto ST_ERROR_EXIT;

    case BlifToken::INPUTS:
      if ( !read_inputs() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::OUTPUTS:
      if ( !read_outputs() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::NAMES:
      if ( !read_names() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::GATE:
      if ( !read_gate() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::LATCH:
      if ( !read_latch()) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::END:
      end_loc = cur_loc();
      next_token();
      goto ST_AFTER_END;

    case BlifToken::EXDC:
      if ( !read_exdc() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::WIRE_LOAD_SLOPE:
      if ( !read_dummy1() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::WIRE:
      if ( !read_dummy1() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::INPUT_ARRIVAL:
      if ( !read_dummy1() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::DEFAULT_INPUT_ARRIVAL:
      if ( !read_dummy1() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::OUTPUT_REQUIRED:
      if ( !read_dummy1() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::DEFAULT_OUTPUT_REQUIRED:
      if ( !read_dummy1() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::INPUT_DRIVE:
      if ( !read_dummy1() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::DEFAULT_INPUT_DRIVE:
      if ( !read_dummy1() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::OUTPUT_LOAD:
      if ( !read_dummy1() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    case BlifToken::DEFAULT_OUTPUT_LOAD:
      if ( !read_dummy1() ) {
	goto ST_ERROR_EXIT;
      }
      break;

    default:
      error_loc = cur_loc();
      goto ST_SYNTAX_ERROR;
    }
  }

 ST_AFTER_END:
  {
    auto tk = cur_token();
    if ( tk == BlifToken::_EOF ) {
      goto ST_NORMAL_EXIT;
    }
    else if ( tk != BlifToken::NL ) {
      MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
		      MsgType::Warning,
		      "SYN06",
		      "Statement after '.end' is ignored.");
    }
    next_token();
    goto ST_AFTER_END;
  }

 ST_AFTER_EOF:
  {
    MsgMgr::put_msg(__FILE__, __LINE__, error_loc,
		    MsgType::Warning,
		    "SYN05",
		    "unexpected EOF. '.end' is assumed.");
    goto ST_NORMAL_EXIT;
  }

 ST_NORMAL_EXIT:
  {
    SizeType n = mRefLocArray.size();
    for ( auto id = 0; id < n; ++ id ) {
      if ( !is_defined(id) ) {
	ostringstream buf;
	buf << id2str(id) << ": Undefined.";
	MsgMgr::put_msg(__FILE__, __LINE__, ref_loc(id),
			MsgType::Error,
			"UNDEF01", buf.str().c_str());
	goto ST_ERROR_EXIT;
      }
    }
  }

  // 出力ノードからファンインをたどり
  // post-order で番号をつける．
  // 結果としてノードは入力からのトポロジカル順
  // に整列される．
  for ( auto id: mModel->mOutputList ) {
    order_node(id);
  }

  // ラッチノードのファンインに番号をつける．
  for ( auto id: mModel->mDffList ) {
    auto& node = mModel->mNodeArray[id];
    order_node(node.input());
  }

  return true;

 ST_SYNTAX_ERROR:
  MsgMgr::put_msg(__FILE__, __LINE__, error_loc,
		  MsgType::Error,
		  "SYN00",
		  "Syntax error.");
  // わざと次の行につづく
  // goto ST_ERROR_EXIT;

 ST_ERROR_EXIT:

  return false;
}

// @brief .model 文の読み込みを行う．
bool
BlifParser::read_model()
{
  // .model を探す．
  FileRegion model_loc;
  bool go_on = true;
  while ( go_on ) {
    next_token();
    auto tk = cur_token();
    switch ( tk ) {
    case BlifToken::NL:
      // 読み飛ばす
      break;

    case BlifToken::MODEL:
      // .model 文の開始
      model_loc = cur_loc();
      go_on = false;
      break;

    default:
      // それ以外はエラー
      MsgMgr::put_msg(__FILE__, __LINE__,
		      cur_loc(),
		      MsgType::Error,
		      "SYN01",
		      "No '.model' statement.");
      return false;
    }
  }

  // モデル名の読み込み
  next_token();
  auto tk = cur_token();
  auto name_loc = cur_loc();
  if ( tk != BlifToken::STRING ) {
    MsgMgr::put_msg(__FILE__, __LINE__,
		    name_loc,
		    MsgType::Error,
		    "SYN02",
		    "String expected after '.model'.");
    return false;
  }

  mModel->mName = cur_string();

  // NL を待つ．
  next_token();
  if ( cur_token() != BlifToken::NL ) {
    MsgMgr::put_msg(__FILE__, __LINE__,
		    cur_loc(),
		    MsgType::Error,
		    "SYN03",
		    "Newline expected.");
    return false;
  }

  // 次のトークンを読んでおく．
  next_token();

  return true;
}

// @brief .inputs 文の読み込みを行う．
bool
BlifParser::read_inputs()
{
  SizeType n_token = 0;
  bool ok = true;
  for ( ; ; ) {
    next_token();
    auto tk = cur_token();
    if ( tk == BlifToken::STRING ) {
      auto name = cur_string();
      auto name_loc = cur_loc();
      auto id = find_id(name, name_loc);
      if ( is_defined(id) ) {
	auto loc = def_loc(id);
	ostringstream buf;
	buf << name << ": Defined more than once. Previous definition is at "
	    << loc << ".";
	MsgMgr::put_msg(__FILE__, __LINE__, name_loc,
			MsgType::Error,
			"MLTDEF01", buf.str().c_str());
	ok = false;
      }

      set_defined(id, name_loc);
      mModel->set_input(id);
      mMark.emplace(id);

      ++ n_token;
    }
    else if ( tk == BlifToken::NL ) {
      if ( n_token == 0 ) {
	MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			MsgType::Warning,
			"SYN07", "Empty '.inputs' statement. Ignored.");
      }
      // 次のトークンを読んでおく
      next_token();
      return ok;
    }
    else {
      return false;
    }
  }
}

// @brief .outputs 文の読み込みを行う．
bool
BlifParser::read_outputs()
{
  SizeType n_token = 0;
  bool ok = true;
  for ( ; ; ) {
    next_token();
    auto tk = cur_token();
    if ( tk == BlifToken::STRING ) {
      auto name = cur_string();
      auto name_loc = cur_loc();
      auto id = find_id(name, name_loc);
      mModel->mOutputList.push_back(id);
      ++ n_token;
    }
    else if ( tk == BlifToken::NL ) {
      if ( n_token == 0 ) {
	MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			MsgType::Warning,
			"SYN08", "Empty '.outputs' statement. Ignored.");
      }
      // 次のトークンを読んでおく
      next_token();
      return ok;
    }
    else {
      return false;
    }
  }
}

// @brief .names 文の読み込みを行う．
bool
BlifParser::read_names()
{
  // .names 文に現れる識別子番号のリスト
  vector<SizeType> names_id_list;

  // .names 文の最後の識別子の定義場所
  FileRegion names_loc;

  // .names 文のキューブ数
  SizeType cube_num{0};

  // .names 文のキューブパタンを表す文字列
  string ipat_str;

  // .names 文のキューブパタンの出力部分
  // 複数行ある場合も同一のはずなので１文字で十分
  char opat_char{'-'};

  // str* nl
  for ( ; ; ) {
    next_token();
    auto tk = cur_token();
    if ( tk == BlifToken::STRING ) {
      auto name = cur_string();
      names_loc = cur_loc();
      auto id = find_id(name, names_loc);
      names_id_list.push_back(id);
    }
    else if ( tk == BlifToken::NL ) {
      auto n = names_id_list.size();
      if ( n == 0 ) {
	// 名前が1つもない場合
	MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			MsgType::Error,
			"SYN09",
			"Empty '.names' statement.");
	return false;
      }
      break;
    }
    else {
      MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
		      MsgType::Error,
		      "SYN00",
		      "Syntax error.");
      return false;
    }
  }

  // 入力数
  auto ni = names_id_list.size() - 1;
  if ( ni == 0 ) {
    // 入力のキューブがない場合
    for ( ; ; ) {
      next_token();
      auto tk = cur_token();
      if ( tk == BlifToken::STRING ) {
	auto tmp_str = cur_string();
	char ochar = tmp_str[0];
	switch ( ochar ) {
	case '0':	break;
	case '1': break;
	default:
	  MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			  MsgType::Error,
			  "SYN15",
			  "Illegal character in output cube.");
	  return false;
	}
	if ( opat_char == '-' ) {
	  opat_char = ochar;
	}
	else if ( opat_char != ochar ) {
	  MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			  MsgType::Error,
			  "SYN10",
			  "Output pattern mismatch.");
	  return false;
	}
	next_token();
	if ( cur_token() == BlifToken::NL ) {
	  ++ cube_num;
	}
	else {
	  MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			  MsgType::Error,
			  "SYN14",
			  "Newline is expected.");
	  return false;
	}
      }
      else if ( tk == BlifToken::NL ) {
	// 空行はスキップ
	;
      }
      else {
	// それ以外のトークンの場合には
	// 読み込まずに終わる．
	break;
      }
    }
  }
  else {
    // 入力と出力のキューブを持つ場合．
    for ( ; ; ) {
      next_token();
      auto tk = cur_token();
      if ( tk == BlifToken::STRING ) {
	auto tmp_str = cur_string();
	auto n = tmp_str.size();
	if ( n != names_id_list.size() - 1 ) {
	  MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			  MsgType::Error,
			  "SYN12",
			  "Input pattern does not fit "
			  "with the number of fanins.");
	  return false;
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
	    MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			    MsgType::Error,
			    "SYN11",
			    "Illegal character in input cube.");
	    return false;
	  }
	}

	// 出力のキューブ
	next_token();
	tk = cur_token();
	if ( tk == BlifToken::STRING ) {
	  auto tmp_str = cur_string();
	  char ochar = tmp_str[0];
	  switch ( ochar ) {
	  case '0': break;
	  case '1': break;
	  default:
	    MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			    MsgType::Error,
			    "SYN15",
			    "Illegal character in output cube.");
	    return false;
	  }
	  if ( opat_char == '-' ) {
	    opat_char = ochar;
	  }
	  else if ( opat_char != ochar ) {
	    MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			    MsgType::Error, "SYN10",
			    "Outpat pattern mismatch.");
	    return false;
	  }

	  next_token();
	  if ( cur_token() == BlifToken::NL ) {
	    ++ cube_num;
	  }
	  else {
	    MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			    MsgType::Error, "SYN14",
			    "Newline is expected.");
	    return false;
	  }
	}
	else {
	  MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			  MsgType::Error, "SYN13",
			  "No output cube.");
	  return false;
	}
      }
      else if ( tk == BlifToken::NL ) {
	// 空行はスキップ
	;
      }
      else {
	// それ以外のトークンの場合には
	// 読み込まずに終わる．
	break;
      }
    }
  }

  auto oid = names_id_list[ni];
  names_id_list.pop_back();
  if ( is_defined(oid) ) {
    // 二重定義
    FileRegion loc = def_loc(oid);
    ostringstream buf;
    buf << id2str(oid) << ": Defined more than once. "
	<< "Previsous Definition is at " << loc << ".";
    MsgMgr::put_msg(__FILE__, __LINE__, names_loc,
		    MsgType::Error,
		    "MLTDEF01", buf.str());
    return false;
  }

  // カバーを作る．
  auto cover_id = mCoverMgr->pat2cover(ni, cube_num, ipat_str, opat_char);

  set_defined(oid, names_loc);
  mModel->set_cover(oid, names_id_list, cover_id);

  return true;
}

/// @brief .gate 文の読み込みを行う．
bool
BlifParser::read_gate()
{
  if ( mCellLibrary.cell_num() == 0 ) {
    MsgMgr::put_msg(__FILE__, __LINE__,
		    cur_loc(),
		    MsgType::Error,
		    "NOCELL01",
		    "No cell-library is specified.");
    return false;
  }

  // 最初のトークンは文字列
  next_token();
  auto tk = cur_token();
  if ( tk != BlifToken::STRING ) {
    MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
		    MsgType::Error,
		    "SYN??",
		    "Syntax error: string is expected after '.gate'");
    return false;
  }

  auto name = cur_string();
  auto name_loc = cur_loc();
  auto gate_id = mCellLibrary.cell_id(name);
  if ( gate_id == CLIB_NULLID ) {
    ostringstream buf;
    buf << name << ": No such cell.";
    MsgMgr::put_msg(__FILE__, __LINE__, name_loc,
		    MsgType::Error,
		    "NOCELL02", buf.str());
    return false;
  }

  auto& cell = mCellLibrary.cell(gate_id);
  if ( !cell.is_logic() ) {
    ostringstream buf;
    buf << name << " : Not a logic cell.";
    MsgMgr::put_msg(__FILE__, __LINE__, name_loc,
		    MsgType::Error,
		    "BNetBlifReader",// TODO 直す
		    buf.str());
    return false;
  }
  if ( cell.output_num() != 1 ) {
    ostringstream buf;
    buf << name << " : Not a single output cell.";
    MsgMgr::put_msg(__FILE__, __LINE__, name_loc,
		    MsgType::Error,
		    "BNetBlifReader",
		    buf.str());
    return false;
  }
  if ( cell.has_tristate(0) ) {
    ostringstream buf;
    buf << name << " : Is a tri-state cell.";
    MsgMgr::put_msg(__FILE__, __LINE__, name_loc,
		    MsgType::Error,
		    "BNetBlifReader",
		    buf.str());
    return false;
  }
  if ( cell.inout_num() > 0 ) {
    ostringstream buf;
    buf << name << " : Has inout pins.";
    MsgMgr::put_msg(__FILE__, __LINE__, name_loc,
		    MsgType::Error,
		    "BNetBlifReader",
		    buf.str());
    return false;
  }

  // 出力ピンに対応する識別子番号
  SizeType oid;
  // 出力ピンの位置
  FileRegion oloc;
  // 入力ピンに対応する識別子番号
  auto ni = cell.input_num();
  vector<SizeType> id_array(ni, -1);

  SizeType n_pins = 0;

  // (str '=' str)* nl
  for ( ; ; ) {
    next_token();
    auto tk = cur_token();
    if ( tk == BlifToken::STRING ) {
      auto pin_name = cur_string();
      auto pin_id = cell.pin_id(pin_name);
      if ( pin_id == CLIB_NULLID ) {
	ostringstream buf;
	buf << pin_name << ": No such pin.";
	MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			MsgType::Error,
			"NOPIN01", buf.str());
	return false;
      }

      next_token();
      tk = cur_token();
      if ( tk != BlifToken::EQ ) {
	MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			MsgType::Error,
			"NOPIN01",
			"Syntax error: '=' is expected.");
	return false;
      }

      next_token();
      tk = cur_token();
      if ( tk != BlifToken::STRING ) {
	MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			MsgType::Error,
			"NOPIN01",
			"Syntax error: string is expected.");
	return false;
      }

      auto name2 = cur_string();
      auto name2_loc = cur_loc();
      auto id2 = find_id(name2, name2_loc);
      const ClibPin& pin = cell.pin(pin_id);
      if ( pin.is_output() ) {
	oid = id2;
	oloc = name2_loc;
	if ( is_defined(id2) ) {
	  // 二重定義
	  auto loc = def_loc(id2);
	  ostringstream buf;
	  buf << name2 << ": Defined more than once. "
	      << "Previous definition is at " << loc << ".";
	  MsgMgr::put_msg(__FILE__, __LINE__, name2_loc,
			  MsgType::Error,
			  "MLTDEF01", buf.str());
	  return false;
	}
      }
      else {
	if ( id_array[pin.input_id()] != -1 ) {
	  ostringstream buf;
	  buf << name2 << ": Appears more than once.";
	  MsgMgr::put_msg(__FILE__, __LINE__, name2_loc,
			  MsgType::Error,
			  "MLTDEF02", buf.str());
	  return false;
	}
	id_array[pin.input_id()] = id2;
      }
      ++ n_pins;
    }
    else if ( tk == BlifToken::NL ) {
      if ( n_pins == 0 ) {
	MsgMgr::put_msg(__FILE__, __LINE__, cur_loc(),
			MsgType::Error,
			"GATEXX",
			"Syntax error: pin assiments expected.");
	return false;
      }

      set_defined(oid, oloc);
      mModel->set_cell(oid, id_array, gate_id);

      // 次のトークンを読み込んでおく
      next_token();
      return true;
    }
  }
  return false;
}

// @brief .latch 文の読み込みを行う．
bool
BlifParser::read_latch()
{
  bool ok = true;
  FileRegion error_loc;
  next_token();
  auto tk = cur_token();
  if ( tk == BlifToken::STRING ) {
    auto name1 = cur_string();
    auto name1_loc = cur_loc();
    auto id1 = find_id(name1, name1_loc);

    next_token();
    tk = cur_token();
    if ( tk != BlifToken::STRING ) {
      error_loc = cur_loc();
      goto ST_LATCH_SYNERROR;
    }

    auto name2 = cur_string();
    auto name2_loc = cur_loc();
    auto id2 = find_id(name2, name2_loc);
    if ( is_defined(id2) ) {
      // 二重定義
      FileRegion loc = def_loc(id2);
      ostringstream buf;
      buf << name2 << ": Defined more than once. "
	  << "Previsous Definition is at " << loc << ".";
      MsgMgr::put_msg(__FILE__, __LINE__, name2_loc,
		      MsgType::Error,
		      "MLTDEF01", buf.str().c_str());
      return false;
    }

    next_token();
    tk = cur_token();
    auto loc3 = cur_loc();
    char rval = ' ';
    if ( tk == BlifToken::STRING ) {
      rval = cur_string()[0];
      if ( rval != '0' && rval != '1' ) {
	MsgMgr::put_msg(__FILE__, __LINE__, loc3,
			MsgType::Error,
			"SYN18",
			"Illegal character for reset value.");
	return false;
      }

      next_token();
      tk = cur_token();
      loc3 = cur_loc();
    }
    if ( tk != BlifToken::NL ) {
      error_loc = loc3;
      goto ST_LATCH_SYNERROR;
    }

    set_defined(id2, name2_loc);
    mModel->set_dff(id2, id1, rval);
    mMark.emplace(id2);

    return true;
  }
  else {
    error_loc = cur_loc();
    goto ST_LATCH_SYNERROR;
  }

 ST_LATCH_SYNERROR:
  MsgMgr::put_msg(__FILE__, __LINE__, error_loc,
		  MsgType::Error,
		  "SYN17", "Syntax error in '.latch' statement.");
  return false;
}

// @brief .exdc 文の読み込みを行う．
bool
BlifParser::read_exdc()
{
  // .end までスキップするだけ．
  for ( ; ; ) {
    next_token();
    auto tk = cur_token();
    if ( tk == BlifToken::END ) {
      return true;
    }
    else if ( tk == BlifToken::_EOF ) {
      return false;
    }
  }
}

// @brief ダミーの１行読み込みを行う．
bool
BlifParser::read_dummy1()
{
  for ( ; ; ) {
    next_token();
    auto tk = cur_token();
    if ( tk == BlifToken::NL ) {
      return true;
    }
    else if ( tk == BlifToken::_EOF ) {
      return false;
    }
  }
}

// @brief 次のトークンを読み出す．
void
BlifParser::next_token()
{
  mCurToken = mScanner->read_token(mCurLoc);
}

// @brief 直前に読み出したトークンを返す．
BlifToken
BlifParser::cur_token() const
{
  return mCurToken;
}

// @brief 直前に読み出したトークンが文字列の場合にその文字列を返す．
string
BlifParser::cur_string() const
{
  return mScanner->cur_string();
}

// @brief 直前に読み出したトークンの位置を返す．
FileRegion
BlifParser::cur_loc() const
{
  return mCurLoc;
}

// @brief BlifNode を生成する．
void
BlifParser::order_node(
  SizeType id
)
{
  if ( mMark.count(id) > 0 ) {
    return;
  }
  auto& node = mModel->mNodeArray[id];
  ASSERT_COND( node.is_cover() || node.is_cell() );
  for ( auto iid: node.fanin_list() ) {
    order_node(iid);
  }
  mModel->mLogicList.push_back(id);
  mMark.emplace(id);
}

END_NAMESPACE_YM_BLIF
