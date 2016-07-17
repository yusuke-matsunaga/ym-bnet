
/// @file BnDffImpl.cc
/// @brief BnDffImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnDffImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnDffImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name 名前
// @param[in] input 入力端子のノード
// @param[in] output 出力端子のノード
// @param[in] clock クロック端子のノード
// @param[in] clear クリア端子のノード
// @param[in] preset プリセット端子のノード
BnDffImpl::BnDffImpl(ymuint id,
		     const string& name,
		     BnNode* input,
		     BnNode* output,
		     BnNode* clock,
		     BnNode* clear,
		     BnNode* preset) :
  mId(id),
  mName(name),
  mInput(input),
  mOutput(output),
  mClock(clock),
  mClear(clear),
  mPreset(preset)
{
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

// @brief データ出力ノードを返す．
const BnNode*
BnDffImpl::output() const
{
  return mOutput;
}

// @brief データ入力ノードを返す．
const BnNode*
BnDffImpl::input() const
{
  return mInput;
}

// @brief クロックのノードを返す．
const BnNode*
BnDffImpl::clock() const
{
  return mClock;
}

// @brief クリア信号のノードを返す．
// @note nullptr の場合もある．
const BnNode*
BnDffImpl::clear() const
{
  return mClear;
}

// @brief プリセット信号のノードを返す．
// @note nullptr の場合もある．
const BnNode*
BnDffImpl::preset() const
{
  return mPreset;
}

END_NAMESPACE_YM_BNET
