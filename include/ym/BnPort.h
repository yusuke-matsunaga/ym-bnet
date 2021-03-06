#ifndef BNPORT_H
#define BNPORT_H

/// @file BnPort.h
/// @brief BnPort のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnPort BnPort.h "ym/BnPort.h"
/// @brief Boolean Network (BnNetwork) のポートを表すクラス
/// @sa BnNetwork
///
/// このクラスは実体を持たない純粋仮想基底クラスである．
//////////////////////////////////////////////////////////////////////
class BnPort
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief デストラクタ
  virtual
  ~BnPort() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ポート番号を返す．
  virtual
  int
  id() const = 0;

  /// @brief 名前を得る．
  virtual
  string
  name() const = 0;

  /// @brief ビット数を得る．
  virtual
  int
  bit_width() const = 0;

  /// @brief pos ビット目のノード番号を得る．
  /// @return 対応するノードのノード番号を返す．
  virtual
  int
  bit(
    int pos ///< [in] ビット位置 ( 0 <= pos < bit_width() )
  ) const = 0;

};

END_NAMESPACE_YM_BNET

#endif // BNPORT_H
