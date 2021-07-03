
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
