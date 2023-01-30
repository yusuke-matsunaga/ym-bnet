
/// @file BlifNode.cc
/// @brief BlifNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BlifNode.h"
#include "BlifInput.h"
#include "BlifLatch.h"
#include "BlifNames.h"
#include "BlifGate.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
// クラス BlifNode
//////////////////////////////////////////////////////////////////////

// @brief 入力ノードを返す．
SizeType
BlifNode::inode() const
{
  ASSERT_NOT_REACHED;
  return -1;
}

// @brief リセット値を返す．
char
BlifNode::rval() const
{
  ASSERT_NOT_REACHED;
  return 'X';
}

// @brief 入力ノード番号のリストを返す．
const vector<SizeType>&
BlifNode::inode_list() const
{
  static vector<SizeType> dummy_list{};
  ASSERT_NOT_REACHED;
  return dummy_list;
}

/// @brief カバー番号を返す．
SizeType
BlifNode::cover_id() const
{
  ASSERT_NOT_REACHED;
  return -1;
}

// @brief セル番号を返す．
SizeType
BlifNode::cell_id() const
{
  ASSERT_NOT_REACHED;
  return CLIB_NULLID;
}


//////////////////////////////////////////////////////////////////////
// クラス BlifInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifInput::BlifInput(
  SizeType id,
  const string& name
) : BlifNode{id, name}
{
}

// @brief デストラクタ
BlifInput::~BlifInput()
{
}

// @brief 種類を返す．
BlifNode::Type
BlifInput::type() const
{
  return Input;
}


//////////////////////////////////////////////////////////////////////
// クラス BlifLatch
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifLatch::BlifLatch(
  SizeType id,
  const string& name,
  SizeType inode,
  char rval
) : BlifNode{id, name},
    mInode{inode},
    mRval{rval}
{
}

// @brief デストラクタ
BlifLatch::~BlifLatch()
{
}

// @brief 種類を返す．
BlifNode::Type
BlifLatch::type() const
{
  return Latch;
}

// @brief 入力ノードを返す．
SizeType
BlifLatch::inode() const
{
  return mInode;
}

/// @brief リセット値を返す．
char
BlifLatch::rval() const
{
  return mRval;
}


//////////////////////////////////////////////////////////////////////
// クラス BlifNames
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifNames::BlifNames(
  SizeType id,
  const string& name,
  const vector<SizeType>& inode_list,
  SizeType cover_id
) : BlifNode{id, name},
    mInodeList{inode_list},
    mCoverId{cover_id}
{
}

// @brief デストラクタ
BlifNames::~BlifNames()
{
}

// @brief 種類を返す．
BlifNode::Type
BlifNames::type() const
{
  return Names;
}

// @brief 入力ノード番号のリストを返す．
const vector<SizeType>&
BlifNames::inode_list() const
{
  return mInodeList;
}

// @brief カバー番号を返す．
SizeType
BlifNames::cover_id() const
{
  return mCoverId;
}


//////////////////////////////////////////////////////////////////////
// クラス BlifGate
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifGate::BlifGate(
  SizeType id,
  const string& name,
  const vector<SizeType>& inode_list,
  SizeType cell_id
) : BlifNode{id, name},
    mInodeList{inode_list},
    mCellId{cell_id}
{
}

// @brief デストラクタ
BlifGate::~BlifGate()
{
}

// @brief 種類を返す．
BlifNode::Type
BlifGate::type() const
{
  return Gate;
}

// @brief 入力ノード番号のリストを返す．
const vector<SizeType>&
BlifGate::inode_list() const
{
  return mInodeList;
}

// @brief セル番号を返す．
SizeType
BlifGate::cell_id() const
{
  return mCellId;
}

END_NAMESPACE_YM_BLIF
