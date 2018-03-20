#ifndef YM_BNNODETYPE_H
#define YM_BNNODETYPE_H

/// @file ym/bnet.h
/// @brief ym-bnet の定義
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
  /// @brief 外部入力ノード
  Input,
  /// @brief 外部出力ノード
  Output,
  /// @brief 論理ノード(定数0)
  Logic_C0,
  /// @brief 論理ノード(定数1)
  Logic_C1,
  /// @brief 論理ノード(バッファ)
  Logic_BUFF,
  /// @brief 論理ノード(NOT)
  Logic_NOT,
  /// @brief 論理ノード(AND)
  Logic_AND,
  /// @brief 論理ノード(NAND)
  Logic_NAND,
  /// @brief 論理ノード(OR)
  Logic_OR,
  /// @brief 論理ノード(NOR)
  Logic_NOR,
  /// @brief 論理ノード(XOR)
  Logic_XOR,
  /// @brief 論理ノード(XNOR)
  Logic_XNOR,
  /// @brief 論理ノード(論理式:Expr)
  Logic_EXPR,
  /// @brief 論理ノード(真理値表:TvFunc)
  Logic_TV,
  /// @brief 論理ノード(不正値)
  Logic_NONE,
};

END_NAMESPACE_YM_BNET

#endif // YM_BNNODETYPE_H
