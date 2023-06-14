﻿
/// @file ParserImpl.cc
/// @brief ParserImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ParserImpl.h"
#include "NvlScanner.h"
#include "ym/NvlModel.h"
#include "ym/NvlHandler.h"
#include "ModelImpl.h"
#include "ym/MsgMgr.h"


BEGIN_NAMESPACE_YM_NVL

//////////////////////////////////////////////////////////////////////
// ParserImpl
//////////////////////////////////////////////////////////////////////

// @brief デストラクタ
ParserImpl::~ParserImpl()
{
}

// 読み込みを行なう．
//
// 文法：
//
// file := 'module' 'top' '(' name-list ')' ';'
//         { input-list | output-list | wire-list | instance-list }
//         'endmodule'
//
// input-list := 'input' name-list ';'
//
// output-list := 'output' name-list ';'
//
// wire-list := 'wire' name-list ';'
//
// instance-list := primitive STRING '(' name-list ')' ';'
//
// primitive := 'buf' | 'not' | 'and' | 'nand' | 'or' | 'nor' | 'xor' | 'xnor'
//
// name-list := name | name ',' name-list
//
// name := STRING | '1'b1' | '1'b0'
//
bool
ParserImpl::read(
  const string& filename,
  ModelImpl* model
)
{
  // ファイルをオープンする．
  ifstream fin{filename};
  if ( !fin ) {
    // エラー
    ostringstream buf;
    buf << filename << " : No such file.";
    MsgMgr::put_msg(__FILE__, __LINE__, FileRegion(),
		    MsgType::Failure, "NVL_PARSER", buf.str());
    return false;
  }

  NvlScanner scanner{fin, {filename}, mHandlerDict};
  mScanner = &scanner;
  mModel = model;

  // パーサー本体
  bool go_on = true;
  bool has_error = false;

  // 開始
  // 'module' name '(' までを読み込む．
  if ( !expect_token(NvlToken::MODULE) ) {
    goto error;
  }

  string top_name;
  if ( !expect_name(top_name) ) {
    goto error;
  }

  if ( !expect_token(NvlToken::LP) ) {
    goto error;
  }

  // ポート名をリストを読み込む．
  vector<string> name_list;
  if ( !read_name_list(NvlToken::RP, name_list) ) {
    goto error;
  }
  // name_list を port 名のリストにする．

  // ';' を読み込む．
  if ( !expect_token(NvlToken::SEMI) ) {
    goto error;
  }

  // input name-list ';'
  // output name-list ';'
  // wire name-list ';'
  // primitive name '(' name-list ')' ';'
  while ( go_on ) {
    SizeType name_id;
    auto token = read_token(name_id);
    auto first_loc = token.loc();
    switch ( token.type() ) {
    case NvlToken::INPUT:
      if ( !read_name_list(NvlToken::SEMI, name_list) ) {
	goto error;
      }
      // name_list を input に追加する．
      break;

    case NvlToken::OUTPUT:
      if ( !read_name_list(NvlToken::SEMI, name_list) ) {
	goto error;
      }
      // name_list を output に追加する．
      break;

    case NvlToken::WIRE:
      if ( !read_name_list(NvlToken::SEMI, name_list) ) {
	goto error;
      }
      // name_list を wire に追加する．
      break;

    case NvlToken::BUF:
    case NvlToken::NOT:
    case NvlToken::AND:
    case NvlToken::NAND:
    case NvlToken::OR:
    case NvlToken::NOR:
    case NvlToken::XOR:
    case NvlToken::XNOR:
      if ( !expect_name(name_id) ) {
	goto error;
      }
      if ( !read_name_list(NvlToken::RP, name_list) ) {
	goto error;
      }
      if ( !expect_token(NvlToken::SEMI) ) {
	goto error;
      }
      // gate instance を追加する．
      break;

    case NvlToken::ENDMODULE:
      go_on = false;
      break;

    case NvlToken::_EOF:
      goto error;

    default:
      goto error;
    }
    continue;

  error:
    has_error = true;
    // ';' まで読み進める．
    for ( ; ; ) {
      auto token = mScanner->read_token();
      if ( token.type() == NvlToken::SEMI ) {
	break;
      }
    }
  }

  {
    SizeType n = mRefLocArray.size();
    for ( auto id = 0; id < n; ++ id ) {
      if ( !is_defined(id) ) {
	ostringstream buf;
	buf << id2str(id) << ": Undefined.";
	MsgMgr::put_msg(__FILE__, __LINE__, ref_loc(id),
			MsgType::Error,
			"UNDEF01", buf.str().c_str());
	return false;
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
    order_node(mModel->node_input(id));
  }

  return !has_error;
}

// @brief INPUT 文を読み込む．
bool
ParserImpl::read_input(
  const FileRegion& first_loc
)
{
  SizeType name_id;
  FileRegion last_loc;
  if ( !parse_name(name_id, last_loc) ) {
    return false;
  }
  FileRegion loc{first_loc, last_loc};
  auto name = id2str(name_id);
  // 二重定義のチェック
  if ( is_defined(name_id) ) {
    auto loc2 = def_loc(name_id);
    ostringstream buf;
    buf << name << ": Defined more than once. Previous definition is at "
	<< loc2;
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Error,
		    "ER_MLTDEF01",
		    buf.str());
    return false;
  }

  set_defined(name_id, loc);
  mModel->set_input(name_id);
  mMark.emplace(name_id);

  return true;
}

// @brief OUTPUT 文を読み込む．
bool
ParserImpl::read_output(
  const FileRegion& first_loc
)
{
  SizeType name_id;
  FileRegion last_loc;
  if ( !parse_name(name_id, last_loc) ) {
    return false;
  }
  FileRegion loc{first_loc, last_loc};
  mModel->mOutputList.push_back(name_id);

  return true;
}

// @brief ゲート/DFF 文を読み込む．
bool
ParserImpl::read_gate(
  const FileRegion& first_loc,
  SizeType name_id
)
{
  bool ok;
  tie(ok, ignore, ignore) = expect(NvlToken::EQ);
  if ( !ok ) {
    return false;
  }

  // 二重定義のチェック
  if ( is_defined(name_id) ) {
    auto oname = id2str(name_id);
    auto loc2 = def_loc(name_id);
    ostringstream buf;
    buf << oname << ": Defined more than once. "
	<< "Previsous Definition is at " << loc2;
    MsgMgr::put_msg(__FILE__, __LINE__, first_loc,
		    MsgType::Error,
		    "ER_MLTDEF01",
		    buf.str());
    return false;
  }

  auto gate_token = mScanner->read_token();
  if ( gate_token.type() == NvlToken::GATE ) {
    vector<SizeType> iname_id_list;
    FileRegion last_loc;
    if ( !parse_name_list(iname_id_list, last_loc) ) {
      return false;
    }
    FileRegion loc{first_loc, last_loc};
    set_gate(name_id, loc, gate_token.gate_type(), iname_id_list);
    return true;
  }
  if ( gate_token.type() == NvlToken::DFF ) {
    SizeType iname_id;
    FileRegion last_loc;
    if ( !parse_name(iname_id, last_loc) ) {
      return false;
    }
    FileRegion loc{first_loc, last_loc};
    set_defined(name_id, loc);
    mModel->set_dff(name_id, iname_id);
    mMark.emplace(name_id);
    return true;
  }
  if ( gate_token.type() == NvlToken::EXGATE ) {
    auto handler = get_handler(gate_token.ex_id());
    return handler->_read(this, first_loc, name_id);
    return false;
  }
  return false;
}

// @brief '(' ')' で囲まれた名前を読み込む．
bool
ParserImpl::parse_name(
  SizeType& name_id,
  FileRegion& last_loc
)
{
  bool ok;
  tie(ok, ignore, ignore) = expect(NvlToken::LPAR);
  if ( !ok ) {
    return false;
  }

  tie(ok, name_id, ignore) = expect(NvlToken::NAME);
  if ( !ok ) {
    return false;
  }

  tie(ok, ignore, last_loc) = expect(NvlToken::RPAR);
  if ( !ok ) {
    return false;
  }

  return true;
}

// @brief '(' ')' で囲まれた名前のリストを読み込む．
bool
ParserImpl::parse_name_list(
  vector<SizeType>& name_id_list,
  FileRegion& last_loc
)
{
  name_id_list.clear();

  bool ok;
  tie(ok, ignore, ignore) = expect(NvlToken::LPAR);
  if ( !ok ) {
    // '(' を期待していたシンタックスエラー
    return false;
  }

  for ( ; ; ) {
    SizeType name_id;
    tie(ok, name_id, ignore) = expect(NvlToken::NAME);
    if ( !ok ) {
      // NAME を期待したシンタックスエラー
      return false;
    }
    name_id_list.push_back(name_id);

    auto token = mScanner->read_token();
    if ( token.type() == NvlToken::RPAR ) {
      last_loc = token.loc();
      break;
    }
    if ( token.type() != NvlToken::COMMA ) {
      // ')' か ',' を期待していたシンタックスエラー
      ostringstream buf;
      buf << "Syntax error: ')' or ',' are expected.";
      MsgMgr::put_msg(__FILE__, __LINE__, token.loc(),
		      MsgType::Error,
		      "ER_SYNTAX03",
		      buf.str());
      return false;
    }
  }
  return true;
}

BEGIN_NONAMESPACE

const char*
token_str(
  NvlToken::Type token
)
{
  switch (token) {
  case NvlToken::LPAR:   return "(";
  case NvlToken::RPAR:   return ")";
  case NvlToken::EQ:     return "=";
  case NvlToken::COMMA:  return ",";
  case NvlToken::SEMI:   return ";";
  case NvlToken::INPUT:  return "input";
  case NvlToken::OUTPUT: return "output";
  case NvlToken::WIRE:   return "wire";
  case NvlToken::BUF:    return "buf";
  case NvlToken::NOT:    return "not";
  case NvlToken::AND:    return "and";
  case NvlToken::NAND:   return "nand";
  case NvlToken::OR:     return "or";
  case NvlToken::NOR:    return "nor";
  case NvlToken::XOR:    return "xor";
  case NvlToken::XNOR:   return "xnor";
  case NvlToken::STR:    return "__str__";
  case NvlToken::_EOF:   return "__eof__";
  case NvlToken::ERROR:  return "__error__";
  }
  ASSERT_NOT_REACHED;
  return "";
}

END_NONAMESPACE

// @brief 次のトークンが期待されている型か調べる．
tuple<bool, SizeType, FileRegion>
ParserImpl::expect(
  NvlToken::Type exp_type
)
{
  SizeType name_id;
  auto token = read_token(name_id);
  if ( token.type() != exp_type ) {
    // トークンが期待値と異なっていた
    ostringstream buf;
    buf << "Syntax error: '" << token_str(exp_type) << "' is expected.";
    MsgMgr::put_msg(__FILE__, __LINE__, token.loc(),
		    MsgType::Error,
		    "ER_SYNTAX01",
		    buf.str());
    return make_tuple(false, 0, token.loc());
  }

  return make_tuple(true, name_id, token.loc());
}

// @brief yylex() 用の処理を行う．
NvlToken
ParserImpl::read_token(
  SizeType& name_id
)
{
  auto token = mScanner->read_token();
  if ( token.type() == NvlToken::NAME ) {
    auto name = token.name();
    name_id = find_id(name, token.loc());
  }
  return token;
}

// @brief トポロジカル順に並べる．
void
ParserImpl::order_node(
  SizeType id
)
{
  if ( mMark.count(id) > 0 ) {
    return;
  }

  auto& node = mModel->mNodeArray[id];
  ASSERT_COND( node.is_gate() || node.is_complex() );
  for ( auto iid: node.fanin_list() ) {
    order_node(iid);
  }
  mMark.emplace(id);
  mModel->mGateList.push_back(id);
}

END_NAMESPACE_YM_NVL