#ifndef ISCAS89TOKEN_H
#define ISCAS89TOKEN_H

/// @file Token.h
/// @brief Token のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @brief トークン
//////////////////////////////////////////////////////////////////////
enum Iscas89Token {
  kIscas89_LPAR,
  kIscas89_RPAR,
  kIscas89_COMMA,
  kIscas89_EQ,
  kIscas89_INPUT,
  kIscas89_OUTPUT,
  kIscas89_BUFF,
  kIscas89_NOT,
  kIscas89_AND,
  kIscas89_NAND,
  kIscas89_OR,
  kIscas89_NOR,
  kIscas89_XOR,
  kIscas89_XNOR,
  kIscas89_DFF,
  kIscas89_NAME,
  kIscas89_EOF,
  kIscas89_ERROR
};

END_NAMESPACE_YM_BNET

#endif // ISCAS89TOKEN_H
