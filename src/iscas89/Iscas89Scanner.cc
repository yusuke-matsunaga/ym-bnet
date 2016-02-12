
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
    case kIscas89_LPAR:   cerr << "("; break;
    case kIscas89_RPAR:   cerr << ")"; break;
    case kIscas89_EQ:     cerr << "="; break;
    case kIscas89_COMMA:  cerr << ","; break;
    case kIscas89_INPUT:  cerr << "INPUT"; break;
    case kIscas89_OUTPUT: cerr << "OUTPUT"; break;
    case kIscas89_BUFF:   cerr << "BUFF"; break;
    case kIscas89_NOT:    cerr << "NOT"; break;
    case kIscas89_AND:    cerr << "AND"; break;
    case kIscas89_NAND:   cerr << "NAND"; break;
    case kIscas89_OR:     cerr << "OR"; break;
    case kIscas89_NOR:    cerr << "NOR"; break;
    case kIscas89_XOR:    cerr << "XOR"; break;
    case kIscas89_XNOR:   cerr << "XNOR"; break;
    case kIscas89_DFF:    cerr << "DFF"; break;
    case kIscas89_NAME:   cerr << "NAME(" << cur_string() << ")"; break;
    case kIscas89_EOF:    cerr << "EOF"; break;
    default:            cerr << static_cast<char>(token); break;
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
    return kIscas89_EOF;

  case ' ':
  case '\t':
  case '\n':
    // ホワイトスペースは読み飛ばす．
    goto ST_INIT;

  case '#':
    goto ST_SHARP;

  case '=':
    return kIscas89_EQ;

  case '(':
    return kIscas89_LPAR;

  case ')':
    return kIscas89_RPAR;

  case ',':
    return kIscas89_COMMA;

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
    return kIscas89_EOF;
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
      return kIscas89_INPUT;
    }
    if ( mCurString == "OUTPUT" ) {
      return kIscas89_OUTPUT;
    }
    if ( mCurString == "BUFF" ) {
      return kIscas89_BUFF;
    }
    if ( mCurString == "NOT" ) {
      return kIscas89_NOT;
    }
    if ( mCurString == "INV" ) {
      return kIscas89_NOT;
    }
    if ( mCurString == "AND" ) {
      return kIscas89_AND;
    }
    if ( mCurString == "NAND" ) {
      return kIscas89_NAND;
    }
    if ( mCurString == "OR" ) {
      return kIscas89_OR;
    }
    if ( mCurString == "NOR" ) {
      return kIscas89_NOR;
    }
    if ( mCurString == "XOR" ) {
      return kIscas89_XOR;
    }
    if ( mCurString == "XNOR" ) {
      return kIscas89_XNOR;
    }
    if ( mCurString == "DFF" ) {
      return kIscas89_DFF;
    }
    return kIscas89_NAME;

  default:
    accept();
    mCurString.put_char(c);
    goto ST_STR;
  }
}

END_NAMESPACE_YM_BNET
