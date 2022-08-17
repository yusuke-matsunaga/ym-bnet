
/// @file BnLatchImpl.cc
/// @brief BnLatchImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021, 2022 Yusuke Matsunaga
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

// @brief 入力端子のノード番号を返す．
SizeType
BnLatchImpl::input() const
{
  return mInput;
}

// @brief 出力端子のノード番号を返す．
SizeType
BnLatchImpl::output() const
{
  return mOutput;
}

// @brief ラッチイネーブル端子のノード番号を返す．
SizeType
BnLatchImpl::enable() const
{
  return mEnable;
}

// @brief クリア端子のノード番号を返す．
SizeType
BnLatchImpl::clear() const
{
  return mClear;
}

// @brief プリセット端子のノード番号を返す．
SizeType
BnLatchImpl::preset() const
{
  return mPreset;
}

// @brief クリアとプリセットが衝突した場合の挙動
BnCPV
BnLatchImpl::clear_preset_value() const
{
  return mCPV;
}

// @brief セルに割り当てられている場合のセル番号を返す．
int
BnLatchImpl::cell_id() const
{
  return -1;
}

// @brief セルに割り当てられている場合の入力端子に対応するノード番号を返す．
SizeType
BnLatchImpl::cell_input(
  SizeType pos
) const
{
  return BNET_NULLID;
}

// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
SizeType
BnLatchImpl::cell_output(
  SizeType pos
) const
{
  return BNET_NULLID;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchCell
//////////////////////////////////////////////////////////////////////

// @brief セルに割り当てられている場合のセル番号を返す．
int
BnLatchCell::cell_id() const
{
  return mCellId;
}

// @brief セルに割り当てられている場合の入力端子に対応するノード番号を返す．
SizeType
BnLatchCell::cell_input(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < mInputs.size() );
  return mInputs[pos];
}

// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
SizeType
BnLatchCell::cell_output(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < mOutputs.size() );
  return mOutputs[pos];
}


END_NAMESPACE_YM_BNET
