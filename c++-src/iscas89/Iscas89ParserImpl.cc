
/// @file Iscas89ParserImpl.cc
/// @brief BlibParser の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2019 Yusuke Matsunaga
/// All rights reserved.


#include "Iscas89ParserImpl.h"
#include "ym/Iscas89Handler.h"
#include "ym/MsgMgr.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// Iscas89ParserImpl
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
Iscas89ParserImpl::read(const string& filename)
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

  mScanner = unique_ptr<Iscas89Scanner>{new Iscas89Scanner{fin, {filename}}};

  for ( auto handler: mHandlerList ) {
    if ( !handler->init() ) {
      delete_scanner();
      return false;
    }
  }

  // パーサー本体
  bool go_on = true;
  bool has_error = false;
  while ( go_on ) {
    Iscas89Token tok;
    int name_id;
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

	Iscas89Token gate_type = parse_gate_type();
	BnNodeType type = BnNodeType::None;
	vector<int> iname_id_list;
	switch ( gate_type ) {
	case Iscas89Token::CONST0: type = BnNodeType::C0; break;
	case Iscas89Token::CONST1: type = BnNodeType::C1; break;
	case Iscas89Token::BUFF:   type = BnNodeType::Buff; break;
	case Iscas89Token::NOT:    type = BnNodeType::Not;  break;
	case Iscas89Token::AND:    type = BnNodeType::And;  break;
	case Iscas89Token::NAND:   type = BnNodeType::Nand; break;
	case Iscas89Token::OR:     type = BnNodeType::Or;   break;
	case Iscas89Token::NOR:    type = BnNodeType::Nor;  break;
	case Iscas89Token::XOR:    type = BnNodeType::Xor;  break;
	case Iscas89Token::XNOR:   type = BnNodeType::Xnor; break;
	case Iscas89Token::DFF:
	  if ( !parse_name_list(iname_id_list, last_loc) ) {
	    goto error;
	  }
	  if ( iname_id_list.size() != 1 ) {
	    // 引数の数が合わない．
#warning "TODO: ちゃんとしたエラーメッセージを出す．"
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
	if ( type != BnNodeType::None ) {
	  if ( type != BnNodeType::C0 && type != BnNodeType::C1 ) {
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

  // 出力文の処理を行う．
  for ( int i = 0; i < mOidArray.size(); ++ i ) {
    int oid;
    FileRegion loc;
    tie(oid, loc) = mOidArray[i];
    IdCell& cell = id2cell(oid);
    for ( auto handler: mHandlerList ) {
      if ( !handler->read_output(loc, oid, cell.name()) ) {
	has_error = true;
      }
    }
  }

  // 終了処理を行う．
  for ( auto handler: mHandlerList ) {
    if ( !handler->end() ) {
      has_error = true;
    }
  }

  delete_scanner();

  if ( !has_error ) {
    // 成功
    for ( auto handler: mHandlerList ) {
      handler->normal_exit();
    }
    mIdHash.clear();
    return true;
  }
  else {
    // 失敗
    for ( auto handler: mHandlerList ) {
      handler->error_exit();
    }
    mIdHash.clear();
    return false;
  }
}

// @brief イベントハンドラの登録
void
Iscas89ParserImpl::add_handler(Iscas89Handler* handler)
{
  mHandlerList.push_back(handler);
}

// @brief ゲート型を読み込む．
// @return トークンを返す．
//
// エラーが起きたら Iscas89Token::ERROR を返す．
Iscas89Token
Iscas89ParserImpl::parse_gate_type()
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
// @param[in] name_id 名前の識別子番号を格納する変数．
// @param[in] last_loc 末尾のファイル位置
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
//
// エラーが起きたらエラーメッセージをセットする．
bool
Iscas89ParserImpl::parse_name(int& name_id,
			      FileRegion& last_loc)
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
// @param[in] name_id_list 名前の識別子番号を格納するリスト．
// @param[in] last_loc 末尾のファイル位置
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
//
// エラーが起きたらエラーメッセージをセットする．
bool
Iscas89ParserImpl::parse_name_list(vector<int>& name_id_list,
				   FileRegion& last_loc)
{
  name_id_list.clear();

  bool ok;
  tie(ok, ignore, ignore) = expect(Iscas89Token::LPAR);
  if ( !ok ) {
    // '(' を期待していたシンタックスエラー
    return false;
  }

  for ( ; ; ) {
    int name_id;
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
// @param[in] loc ファイル位置
// @param[in] name_id 入力ピン名の ID 番号
// @return エラーが起きたら false を返す．
bool
Iscas89ParserImpl::read_input(const FileRegion& loc,
			      int name_id)
{
  IdCell& cell = id2cell(name_id);
  if ( cell.is_defined() ) {
    ostringstream buf;
    buf << cell.name() << ": Defined more than once. Previous definition is "
	<< cell.loc();
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Error,
		    "ER_MLTDEF01",
		    buf.str());
    return false;
  }
  cell.set_defined(loc);
  cell.set_input();
  for ( auto handler: mHandlerList ) {
    if ( !handler->read_input(loc, name_id, cell.name()) ) {
      return false;
    }
  }
  return true;
}

// @brief OUTPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 出力ピン名の ID 番号
// @return エラーが起きたら false を返す．
bool
Iscas89ParserImpl::read_output(const FileRegion& loc,
			       int name_id)
{
  IdCell& cell = id2cell(name_id);
  if ( cell.is_input() ) {
    ostringstream buf;
    buf << cell.name() << ": Defined as both input and output. "
	<< "Previous definition is " << cell.loc();
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Warning,
		    "WR_MLTDEF02",
		    buf.str());
  }
  cell.set_output();
  mOidArray.push_back(make_pair(name_id, loc));

  return true;
}

// @brief ゲート文を読み込む．
// @param[in] loc ファイル位置
// @param[in] oname_id 出力名の ID 番号
// @param[in] logic_type ゲートタイプ
// @return エラーが起きたら false を返す．
bool
Iscas89ParserImpl::read_gate(const FileRegion& loc,
			     int oname_id,
			     BnNodeType logic_type,
			     const vector<int>& iname_id_list)
{
  IdCell& cell = id2cell(oname_id);
  if ( cell.is_defined() ) {
    // 二重定義
    ostringstream buf;
    buf << cell.name() << ": Defined more than once. "
	<< "Previsous Definition is " << cell.loc();
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Error,
		    "ER_MLTDEF01",
		    buf.str());
    return false;
  }

  cell.set_defined(loc);
  bool stat = true;
  for ( auto handler: mHandlerList ) {
    if ( !handler->read_gate(loc, logic_type, oname_id, cell.name(), iname_id_list) ) {
      stat = false;
      break;
    }
  }
  return stat;
}

// @brief ゲート文(MUX)を読み込む．
// @param[in] loc ファイル位置
// @param[in] oname_id 出力名の ID 番号
// @return エラーが起きたら false を返す．
bool
Iscas89ParserImpl::read_mux(const FileRegion& loc,
			    int oname_id,
			    const vector<int>& iname_id_list)
{
  IdCell& cell = id2cell(oname_id);
  if ( cell.is_defined() ) {
    // 二重定義
    ostringstream buf;
    buf << cell.name() << ": Defined more than once. "
	<< "Previsous Definition is " << cell.loc();
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Error,
		    "ER_MLTDEF01",
		    buf.str());
    return false;
  }

  cell.set_defined(loc);

  { // 入力数をチェックする．
    int ni = iname_id_list.size();
    int nc = 0;
    int nd = 1;
    while ( nc + nd < ni ) {
      ++ nc;
      nd <<= 1;
    }
    if ( nc + nd != ni ) {
      // 引数の数が合わない．
      ostringstream buf;
      buf << cell.name() << ": Wrong # of inputs for MUX-type.";
      MsgMgr::put_msg(__FILE__, __LINE__, cell.loc(),
		      MsgType::Error,
		      "ER_MUX01",
		      buf.str());
      return false;
    }
  }

  bool stat = true;
  for ( auto handler: mHandlerList ) {
    if ( !handler->read_mux(loc, oname_id, cell.name(), iname_id_list) ) {
      stat = false;
      break;
    }
  }
  return stat;
}

// @brief D-FF用のゲート文を読み込む．
// @param[in] loc ファイル位置
// @param[in] oname_id 出力名の ID 番号
// @param[in] type ゲートタイプ
// @return エラーが起きたら false を返す．
bool
Iscas89ParserImpl::read_dff(const FileRegion& loc,
			    int oname_id,
			    int iname_id)
{
  IdCell& cell = id2cell(oname_id);
  if ( cell.is_defined() ) {
    // 二重定義
    ostringstream buf;
    buf << cell.name() << ": Defined more than once. "
	<< "Previsous Definition is " << cell.loc();
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Error,
		    "ER_MLTDEF01",
		    buf.str());
    return false;
  }

  cell.set_defined(loc);
  bool stat = true;
  for ( auto handler: mHandlerList ) {
    if ( !handler->read_dff(loc, oname_id, cell.name(), iname_id) ) {
      stat = false;
      break;
    }
  }
  return stat;
}

BEGIN_NONAMESPACE

const char*
token_str(Iscas89Token token)
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
}

