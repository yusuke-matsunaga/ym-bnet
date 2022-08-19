
/// @file BnLogicNode.cc
/// @brief BnLogicNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BnLogicNode.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnLogicNode
//////////////////////////////////////////////////////////////////////

// @brief 外部入力の時 true を返す．
bool
BnLogicNode::is_logic() const
{
  return true;
}

// @brief ファンイン数を得る．
SizeType
BnLogicNode::fanin_num() const
{
  return mFanins.size();
}

// @brief ファンインを求める．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
SizeType
BnLogicNode::fanin_id(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < fanin_num() );

  return mFanins[pos];
}

// @brief ファンインのノード番号のリストを返す．
vector<SizeType>
BnLogicNode::fanin_id_list() const
{
  return mFanins;
}

// @brief ファンインを設定する．
// @param[in] ipos 入力位置
// @param[in] fanin_id ファンインのノード番号
void
BnLogicNode::set_fanin(
  SizeType ipos,
  SizeType fanin_id
)
{
  ASSERT_COND( ipos >= 0 && ipos < fanin_num() );

  mFanins[ipos] = fanin_id;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPrimNode
//////////////////////////////////////////////////////////////////////

// @brief タイプを返す．
BnNodeType
BnPrimNode::type() const
{
  return mLogicType;
}


//////////////////////////////////////////////////////////////////////
// クラス BnExprNode
//////////////////////////////////////////////////////////////////////

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
SizeType
BnExprNode::expr_id() const
{
  return mExprId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnTvNode
//////////////////////////////////////////////////////////////////////

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
SizeType
BnTvNode::func_id() const
{
  return mFuncId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnBddNode
//////////////////////////////////////////////////////////////////////

// @brief タイプを返す．
BnNodeType
BnBddNode::type() const
{
  return BnNodeType::Bdd;
}

// @brief Bdd を返す．
Bdd
BnBddNode::bdd() const
{
  return mBdd;
}

END_NAMESPACE_YM_BNET
