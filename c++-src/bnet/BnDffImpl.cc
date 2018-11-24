
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
// @param[in] cell_id セル番号
BnDffImpl::BnDffImpl(int id,
		     const string& name,
		     int input,
		     int output,
		     int xoutput,
		     int clock,
		     int clear,
		     int preset,
		     int cell_id) :
  mId(id),
  mName(name),
  mInput(input),
  mOutput(output),
  mXOutput(xoutput),
  mClock(clock),
  mClear(clear),
  mPreset(preset),
  mCellId(cell_id)
{
}

// @brief デストラクタ
BnDffImpl::~BnDffImpl()
{
}

// @brief ID 番号の取得
// @return ID 番号を返す．
int
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
int
BnDffImpl::output() const
{
  return mOutput;
}

// @brief データ出力のノード番号を返す．
int
BnDffImpl::xoutput() const
{
  return mXOutput;
}

// @brief データ入力のノード番号を返す．
int
BnDffImpl::input() const
{
  return mInput;
}

// @brief クロックのノード番号を返す．
int
BnDffImpl::clock() const
{
  return mClock;
}

// @brief クリア信号のノード番号を返す．
//
// kBnNullId の場合もある．
int
BnDffImpl::clear() const
{
  return mClear;
}

// @brief プリセット信号のノードを返す．
//
// kBnNullId の場合もある．
int
BnDffImpl::preset() const
{
  return mPreset;
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
