
/// @file BnLatchImpl.cc
/// @brief BnLatchImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnLatchImpl.h"
#include "ym/Cell.h"
#include "ym/CellGroup.h"
#include "ym/CellLatchInfo.h"


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
BnLatchImpl::BnLatchImpl(ymuint id,
			 const string& name,
			 ymuint input,
			 ymuint output,
			 ymuint enable,
			 ymuint clear,
			 ymuint preset,
			 const Cell* cell) :
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
ymuint
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
ymuint
BnLatchImpl::output() const
{
  return mOutput;
}

// @brief データ入力のノード番号を返す．
ymuint
BnLatchImpl::input() const
{
  return mInput;
}

// @brief イネーブルのノード番号を返す．
ymuint
BnLatchImpl::enable() const
{
  return mEnable;
}

// @brief クリア信号のノード番号を返す．
//
// kBnNullId の場合もある．
ymuint
BnLatchImpl::clear() const
{
  return mClear;
}

// @brief プリセット信号のノード番号を返す．
//
// kBnNullId の場合もある．
ymuint
BnLatchImpl::preset() const
{
  return mPreset;
}

// @brief セルを返す．
//
// nullptr の場合もある．
const Cell*
BnLatchImpl::cell() const
{
  return mCell;
}

// @brief データ出力のピン番号を返す．
//
// cell() == nullptr の場合の値は不定
ymuint
BnLatchImpl::output_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  const CellGroup* cg = cell()->cell_group();
  CellLatchInfo ffinfo = cg->latch_info();
  return ffinfo.q_pos();
}

// @brief データ入力のピン番号を返す．
//
// cell() == nullptr の場合の値は不定
ymuint
BnLatchImpl::input_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  const CellGroup* cg = cell()->cell_group();
  CellLatchInfo ffinfo = cg->latch_info();
  return ffinfo.data_pos();
}

// @brief イネーブル端子のピン番号を返す．
//
// cell() == nullptr の場合の値は不定
ymuint
BnLatchImpl::enable_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  const CellGroup* cg = cell()->cell_group();
  CellLatchInfo ffinfo = cg->latch_info();
  return ffinfo.enable_pos();
}

// @brief クリア信号のピン番号を返す．
//
// kBnNullId の場合もある．
// cell() == nullptr の場合の値は不定
ymuint
BnLatchImpl::clear_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  const CellGroup* cg = cell()->cell_group();
  CellLatchInfo ffinfo = cg->latch_info();
  return ffinfo.clear_pos();
}

// @brief プリセット信号のピン番号を返す．
//
// kBnNullId の場合もある．
// cell() == nullptr の場合の値は不定
ymuint
BnLatchImpl::preset_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  const CellGroup* cg = cell()->cell_group();
  CellLatchInfo ffinfo = cg->latch_info();
  return ffinfo.preset_pos();
}

END_NAMESPACE_YM_BNET
