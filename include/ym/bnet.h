﻿#ifndef YM_BNET_H
#define YM_BNET_H

/// @file ym/bnet.h
/// @brief ym-bnet の定義
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2017, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

/// @defgroup ym-bnet Boolean Network を扱うモジュール
///
///
///

#include "ym_config.h"


/// @brief bnet 用の名前空間の開始
#define BEGIN_NAMESPACE_YM_BNET \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsBnet)

/// @brief bnet 用の名前空間の終了
#define END_NAMESPACE_YM_BNET \
END_NAMESPACE(nsBnet) \
END_NAMESPACE_YM


BEGIN_NAMESPACE_YM_BNET

// クラスの前方宣言
class BnNetwork;
class BnPort;
class BnPortList;
class BnDff;
class BnDffList;
class BnNode;
class BnNodeMap;
class BnNodeList;
class BnModifier;

END_NAMESPACE_YM_BNET

BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @brief ポートの向きを表す列挙型(サイズは8ビット)
//////////////////////////////////////////////////////////////////////
enum class BnDir : std::uint8_t {
  INPUT  = 0, ///< 入力
  OUTPUT = 1  ///< 出力
};

/// @relates BnDir
/// @brief BnDir の内容をストリームに出力する．
/// @return s を返す．
inline
ostream&
operator<<(
  ostream& s, ///< [in] 出力先のストリーム
  BnDir dir   ///< [in] 向き
)
{
  switch ( dir ) {
  case BnDir::INPUT:  s << "INPUT"; break;
  case BnDir::OUTPUT: s << "OUTPUT"; break;
  default:            s << "---"; break;
  }
  return s;
}


//////////////////////////////////////////////////////////////////////
/// @brief BnNode のタイプ
//////////////////////////////////////////////////////////////////////
enum class BnNodeType : std::uint8_t {
  None,   ///< 不正値
  Input,  ///< 外部入力ノード
  Output, ///< 外部出力ノード
  Prim,   ///< 論理ノード(組み込みタイプ)
  Expr,   ///< 論理ノード(論理式:Expr)
  TvFunc, ///< 論理ノード(真理値表:TvFunc)
  Bdd,    ///< 論理ノード(BDD:Bdd)
  Cell,   ///< 論理ノード(セル)
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
  case BnNodeType::Prim:   s << "Prim"; break;
  case BnNodeType::Expr:   s << "Expr"; break;
  case BnNodeType::TvFunc: s << "TvFunc"; break;
  case BnNodeType::Bdd:    s << "Bdd"; break;
  case BnNodeType::Cell:   s << "Cell"; break;
  defult: ASSERT_NOT_REACHED; break;
  }
  return s;
}


//////////////////////////////////////////////////////////////////////
/// @brief BnDff のタイプ
//////////////////////////////////////////////////////////////////////
enum class BnDffType : std::uint8_t {
  None   = 0, ///< 不正値
  Dff    = 1, ///< D-FF
  Latch  = 2, ///< ラッチ
  Cell   = 3, ///< セル
};


//////////////////////////////////////////////////////////////////////
/// @brief D-FF/Latch でクリアとプリセットが同時にアサートされた時の挙動
//////////////////////////////////////////////////////////////////////
enum class BnCPV : std::uint8_t {
  L = 0, ///< 0
  H = 1, ///< 1
  N = 2, ///< 不変
  T = 3, ///< 反転
  X = 4  ///< 不定値
};


//////////////////////////////////////////////////////////////////////
/// @brief 不正なノード番号を表す定数
//////////////////////////////////////////////////////////////////////
const SizeType BNET_NULLID = 0;

using nsBnet::BnNetwork;
using nsBnet::BnPort;
using nsBnet::BnPortList;
using nsBnet::BnDff;
using nsBnet::BnDffList;
using nsBnet::BnNode;
using nsBnet::BnNodeMap;
using nsBnet::BnNodeList;
using nsBnet::BnModifier;

END_NAMESPACE_YM

#endif // YM_BNET_H
