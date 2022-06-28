
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

END_NAMESPACE_YM_BNET
