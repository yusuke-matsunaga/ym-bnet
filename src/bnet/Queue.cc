
/// @file Queue.cc
/// @brief Queue の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "Queue.h"
#include "ym/BnNode.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス Queue
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] size キューの最大サイズ
Queue::Queue(ymuint size) :
  mNodeArray(size)
{
  mWpos = 0;
  mRpos = 0;
}

// @brief デストラクタ
Queue::~Queue()
{
}

// @brief キューに積む．
// @param[in] node ノード
void
Queue::put(BnNode* node)
{
  if ( !mMark.check(node->id()) ) {
    mMark.add(node->id());
    mNodeArray[mWpos] = node;
    ++ mWpos;
  }
}

// @brief キューから取り出す．
// @return 取り出されたノードを返す．
//
// 空の時には nullptr を返す．
BnNode*
Queue::get()
{
  if ( mRpos >= mWpos ) {
    return nullptr;
  }
  BnNode* node = mNodeArray[mRpos];
  ++ mRpos;

  return node;
}

// @brief キューに含まれているか調べる．
// @param[in] node ノード
bool
Queue::check(BnNode* node) const
{
  return mMark.check(node->id());
}

END_NAMESPACE_YM_BNET
