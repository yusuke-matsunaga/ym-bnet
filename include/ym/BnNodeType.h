#ifndef YM_BNNODETYPE_H
#define YM_BNNODETYPE_H

/// @file ym/BnNodeType.h
/// @brief BnNodeType の定義
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2017, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @brief BnNode のタイプ
//////////////////////////////////////////////////////////////////////
enum class BnNodeType {
  None,   ///< 不正値
  Input,  ///< 外部入力ノード
  Output, ///< 外部出力ノード
  C0,     ///< 論理ノード(定数0)
  C1,     ///< 論理ノード(定数1)
  Buff,   ///< 論理ノード(バッファ)
  Not,    ///< 論理ノード(NOT)
  And,    ///< 論理ノード(AND)
  Nand,   ///< 論理ノード(NAND)
  Or,     ///< 論理ノード(OR)
  Nor,    ///< 論理ノード(NOR)
  Xor,    ///< 論理ノード(XOR)
  Xnor,   ///< 論理ノード(XNOR)
  Expr,   ///< 論理ノード(論理式:Expr)
  TvFunc, ///< 論理ノード(真理値表:TvFunc)
  Bdd,    ///< 論理ノード(BDD:Bdd)
};

/// @relates BnNodeType
/// @brief BnNodeType の内容をストリームに出力する．
/// @return s を返す．
inline
ostream&
operator<<(
  ostream& s,     ///< [in] 出力先のストリーム
  BnNodeType type ///< [in] ノードタイプ
)
{
  switch ( type ) {
  case BnNodeType::None:   s << "None"; break;
  case BnNodeType::Input:  s << "Input"; break;
  case BnNodeType::Output: s << "Output"; break;
  case BnNodeType::C0:     s << "C0"; break;
  case BnNodeType::C1:     s << "C1"; break;
  case BnNodeType::Buff:   s << "Buff"; break;
  case BnNodeType::Not:    s << "Not"; break;
  case BnNodeType::And:    s << "And"; break;
  case BnNodeType::Nand:   s << "Nand"; break;
  case BnNodeType::Or:     s << "Or"; break;
  case BnNodeType::Nor:    s << "Nor"; break;
  case BnNodeType::Xor:    s << "Xor"; break;
  case BnNodeType::Xnor:   s << "Xnor"; break;
  case BnNodeType::Expr:   s << "Expr"; break;
  case BnNodeType::TvFunc: s << "TvFunc"; break;
  case BnNodeType::Bdd:    s << "Bdd"; break;
  defult: ASSERT_NOT_REACHED; break;
  }
  return s;
}

// cython 用の変換関数
extern
int __bnnodetype_to_int(BnNodeType type);

extern
BnNodeType __int_to_bnnodetype(int val);

END_NAMESPACE_YM_BNET

#endif // YM_BNNODETYPE_H
