﻿#ifndef YM_BNLATCH_H
#define YM_BNLATCH_H

/// @file ym/BnLatch.h
/// @brief BnLatch のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnLatch BnLatch.h "ym/BnLatch.h"
/// @brief ラッチを表すクラス
/// @sa BnNetwork
/// @sa BnNode
///
/// このクラスは実体を持たない純粋仮想基底クラスである．
//////////////////////////////////////////////////////////////////////
class BnLatch
{
public:

  /// @brief デストラクタ
  virtual
  ~BnLatch() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号の取得
  /// @return ID番号を返す．
  virtual
  ymuint
  id() const = 0;

  /// @brief 名前を返す．
  virtual
  string
  name() const = 0;

  /// @brief データ出力のノード番号を返す．
  virtual
  ymuint
  output() const = 0;

  /// @brief データ入力のノード番号を返す．
  virtual
  ymuint
  input() const = 0;

  /// @brief ラッチイネーブルのノード番号を返す．
  virtual
  ymuint
  enable() const = 0;

  /// @brief クリア信号のノード番号を返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  ymuint
  clear() const = 0;

  /// @brief プリセット信号のノード番号を返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  ymuint
  preset() const = 0;

};

END_NAMESPACE_YM_BNET

#endif // YM_BNLATCH_H
