
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

END_NAMESPACE_YM_BNET
