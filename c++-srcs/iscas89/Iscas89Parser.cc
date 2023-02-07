﻿
/// @file Iscas89Parser.cc
/// @brief Iscas89Parser の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2019, 2021, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Iscas89Parser.h"
#include "Iscas89Scanner.h"
#include "ym/Iscas89Model.h"
#include "ModelImpl.h"
#include "ym/MsgMgr.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
// Iscas89Parser
//////////////////////////////////////////////////////////////////////

// 読み込みを行なう．
//
// 文法：
//
// file   = { line } ;
// line   = input | output | buff | not | and | nand | or | nor | xor | xnor | dff ;
// input  = 'INPUT' '(' NAME ')'
// output = 'OUTPUT' '(' NAME ')'
// const0 = NAME '=' 'CONST0' '(' ')'  // ym-bnet オリジナル
// const1 = NAME '=' 'CONST1' '(' ')'  // ym-bnet オリジナル
// buff   = NAME '=' 'BUFF' '(' NAME ')'
// not    = NAME '=' 'NOT' '(' NAME ')'
// and    = NAME '=' 'AND' '(' NAME ',' NAME { ',' NAME } ')'
// nand   = NAME '=' 'NAND' '(' NAME ',' NAME { ',' NAME } ')'
// or     = NAME '=' 'OR' '(' NAME ',' NAME { ',' NAME } ')'
// nor    = NAME '=' 'NOR' '(' NAME ',' NAME { ',' NAME } ')'
// xor    = NAME '=' 'XOR' '(' NAME ',' NAME { ',' NAME } ')'
// xnor   = NAME '=' 'XNOR' '(' NAME ',' NAME { ',' NAME } ')'
// mux    = NAME '=' 'MUX' '(' NAME, NAME, NAME ')' // host2015 オリジナル．3入力決め打ち？
// dff    = NAME '=' 'DFF' '(' NAME ')'
//
bool
Iscas89Parser::read(
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
		    MsgType::Failure, "ISCAS89_PARSER", buf.str());
    return false;
  }

  Iscas89Scanner scanner(fin, {filename});
  mScanner = &scanner;
  mModel = model;

  // パーサー本体
  bool go_on = true;
  bool has_error = false;
  while ( go_on ) {
    Iscas89Token tok;
    SizeType name_id;
    FileRegion first_loc;
    FileRegion last_loc;
    tie(tok, name_id, first_loc) = read_token();
    switch ( tok ) {
    case Iscas89Token::INPUT:
      if ( !parse_name(name_id, last_loc) ) {
	goto error;
      }
      if ( !read_input(FileRegion(first_loc, last_loc), name_id) ) {
	goto error;
      }
      break;

    case Iscas89Token::OUTPUT:
      if ( !parse_name(name_id, last_loc) ) {
	goto error;
      }
      if ( !read_output(FileRegion(first_loc, last_loc), name_id) ) {
	goto error;
      }
      break;

    case Iscas89Token::NAME:
      {
	bool ok;
	tie(ok, ignore, ignore) = expect(Iscas89Token::EQ);
	if ( !ok ) {
	  goto error;
	}

	auto gate_type = parse_gate_type();
	auto type = PrimType::None;
	vector<SizeType> iname_id_list;
	switch ( gate_type ) {
	case Iscas89Token::CONST0: type = PrimType::C0;   break;
	case Iscas89Token::CONST1: type = PrimType::C1;   break;
	case Iscas89Token::BUFF:   type = PrimType::Buff; break;
	case Iscas89Token::NOT:    type = PrimType::Not;  break;
	case Iscas89Token::AND:    type = PrimType::And;  break;
	case Iscas89Token::NAND:   type = PrimType::Nand; break;
	case Iscas89Token::OR:     type = PrimType::Or;   break;
	case Iscas89Token::NOR:    type = PrimType::Nor;  break;
	case Iscas89Token::XOR:    type = PrimType::Xor;  break;
	case Iscas89Token::XNOR:   type = PrimType::Xnor; break;
	case Iscas89Token::DFF:
	  if ( !parse_name_list(iname_id_list, last_loc) ) {
	    goto error;
	  }
	  if ( iname_id_list.size() != 1 ) {
	    // 引数の数が合わない．
	    ostringstream buf;
	    buf << "Syntax error: dff should have exact one input.";
	    MsgMgr::put_msg(__FILE__, __LINE__, last_loc,
			    MsgType::Error,
			    "ER_SYNTAX04",
			    buf.str());
	    goto error;
	  }
	  if ( !read_dff(FileRegion(first_loc, last_loc),
			 name_id, iname_id_list[0]) ) {
	    goto error;
	  }
	  break;
	case Iscas89Token::MUX:
	  if ( !parse_name_list(iname_id_list, last_loc) ) {
	    goto error;
	  }
	  if ( !read_mux(FileRegion(first_loc, last_loc),
			 name_id, iname_id_list) ) {
	    goto error;
	  }
	  break;
	default:
	  goto error;
	}
	if ( type != PrimType::None ) {
	  if ( type != PrimType::C0 && type != PrimType::C1 ) {
	    if ( !parse_name_list(iname_id_list, last_loc) ) {
	      goto error;
	    }
	  }
	  if ( !read_gate(FileRegion(first_loc, last_loc),
			  name_id, type, iname_id_list) ) {
	    goto error;
	  }
	}
      }
      break;

    case Iscas89Token::_EOF:
      go_on = false;
      break;

    default:
      goto error;
    }
    continue;

  error:
    has_error = true;
    // ')' まで読み進める．
    for ( ; ; ) {
      Iscas89Token tok;
      tie(tok, ignore, ignore) = read_token();
      if ( tok == Iscas89Token::RPAR || tok == Iscas89Token::_EOF ) {
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

// @brief ゲート型を読み込む．
Iscas89Token
Iscas89Parser::parse_gate_type()
{
  FileRegion cur_loc;
  Iscas89Token tok;
  tie(tok, ignore, cur_loc) = read_token();
  switch ( tok ) {
  case Iscas89Token::CONST0:
  case Iscas89Token::CONST1:
  case Iscas89Token::BUFF:
  case Iscas89Token::NOT:
  case Iscas89Token::DFF:
  case Iscas89Token::AND:
  case Iscas89Token::NAND:
  case Iscas89Token::OR:
  case Iscas89Token::NOR:
  case Iscas89Token::XOR:
  case Iscas89Token::XNOR:
  case Iscas89Token::MUX:
    return tok;

  default:
    // シンタックスエラー
    // トークンが期待値と異なっていた
    ostringstream buf;
    buf << "Syntax error: gate-type is expected.";
    MsgMgr::put_msg(__FILE__, __LINE__, cur_loc,
		    MsgType::Error,
		    "ER_SYNTAX02",
		    buf.str());
  }

  return Iscas89Token::ERROR;
}

// @brief '(' ')' で囲まれた名前を読み込む．
bool
Iscas89Parser::parse_name(
  SizeType& name_id,
  FileRegion& last_loc
)
{
  bool ok;
  tie(ok, ignore, ignore) = expect(Iscas89Token::LPAR);
  if ( !ok ) {
    return false;
  }

  tie(ok, name_id, ignore) = expect(Iscas89Token::NAME);
  if ( !ok ) {
    return false;
  }

  tie(ok, ignore, last_loc) = expect(Iscas89Token::RPAR);
  if ( !ok ) {
    return false;
  }

  return true;
}

// @brief '(' ')' で囲まれた名前のリストを読み込む．
bool
Iscas89Parser::parse_name_list(
  vector<SizeType>& name_id_list,
  FileRegion& last_loc
)
{
  name_id_list.clear();

  bool ok;
  tie(ok, ignore, ignore) = expect(Iscas89Token::LPAR);
  if ( !ok ) {
    // '(' を期待していたシンタックスエラー
    return false;
  }

  for ( ; ; ) {
    SizeType name_id;
    tie(ok, name_id, ignore) = expect(Iscas89Token::NAME);
    if ( !ok ) {
      // NAME を期待したシンタックスエラー
      return false;
    }
    name_id_list.push_back(name_id);

    Iscas89Token tok;
    tie(tok, ignore, last_loc) = read_token();
    if ( tok == Iscas89Token::RPAR ) {
      break;
    }
    if ( tok != Iscas89Token::COMMA ) {
      // ')' か ',' を期待していたシンタックスエラー
      ostringstream buf;
      buf << "Syntax error: ')' or ',' are expected.";
      MsgMgr::put_msg(__FILE__, __LINE__, last_loc,
		      MsgType::Error,
		      "ER_SYNTAX03",
		      buf.str());
      return false;
    }
  }

  return true;
}

// @brief INPUT 文を読み込む．
bool
Iscas89Parser::read_input(
  const FileRegion& loc,
  SizeType name_id
)
{
  auto name = id2str(name_id);
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
Iscas89Parser::read_output(
  const FileRegion& loc,
  SizeType name_id
)
{
  mModel->mOutputList.push_back(name_id);

  return true;
}

// @brief ゲート文を読み込む．
bool
Iscas89Parser::read_gate(
  const FileRegion& loc,
  SizeType oname_id,
  PrimType gate_type,
  const vector<SizeType>& iname_id_list
)
{
  auto oname = id2str(oname_id);
  if ( is_defined(oname_id) ) {
    // 二重定義
    auto loc2 = def_loc(oname_id);
    ostringstream buf;
    buf << oname << ": Defined more than once. "
	<< "Previsous Definition is at " << loc2;
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Error,
		    "ER_MLTDEF01",
		    buf.str());
    return false;
  }

  set_gate(oname_id, loc, gate_type, iname_id_list);

  return true;
}

// @brief ゲート文(MUX)を読み込む．
bool
Iscas89Parser::read_mux(
  const FileRegion& loc,
  SizeType oname_id,
  const vector<SizeType>& iname_id_list
)
{
  auto oname = id2str(oname_id);
  if ( is_defined(oname_id) ) {
    // 二重定義
    auto loc2 = def_loc(oname_id);
    ostringstream buf;
    buf << oname << ": Defined more than once. "
	<< "Previsous Definition is at " << loc2;
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Error,
		    "ER_MLTDEF01",
		    buf.str());
    return false;
  }


  // 入力数をチェックする．
  SizeType ni = iname_id_list.size();
  SizeType nc = 0;
  SizeType nd = 1;
  while ( nc + nd < ni ) {
    ++ nc;
    nd <<= 1;
  }
  if ( nc + nd != ni ) {
    // 引数の数が合わない．
    ostringstream buf;
    buf << oname << ": Wrong # of inputs for MUX-type.";
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Error,
		    "ER_MUX01",
		    buf.str());
    return false;
  }

  vector<SizeType> control_list;
  vector<SizeType> control_inv_list;
  control_list.reserve(nc);
  for ( SizeType i = 0; i < nc; ++ i ) {
    SizeType id = iname_id_list[i];
    control_list.push_back(id);
    SizeType inv_id = new_gate(loc, PrimType::Not, {id});
    control_inv_list.push_back(inv_id);
  }
  vector<SizeType> tmp_list;
  tmp_list.reserve(nd);
  for ( SizeType i = 0; i < nd; ++ i ) {
    vector<SizeType> fanin_list;
    fanin_list.reserve(nc + 1);
    SizeType id = iname_id_list[i + nc];
    fanin_list.push_back(id);
    for ( SizeType j = 0; j < nc; ++ j ) {
      if ( i & (1 << j) ) {
	fanin_list.push_back(control_list[j]);
      }
      else {
	fanin_list.push_back(control_inv_list[j]);
      }
    }
    SizeType and_id = new_gate(loc, PrimType::And, fanin_list);
    tmp_list.push_back(and_id);
  }

  set_gate(oname_id, loc, PrimType::Or, tmp_list);

  return true;
}

// @brief D-FF用のゲート文を読み込む．
bool
Iscas89Parser::read_dff(
  const FileRegion& loc,
  SizeType oname_id,
  SizeType iname_id
)
{
  auto oname = id2str(oname_id);
  if ( is_defined(oname_id) ) {
    // 二重定義
    auto loc2 = def_loc(oname_id);
    ostringstream buf;
    buf << oname << ": Defined more than once. "
	<< "Previsous Definition is " << loc2;
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Error,
		    "ER_MLTDEF01",
		    buf.str());
    return false;
  }

  set_defined(oname_id, loc);
  mModel->set_dff(oname_id, iname_id);
  mMark.emplace(oname_id);

  return true;
}

