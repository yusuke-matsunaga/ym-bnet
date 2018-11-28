
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
// @param[in] ni 入力数
// @param[in] cell_id セル番号 (-1 の場合もあり)
BnLogicNode::BnLogicNode(int id,
			 const string& name,
			 int ni,
			 int cell_id) :
  BnNodeImpl(id, name),
  mFanins(ni, kBnNullId),
  mCellId(cell_id)
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
int
BnLogicNode::fanin_num() const
{
  return mFanins.size();
}

// @brief ファンインを求める．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
int
BnLogicNode::fanin_id(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < fanin_num() );

  return mFanins[pos];
}

// @brief ファンインのノード番号のリストを返す．
const vector<int>&
BnLogicNode::fanin_id_list() const
{
  return mFanins;
}

// @brief セル番号を返す．
//
// is_logic() == false の時の動作は不定
// 場合によっては nullptr を返す．
int
BnLogicNode::cell_id() const
{
  return mCellId;
}

// @brief ファンインを設定する．
// @param[in] ipos 入力位置
// @param[in] fanin_id ファンインのノード番号
void
BnLogicNode::set_fanin(int ipos,
		       int fanin_id)
{
  ASSERT_COND( ipos >= 0 && ipos < fanin_num() );

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
// @param[in] cell_id セル番号 (nullptr の場合もあり)
BnPrimNode::BnPrimNode(int id,
		       const string& name,
		       int ni,
		       BnNodeType logic_type,
		       int cell_id) :
  BnLogicNode(id, name, ni, cell_id),
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
// @param[in] expr_id 論理式番号
// @param[in] cell_id セル番号 (-1 の場合もあり)
BnExprNode::BnExprNode(int id,
		       const string& name,
		       int ni,
		       int expr_id,
		       int cell_id) :
  BnLogicNode(id, name, ni, cell_id),
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


//////////////////////////////////////////////////////////////////////
// クラス BnTvNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] ni 入力数
// @param[in] func_id 関数番号
// @param[in] cell_id セル (-1 の場合もあり)
BnTvNode::BnTvNode(int id,
		   const string& name,
		   int ni,
		   int func_id,
		   int cell_id) :
  BnLogicNode(id, name, ni, cell_id),
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

END_NAMESPACE_YM_BNET
