
/// @file BlifDic.cc
/// @brief BlibDic の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BlifScanner.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

SizeType
hash_func(const char* str)
{
  SizeType h = 0;
  int c;
  for ( ; (c = static_cast<int>(*str)); ++ str) {
    h = h * 37 + c;
  }
  return h;
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// blif 用の予約語辞書
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifDic::BlifDic()
{
  // 予約語テーブルの生成
  struct InitData {
    const char* mKey;
    BlifToken mVal;
  };
  static InitData init_data[] = {
    {"model", BlifToken::MODEL},
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
    {"default_output_load", BlifToken::DEFAULT_OUTPUT_LOAD}
  };

  int n = sizeof(init_data) / sizeof(Cell);
  mTableSize = n;
  mHashTable = new Cell*[mTableSize];
  for ( int i = 0; i < n; ++ i ) {
    mHashTable[i] = nullptr;
  }
  mCellBlock = new Cell[n];
  for ( int i = 0; i < n; ++ i ) {
    Cell* cell = &mCellBlock[i];
    cell->mStr = init_data[i].mKey;
    cell->mToken = init_data[i].mVal;
    SizeType pos = hash_func(cell->mStr) % mTableSize;
    cell->mLink = mHashTable[pos];
    mHashTable[pos] = cell;
  }
}

// @brief デストラクタ
BlifDic::~BlifDic()
{
  delete [] mHashTable;
  delete [] mCellBlock;
}

/// @brief str に対応したトークンを返す．
BlifToken
BlifDic::get_token(const char* str)
{
  SizeType pos = hash_func(str) % mTableSize;
  for ( Cell* cell = mHashTable[pos]; cell; cell = cell->mLink ) {
    if ( strcmp(cell->mStr, str) == 0 ) {
      return cell->mToken;
    }
  }
  return BlifToken::_EOF;
}

// @brief トークンの内容を出力する．
void
BlifDic::dump_token(ostream& s,
		    BlifToken token)
{
  switch (token) {
  case BlifToken::_EOF   : s << "EOF"; break;
  case BlifToken::NL     : s << "NL"; break;
  case BlifToken::STRING : s << "STRING"; break;
  case BlifToken::EQ     : s << "EQ"; break;
  case BlifToken::MODEL  : s << "MODEL"; break;
  case BlifToken::INPUTS : s << "INPUTS"; break;
  case BlifToken::OUTPUTS: s << "OUTPUTS"; break;
  case BlifToken::CLOCK  : s << "CLOCK"; break;
  case BlifToken::END    : s << "END"; break;
  case BlifToken::NAMES  : s << "NAMES"; break;
  case BlifToken::EXDC   : s << "EXDC"; break;
  case BlifToken::LATCH  : s << "LATCH"; break;
  case BlifToken::GATE   : s << "GATE"; break;
  case BlifToken::MLATCH: s << "MLATCH"; break;
  case BlifToken::SUBCKT: s << "SUBCKT"; break;
  case BlifToken::SEARCH: s << "SEARCH"; break;
  case BlifToken::START_KISS: s << "START_KISS"; break;
  case BlifToken::I: s << "I"; break;
  case BlifToken::O: s << "O"; break;
  case BlifToken::P: s << "P"; break;
  case BlifToken::R: s << "R"; break;
  case BlifToken::END_KISS: s << "END_KISS"; break;
  case BlifToken::LATCH_ORDER: s << "LATCH_ORDER"; break;
  case BlifToken::CODE: s << "CODE"; break;
  case BlifToken::CYCLE: s << "CYCLE"; break;
  case BlifToken::CLOCK_EVENT: s << "CLOCK_EVENT"; break;
  case BlifToken::AREA: s << "AREA"; break;
  case BlifToken::DELAY: s << "DELAY"; break;
  case BlifToken::WIRE_LOAD_SLOPE: s << "WIRE_LOAD_SLOPE"; break;
  case BlifToken::WIRE : s << "WIRE"; break;
  case BlifToken::INPUT_ARRIVAL: s << "INPUT_ARRIVAL"; break;
  case BlifToken::DEFAULT_INPUT_ARRIVAL: s << "DEFAULT_INPUT_ARRIVAL"; break;
  case BlifToken::OUTPUT_REQUIRED: s << "OUTPUT_REQUIRED"; break;
  case BlifToken::DEFAULT_OUTPUT_REQUIRED: s << "DEFAULT_OUTPUT_REQUIRED"; break;
  case BlifToken::INPUT_DRIVE: s << "INPUT_DRIVE"; break;
  case BlifToken::DEFAULT_INPUT_DRIVE: s << "DEFAULT_INPUT_DRIVE"; break;
  case BlifToken::OUTPUT_LOAD: s << "OUTPUT_LOAD"; break;
  case BlifToken::DEFAULT_OUTPUT_LOAD: s << "DEFAULT_OUTPUT_LOAD"; break;
  default: break;
  }
}

// トークンの出力関数
ostream&
operator<<(ostream& s,
	   BlifToken token)
{
  BlifDic::dump_token(s, token);
  return s;
}

END_NAMESPACE_YM_BNET
