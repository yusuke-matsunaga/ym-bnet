#ifndef YM_BNDFF_H
#define YM_BNDFF_H

/// @file ym/BnDff.h
/// @brief BnDff のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018, 2021 Yusuke Matsunaga
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
/// - 反転出力のノード番号: 場合によっては BNET_NULLID となる．
/// - 入力のノード番号:
/// - クロックのノード番号:
/// - クリア端子のノード番号: 場合によっては BNET_NULLID となる．
/// - プリセット端子のノード番号: 場合によっては BNET_NULLID となる．
/// - セル: 場合によっては nullptr となる．
/// - 各端子とセルのピン番号との対応表: セルと対応づいていなければ無効
//////////////////////////////////////////////////////////////////////
class BnDff
{
public:

  /// @brief デストラクタ
  virtual
  ~BnDff() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
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

  /// @brief データ出力のノード番号を返す．
  virtual
  SizeType
  output() const = 0;

  /// @brief 反転データ出力のノード番号を返す．
  ///
  /// BNET_NULLID の場合もある．
  virtual
  SizeType
  xoutput() const = 0;

  /// @brief データ入力のノード番号を返す．
  virtual
  SizeType
  input() const = 0;

  /// @brief クロックのノード番号を返す．
  virtual
  SizeType
  clock() const = 0;

  /// @brief クリア信号のノード番号を返す．
  ///
  /// BNET_NULLID の場合もある．
  virtual
  SizeType
  clear() const = 0;

  /// @brief プリセット信号のノード番号を返す．
  ///
  /// BNET_NULLID の場合もある．
  virtual
  SizeType
  preset() const = 0;

  /// @brief セル番号を返す．
  ///
  /// -1 の場合もある．
  virtual
  int
  cell_id() const = 0;

};

END_NAMESPACE_YM_BNET

#endif // YM_BNDFF_H
