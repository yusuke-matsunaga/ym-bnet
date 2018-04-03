
/// @file BnLogicNode.cc
/// @brief BnLogicNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
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
BnLogicNode::BnLogicNode(int id,
			 const string& name,
			 int ni,
			 const ClibCell* cell) :
  BnNodeImpl(id, name),
  mFaninNum(ni),
  mCell(cell)
{
  mFanins = new int[mFaninNum];
  for ( int i = 0; i < mFaninNum; ++ i ) {
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
int
BnLogicNode::fanin_num() const
{
  return mFaninNum;
}

// @brief ファンインを求める．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
int
BnLogicNode::fanin(int pos) const
{
  ASSERT_COND( pos < fanin_num() );
  return mFanins[pos];
}

// @brief ファンインのノード番号のリストを返す．
Array<int>
BnLogicNode::fanin_list() const
{
  return Array<int>(mFanins, 0, fanin_num());
}

// @brief セルを返す．
//
// is_logic() == false の時の動作は不定
// 場合によっては nullptr を返す．
const ClibCell*
BnLogicNode::cell() const
{
  return mCell;
}

// @brief ファンインを設定する．
// @param[in] ipos 入力位置
// @param[in] fanin_id ファンインのノード番号
void
BnLogicNode::set_fanin(int ipos,
		       int fanin_id)
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
BnPrimNode::BnPrimNode(int id,
		       const string& name,
		       int ni,
		       BnNodeType logic_type,
		       const ClibCell* cell) :
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
BnExprNode::BnExprNode(int id,
		       const string& name,
		       int ni,
		       const Expr& expr,
		       int expr_id,
		       const ClibCell* cell) :
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
  return BnNodeType::Expr;
}

// @brief 論理式番号を返す．
//
// logic_type() == BnNodeType::Expr の時のみ意味を持つ．
// 論理式番号は同じ BnNetwork 内で唯一となるもの．
int
BnExprNode::expr_id() const
{
  return mExprId;
}

// @brief 論理式を返す．
//
// type() != BnNodeType::Expr の時の動作は不定
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
BnTvNode::BnTvNode(int id,
		   const string& name,
		   int ni,
		   const TvFunc& func,
		   int func_id,
		   const ClibCell* cell) :
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
  return BnNodeType::TvFunc;
}

// @brief 関数番号を返す．
//
// type() == BnNodeType::TvFunc の時のみ意味を持つ．
// 関数番号は同じ BnNetwork 内で唯一となるもの．
int
BnTvNode::func_id() const
{
  return mFuncId;
}

// @brief 真理値表を返す．
//
// is_logic() == false の時の動作は不定
// logic_type() != BnNodeType::TvFunc の時の動作は不定
TvFunc
BnTvNode::func() const
{
  return mFunc;
}

END_NAMESPACE_YM_BNET
