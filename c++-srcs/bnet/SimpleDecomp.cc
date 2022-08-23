
/// @file simple_decomp.cc
/// @brief simple_decomp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/Range.h"
#include "SimpleDecomp.h"


BEGIN_NAMESPACE_YM_BNET

// @brief 単純なノードに分解する．
BnNetwork
BnNetwork::simple_decomp() const
{
  SimpleDecomp op;
  return BnNetwork{std::move(op)};
}

// @brief 各ノードが simple primitive になるように分解する．
void
SimpleDecomp::decomp()
{
  // 分解するノードのリストを作る．
  vector<SizeType> node_id_list;
  node_id_list.reserve(logic_num());
  for ( auto id: logic_id_list() ) {
    auto& node = this->node(id);
    if ( node.type() == BnNodeType::Expr ) {
      node_id_list.push_back(id);
    }
  }

  // node_list の各ノードを分解する．
  for ( auto id: node_id_list ) {
    auto& node = this->node(id);
    decomp_node(node);
  }
}

// @brief ノードを分解する．
void
SimpleDecomp::decomp_node(
  const BnNode& node
)
{
  SizeType ni = node.fanin_num();
  mTermList.clear();
  mTermList.resize(ni * 2, BNET_NULLID);
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto iid = node.fanin_id(i);
    mTermList[i * 2 + 0] = iid;
  }
  const Expr& expr = this->expr(node.expr_id());
  decomp_expr(node.id(), expr);
}

// @brief 論理式型のノードを分解する．
SizeType
SimpleDecomp::decomp_expr(
  SizeType id,
  const Expr& expr
)
{
  if ( expr.is_posi_literal() ) {
    VarId varid = expr.varid();
    auto index = varid.val() * 2 + 0;
    return mTermList[index];
  }
  else if ( expr.is_nega_literal() ) {
    VarId varid = expr.varid();
    auto index = varid.val() * 2 + 1;
    if ( mTermList[index] == BNET_NULLID ) {
      mTermList[index] = new_not(string{}, mTermList[index - 1]);
    }
    return mTermList[index];
  }
  // 定数はありえない．
  ASSERT_COND( expr.is_op() );

  auto nc = expr.child_num();
  vector<SizeType> new_fanin_list(nc);
  for ( auto i: Range(nc) ) {
    auto iid = decomp_expr(BNET_NULLID, expr.child(i));
    new_fanin_list[i] = iid;
  }
  BnNodeType node_type{BnNodeType::None};
  if ( expr.is_and() ) {
    node_type = BnNodeType::And;
  }
  else if ( expr.is_or() ) {
    node_type = BnNodeType::Or;
  }
  else if ( expr.is_xor() ) {
    node_type = BnNodeType::Xor;
  }
  else {
    ASSERT_NOT_REACHED;
  }

  if ( id == BNET_NULLID ) {
    id = new_logic(string(), node_type, new_fanin_list);
  }
  else {
    change_primitive(id, node_type, new_fanin_list);
  }
  return id;
}

END_NAMESPACE_YM_BNET
