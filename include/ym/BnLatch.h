#ifndef YM_BNLATCH_H
#define YM_BNLATCH_H

/// @file ym/BnLatch.h
/// @brief BnLatch のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/clib.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnLatch BnLatch.h "ym/BnLatch.h"
/// @brief ラッチを表すクラス
/// @sa BnNetwork
/// @sa BnNode
///
/// このクラスは実体を持たない純粋仮想基底クラスである．
/// 以下の情報を持つ．
/// - ID番号: これは親の BnNetwork 内で唯一のもの．
/// - 名前: 場合によっては空文字列となる．
/// - 出力のノード番号
/// - 反転出力のノード番号: 場合によっては BNET_NULLID となる．
/// - 入力のノード番号
/// - イネーブル端子のノード番号
/// - クリア端子のノード番号: 場合によっては BNET_NULLID となる．
/// - プリセット端子のノード番号: 場合によっては BNET_NULLID となる．
/// - セル: 場合によっては nullptr となる．
/// - 各端子とセルのピン番号との対応表: セルと対応づいていなければ無効
//////////////////////////////////////////////////////////////////////
class BnLatch
{
public:

  /// @brief デストラクタ
  virtual
  ~BnLatch() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号の取得
  /// @return ID番号を返す．
  virtual
  SizeType
  id() const = 0;

  /// @brief 名前を返す．
  virtual
  string
  name() const = 0;

  /// @brief 入力数を返す．
  virtual
  SizeType
  input_num() const = 0;

  /// @brief 入力端子のノード番号を返す．
  virtual
  SizeType
  input(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < input_num() )
  ) const = 0;

  /// @brief 出力数を返す．
  virtual
  SizeType
  output_num() const = 0;

  /// @brief 出力のノード番号を返す．
  virtual
  SizeType
  output(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const = 0;

  /// @brief 出力の論理式を返す．
  virtual
  Expr
  output_expr(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const = 0;

  /// @brief データ入力の論理式を返す．
  virtual
  Expr
  data_in_expr() const = 0;

  /// @brief ラッチイネーブルの論理式を返す．
  virtual
  Expr
  enable_expr() const = 0;

  /// @brief クリア条件の論理式を返す．
  virtual
  Expr
  clear_expr() const = 0;

  /// @brief プリセット条件の論理式を返す．
  virtual
  Expr
  preset_expr() const = 0;

  /// @brief セル番号を返す．
  ///
  /// -1 の場合もある．
  virtual
  int
  cell_id() const = 0;

};

END_NAMESPACE_YM_BNET

#endif // YM_BNLATCH_H
