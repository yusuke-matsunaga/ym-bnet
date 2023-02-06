#ifndef YM_BNDFF_H
#define YM_BNDFF_H

/// @file ym/BnDff.h
/// @brief BnDff のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

class BnNetworkImpl;
class BnDffImpl;

//////////////////////////////////////////////////////////////////////
/// @class BnDff BnDff.h "ym/BnDff.h"
/// @brief D-FFを表すクラス
/// @sa BnNetwork
/// @sa BnNode
///
/// このクラスは実体を持たない純粋仮想基底クラスである．
///
/// 大きく分けて以下の3種類のタイプがある．
/// - BnDffType::Dff: クリア・プリセット（オプション）付きのDフリップフロップ
/// - BnDffType::Latch: クリア・プリセット（オプション）付きのラッチ
/// - BnDffType::Cell: DFF/ラッチタイプのセル
///
/// Dff/Latch タイプの場合，予め定義された入出力端子を持つ．
/// Cell タイプの場合，そのセルに応じた入出力端子を持つ．
///
/// 具体的には以下の情報を持つ．
/// - ID番号: これは親の BnNetwork 内で唯一のもの．
/// - 名前: 場合によっては空文字列となる．
/// - データ出力のノード番号
/// - データ入力のノード番号
/// - クロック/イネーブル入力のノード番号
/// - クリア端子のノード番号: 場合によっては BNET_NULLID となる．
/// - プリセット端子のノード番号: 場合によっては BNET_NULLID となる．
/// - クリアとプリセットが衝突した場合の挙動
/// - セルタイプの場合のセル番号
/// - セルタイプの場合の入力端子のノード番号
/// - セルタイプの場合の出力端子のノード番号
//////////////////////////////////////////////////////////////////////
class BnDff
{
  friend class BnNetworkImpl;

public:
  //////////////////////////////////////////////////////////////////////
  /// @name コンストラクタ/デストラクタ
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  BnDff(
    const BnNetworkImpl* network, ///< [in] ネットワーク
    SizeType id                   ///< [in] DFF番号
  ) : mNetwork{network},
      mId{id}
  {
  }

  /// @brief デストラクタ
  ~BnDff() = default;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 全タイプ共通の外部インターフェイス
  /// @{
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

  /// @brief ID 番号の取得
  /// @return ID 番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 名前を返す．
  string
  name() const;

  /// @brief タイプを返す．
  BnDffType
  type() const;

  /// @brief DFF タイプの時 true を返す．
  bool
  is_dff() const;

  /// @brief ラッチタイプの時 true を返す．
  bool
  is_latch() const;

  /// @brief DFF/ラッチセルタイプの時 true を返す．
  bool
  is_cell() const;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name DFFタイプ/ラッチタイプの外部インターフェイス
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力端子のノードを返す．
  BnNode
  data_in() const;

  /// @brief 出力端子のノードを返す．
  BnNode
  data_out() const;

  /// @brief クロック端子のノードを返す．
  BnNode
  clock() const;

  /// @brief クリア端子のノードを返す．
  BnNode
  clear() const;

  /// @brief プリセット端子のノードを返す．
  BnNode
  preset() const;

  /// @brief クリアとプリセットが衝突した場合の挙動
  BnCPV
  clear_preset_value() const;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name セルタイプの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief セルに割り当てられている場合のセル番号を返す．
  ///
  /// セルが割り当てられていない場合には CLIB_NULLID を返す．
  SizeType
  cell_id() const;

  /// @brief セルに割り当てられている場合の入力端子数を返す．
  SizeType
  cell_input_num() const;

  /// @brief セルに割り当てられている場合の入力端子に対応するノードを返す．
  ///
  /// 入力端子だが実体は外部出力ノードとなる．
  BnNode
  cell_input(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < cell_input_num() )
  ) const;

  /// @brief セルに割り当てられている場合の出力端子数を返す．
  SizeType
  cell_output_num() const;

  /// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
  ///
  /// 入力端子だが実体は外部入力ノードとなる．
  BnNode
  cell_output(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < cell_output_num() )
  ) const;


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 等価比較演算
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 等価比較演算
  bool
  operator==(
    const BnDff& right
  ) const
  {
    return mNetwork == right.mNetwork && mId == right.mId;
  }

  /// @brief 非等価比較演算
  bool
  operator!=(
    const BnDff& right
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
  BnDffImpl*
  _impl();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  const BnNetworkImpl* mNetwork;

  // ID番号
  SizeType mId;

};

END_NAMESPACE_YM_BNET

#endif // YM_BNDFF_H
