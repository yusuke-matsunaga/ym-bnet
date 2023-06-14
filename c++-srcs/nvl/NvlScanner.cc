
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
) : Scanner{s, file_info}
{
  // 予約語辞書を作る．
  mRsvDict.emplace("input", RsvInfo{NvlToken::INPUT, PrimType::None});
  mRsvDict.emplace("output", RsvInfo{NvlToken::OUTPUT, PrimType::None});
  mRsvDict.emplace("wire", RsvInfo{NvlToken::WIRE, PrimType::None});
  mRsvDict.emplace("buf", RsvInfo{NvlToken::GATE, PrimType::Buff});
  mRsvDict.emplace("not", RsvInfo{NvlToken::GATE, PrimType::Not});
  mRsvDict.emplace("and", RsvInfo{NvlToken::GATE, PrimType::And});
  mRsvDict.emplace("nand", RsvInfo{NvlToken::GATE, PrimType::Nand});
  mRsvDict.emplace("or", RsvInfo{NvlToken::GATE, PrimType::Or});
  mRsvDict.emplace("nor", RsvInfo{NvlToken::GATE, PrimType::Nor});
  mRsvDict.emplace("xor", RsvInfo{NvlToken::GATE, PrimType::Xor});
  mRsvDict.emplace("xnor", RsvInfo{NvlToken::GATE, PrimType::Xnor});
  mRsvDict.emplace("module", RsvInfo{NvlToken::MODULE, PrimType::None});
  mRsvDict.emplace("endmodule", RsvInfo{NvlToken::ENDMODULE, PrimType::None});
}

// @brief トークンを一つ読み出す．
NvlToken
NvlScanner::read_token()
{
  auto type = scan();
  auto loc = cur_region();
  NvlToken token;
  if ( type == NvlToken::NAME ) {
    auto name = cur_string();
    if ( mRsvDict.count(name) > 0 ) {
      auto info = mRsvDict.at(name);
      token = NvlToken{info.type, loc, info.gate_type, {}};
    }
    else {
      token = NvlToken{NvlToken::NAME, loc, PrimType::None, name};
    }
  }
  else {
    token = NvlToken{type, loc};
  }

  if ( debug_read_token ) {
    cerr << "read_token()" << " --> "
	 << token.loc() << ": ";
    switch ( token.type() ) {
    case NvlToken::LPAR:   cerr << "("; break;
    case NvlToken::RPAR:   cerr << ")"; break;
    case NvlToken::EQ:     cerr << "="; break;
    case NvlToken::COMMA:  cerr << ","; break;
    case NvlToken::SEMI:   cerr << ";"; break;
    case NvlToken::INPUT:  cerr << "input"; break;
    case NvlToken::OUTPUT: cerr << "output"; break;
    case NvlToken::GATE:
      switch ( token.gate_type() ) {
      case PrimType::Buff:   cerr << "buf"; break;
      case PrimType::Not:    cerr << "not"; break;
      case PrimType::And:    cerr << "and"; break;
      case PrimType::Nand:   cerr << "nand"; break;
      case PrimType::Or:     cerr << "or"; break;
      case PrimType::Nor:    cerr << "nor"; break;
      case PrimType::Xor:    cerr << "xor"; break;
      case PrimType::Xnor:   cerr << "xnor"; break;
      default: ASSERT_NOT_REACHED; break;
      }
      break;
    case NvlToken::NAME:   cerr << "NAME(" << token.name() << ")"; break;
    case NvlToken::_EOF:   cerr << "EOF"; break;
    default:                   cerr << static_cast<char>(token.type()); break;
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
    return NvlToken::LPAR;

  case ')':
    return NvlToken::RPAR;

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
  case '=':
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
