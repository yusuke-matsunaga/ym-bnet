
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

// @brief 入力端子のノード番号を返す．
SizeType
BnDffImpl::input() const
{
  return mInput;
}

// @brief 出力端子のノード番号を返す．
SizeType
BnDffImpl::output() const
{
  return mOutput;
}

// @brief クロック端子のノード番号を返す．
SizeType
BnDffImpl::clock() const
{
  return mClock;
}

// @brief クリア端子のノード番号を返す．
SizeType
BnDffImpl::clear() const
{
  return mClear;
}

// @brief プリセット端子のノード番号を返す．
SizeType
BnDffImpl::preset() const
{
  return mPreset;
}

// @brief クリアとプリセットが衝突した場合の挙動
BnCPV
BnDffImpl::clear_preset_value() const
{
  return mCPV;
}

// @brief セルに割り当てられている場合のセル番号を返す．
int
BnDffImpl::cell_id() const
{
  return -1;
}

// @brief セルに割り当てられている場合の入力端子に対応するノード番号を返す．
SizeType
BnDffImpl::cell_input(
  SizeType pos
) const
{
  return BNET_NULLID;
}

// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
SizeType
BnDffImpl::cell_output(
  SizeType pos
) const
{
  return BNET_NULLID;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffCell
//////////////////////////////////////////////////////////////////////

// @brief セルに割り当てられている場合のセル番号を返す．
int
BnDffCell::cell_id() const
{
  return mCellId;
}

// @brief セルに割り当てられている場合の入力端子に対応するノード番号を返す．
SizeType
BnDffCell::cell_input(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < mInputs.size() );
  return mInputs[pos];
}

// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
SizeType
BnDffCell::cell_output(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < mOutputs.size() );
  return mOutputs[pos];
}

END_NAMESPACE_YM_BNET
