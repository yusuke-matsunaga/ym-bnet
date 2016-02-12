
/// @file BnNodeImpl.cc
/// @brief BnNodeImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BnNodeImpl.h"
#include "ym/BlifCover.h"


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

// @brief ゲートの型を返す．
GateType
BnNodeImpl::gate_type() const
{
  ASSERT_NOT_REACHED;
  return kGt_BUFF;
}

// @brief セルを返す．
const Cell*
BnNodeImpl::cell() const
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief カバーを得る．
const BlifCover*
BnNodeImpl::cover() const
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief 論理式を得る．
Expr
BnNodeImpl::expr() const
{
  ASSERT_NOT_REACHED;
  return Expr();
}

// @brief 真理値表を得る．
TvFunc
BnNodeImpl::truth_vector() const
{
  ASSERT_NOT_REACHED;
  return TvFunc();
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

// @brief 型を返す．
BnNode::Type
BnInputNode::type() const
{
  return kInput;
}

// @brief 内容を blif 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnInputNode::write_blif(ostream& s) const
{
  ASSERT_NOT_REACHED;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLogicNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] ni ファンイン数
// @param[in] fanins ファンインのID番号の配列
BnLogicNode::BnLogicNode(ymuint id,
			     const char* name,
			     ymuint ni,
			     const ymuint* fanins) :
  BnNodeImpl(id, name),
  mFaninNum(ni),
  mFanins(fanins)
{
}

// @brief デストラクタ
BnLogicNode::~BnLogicNode()
{
}

// @brief ファンイン数を得る．
ymuint
BnLogicNode::fanin_num() const
{
  return mFaninNum;
}

// @brief ファンインを求める．n
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
ymuint
BnLogicNode::fanin_id(ymuint pos) const
{
  ASSERT_COND( pos < fanin_num() );
  return mFanins[pos];
}


//////////////////////////////////////////////////////////////////////
// クラス BnPrimNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] ni ファンイン数
// @param[in] fanins ファンインのID番号の配列
// @param[in] gate_type ゲートの型
BnPrimNode::BnPrimNode(ymuint id,
		       const char* name,
		       ymuint ni,
		       const ymuint* fanins,
		       GateType gate_type) :
  BnLogicNode(id, name, ni, fanins),
  mGateType(gate_type)
{
}

// @brief デストラクタ
BnPrimNode::~BnPrimNode()
{
}

// @brief 型を返す．
BnNode::Type
BnPrimNode::type() const
{
  return kPrimGate;
}

// @brief 内容を blif 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnPrimNode::write_blif(ostream& s) const
{
}

// @brief ゲートの型を返す．
GateType
BnPrimNode::gate_type() const
{
  return mGateType;
}


//////////////////////////////////////////////////////////////////////
// クラス BnCellNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] ni ファンイン数
// @param[in] fanins ファンインのID番号の配列
/// @param[in] cell セルへのポインタ
BnCellNode::BnCellNode(ymuint id,
		       const char* name,
		       ymuint ni,
		       const ymuint* fanins,
		       const Cell* cell) :
  BnLogicNode(id, name, ni, fanins),
  mCell(cell)
{
}

// @brief デストラクタ
BnCellNode::~BnCellNode()
{
}

// @brief 型を返す．
BnNode::Type
BnCellNode::type() const
{
  return kCell;
}

// @brief 内容を blif 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnCellNode::write_blif(ostream& s) const
{
}

// @brief セルを返す．
const Cell*
BnCellNode::cell() const
{
  return mCell;
}


//////////////////////////////////////////////////////////////////////
// クラス BnCoverNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] ni ファンイン数
// @param[in] name ノード名
// @param[in] fanins ファンインのID番号の配列
// @param[in] cover カバー
BnCoverNode::BnCoverNode(ymuint id,
			 const char* name,
			 ymuint ni,
			 const ymuint* fanins,
			 const BlifCover* cover) :
  BnLogicNode(id, name, ni, fanins),
  mCover(cover)
{
}

// @brief デストラクタ
BnCoverNode::~BnCoverNode()
{
}

// @brief 型を返す．
BnNode::Type
BnCoverNode::type() const
{
  return kCover;
}

// @brief 内容を blif 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnCoverNode::write_blif(ostream& s) const
{
}

// @brief カバーを得る．
const BlifCover*
BnCoverNode::cover() const
{
  return mCover;
}


//////////////////////////////////////////////////////////////////////
// クラス BnExprNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] ni ファンイン数
// @param[in] name ノード名
// @param[in] fanins ファンインのID番号の配列
// @param[in] expr 論理式
BnExprNode::BnExprNode(ymuint id,
		       const char* name,
		       ymuint ni,
		       const ymuint* fanins,
		       Expr expr) :
  BnLogicNode(id, name, ni, fanins),
  mExpr(expr)
{
}

// @brief デストラクタ
BnExprNode::~BnExprNode()
{
}

// @brief 型を返す．
BnNode::Type
BnExprNode::type() const
{
  return kCover;
}

// @brief 内容を blif 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnExprNode::write_blif(ostream& s) const
{
}

// @brief 論理式を得る．
Expr
BnExprNode::expr() const
{
  return mExpr;
}


//////////////////////////////////////////////////////////////////////
// クラス BnTvNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] ni ファンイン数
// @param[in] name ノード名
// @param[in] fanins ファンインのID番号の配列
// @param[in] tv 真理値表
BnTvNode::BnTvNode(ymuint id,
		   const char* name,
		   ymuint ni,
		   const ymuint* fanins,
		   TvFunc tv) :
  BnLogicNode(id, name, ni, fanins),
  mTv(tv)
{
}

// @brief デストラクタ
BnTvNode::~BnTvNode()
{
}

// @brief 型を返す．
BnNode::Type
BnTvNode::type() const
{
  return kTv;
}

// @brief 内容を blif 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnTvNode::write_blif(ostream& s) const
{
}

// @brief 真理値表を得る．
TvFunc
BnTvNode::truth_vector() const
{
  return mTv;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] inode_id 入力ノードのID番号
// @param[in] rval リセット値
BnLatchNode::BnLatchNode(ymuint id,
			 const char* name,
			 ymuint inode_id,
			 char rval) :
  BnNodeImpl(id, name),
  mInodeId(inode_id),
  mResetVal(rval)
{
}

// @brief デストラクタ
BnLatchNode::~BnLatchNode()
{
}

// @brief 型を返す．
BnNode::Type
BnLatchNode::type() const
{
  return kLatch;
}

// @brief 内容を blif 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnLatchNode::write_blif(ostream& s) const
{
}

// @brief 入力ノードのID番号を返す．
ymuint
BnLatchNode::inode_id() const
{
  return mInodeId;
}

// @brief リセット値を返す．
char
BnLatchNode::reset_val() const
{
  return mResetVal;
}

END_NAMESPACE_YM_BNET
