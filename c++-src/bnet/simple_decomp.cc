
/// @file simple_decomp.cc
/// @brief simple_decomp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

int
decomp_expr(BnNetwork& network,
	    const Expr& expr,
	    const vector<int>& fanin_id_list)
{
  if ( expr.is_posi_literal() ) {
    VarId varid = expr.varid();
    return fanin_id_list[varid.val()];
  }
  else if ( expr.is_nega_literal() ) {
    VarId varid = expr.varid();
    return network.new_logic(string(), BnNodeType::Not,
			     vector<int>{ fanin_id_list[varid.val()] });
  }
  // 定数はありえない．
  ASSERT_COND( expr.is_op() );

  int nc = expr.child_num();
  vector<int> new_fanin_list(nc);
  for ( int i: Range(nc) ) {
    int iid = decomp_expr(network, expr.child(i), fanin_id_list);
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

  return network.new_logic(string(), node_type, new_fanin_list);
}

int
decomp_tvfunc(BnNetwork& network,
	      const TvFunc& func,
	      const vector<int>& fanin_id_list)
{
  return -1;
}

END_NONAMESPACE

// @brief 単純なノードに分解する．
//
// 単純なノードとは以下のノード型
// * BnNodeType::C0
// * BnNodeType::C1
// * BnNodeType::Buff
// * BnNodeType::Not
// * BnNodeType::And
// * BnNodeType::Nand
// * BnNodeType::Or
// * BnNodeType::Nor
// * BnNodeType::Xor
// * BnNodeType::Xnor
void
BnNetwork::simple_decomp()
{
  for ( auto id: logic_id_list() ) {
    auto& node = this->node(id);
    int new_id = -1;
    switch ( node.type() ) {
    case BnNodeType::Expr:
      new_id = decomp_expr(*this, expr(node.expr_id()), node.fanin_id_list());
      // id のファンアウトを new_id に付け替える．
      substitute_fanout(id, new_id);
      break;

    case BnNodeType::TvFunc:
      new_id = decomp_tvfunc(*this, func(node.func_id()), node.fanin_id_list());
      // id のファンアウトを new_id に付け替える．
      substitute_fanout(id, new_id);
      break;

    default:
      break;
    }
  }

  wrap_up();
}

END_NAMESPACE_YM_BNET
