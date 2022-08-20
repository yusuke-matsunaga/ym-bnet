
/// @file BnDffImpl.cc
/// @brief BnDffImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "BnDffImpl.h"
#include "ym/clib.h" // for CLIB_NULLID


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnDffBase
//////////////////////////////////////////////////////////////////////

// @brief ID 番号の取得
// @return ID 番号を返す．
SizeType
BnDffBase::id() const
{
  return mId;
}

// @brief 名前を返す．
string
BnDffBase::name() const
{
  return mName;
}

// @brief 入力端子のノード番号を返す．
SizeType
BnDffBase::data_in() const
{
  return BNET_NULLID;
}

// @brief 出力端子のノード番号を返す．
SizeType
BnDffBase::data_out() const
{
  return BNET_NULLID;
}

// @brief クロック端子のノード番号を返す．
SizeType
BnDffBase::clock() const
{
  return BNET_NULLID;
}

// @brief クリア端子のノード番号を返す．
SizeType
BnDffBase::clear() const
{
  return BNET_NULLID;
}

// @brief プリセット端子のノード番号を返す．
SizeType
BnDffBase::preset() const
{
  return BNET_NULLID;
}

// @brief クリアとプリセットが衝突した場合の挙動
BnCPV
BnDffBase::clear_preset_value() const
{
  return BnCPV::L;
}

// @brief セルに割り当てられている場合のセル番号を返す．
SizeType
BnDffBase::cell_id() const
{
  return CLIB_NULLID;
}

// @brief セルに割り当てられている場合の入力端子に対応するノード番号を返す．
SizeType
BnDffBase::cell_input(
  SizeType pos
) const
{
  return BNET_NULLID;
}

// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
SizeType
BnDffBase::cell_output(
  SizeType pos
) const
{
  return BNET_NULLID;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDLBase
//////////////////////////////////////////////////////////////////////

// @brief 入力端子のノード番号を返す．
SizeType
BnDLBase::data_in() const
{
  return mInput;
}

// @brief 出力端子のノード番号を返す．
SizeType
BnDLBase::data_out() const
{
  return mOutput;
}

// @brief クロック端子のノード番号を返す．
SizeType
BnDLBase::clock() const
{
  return mClock;
}

// @brief クリア端子のノード番号を返す．
SizeType
BnDLBase::clear() const
{
  return mClear;
}

// @brief プリセット端子のノード番号を返す．
SizeType
BnDLBase::preset() const
{
  return mPreset;
}

// @brief クリアとプリセットが衝突した場合の挙動
BnCPV
BnDLBase::clear_preset_value() const
{
  return mCPV;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffImpl
//////////////////////////////////////////////////////////////////////

// @brief タイプを返す．
BnDffType
BnDffImpl::type() const
{
  return BnDffType::Dff;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchImpl
//////////////////////////////////////////////////////////////////////

// @brief タイプを返す．
BnDffType
BnLatchImpl::type() const
{
  return BnDffType::Latch;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffCell
//////////////////////////////////////////////////////////////////////

// @brief タイプを返す．
BnDffType
BnDffCell::type() const
{
  return BnDffType::Cell;
}

// @brief セルに割り当てられている場合のセル番号を返す．
SizeType
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
  ASSERT_COND( 0 <= pos && pos < mInputList.size() );
  return mInputList[pos];
}

// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
SizeType
BnDffCell::cell_output(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < mOutputList.size() );
  return mOutputList[pos];
}

END_NAMESPACE_YM_BNET
