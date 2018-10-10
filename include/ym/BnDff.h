#ifndef YM_BNDFF_H
#define YM_BNDFF_H

/// @file ym/BnDff.h
/// @brief BnDff のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"
#include "ym/clib.h"


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
/// - 出力のノード番号:
/// - 反転出力のノード番号: 場合によっては kBnNullId となる．
/// - 入力のノード番号:
/// - クロックのノード番号:
/// - クリア端子のノード番号: 場合によっては kBnNullId となる．
/// - プリセット端子のノード番号: 場合によっては kBnNullId となる．
/// - セル: 場合によっては nullptr となる．
/// - 各端子とセルのピン番号との対応表: セルと対応づいていなければ無効
//////////////////////////////////////////////////////////////////////
class BnDff
{
public:

  /// @brief デストラクタ
  virtual
  ~BnDff() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID 番号の取得
  /// @return ID 番号を返す．
  virtual
  int
  id() const = 0;

  /// @brief 名前を返す．
  virtual
  string
  name() const = 0;

  /// @brief データ出力のノード番号を返す．
  virtual
  int
  output() const = 0;

  /// @brief 反転データ出力のノード番号を返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  int
  xoutput() const = 0;

  /// @brief データ入力のノード番号を返す．
  virtual
  int
  input() const = 0;

  /// @brief クロックのノード番号を返す．
  virtual
  int
  clock() const = 0;

  /// @brief クリア信号のノード番号を返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  int
  clear() const = 0;

  /// @brief プリセット信号のノード番号を返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  int
  preset() const = 0;

  /// @brief セルを返す．
  ///
  /// nullptr の場合もある．
  virtual
  const ClibCell*
  cell() const = 0;

  /// @brief データ出力のピン番号を返す．
  ///
  /// cell() == nullptr の場合の値は不定
  virtual
  int
  output_pin_id() const = 0;

  /// @brief 反転データ出力のピン番号を返す．
  ///
  /// cell() == nullptr の場合の値は不定
  virtual
  int
  xoutput_pin_id() const = 0;

  /// @brief データ入力のピン番号を返す．
  ///
  /// cell() == nullptr の場合の値は不定
  virtual
  int
  input_pin_id() const = 0;

  /// @brief クロックのピン番号を返す．
  ///
  /// cell() == nullptr の場合の値は不定
  virtual
  int
  clock_pin_id() const = 0;

  /// @brief クリア信号のピン番号を返す．
  ///
  /// kBnNullId の場合もある．
  /// cell() == nullptr の場合の値は不定
  virtual
  int
  clear_pin_id() const = 0;

  /// @brief プリセット信号のピン番号を返す．
  ///
  /// kBnNullId の場合もある．
  /// cell() == nullptr の場合の値は不定
  virtual
  int
  preset_pin_id() const = 0;

};

END_NAMESPACE_YM_BNET

#endif // YM_BNDFF_H
