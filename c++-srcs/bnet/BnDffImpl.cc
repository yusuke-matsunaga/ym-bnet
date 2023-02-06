
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
// クラス BnDffImpl
//////////////////////////////////////////////////////////////////////

// @brief DFF タイプの時 true を返す．
bool
BnDffImpl::is_dff() const
{
  return false;
}

// @brief ラッチタイプの時 true を返す．
bool
BnDffImpl::is_latch() const
{
  return false;
}

// @brief DFF/ラッチセルタイプの時 true を返す．
bool
BnDffImpl::is_cell() const
{
  return false;
}

// @brief 入力端子のノード番号を返す．
SizeType
BnDffImpl::data_in() const
{
  return BNET_NULLID;
}

// @brief 出力端子のノード番号を返す．
SizeType
BnDffImpl::data_out() const
{
  return BNET_NULLID;
}

// @brief クロック端子のノード番号を返す．
SizeType
BnDffImpl::clock() const
{
  return BNET_NULLID;
}

// @brief クリア端子のノード番号を返す．
SizeType
BnDffImpl::clear() const
{
  return BNET_NULLID;
}

// @brief プリセット端子のノード番号を返す．
SizeType
BnDffImpl::preset() const
{
  return BNET_NULLID;
}

// @brief クリアとプリセットが衝突した場合の挙動
BnCPV
BnDffImpl::clear_preset_value() const
{
  return BnCPV::L;
}

// @brief セルに割り当てられている場合のセル番号を返す．
SizeType
BnDffImpl::cell_id() const
{
  return CLIB_NULLID;
}

// @brief セルに割り当てられている場合の入力端子数を返す．
SizeType
BnDffImpl::cell_input_num() const
{
  return 0;
}

// @brief セルに割り当てられている場合の入力端子に対応するノード番号を返す．
SizeType
BnDffImpl::cell_input(
  SizeType pos
) const
{
  return BNET_NULLID;
}

// @brief セルに割り当てられている場合の出力端子数を返す．
SizeType
BnDffImpl::cell_output_num() const
{
  return 0;
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
// クラス BnDff_FF
//////////////////////////////////////////////////////////////////////

// @brief タイプを返す．
BnDffType
BnDff_FF::type() const
{
  return BnDffType::Dff;
}

// @brief DFF タイプの時 true を返す．
bool
BnDff_FF::is_dff() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDff_Latch
//////////////////////////////////////////////////////////////////////

// @brief タイプを返す．
BnDffType
BnDff_Latch::type() const
{
  return BnDffType::Latch;
}

// @brief ラッチタイプの時 true を返す．
bool
BnDff_Latch::is_latch() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDff_Cell
//////////////////////////////////////////////////////////////////////

// @brief タイプを返す．
BnDffType
BnDff_Cell::type() const
{
  return BnDffType::Cell;
}

// @brief DFF/ラッチセルタイプの時 true を返す．
bool
BnDff_Cell::is_cell() const
{
  return true;
}

// @brief セルに割り当てられている場合のセル番号を返す．
SizeType
BnDff_Cell::cell_id() const
{
  return mCellId;
}

// @brief セルに割り当てられている場合の入力端子数を返す．
SizeType
BnDff_Cell::cell_input_num() const
{
  return mInputList.size();
}

// @brief セルに割り当てられている場合の入力端子に対応するノード番号を返す．
SizeType
BnDff_Cell::cell_input(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < cell_input_num() );
  return mInputList[pos];
}

// @brief セルに割り当てられている場合の出力端子数を返す．
SizeType
BnDff_Cell::cell_output_num() const
{
  return mOutputList.size();
}

// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
SizeType
BnDff_Cell::cell_output(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < cell_output_num() );
  return mOutputList[pos];
}

END_NAMESPACE_YM_BNET
