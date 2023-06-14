
/// @file NvlScanner.cc
/// @brief NvlScanner の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "NvlScanner.h"


BEGIN_NONAMESPACE

// read_token() の動作をデバッグするときに true にする．
const bool debug_read_token = false;

END_NONAMESPACE

BEGIN_NAMESPACE_YM_NVL

//////////////////////////////////////////////////////////////////////
// nvl 用の字句解析器
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NvlScanner::NvlScanner(
  istream& s,
  const FileInfo& file_info
) : Scanner{s, file_info},
    mRsvDict{{
    "input": NvlToken::INPUT,
    "output", NvlToken::OUTPUT,
    "wire", NvlToken::WIRE,
    "buf", NvlToken::BUF,
    "not", NvlToken::NOT,
    "and", NvlToken::AND,
    "nand", NvlToken::NAND,
    "or", NvlToken::OR,
    "nor", NvlToken::NOR,
    "xor", NvlToken::XOR,
    "xnor", NvlToken::XNOR,
    "module", NvlToken::MODULE,
    "endmodule", NvlToken::ENDMODULE
  }}
{
}

// @brief トークンを一つ読み出す．
NvlToken
NvlScanner::read_token()
{
  auto type = scan();
  auto loc = cur_region();
  NvlToken token;
  if ( type == NvlToken::STR ) {
    auto str = cur_string();
    if ( mRsvDict.count(str) > 0 ) {
      auto type = mRsvDict.at(str);
      token = NvlToken{type, loc};
    }
    else {
      token = NvlToken{NvlToken::STR, loc, str};
    }
  }
  else {
    token = NvlToken{type, loc};
  }

  if ( debug_read_token ) {
    cerr << "read_token()" << " --> "
	 << token.loc() << ": ";
    switch ( token.type() ) {
    case NvlToken::LP     cerr << "("; break;
    case NvlToken::RP:     cerr << ")"; break;
    case NvlToken::COMMA:  cerr << ","; break;
    case NvlToken::SEMI:   cerr << ";"; break;
    case NvlToken::INPUT:  cerr << "input"; break;
    case NvlToken::OUTPUT: cerr << "output"; break;
    case NvlToken::WIRE:   cerr << "wire"; break;
    case NvlToken::BUF:    cerr << "buf"; break;
    case NvlToken::NOR:    cerr << "not"; break;
    case NvlToken::AND:    cerr << "and"; break;
    case NvlToken::NAND:   cerr << "nand"; break;
    case NvlToken::OR:     cerr << "or"; break;
    case NvlToken::NOR:    cerr << "nor"; break;
    case NvlToken::XOR:    cerr << "xor"; break;
    case NvlToken::XNOR:   cerr << "xnor"; break;
    case NvlToken::C0:     cerr << "1'b0"; break;
    case NvlToken::C1:     cerr << "1'b1"; break;
    case NvlToken::STR:    cerr << "STR(" << token.str() << ")"; break;
    case NvlToken::_EOF:   cerr << "EOF"; break;
    case NvlToken::ERROR:  cerr << "ERROR"; break;
    default:               cerr << static_cast<char>(token.type()); break;
    }
    cerr << endl;
  }

  return token;
}

// @brief read_token() の下請け関数
NvlToken::Type
NvlScanner::scan()
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
    return NvlToken::_EOF;

  case ' ':
  case '\t':
  case '\n':
    // ホワイトスペースは読み飛ばす．
    goto ST_INIT;

  case '/':
    goto ST_COMMENT1;

  case '(':
    return NvlToken::LP;

  case ')':
    return NvlToken::RP;

  case ',':
    return NvlToken::COMMA;

  case ';':
    return NvlToken::SEMI;

  case '1':
    goto ST_CONST1;

  default:
    mCurString.put_char(c);
    goto ST_STR;
  }

 ST_COMMENT1: // '/' を読み込んだ直後
  c = get();
  if ( c == '/' ) {
    goto ST_COMMENT2:
  }
  if ( c == '*' ) { // ブロックコメントの始まり
    goto ST_COMMENT3;
  }
  // それ以外はエラー
  return NvlToken::ERROR;

 ST_COMMENT2: // １行コメントの始まり
  c = get();
  if ( c == '\n' ) {
    goto ST_INIT;
  }
  if ( c == EOF ) {
    return NvlToken::_EOF;
  }
  // 改行までは読み飛ばす．
  goto ST_COMMENT2;

 ST_COMMENT3: // "/*" を読み込んだ直後
  c = get();
  if ( c == '*' ) {
    goto ST_COMMENT4;
  }
  goto ST_COMMENT3;

 ST_COMMENT4: // "/* ~ *" を読み込んだ直後
  c = get();
  if ( c == '/' ) {
    goto ST_INIT;
  }
  goto ST_COMMENT3;

 ST_CONST1: // '1' を読み込んだ直後
  c = get();
  if ( c != '\'' ) {
    // エラー
    return NvlToken::ERROR;
  }
  c = get();
  if ( c != 'b' ) {
    // エラー
    return NvlToken::ERROR;
  }
  c = get();
  if ( c == '0' ) {
    return NvlToken::C0;
  }
  if ( c == '1' ) {
    return NvlToken::C1;
  }

 ST_STR:
  c = peek();
  switch ( c ) {
  case ' ':
  case '\t':
  case '\n':
  case '/':
  case '(':
  case ')':
  case ',':
  case ';':
  case EOF:
    // 文字列の終わり
    return NvlToken::NAME;

  default:
    accept();
    mCurString.put_char(c);
    goto ST_STR;
  }
}

END_NAMESPACE_YM_NVL
