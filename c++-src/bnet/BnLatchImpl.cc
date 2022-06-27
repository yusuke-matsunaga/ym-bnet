
/// @file BnLatchImpl.cc
/// @brief BnLatchImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BnLatchImpl.h"
#include "ym/ClibCell.h"
#include "ym/ClibCellGroup.h"
#include "ym/ClibLatchInfo.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnLatchImpl
//////////////////////////////////////////////////////////////////////

// @brief ID 番号の取得
// @return ID 番号を返す．
SizeType
BnLatchImpl::id() const
{
  return mId;
}

// @brief 名前を返す．
string
BnLatchImpl::name() const
{
  return mName;
}

// @brief 入力数を返す．
SizeType
BnLatchImpl::input_num() const
{
  return mInputs.size();
}

// @brief 入力端子のノード番号を返す．
SizeType
BnLatchImpl::input(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < input_num() );
  return mInputs[pos];
}

// @brief 出力数を返す．
SizeType
BnLatchImpl::output_num() const
{
  return mOutputs.size();
}

// @brief 出力のノード番号を返す．
SizeType
BnLatchImpl::output(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < output_num() );
  return mOutputs[pos];
}

// @brief 出力の論理式を返す．
Expr
BnLatchImpl::output_expr(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < output_num() );
  return mOutputExprs[pos];
}

// @brief データ入力の論理式を返す．
Expr
BnLatchImpl::data_in_expr() const
{
  return mDataInExpr;
}

// @brief ラッチイネーブルの論理式を返す．
Expr
BnLatchImpl::enable_expr() const
{
  return mEnableExpr;
}

// @brief クリア条件の論理式を返す．
Expr
BnLatchImpl::clear_expr() const
{
  return mClearExpr;
}

// @brief プリセット条件の論理式を返す．
Expr
BnLatchImpl::preset_expr() const
{
  return mPresetExpr;
}

// @brief セル番号を返す．
//
// -1 の場合もある．
int
BnLatchImpl::cell_id() const
{
  return mCellId;
}

END_NAMESPACE_YM_BNET