BEGIN_NONAMESPACE

const char*
token_str(
  Iscas89Token token
)
{
  switch (token) {
  case Iscas89Token::LPAR:   return "(";
  case Iscas89Token::RPAR:   return ")";
  case Iscas89Token::EQ:     return "=";
  case Iscas89Token::COMMA:  return ",";
  case Iscas89Token::INPUT:  return "INPUT";
  case Iscas89Token::OUTPUT: return "OUTPUT";
  case Iscas89Token::CONST0: return "CONST0";
  case Iscas89Token::CONST1: return "CONST1";
  case Iscas89Token::BUFF:   return "BUFF";
  case Iscas89Token::NOT:    return "NOT";
  case Iscas89Token::AND:    return "AND";
  case Iscas89Token::NAND:   return "NAND";
  case Iscas89Token::OR:     return "OR";
  case Iscas89Token::NOR:    return "NOR";
  case Iscas89Token::XOR:    return "XOR";
  case Iscas89Token::XNOR:   return "XNOR";
  case Iscas89Token::DFF:    return "DFF";
  case Iscas89Token::MUX:    return "MUX";
  case Iscas89Token::NAME:   return "__name__";
  case Iscas89Token::_EOF:   return "__eof__";
  case Iscas89Token::ERROR:  return "__error__";
  }
  ASSERT_NOT_REACHED;
  return "";
}

