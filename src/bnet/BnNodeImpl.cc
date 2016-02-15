
/// @file BnNodeImpl.cc
/// @brief BnNodeImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BnNodeImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNodeImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
BnNodeImpl::BnNodeImpl(ymuint id,
		       const char* name) :
  mId(id),
  mName(name)
{
  mFanoutNum = 0;
  mFanoutList = nullptr;
}

// @brief デストラクタ
BnNodeImpl::~BnNodeImpl()
{
  // mFanoutList は BnNetworkImpl が管理している．
}

// @brief ID を返す．
ymuint
BnNodeImpl::id() const
{
  return mId;
}

// @brief 名前を返す．
const char*
BnNodeImpl::name() const
{
  return mName;
}

// @brief 外部入力の時 true を返す．
bool
BnNodeImpl::is_input() const
{
  return false;
}

// @brief 論理ノードの時 true を返す．
bool
BnNodeImpl::is_logic() const
{
  return false;
}

// @brief D-FF ノードの時 true を返す．
bool
BnNodeImpl::is_dff() const
{
  return false;
}

// @brief ファンアウト数を得る．
ymuint
BnNodeImpl::fanout_num() const
{
  return mFanoutNum;
}

// @brief ファンアウトのノードIDを返す．
// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
ymuint
BnNodeImpl::fanout_id(ymuint pos) const
{
  ASSERT_COND( pos < fanout_num() );
  return mFanoutList[pos];
}

// @brief ファンイン数を得る．
ymuint
BnNodeImpl::fanin_num() const
{
  return 0;
}

// @brief ファンインのノードIDを返す．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
ymuint
BnNodeImpl::fanin_id(ymuint pos) const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 関数のタイプを返す．
const BnFuncType*
BnNodeImpl::func_type() const
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief 入力ノードのID番号を返す．
ymuint
BnNodeImpl::inode_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief リセット値を返す．
char
BnNodeImpl::reset_val() const
{
  ASSERT_NOT_REACHED;
  return ' ';
}


//////////////////////////////////////////////////////////////////////
// クラス BnInputNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
BnInputNode::BnInputNode(ymuint id,
			 const char* name) :
  BnNodeImpl(id, name)
{
}

// @brief デストラクタ
BnInputNode::~BnInputNode()
{
}

// @brief 外部入力の時 true を返す．
bool
BnInputNode::is_input() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLogicNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] fanins ファンインのID番号の配列
// @param[in] func_type 関数の型
BnLogicNode::BnLogicNode(ymuint id,
			 const char* name,
			 const ymuint* fanins,
			 const BnFuncType* func_type) :
  BnNodeImpl(id, name),
  mFanins(fanins),
  mFuncType(func_type)
{
}

// @brief デストラクタ
BnLogicNode::~BnLogicNode()
{
}

// @brief 外部入力の時 true を返す．
bool
BnLogicNode::is_logic() const
{
  return true;
}

// @brief ファンイン数を得る．
ymuint
BnLogicNode::fanin_num() const
{
  return func_type()->input_num();
}

// @brief ファンインを求める．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
ymuint
BnLogicNode::fanin_id(ymuint pos) const
{
  ASSERT_COND( pos < fanin_num() );
  return mFanins[pos];
}

// @brief 関数のタイプを返す．
const BnFuncType*
BnLogicNode::func_type() const
{
  return mFuncType;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] inode_id 入力ノードのID番号
// @param[in] rval リセット値
BnDffNode::BnDffNode(ymuint id,
		     const char* name,
		     ymuint inode_id,
		     char rval) :
  BnNodeImpl(id, name),
  mInodeId(inode_id),
  mResetVal(rval)
{
}

// @brief デストラクタ
BnDffNode::~BnDffNode()
{
}

// @brief D-FF ノードの時 true を返す．
bool
BnDffNode::is_dff() const
{
  return true;
}

// @brief 入力ノードのID番号を返す．
ymuint
BnDffNode::inode_id() const
{
  return mInodeId;
}

// @brief リセット値を返す．
char
BnDffNode::reset_val() const
{
  return mResetVal;
}

END_NAMESPACE_YM_BNET
