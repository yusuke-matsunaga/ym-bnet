
/// @file BnLatchImpl.cc
/// @brief BnLatchImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnLatchImpl.h"
#include "ym/ClibCell.h"
#include "ym/ClibCellGroup.h"
#include "ym/ClibLatchInfo.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnLatchImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name 名前
// @param[in] input 入力端子のノード番号
// @param[in] output 出力端子のノード番号
// @param[in] enable イネーブル端子のノード番号
// @param[in] clear クリア端子のノード番号
// @param[in] preset プリセット端子のノード番号
// @param[in] cell セル
BnLatchImpl::BnLatchImpl(int id,
			 const string& name,
			 int input,
			 int output,
			 int enable,
			 int clear,
			 int preset,
			 const ClibCell* cell) :
  mId(id),
  mName(name),
  mInput(input),
  mOutput(output),
  mEnable(enable),
  mClear(clear),
  mPreset(preset),
  mCell(cell)
{
  if ( cell != nullptr ) {
    ASSERT_COND( cell->is_latch() );
  }
}

// @brief デストラクタ
BnLatchImpl::~BnLatchImpl()
{
}

// @brief ID 番号の取得
// @return ID 番号を返す．
int
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

// @brief データ出力のノード番号を返す．
int
BnLatchImpl::output() const
{
  return mOutput;
}

// @brief データ入力のノード番号を返す．
int
BnLatchImpl::input() const
{
  return mInput;
}

// @brief イネーブルのノード番号を返す．
int
BnLatchImpl::enable() const
{
  return mEnable;
}

// @brief クリア信号のノード番号を返す．
//
// kBnNullId の場合もある．
int
BnLatchImpl::clear() const
{
  return mClear;
}

// @brief プリセット信号のノード番号を返す．
//
// kBnNullId の場合もある．
int
BnLatchImpl::preset() const
{
  return mPreset;
}

// @brief セルを返す．
//
// nullptr の場合もある．
const ClibCell*
BnLatchImpl::cell() const
{
  return mCell;
}

// @brief データ出力のピン番号を返す．
//
// cell() == nullptr の場合の値は不定
int
BnLatchImpl::output_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  ClibLatchInfo ffinfo = cell()->latch_info();
  return ffinfo.q_pos();
}

// @brief データ入力のピン番号を返す．
//
// cell() == nullptr の場合の値は不定
int
BnLatchImpl::input_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  ClibLatchInfo ffinfo = cell()->latch_info();
  return ffinfo.data_pos();
}

// @brief イネーブル端子のピン番号を返す．
//
// cell() == nullptr の場合の値は不定
int
BnLatchImpl::enable_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  ClibLatchInfo ffinfo = cell()->latch_info();
  return ffinfo.enable_pos();
}

// @brief クリア信号のピン番号を返す．
//
// kBnNullId の場合もある．
// cell() == nullptr の場合の値は不定
int
BnLatchImpl::clear_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  ClibLatchInfo ffinfo = cell()->latch_info();
  return ffinfo.clear_pos();
}

// @brief プリセット信号のピン番号を返す．
//
// kBnNullId の場合もある．
// cell() == nullptr の場合の値は不定
int
BnLatchImpl::preset_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  ClibLatchInfo ffinfo = cell()->latch_info();
  return ffinfo.preset_pos();
}

END_NAMESPACE_YM_BNET
