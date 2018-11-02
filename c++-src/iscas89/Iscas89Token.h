#ifndef ISCAS89TOKEN_H
#define ISCAS89TOKEN_H

/// @file Token.h
/// @brief Token のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @brief トークン
//////////////////////////////////////////////////////////////////////
enum class Iscas89Token {
  LPAR,
  RPAR,
  COMMA,
  EQ,
  INPUT,
  OUTPUT,
  BUFF,
  NOT,
  AND,
  NAND,
  OR,
  NOR,
  XOR,
  XNOR,
  MUX,
  DFF,
  NAME,
  _EOF,
  ERROR
};

END_NAMESPACE_YM_BNET

#endif // ISCAS89TOKEN_H
