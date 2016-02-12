#ifndef TOKEN_H
#define TOKEN_H

/// @file Token.h
/// @brief Token のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @brief トークン
//////////////////////////////////////////////////////////////////////
enum Token {
  kToken_LPAR,
  kToken_RPAR,
  kToken_COMMA,
  kToken_EQ,
  kToken_INPUT,
  kToken_OUTPUT,
  kToken_BUFF,
  kToken_NOT,
  kToken_AND,
  kToken_NAND,
  kToken_OR,
  kToken_NOR,
  kToken_XOR,
  kToken_XNOR,
  kToken_DFF,
  kToken_NAME,
  kToken_EOF,
  kToken_ERROR
};

END_NAMESPACE_YM_BNET

#endif // TOKEN_H
