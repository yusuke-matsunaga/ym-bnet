#ifndef ISCAS89TOKEN_H
#define ISCAS89TOKEN_H

/// @file Token.h
/// @brief Token のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/iscas89_nsdef.h"


BEGIN_NAMESPACE_YM_ISCAS89

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
  MUX, // host2015 で使用．オリジナルではないはず．
  DFF,
  CONST0, // ym-bnet オリジナル
  CONST1, // ym-bnet オリジナル
  NAME,
  _EOF,
  ERROR
};

END_NAMESPACE_YM_ISCAS89

#endif // ISCAS89TOKEN_H
