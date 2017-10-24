
/// @file BnDffImpl.cc
/// @brief BnDffImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BnDffImpl.h"
#include "ym/ClibCell.h"
#include "ym/ClibFFInfo.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnDffImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name 名前
// @param[in] input 入力端子のノード番号
// @param[in] output 出力端子のノード番号
// @param[in] xoutput 出力端子のノード番号
// @param[in] clock クロック端子のノード番号
// @param[in] clear クリア端子のノード番号
// @param[in] preset プリセット端子のノード番号
// @param[in] cell セル
BnDffImpl::BnDffImpl(ymuint id,
		     const string& name,
		     ymuint input,
		     ymuint output,
		     ymuint xoutput,
		     ymuint clock,
		     ymuint clear,
		     ymuint preset,
		     const ClibCell* cell) :
  mId(id),
  mName(name),
  mInput(input),
  mOutput(output),
  mXOutput(xoutput),
  mClock(clock),
  mClear(clear),
  mPreset(preset),
  mCell(cell)
{
  if ( cell != nullptr ) {
    ASSERT_COND( cell->is_ff() );
  }
}

// @brief デストラクタ
BnDffImpl::~BnDffImpl()
{
}

// @brief ID 番号の取得
// @return ID 番号を返す．
ymuint
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

// @brief データ出力のノード番号を返す．
ymuint
BnDffImpl::output() const
{
  return mOutput;
}

// @brief データ出力のノード番号を返す．
ymuint
BnDffImpl::xoutput() const
{
  return mXOutput;
}

// @brief データ入力のノード番号を返す．
ymuint
BnDffImpl::input() const
{
  return mInput;
}

// @brief クロックのノード番号を返す．
ymuint
BnDffImpl::clock() const
{
  return mClock;
}

// @brief クリア信号のノード番号を返す．
//
// kBnNullId の場合もある．
ymuint
BnDffImpl::clear() const
{
  return mClear;
}

// @brief プリセット信号のノードを返す．
//
// kBnNullId の場合もある．
ymuint
BnDffImpl::preset() const
{
  return mPreset;
}

// @brief セルを返す．
//
// nullptr の場合もある．
const ClibCell*
BnDffImpl::cell() const
{
  return mCell;
}

// @brief データ出力のピン番号を返す．
//
// cell() == nullptr の場合の値は不定
ymuint
BnDffImpl::output_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  ClibFFInfo ffinfo = cell()->ff_info();
  return ffinfo.q_pos();
}

// @brief 反転データ出力のピン番号を返す．
//
// cell() == nullptr の場合の値は不定
ymuint
BnDffImpl::xoutput_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  ClibFFInfo ffinfo = cell()->ff_info();
  return ffinfo.xq_pos();
}

// @brief データ入力のピン番号を返す．
//
// cell() == nullptr の場合の値は不定
ymuint
BnDffImpl::input_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  ClibFFInfo ffinfo = cell()->ff_info();
  return ffinfo.data_pos();
}

// @brief クロックのピン番号を返す．
//
// cell() == nullptr の場合の値は不定
ymuint
BnDffImpl::clock_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  ClibFFInfo ffinfo = cell()->ff_info();
  return ffinfo.clock_pos();
}

// @brief クリア信号のピン番号を返す．
//
// kBnNullId の場合もある．
// cell() == nullptr の場合の値は不定
ymuint
BnDffImpl::clear_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  ClibFFInfo ffinfo = cell()->ff_info();
  return ffinfo.clear_pos();
}

// @brief プリセット信号のピン番号を返す．
//
// kBnNullId の場合もある．
// cell() == nullptr の場合の値は不定
ymuint
BnDffImpl::preset_pin_id() const
{
  if ( cell() == nullptr ) {
    return 0;
  }

  ClibFFInfo ffinfo = cell()->ff_info();
  return ffinfo.preset_pos();
}

END_NAMESPACE_YM_BNET
