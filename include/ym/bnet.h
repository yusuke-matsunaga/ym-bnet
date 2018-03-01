#ifndef YM_BNET_H
#define YM_BNET_H

/// @file ym/bnet.h
/// @brief ym-bnet の定義
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2017 Yusuke Matsunaga
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
class BlifCover;
class BlifHandler;
class BlifParser;

class BnBlifReader;
class BnIscas89Reader;

class BnNetwork;
class BnPort;
class BnDff;
class BnLatch;
class BnNode;

class Iscas89Handler;
class Iscas89Parser;

END_NAMESPACE_YM_BNET

BEGIN_NAMESPACE_YM

/// @brief 不正なノード番号を表す定数
const int kBnNullId = -1;


//////////////////////////////////////////////////////////////////////
/// @brief BnNode のタイプ
//////////////////////////////////////////////////////////////////////
enum BnNodeType {
  /// @brief 外部入力ノード
  kBnInput,
  /// @brief 外部出力ノード
  kBnOutput,
  /// @brief 論理ノード(定数0)
  kBnLogic_C0,
  /// @brief 論理ノード(定数1)
  kBnLogic_C1,
  /// @brief 論理ノード(バッファ)
  kBnLogic_BUFF,
  /// @brief 論理ノード(NOT)
  kBnLogic_NOT,
  /// @brief 論理ノード(AND)
  kBnLogic_AND,
  /// @brief 論理ノード(NAND)
  kBnLogic_NAND,
  /// @brief 論理ノード(OR)
  kBnLogic_OR,
  /// @brief 論理ノード(NOR)
  kBnLogic_NOR,
  /// @brief 論理ノード(XOR)
  kBnLogic_XOR,
  /// @brief 論理ノード(XNOR)
  kBnLogic_XNOR,
  /// @brief 論理ノード(論理式:Expr)
  kBnLogic_EXPR,
  /// @brief 論理ノード(真理値表:TvFunc)
  kBnLogic_TV,
  /// @brief 論理ノード(不正値)
  kBnLogic_NONE,
};


using nsBnet::BlifCover;
using nsBnet::BlifHandler;
using nsBnet::BlifParser;

using nsBnet::BnBlifReader;
using nsBnet::BnIscas89Reader;
using nsBnet::BnNetwork;
using nsBnet::BnPort;
using nsBnet::BnDff;
using nsBnet::BnLatch;
using nsBnet::BnNode;

using nsBnet::Iscas89Parser;
using nsBnet::Iscas89Handler;

END_NAMESPACE_YM

#endif // YM_BNET_H
