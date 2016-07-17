
/// @file BnLatchImpl.cc
/// @brief BnLatchImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnLatchImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnLatchImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name 名前
// @param[in] input 入力端子のノード
// @param[in] output 出力端子のノード
// @param[in] enable イネーブル端子のノード
// @param[in] clear クリア端子のノード
// @param[in] preset プリセット端子のノード
BnLatchImpl::BnLatchImpl(ymuint id,
			 const string& name,
			 BnNode* input,
			 BnNode* output,
			 BnNode* enable,
			 BnNode* clear,
			 BnNode* preset) :
  mId(id),
  mName(name),
  mInput(input),
  mOutput(output),
  mEnable(enable),
  mClear(clear),
  mPreset(preset)
{
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

// @brief データ出力ノードを返す．
const BnNode*
BnLatchImpl::output() const
{
  return mOutput;
}

// @brief データ入力ノードを返す．
const BnNode*
BnLatchImpl::input() const
{
  return mInput;
}

// @brief イネーブルのノードを返す．
const BnNode*
BnLatchImpl::enable() const
{
  return mEnable;
}

// @brief クリア信号のノードを返す．
// @note nullptr の場合もある．
const BnNode*
BnLatchImpl::clear() const
{
  return mClear;
}

// @brief プリセット信号のノードを返す．
// @note nullptr の場合もある．
const BnNode*
BnLatchImpl::preset() const
{
  return mPreset;
}

END_NAMESPACE_YM_BNET
