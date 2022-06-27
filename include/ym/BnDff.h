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
/// - 出力の論理式
/// - 次状態関数の論理式
/// - クロックの論理式
/// - クリア条件の論理式
/// - プリセット条件の論理式
/// - セル: 場合によっては nullptr となる．
///
/// 出力および次状態関数の論理式には ni 番目の変数として
/// 現状態の値を用いることができる．
/// 出力にはさらに ni + 1 番目の変数として現状態の否定
/// も用いることができる．
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
  Expr
  output_expr(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const = 0;

  /// @brief 次状態関数の論理式を返す．
  Expr
  next_state_expr() const = 0;

  /// @brief クロックの論理式を返す．
  virtual
  Expr
  clock_expr() const = 0;

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

#endif // YM_BNDFF_H
