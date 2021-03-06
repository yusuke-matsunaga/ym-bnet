﻿
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
vector<int>
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
int
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
int
BnTvNode::func_id() const
{
  return mFuncId;
}

END_NAMESPACE_YM_BNET
