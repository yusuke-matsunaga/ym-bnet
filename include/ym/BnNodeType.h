#ifndef YM_BNNODETYPE_H
#define YM_BNNODETYPE_H

/// @file ym/BnNodeType.h
/// @brief BnNodeType の定義
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @brief BnNode のタイプ
//////////////////////////////////////////////////////////////////////
enum class BnNodeType {
  /// @brief 不正値
  None,
  /// @brief 外部入力ノード
  Input,
  /// @brief 外部出力ノード
  Output,
  /// @brief 論理ノード(定数0)
  C0,
  /// @brief 論理ノード(定数1)
  C1,
  /// @brief 論理ノード(バッファ)
  Buff,
  /// @brief 論理ノード(NOT)
  Not,
  /// @brief 論理ノード(AND)
  And,
  /// @brief 論理ノード(NAND)
  Nand,
  /// @brief 論理ノード(OR)
  Or,
  /// @brief 論理ノード(NOR)
  Nor,
  /// @brief 論理ノード(XOR)
  Xor,
  /// @brief 論理ノード(XNOR)
  Xnor,
  /// @brief 論理ノード(論理式:Expr)
  Expr,
  /// @brief 論理ノード(真理値表:TvFunc)
  TvFunc,
};

/// @relates BnNodeType
/// @brief BnNodeType の内容をストリームに出力する．
ostream&
operator<<(ostream& s,
	   BnNodeType type);

// cython 用の変換関数
extern
int __bnnodetype_to_int(BnNodeType type);

extern
BnNodeType __int_to_bnnodetype(int val);

END_NAMESPACE_YM_BNET

#endif // YM_BNNODETYPE_H
