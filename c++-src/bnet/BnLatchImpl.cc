
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
// @param[in] xoutput 反転出力端子のノード番号
// @param[in] enable イネーブル端子のノード番号
// @param[in] clear クリア端子のノード番号
// @param[in] preset プリセット端子のノード番号
// @param[in] cell_id セル番号
BnLatchImpl::BnLatchImpl(int id,
			 const string& name,
			 int input,
			 int output,
			 int xoutput,
			 int enable,
			 int clear,
			 int preset,
			 int cell_id) :
  mId(id),
  mName(name),
  mInput(input),
  mOutput(output),
  mXoutput(xoutput),
  mEnable(enable),
  mClear(clear),
  mPreset(preset),
  mCellId(cell_id)
{
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

// @brief データ反転出力のノード番号を返す．
int
BnLatchImpl::xoutput() const
{
  return mXoutput;
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

// @brief セル番号を返す．
//
// -1 の場合もある．
int
BnLatchImpl::cell_id() const
{
  return mCellId;
}

END_NAMESPACE_YM_BNET
