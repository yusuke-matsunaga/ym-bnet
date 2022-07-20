#ifndef BLIFTOKEN_H
#define BLIFTOKEN_H

/// @file BlifToken.h
/// @brief BlibToken のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2019, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

/// @brief blif 形式のトークン
enum class BlifToken {
  _EOF,
  NL,
  STRING,
  EQ,
  MODEL,
  INPUTS,
  OUTPUTS,
  CLOCK,
  END,
  NAMES,
  EXDC,
  LATCH,
  GATE,
  MLATCH,
  SUBCKT,
  SEARCH,
  START_KISS,
  I,
  O,
  P,
  R,
  END_KISS,
  LATCH_ORDER,
  CODE,
  CYCLE,
  CLOCK_EVENT,
  AREA,
  DELAY,
  WIRE_LOAD_SLOPE,
  WIRE,
  INPUT_ARRIVAL,
  DEFAULT_INPUT_ARRIVAL,
  OUTPUT_REQUIRED,
  DEFAULT_OUTPUT_REQUIRED,
  INPUT_DRIVE,
  DEFAULT_INPUT_DRIVE,
  OUTPUT_LOAD,
  DEFAULT_OUTPUT_LOAD
};

/// @relates Token
/// @brief トークンを出力する．
inline
ostream&
operator<<(
  ostream& s,     ///< [in] 出力先のストリーム
  BlifToken token ///< [in] トークン
)
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

  return s;
}

END_NAMESPACE_YM_BNET

#endif // BLIFTOKEN_H
