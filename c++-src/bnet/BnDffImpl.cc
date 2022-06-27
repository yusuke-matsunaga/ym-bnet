
/// @file BnDffImpl.cc
/// @brief BnDffImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "BnDffImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnDffImpl
//////////////////////////////////////////////////////////////////////

// @brief ID 番号の取得
// @return ID 番号を返す．
SizeType
BnDffImpl::id() const
{
  return mId;
}

// @brief 名前を返す．
string
BnDffImpl::name() const
{
  return mName;
}

// @brief 入力数を返す．
SizeType
BnDffImpl::input_num() const
{
  return mInputs.size();
}

// @brief 入力端子のノード番号を返す．
SizeType
BnDffImpl::input(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < input_num() );
  return mInputs[pos];
}

// @brief 出力数を返す．
SizeType
BnDffImpl::output_num() const
{
  return mOutputs.size();
}

// @brief 出力のノード番号を返す．
SizeType
BnDffImpl::output(
  SizeType pos
) const
{
  ASSERT_COND ( 0 <= pos && pos < output_num() );
  return mOutputs[pos];
}

// @brief 出力の論理式を返す．
Expr
BnDffImpl::output_expr(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < output_num() );
  return mOutputExprs[pos];
}

// @brief 次状態関数の論理式を返す．
Expr
BnDffImpl::next_state_expr() const
{
  return mNextStateExpr;
}

// @brief クロックの論理式を返す．
Expr
BnDffImpl::clock_expr() const
{
  return mClockExpr;
}

// @brief クリア条件の論理式を返す．
Expr
BnDffImpl::clear_expr() const
{
  return mClearExpr;
}

// @brief プリセット条件の論理式を返す．
Expr
BnDffImpl::preset_expr() const
{
  return mPresetExpr;
}

// @brief セル番号を返す．
//
// -1 の場合もある．
int
BnDffImpl::cell_id() const
{
  return mCellId;
}

END_NAMESPACE_YM_BNET