END_NONAMESPACE

// @brief 次のトークンが期待されている型か調べる．
// @param[in] exp_token トークンの期待値
// @param[out] lval トークンの値を格納する変数
// @param[out] loc トークンのファイル位置を格納する変数．
// @retval true トークンの型が一致した．
// @retval false トークンの方が一致しなかった．
//
// トークンの方が一致しなかった場合にはエラーメッセージをセットする．
tuple<bool, int, FileRegion>
Iscas89ParserImpl::expect(Iscas89Token exp_token)
{
  Iscas89Token tok;
  int name_id;
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
tuple<Iscas89Token, int, FileRegion>
Iscas89ParserImpl::read_token()
{
  FileRegion lloc;
  Iscas89Token token = mScanner->read_token(lloc);
  int id;
  if ( token == Iscas89Token::NAME ) {
    const char* name = mScanner->cur_string();
    if ( mIdHash.count(name) == 0 ) {
      id = mIdArray.size();
      mIdArray.push_back({id, name});
      auto cell = &mIdArray[id];
      mIdHash.emplace(cell->name(), cell);
    }
    else {
      id = mIdHash.at(name)->id();
    }
  }
  return make_tuple(token, id, lloc);
}

// @brief スキャナーを削除する．
void
Iscas89ParserImpl::delete_scanner()
{
  unique_ptr<Iscas89Scanner> dummy;
  mScanner.swap(dummy);
}

END_NAMESPACE_YM_BNET