END_NONAMESPACE

// @brief 次のトークンが期待されている型か調べる．
tuple<bool, SizeType, FileRegion>
Iscas89Parser::expect(
  Iscas89Token exp_token
)
{
  Iscas89Token tok;
  SizeType name_id;
  FileRegion loc;
  tie(tok, name_id, loc) = read_token();
  if ( tok != exp_token ) {
    // トークンが期待値と異なっていた
    ostringstream buf;
    buf << "Syntax error: '" << token_str(exp_token) << "' is expected.";
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Error,
		    "ER_SYNTAX01",
		    buf.str());
    return make_tuple(false, 0, loc);
  }

  return make_tuple(true, name_id, loc);
}

// @brief yylex() 用の処理を行う．
// @return トークンの型を返す．
tuple<Iscas89Token, SizeType, FileRegion>
Iscas89Parser::read_token()
{
  FileRegion lloc;
  auto token = mScanner->read_token(lloc);
  SizeType id{0};
  if ( token == Iscas89Token::NAME ) {
    auto name = mScanner->cur_string();
    id = find_id(name, lloc);
  }
  return make_tuple(token, id, lloc);
}

// @brief トロロジカル順に並べる．
void
Iscas89Parser::order_node(
  SizeType id
)
{
  if ( mMark.count(id) > 0 ) {
    return;
  }

  auto& node = mModel->mNodeArray[id];
  ASSERT_COND( node.is_gate() );
  for ( auto iid: node.fanin_list() ) {
    order_node(iid);
  }
  mMark.emplace(id);
  mModel->mGateList.push_back(id);
}

END_NAMESPACE_YM_ISCAS89
