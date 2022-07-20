
/// @file BlifScanner.cc
/// @brief BlibScanner の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2019, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "BlifScanner.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

// read_token() の動作をデバッグするときに true にする．
const bool debug_read_token = false;

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// blif 用の字句解析器
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifScanner::BlifScanner(
  istream& s,
  const FileInfo& file_info
) : Scanner(s, file_info),
    mDic{ {"model", BlifToken::MODEL},
	  {"inputs", BlifToken::INPUTS},
	  {"outputs", BlifToken::OUTPUTS},
	  {"clock", BlifToken::CLOCK},
	  {"end", BlifToken::END},
	  {"names", BlifToken::NAMES},
	  {"exdc", BlifToken::EXDC},
	  {"latch", BlifToken::LATCH},
	  {"gate", BlifToken::GATE},
	  {"mlatch", BlifToken::MLATCH},
	  {"subckt", BlifToken::SUBCKT},
	  {"search", BlifToken::SEARCH},
	  {"start_kiss", BlifToken::START_KISS},
	  {"i", BlifToken::I},
	  {"o", BlifToken::O},
	  {"p", BlifToken::P},
	  {"r", BlifToken::R},
	  {"end_kiss", BlifToken::END_KISS},
	  {"latch_order", BlifToken::LATCH_ORDER},
	  {"code", BlifToken::CODE},
	  {"cycle", BlifToken::CYCLE},
	  {"clock_event", BlifToken::CLOCK_EVENT},
	  {"area", BlifToken::AREA},
	  {"delay", BlifToken::DELAY},
	  {"wire_load_slope", BlifToken::WIRE_LOAD_SLOPE},
	  {"wire", BlifToken::WIRE},
	  {"input_arrival", BlifToken::INPUT_ARRIVAL},
	  {"default_input_arrival", BlifToken::DEFAULT_INPUT_ARRIVAL},
	  {"output_required", BlifToken::OUTPUT_REQUIRED},
	  {"default_output_required", BlifToken::DEFAULT_OUTPUT_REQUIRED},
	  {"input_drive", BlifToken::INPUT_DRIVE},
	  {"default_input_drive", BlifToken::DEFAULT_INPUT_DRIVE},
	  {"output_load", BlifToken::OUTPUT_LOAD},
	  {"default_output_load", BlifToken::DEFAULT_OUTPUT_LOAD} }
{
}

// @brief トークンを一つ読み出す．
BlifToken
BlifScanner::read_token(
  FileRegion& loc
)
{
  auto token = scan();
  loc = cur_region();

  if ( debug_read_token ) {
    cerr << "read_token()" << " --> "
	 << loc << ": "
	 << token;
    if ( token == BlifToken::STRING ) {
      cerr << "(\'" << cur_string() << "\')";
    }
    cerr<< endl;
  }

  return token;
}

// @brief read_token() の下請け関数
// @return トークンを返す．
BlifToken
BlifScanner::scan()
{
  mCurString = "";
  bool StartWithDot = false;
  int c;

  // 状態遷移を goto 文で表現したもの
  // 効率はよい．

 ST_INIT:
  c = get();
  set_first_loc();

  switch ( c ) {
  case EOF:
    return BlifToken::_EOF;

  case ' ':
  case '\t':
    // ホワイトスペースは読み飛ばす．
    goto ST_INIT;

  case '\n':
    return BlifToken::NL;

  case '=':
    return BlifToken::EQ;

  case '.':
    StartWithDot = true;
    goto ST_STR;

  case '#':
    goto ST_SHARP;

  case '/':
    if ( peek() == '*' ) {
      accept();
      // ここまでで "/*" を読んでいる．
      goto ST_CM1;
    }
    mCurString.put_char('/');
    goto ST_STR;

  case '\\':
    goto ST_ESC;

  default:
    mCurString.put_char(c);
    goto ST_STR;
  }

 ST_SHARP:
  c = get();
  if ( c == '\n' ) {
    return BlifToken::NL;
  }
  if ( c == EOF ) {
    return BlifToken::_EOF;
  }
  // 改行までは読み飛ばす．
  goto ST_SHARP;

 ST_CM1:
  c = get();
  if ( c == '*' ) {
    goto ST_CM2;
  }
  if ( c == EOF ) {
    return BlifToken::_EOF;
  }
  // '*' までは読み飛ばす．
  goto ST_CM1;

 ST_CM2:
  c = get();
  if ( c == '/' ) {
    // コメントは空白扱いにする．
    goto ST_INIT;
  }
  if ( c == EOF ) {
    return BlifToken::_EOF;
  }
  goto ST_CM1;

 ST_ESC:
  c = get();
  if ( c == '\n' ) {
    // エスケープされた改行は空白扱いにする．
    goto ST_INIT;
  }
  if ( c == EOF ) {
    // これはおかしいけど無視する．
    return check_word(StartWithDot);
  }
  // それ以外は普通の文字として扱う．
  mCurString.put_char(c);
  goto ST_STR;

 ST_STR:
  c = peek();
  switch ( c ) {
  case ' ':
  case '\t':
  case '\n':
  case '=':
  case '#':
  case '\\':
  case EOF:
    // 文字列の終わり
    return check_word(StartWithDot);

  default:
    accept();
    mCurString.put_char(c);
    goto ST_STR;
  }
}

// @brief 予約後の検査をする．
BlifToken
BlifScanner::check_word(
  bool start_with_dot
)
{
  if ( start_with_dot ) {
    // 予約後の検索
    auto p = mDic.find(string{cur_string()});
    if ( p != mDic.end() ) {
      return p->second;
    }
  }
  return BlifToken::STRING;
}

END_NAMESPACE_YM_BNET
