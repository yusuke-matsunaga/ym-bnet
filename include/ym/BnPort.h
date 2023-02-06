#ifndef BNPORT_H
#define BNPORT_H

/// @file BnPort.h
/// @brief BnPort のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

class BnNetworkImpl;
class BnPortImpl;

//////////////////////////////////////////////////////////////////////
/// @class BnPort BnPort.h "ym/BnPort.h"
/// @brief Boolean Network (BnNetwork) のポートを表すクラス
/// @sa BnNetwork
///
/// ポートは1つもしくは複数の入力ノード/出力ノードをまとめたもの．
/// ネットワーク内で唯一の名前を持つ．
///
/// このクラスは実体を持たない純粋仮想基底クラスである．
//////////////////////////////////////////////////////////////////////
class BnPort
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief 空のコンストラクタ
  BnPort() = default;

  /// @brief コンストラクタ
  BnPort(
    const BnNetworkImpl* network, ///< [in] ネットワーク
    SizeType id                   ///< [in] ID番号
  ) : mNetwork{network},
      mId{id}
  {
  }

  /// @brief デストラクタ
  ~BnPort() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 適正な値を持っている時 true を返す．
  bool
  is_valid() const
  {
    return mId != -1;
  }

  /// @brief 不正な値を持っている時 true を返す．
  bool
  is_invalid() const
  {
    return !is_valid();
  }

  /// @brief ポート番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 名前を得る．
  string
  name() const;

  /// @brief ビット数を得る．
  SizeType
  bit_width() const;

  /// @brief pos ビット目のノード番号を得る．
  /// @return 対応するノードのノード番号を返す．
  BnNode
  bit(
    SizeType pos ///< [in] ビット位置 ( 0 <= pos < bit_width() )
  ) const;


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 等価比較演算
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 等価比較演算
  bool
  operator==(
    const BnPort& right
  ) const
  {
    return mNetwork == right.mNetwork && mId == right.mId;
  }

  /// @brief 非等価比較演算
  bool
  operator!=(
    const BnPort& right
  ) const
  {
    return !operator==(right);
  }

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  // 内部の実装に関する走査
  //////////////////////////////////////////////////////////////////////

  /// @brief 実装クラスを取り出す．
  BnPortImpl*
  _impl();

  /// @brief ネットワークを取り出す．
  const BnNetworkImpl*
  _network()
  {
    return mNetwork;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  const BnNetworkImpl* mNetwork{nullptr};

  // ID番号
  SizeType mId{BNET_NULLID};

};

END_NAMESPACE_YM_BNET

#endif // BNPORT_H
