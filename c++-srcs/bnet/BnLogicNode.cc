
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

// @brief プリミティブ型のノードを作る．
BnNodeImpl*
BnNodeImpl::new_primitive(
  const string& name,                   ///< [in] 名前
  BnNodeType type,                      ///< [in] タイプ
  const vector<SizeType>& fanin_id_list ///< [in] ファンインの番号のリスト
)
{
  return new BnPrimNode{name, type, fanin_id_list};
}

// @brief タイプを返す．
BnNodeType
BnPrimNode::type() const
{
  return mLogicType;
}

// @brief プリミティブ型の論理ノードの時 true を返す．
bool
BnPrimNode::is_primitive_logic() const
{
  return true;
}

// @brief 自分と同じタイプのノードを作る．
BnNodeImpl*
BnPrimNode::duplicate(
  const string& name,
  const vector<SizeType>& fanin_id_list
)
{
  return new BnPrimNode{name, type(), fanin_id_list};
}


//////////////////////////////////////////////////////////////////////
// クラス BnExprNode
//////////////////////////////////////////////////////////////////////

// @brief 論理式型のノードを作る．
BnNodeImpl*
BnNodeImpl::new_expr(
  const string& name,
  SizeType expr_id,
  const vector<SizeType>& fanin_id_list
)
{
  return new BnExprNode{name, expr_id, fanin_id_list};
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
SizeType
BnExprNode::expr_id() const
{
  return mExprId;
}

// @brief 自分と同じタイプのノードを作る．
BnNodeImpl*
BnExprNode::duplicate(
  const string& name,
  const vector<SizeType>& fanin_id_list
)
{
  return new BnExprNode{name, expr_id(), fanin_id_list};
}


//////////////////////////////////////////////////////////////////////
// クラス BnTvNode
//////////////////////////////////////////////////////////////////////

// @brief 真理値表型のノードを作る．
BnNodeImpl*
BnNodeImpl::new_tv(
  const string& name,
  SizeType func_id,
  const vector<SizeType>& fanin_id_list
)
{
  return new BnTvNode{name, func_id, fanin_id_list};
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
SizeType
BnTvNode::func_id() const
{
  return mFuncId;
}

// @brief 自分と同じタイプのノードを作る．
BnNodeImpl*
BnTvNode::duplicate(
  const string& name,
  const vector<SizeType>& fanin_id_list
)
{
  return new BnTvNode{name, func_id(), fanin_id_list};
}


//////////////////////////////////////////////////////////////////////
// クラス BnBddNode
//////////////////////////////////////////////////////////////////////

// @brief BDD型のノードを作る．
BnNodeImpl*
BnNodeImpl::new_bdd(
  const string& name,
  const Bdd& bdd,
  const vector<SizeType>& fanin_id_list
)
{
  return new BnBddNode{name, bdd, fanin_id_list};
}

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

// @brief 自分と同じタイプのノードを作る．
BnNodeImpl*
BnBddNode::duplicate(
  const string& name,
  const vector<SizeType>& fanin_id_list
)
{
  return new BnBddNode{name, bdd(), fanin_id_list};
}


//////////////////////////////////////////////////////////////////////
// クラス BnCellNode
//////////////////////////////////////////////////////////////////////

// @brief 論理セル型のノードを作る．
BnNodeImpl*
BnNodeImpl::new_cell(
  const string& name,
  SizeType cell_id,
  const vector<SizeType>& fanin_id_list
)
{
  return new BnCellNode{name, cell_id, fanin_id_list};
}

// @brief タイプを返す．
BnNodeType
BnCellNode::type() const
{
  return BnNodeType::Cell;
}

// @brief セル番号を返す．
SizeType
BnCellNode::cell_id() const
{
  return mCellId;
}

// @brief 自分と同じタイプのノードを作る．
BnNodeImpl*
BnCellNode::duplicate(
  const string& name,
  const vector<SizeType>& fanin_id_list
)
{
  return new BnCellNode{name, cell_id(), fanin_id_list};
}

END_NAMESPACE_YM_BNET
