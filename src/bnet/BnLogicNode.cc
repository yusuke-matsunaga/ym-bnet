
/// @file BnLogicNode.cc
/// @brief BnLogicNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnLogicNode.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnLogicNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
/// @param[in] ni 入力数
// @param[in] cell セル (nullptr の場合もあり)
BnLogicNode::BnLogicNode(ymuint id,
			 const string& name,
			 ymuint ni,
			 const Cell* cell) :
  BnNodeImpl(id, name),
  mFaninNum(ni),
  mCell(cell)
{
  mFanins = new ymuint[mFaninNum];
  for (ymuint i = 0; i < mFaninNum; ++ i) {
    mFanins[i] = kBnNullId;
  }
}

// @brief デストラクタ
BnLogicNode::~BnLogicNode()
{
  delete [] mFanins;
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
  return mFaninNum;
}

// @brief ファンインを求める．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
ymuint
BnLogicNode::fanin(ymuint pos) const
{
  ASSERT_COND( pos < fanin_num() );
  return mFanins[pos];
}

// @brief セルを返す．
//
// is_logic() == false の時の動作は不定
// 場合によっては nullptr を返す．
const Cell*
BnLogicNode::cell() const
{
  return mCell;
}

// @brief ファンインを設定する．
// @param[in] ipos 入力位置
// @param[in] fanin_id ファンインのノード番号
void
BnLogicNode::set_fanin(ymuint ipos,
		       ymuint fanin_id)
{
  ASSERT_COND( ipos < fanin_num() );
  mFanins[ipos] = fanin_id;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPrimNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] ni 入力数
// @param[in] logic_type 論理タイプ
// @param[in] cell セル (nullptr の場合もあり)
BnPrimNode::BnPrimNode(ymuint id,
		       const string& name,
		       ymuint ni,
		       BnNodeType logic_type,
		       const Cell* cell) :
  BnLogicNode(id, name, ni, cell),
  mLogicType(logic_type)
{
}

// @brief デストラクタ
BnPrimNode::~BnPrimNode()
{
}

// @brief タイプを返す．
BnNodeType
BnPrimNode::type() const
{
  return mLogicType;
}


//////////////////////////////////////////////////////////////////////
// クラス BnExprNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] ni 入力数
// @param[in] expr 論理式
// @param[in] expr_id 論理式番号
// @param[in] cell セル (nullptr の場合もあり)
BnExprNode::BnExprNode(ymuint id,
		       const string& name,
		       ymuint ni,
		       const Expr& expr,
		       ymuint expr_id,
		       const Cell* cell) :
  BnLogicNode(id, name, ni, cell),
  mExpr(expr),
  mExprId(expr_id)
{
}

// @brief デストラクタ
BnExprNode::~BnExprNode()
{
}

// @brief タイプを返す．
BnNodeType
BnExprNode::type() const
{
  return kBnLogic_EXPR;
}

// @brief 論理式番号を返す．
//
// logic_type() == kBnLogic_EXPR の時のみ意味を持つ．
// 論理式番号は同じ BnNetwork 内で唯一となるもの．
ymuint
BnExprNode::expr_id() const
{
  return mExprId;
}

// @brief 論理式を返す．
//
// type() != kBnLogic_EXPR の時の動作は不定
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
// @param[in] name ノード名
// @param[in] ni 入力数
// @param[in] tv 真理値表
// @param[in] func_id 関数番号
// @param[in] cell セル (nullptr の場合もあり)
BnTvNode::BnTvNode(ymuint id,
		   const string& name,
		   ymuint ni,
		   const TvFunc& func,
		   ymuint func_id,
		   const Cell* cell) :
  BnLogicNode(id, name, ni, cell),
  mFunc(func),
  mFuncId(func_id)
{
}

// @brief デストラクタ
BnTvNode::~BnTvNode()
{
}

// @brief タイプを返す．
BnNodeType
BnTvNode::type() const
{
  return kBnLogic_TV;
}

// @brief 関数番号を返す．
//
// type() == kBnLogic_TV の時のみ意味を持つ．
// 関数番号は同じ BnNetwork 内で唯一となるもの．
ymuint
BnTvNode::func_id() const
{
  return mFuncId;
}

// @brief 真理値表を返す．
//
// is_logic() == false の時の動作は不定
// logic_type() != kBnLogic_TV の時の動作は不定
TvFunc
BnTvNode::func() const
{
  return mFunc;
}

END_NAMESPACE_YM_BNET
