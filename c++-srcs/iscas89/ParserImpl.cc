
/// @file ParserImpl.cc
/// @brief ParserImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2019, 2021, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ParserImpl.h"
#include "Iscas89Scanner.h"
#include "ym/Iscas89Model.h"
#include "ym/Iscas89Handler.h"
#include "ModelImpl.h"
#include "ym/MsgMgr.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
// ParserImpl
//////////////////////////////////////////////////////////////////////

// @brief デストラクタ
ParserImpl::~ParserImpl()
{
  for ( auto handler: mHandlerList ) {
    delete handler;
  }
}

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
		    MsgType::Failure, "ISCAS89_PARSER", buf.str());
    return false;
  }

  Iscas89Scanner scanner{fin, {filename}, mHandlerDict};
  mScanner = &scanner;
  mModel = model;

  // パーサー本体
  bool go_on = true;
  bool has_error = false;
  while ( go_on ) {
    SizeType name_id;
    auto token = read_token(name_id);
    auto first_loc = token.loc();
    switch ( token.type() ) {
    case Iscas89Token::INPUT:
      if ( !read_input(first_loc) ) {
	goto error;
      }
      break;

    case Iscas89Token::OUTPUT:
      if ( !read_output(first_loc) ) {
	goto error;
      }
      break;

    case Iscas89Token::NAME:
      if ( !read_gate(first_loc, name_id) ) {
	goto error;
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
      auto token = mScanner->read_token();
      if ( token.type() == Iscas89Token::RPAR ||
	   token.type() == Iscas89Token::_EOF ) {
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
  tie(ok, ignore, ignore) = expect(Iscas89Token::EQ);
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
  if ( gate_token.type() == Iscas89Token::GATE ) {
    vector<SizeType> iname_id_list;
    FileRegion last_loc;
    if ( !parse_name_list(iname_id_list, last_loc) ) {
      return false;
    }
    FileRegion loc{first_loc, last_loc};
    set_gate(name_id, loc, gate_token.gate_type(), iname_id_list);
    return true;
  }
  if ( gate_token.type() == Iscas89Token::DFF ) {
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
  if ( gate_token.type() == Iscas89Token::EXGATE ) {
    auto handler = get_handler(gate_token.ex_id());
    return handler->_read(this, first_loc, name_id);
    return false;
  }
  return false;
}

// @brief 論理式を登録する．
SizeType
ParserImpl::reg_expr(
  const Expr& expr ///< [in] 論理式
)
{
  auto key = expr.rep_string();
  if ( mExprDict.count(key) > 0 ) {
    // 登録済み
    return mExprDict.at(key);
  }
  // 新規に登録する．
  SizeType id = mModel->add_expr(expr);
  mExprDict.emplace(key, id);
  return id;
}

// @brief '(' ')' で囲まれた名前を読み込む．
bool
ParserImpl::parse_name(
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
ParserImpl::parse_name_list(
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

    auto token = mScanner->read_token();
    if ( token.type() == Iscas89Token::RPAR ) {
      last_loc = token.loc();
      break;
    }
    if ( token.type() != Iscas89Token::COMMA ) {
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
  Iscas89Token::Type token
)
{
  switch (token) {
  case Iscas89Token::LPAR:   return "(";
  case Iscas89Token::RPAR:   return ")";
  case Iscas89Token::EQ:     return "=";
  case Iscas89Token::COMMA:  return ",";
  case Iscas89Token::INPUT:  return "INPUT";
  case Iscas89Token::OUTPUT: return "OUTPUT";
  case Iscas89Token::GATE:   return "GATE";
  case Iscas89Token::EXGATE: return "EXGATE";
  case Iscas89Token::DFF:    return "DFF";
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
ParserImpl::expect(
  Iscas89Token::Type exp_type
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
Iscas89Token
ParserImpl::read_token(
  SizeType& name_id
)
{
  auto token = mScanner->read_token();
  if ( token.type() == Iscas89Token::NAME ) {
    auto name = token.name();
    name_id = find_id(name, token.loc());
  }
  return token;
}

// @brief トロロジカル順に並べる．
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

END_NAMESPACE_YM_ISCAS89
