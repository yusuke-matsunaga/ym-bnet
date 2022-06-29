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
/// - 入力のノード番号
/// - イネーブル端子のノード番号
/// - クリア端子のノード番号: 場合によっては BNET_NULLID となる．
/// - プリセット端子のノード番号: 場合によっては BNET_NULLID となる．
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

  /// @brief 入力端子のノード番号を返す．
  virtual
  SizeType
  input() const = 0;

  /// @brief 出力端子のノード番号を返す．
  virtual
  SizeType
  output() const = 0;

  /// @brief ラッチイネーブル端子のノード番号を返す．
  virtual
  SizeType
  enable() const = 0;

  /// @brief クリア端子のノード番号を返す．
  virtual
  SizeType
  clear() const = 0;

  /// @brief プリセット端子のノード番号を返す．
  virtual
  SizeType
  preset() const = 0;

  /// @brief セルに割り当てられている場合のセル番号を返す．
  ///
  /// セルが割り当てられていない場合には -1 を返す．
  virtual
  int
  cell_id() const = 0;

  /// @brief セルに割り当てられている場合の入力端子に対応するノード番号を返す．
  virtual
  SizeType
  cell_input(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < cell.input_num() )
  ) const = 0;

  /// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
  virtual
  SizeType
  cell_output(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < cell.output_num() )
  ) const = 0;

};

END_NAMESPACE_YM_BNET

#endif // YM_BNLATCH_H
