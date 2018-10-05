﻿
/// @file Iscas89Scanner.cc
/// @brief Iscas89Scanner の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "Iscas89Scanner.h"


BEGIN_NONAMESPACE

// read_token() の動作をデバッグするときに true にする．
const bool debug_read_token = false;

END_NONAMESPACE

BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// iscas89 用の字句解析器
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] ido 入力データ
Iscas89Scanner::Iscas89Scanner(IDO& ido) :
  Scanner(ido)
{
}

// @brief デストラクタ
Iscas89Scanner::~Iscas89Scanner()
{
}

// @brief トークンを一つ読み出す．
// @param[out] loc トークンの位置を格納する変数
Iscas89Token
Iscas89Scanner::read_token(FileRegion& loc)
{
  Iscas89Token token = scan();
  loc = cur_loc();

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

    // 予約後の検索
    if ( mCurString == "INPUT" ) {
      return Iscas89Token::INPUT;
    }
    if ( mCurString == "OUTPUT" ) {
      return Iscas89Token::OUTPUT;
    }
    if ( mCurString == "BUFF" ) {
      return Iscas89Token::BUFF;
    }
    if ( mCurString == "NOT" ) {
      return Iscas89Token::NOT;
    }
    if ( mCurString == "INV" ) {
      return Iscas89Token::NOT;
    }
    if ( mCurString == "AND" ) {
      return Iscas89Token::AND;
    }
    if ( mCurString == "NAND" ) {
      return Iscas89Token::NAND;
    }
    if ( mCurString == "OR" ) {
      return Iscas89Token::OR;
    }
    if ( mCurString == "NOR" ) {
      return Iscas89Token::NOR;
    }
    if ( mCurString == "XOR" ) {
      return Iscas89Token::XOR;
    }
    if ( mCurString == "XNOR" ) {
      return Iscas89Token::XNOR;
    }
    if ( mCurString == "DFF" ) {
      return Iscas89Token::DFF;
    }
    return Iscas89Token::NAME;

  default:
    accept();
    mCurString.put_char(c);
    goto ST_STR;
  }
}

END_NAMESPACE_YM_BNET