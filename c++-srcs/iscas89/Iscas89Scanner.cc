
/// @file Iscas89Scanner.cc
/// @brief Iscas89Scanner の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014, 2019, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Iscas89Scanner.h"


BEGIN_NONAMESPACE

// read_token() の動作をデバッグするときに true にする．
const bool debug_read_token = false;

END_NONAMESPACE

BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
// iscas89 用の字句解析器
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Iscas89Scanner::Iscas89Scanner(
  istream& s,
  const FileInfo& file_info
) : Scanner(s, file_info)
{
}

// @brief トークンを一つ読み出す．
// @param[out] loc トークンの位置を格納する変数
Iscas89Token
Iscas89Scanner::read_token(FileRegion& loc)
{
  auto token = scan();
  if ( token == Iscas89Token::NAME ) {
    token = check_word();
  }
  loc = cur_region();

  if ( debug_read_token ) {
    cerr << "read_token()" << " --> "
	 << loc << ": ";
    switch ( token ) {
    case Iscas89Token::LPAR:   cerr << "("; break;
    case Iscas89Token::RPAR:   cerr << ")"; break;
    case Iscas89Token::EQ:     cerr << "="; break;
    case Iscas89Token::COMMA:  cerr << ","; break;
    case Iscas89Token::INPUT:  cerr << "INPUT"; break;
    case Iscas89Token::OUTPUT: cerr << "OUTPUT"; break;
    case Iscas89Token::BUFF:   cerr << "BUFF"; break;
    case Iscas89Token::NOT:    cerr << "NOT"; break;
    case Iscas89Token::AND:    cerr << "AND"; break;
    case Iscas89Token::NAND:   cerr << "NAND"; break;
    case Iscas89Token::OR:     cerr << "OR"; break;
    case Iscas89Token::NOR:    cerr << "NOR"; break;
    case Iscas89Token::XOR:    cerr << "XOR"; break;
    case Iscas89Token::XNOR:   cerr << "XNOR"; break;
    case Iscas89Token::MUX:    cerr << "MUX"; break;
    case Iscas89Token::CONST0: cerr << "CONST0"; break;
    case Iscas89Token::CONST1: cerr << "CONST1"; break;
    case Iscas89Token::DFF:    cerr << "DFF"; break;
    case Iscas89Token::NAME:   cerr << "NAME(" << cur_string() << ")"; break;
    case Iscas89Token::_EOF:   cerr << "EOF"; break;
    default:                   cerr << static_cast<char>(token); break;
    }
    cerr << endl;
  }

  return token;
}

// @brief read_token() の下請け関数
// @return トークンを返す．
Iscas89Token
Iscas89Scanner::scan()
{
  int c;

  mCurString = "";

  // 状態遷移を goto 文で表現したもの
  // 効率はよい．

 ST_INIT:
  c = get();
  set_first_loc();
  switch ( c ) {
  case EOF:
    return Iscas89Token::_EOF;

  case ' ':
  case '\t':
  case '\n':
    // ホワイトスペースは読み飛ばす．
    goto ST_INIT;

  case '#':
    goto ST_SHARP;

  case '=':
    return Iscas89Token::EQ;

  case '(':
    return Iscas89Token::LPAR;

  case ')':
    return Iscas89Token::RPAR;

  case ',':
    return Iscas89Token::COMMA;

  default:
    mCurString.put_char(c);
    goto ST_STR;
  }

 ST_SHARP: // 1行コメントの始まり
  c = get();
  if ( c == '\n' ) {
    goto ST_INIT;
  }
  if ( c == EOF ) {
    return Iscas89Token::_EOF;
  }
  // 改行までは読み飛ばす．
  goto ST_SHARP;

 ST_STR:
  c = peek();
  switch ( c ) {
  case ' ':
  case '\t':
  case '\n':
  case '#':
  case '=':
  case '(':
  case ')':
  case ',':
  case EOF:
    // 文字列の終わり
    return Iscas89Token::NAME;

  default:
    accept();
    mCurString.put_char(c);
    goto ST_STR;
  }
}

// @brief 予約語の検査を行う．
// @return トークンを返す．
//
// 予約語でなければ NAME を返す．
Iscas89Token
Iscas89Scanner::check_word()
{
  if ( mCurString == "INPUT" || mCurString == "input" ) {
    return Iscas89Token::INPUT;
  }
  if ( mCurString == "OUTPUT" || mCurString == "output" ) {
    return Iscas89Token::OUTPUT;
  }
  if ( mCurString == "BUFF" || mCurString == "buff" ) {
    return Iscas89Token::BUFF;
  }
  if ( mCurString == "BUF" || mCurString == "buf" ) {
    return Iscas89Token::BUFF;
  }
  if ( mCurString == "NOT" || mCurString == "not" ) {
    return Iscas89Token::NOT;
  }
  if ( mCurString == "INV" || mCurString == "inv" ) {
    return Iscas89Token::NOT;
  }
  if ( mCurString == "AND" || mCurString == "and" ) {
    return Iscas89Token::AND;
  }
  if ( mCurString == "NAND" || mCurString == "nand" ) {
    return Iscas89Token::NAND;
  }
  if ( mCurString == "OR" || mCurString == "or" ) {
    return Iscas89Token::OR;
  }
  if ( mCurString == "NOR" || mCurString == "nor" ) {
    return Iscas89Token::NOR;
  }
  if ( mCurString == "XOR" || mCurString == "xor" ) {
    return Iscas89Token::XOR;
  }
  if ( mCurString == "XNOR" || mCurString == "xnor" ) {
    return Iscas89Token::XNOR;
  }
  if ( mCurString == "MUX" || mCurString == "mux" ) {
    return Iscas89Token::MUX;
  }
  if ( mCurString == "CONST0" || mCurString == "const0" ) {
    return Iscas89Token::CONST0;
  }
  if ( mCurString == "CONST1" || mCurString == "const1" ) {
    return Iscas89Token::CONST1;
  }
  if ( mCurString == "DFF" || mCurString == "dff" ) {
    return Iscas89Token::DFF;
  }
  return Iscas89Token::NAME;
}

END_NAMESPACE_YM_ISCAS89
