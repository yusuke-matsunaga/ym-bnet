#ifndef BNDFFIMPL_H
#define BNDFFIMPL_H

/// @file BnDffImpl.h
/// @brief BnDffImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnDff.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnDffImpl BnDffImpl.h "BnDffImpl.h"
/// @brief BnDff の実装クラス
//////////////////////////////////////////////////////////////////////
class BnDffImpl :
  public BnDff
{
public:

  /// @brief コンストラクタ
  BnDffImpl(
    SizeType id,                      ///< [in] ID番号
    const string& name, 	      ///< [in] 名前
    const vector<SizeType>& inputs,   ///< [in] 入力のノード番号のリスト
    const vector<SizeType>& outputs,  ///< [in] 出力のノード番号のリスト
    const vector<Expr>& output_exprs, ///< [in] 出力の論理式のリスト
    const Expr& next_state_expr,      ///< [in] 次状態関数の論理式
    const Expr& clock_expr,           ///< [in] クロックの論理式
    const Expr& clear_expr,           ///< [in] クリアの論理式
    const Expr& preset_expr,          ///< [in] プリセットの論理式
    int cell_id                       ///< [in] セル番号
  ) : mId{id},
      mName{name},
      mInputs{inputs},
      mOutputs{outputs},
      mOutputExprs{output_exprs},
      mNextStateExpr{next_state_expr},
      mClockExpr{clock_expr},
      mClearExpr{clear_expr},
      mPresetExpr{preset_expr},
      mCellId{cell_id}
  {
    ASSERT_COND( mOutputs.size() == mOutputExprs.size() );
  }

  /// @brief デストラクタ
  ~BnDffImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID 番号の取得
  /// @return ID 番号を返す．
  SizeType
  id() const override;

  /// @brief 名前を返す．
  string
  name() const override;

  /// @brief 入力数を返す．
  SizeType
  input_num() const override;

  /// @brief 入力端子のノード番号を返す．
  SizeType
  input(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < input_num() )
  ) const override;

  /// @brief 出力数を返す．
  SizeType
  output_num() const override;

  /// @brief 出力のノード番号を返す．
  SizeType
  output(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const override;

  /// @brief 出力の論理式を返す．
  Expr
  output_expr(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const override;

  /// @brief 次状態関数の論理式を返す．
  Expr
  next_state_expr() const override;

  /// @brief クロックの論理式を返す．
  Expr
  clock_expr() const override;

  /// @brief クリア条件の論理式を返す．
  Expr
  clear_expr() const override;

  /// @brief プリセット条件の論理式を返す．
  Expr
  preset_expr() const override;

  /// @brief セル番号を返す．
  ///
  /// -1 の場合もある．
  int
  cell_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId;

  // 名前
  string mName;

  // 入力のノード番号
  vector<SizeType> mInputs;

  // 出力のノード番号
  vector<SizeType> mOutputs;

  // 出力の論理式
  vector<Expr> mOutputExprs;

  // 次状態関数の論理式
  Expr mNextStateExpr;

  // クロックの論理式
  Expr mClockExpr;

  // クリアの論理式
  Expr mClearExpr;

  // プリセットの論理式
  Expr mPresetExpr;

  // セル番号
  int mCellId;

};

END_NAMESPACE_YM_BNET

#endif // BNDFFIMPL_H
