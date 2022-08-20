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

//////////////////////////////////////////////////////////////////////
/// @class BnDff BnDff.h "ym/BnDff.h"
/// @brief D-FFを表すクラス
/// @sa BnNetwork
/// @sa BnNode
///
/// このクラスは実体を持たない純粋仮想基底クラスである．
/// 以下の情報を持つ．
/// - ID番号: これは親の BnNetwork 内で唯一のもの．
/// - 名前: 場合によっては空文字列となる．
/// - 出力のノード番号
/// - データ入力のノード番号
/// - クロック/イネーブル入力のノード番号
/// - クリア端子のノード番号: 場合によっては BNET_NULLID となる．
/// - プリセット端子のノード番号: 場合によっては BNET_NULLID となる．
/// - クリアとプリセットが衝突した場合の挙動
//////////////////////////////////////////////////////////////////////
class BnDff
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @name コンストラクタ/デストラクタ
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief デストラクタ
  virtual
  ~BnDff() = default;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 全タイプ共通の外部インターフェイス
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief ID 番号の取得
  /// @return ID 番号を返す．
  virtual
  SizeType
  id() const = 0;

  /// @brief 名前を返す．
  virtual
  string
  name() const = 0;

  /// @brief タイプを返す．
  virtual
  BnDffType
  type() const = 0;

  /// @brief DFF タイプの時 true を返す．
  bool
  is_dff() const
  {
    return type() == BnDffType::Dff;
  }

  /// @brief ラッチタイプの時 true を返す．
  bool
  is_latch() const
  {
    return type() == BnDffType::Latch;
  }

  /// @brief DFF/ラッチセルタイプの時 true を返す．
  bool
  is_cell() const
  {
    return type() == BnDffType::Cell;
  }

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name DFFタイプ/ラッチタイプの外部インターフェイス
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力端子のノード番号を返す．
  virtual
  SizeType
  data_in() const = 0;

  /// @brief 出力端子のノード番号を返す．
  virtual
  SizeType
  data_out() const = 0;

  /// @brief クロック端子のノード番号を返す．
  virtual
  SizeType
  clock() const = 0;

  /// @brief クリア端子のノード番号を返す．
  virtual
  SizeType
  clear() const = 0;

  /// @brief プリセット端子のノード番号を返す．
  virtual
  SizeType
  preset() const = 0;

  /// @brief クリアとプリセットが衝突した場合の挙動
  virtual
  BnCPV
  clear_preset_value() const = 0;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name セルタイプの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief セルに割り当てられている場合のセル番号を返す．
  ///
  /// セルが割り当てられていない場合には CLIB_NULLID を返す．
  virtual
  SizeType
  cell_id() const = 0;

  /// @brief セルに割り当てられている場合の入力端子に対応するノード番号を返す．
  ///
  /// 入力端子だが実体は外部出力ノードとなる．
  virtual
  SizeType
  cell_input(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < cell.input_num() )
  ) const = 0;

  /// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
  ///
  /// 入力端子だが実体は外部入力ノードとなる．
  virtual
  SizeType
  cell_output(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < cell.output_num() )
  ) const = 0;

};

END_NAMESPACE_YM_BNET

#endif // YM_BNDFF_H
