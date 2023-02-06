
/// @file simple_decomp.cc
/// @brief simple_decomp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/Range.h"
#include "SimpleDecomp.h"


BEGIN_NAMESPACE_YM_BNET

// @brief 単純なノードに分解する．
BnNetwork
BnNetwork::simple_decomp() const
{
  SimpleDecomp op;
  op.decomp(*this);
  return BnNetwork{std::move(op)};
}

// @brief 各ノードが simple primitive になるように分解する．
void
SimpleDecomp::decomp(
  const BnNetwork& src_network
)
{
  unordered_map<SizeType, BnNode> node_map;

  make_skelton_copy(src_network, node_map);

  // DFFをコピーする．
  for ( auto src_dff: src_network.dff_list() ) {
    copy_dff(src_dff, node_map);
  }

  // 論理ノードを分解しつつコピーする．
  for ( auto src_node: src_network.logic_list() ) {
    BnNode dst_node;
    if ( src_node.type() == BnNodeType::Expr ) {
      SizeType ni = src_node.fanin_num();
      mTermList.clear();
      mTermList.resize(ni * 2);
      for ( SizeType i = 0; i < ni; ++ i ) {
	auto src_iid = src_node.fanin_id(i);
	ASSERT_COND( node_map.count(src_iid) > 0 );
	auto dst_inode = node_map.at(src_iid);
	mTermList[i * 2 + 0] = dst_inode;
      }
      auto expr = src_network.expr(src_node.expr_id());
      dst_node = decomp_expr(expr);
    }
    else {
      dst_node = copy_logic(src_node, node_map);
    }
    node_map.emplace(src_node.id(), dst_node);
  }

  // 出力のファンインをセットする．
  for ( auto src_node: src_network.output_list() ) {
    copy_output(src_node, node_map);
  }
}

// @brief 論理式型のノードを分解する．
BnNode
SimpleDecomp::decomp_expr(
  const Expr& expr
)
{
  if ( expr.is_posi_literal() ) {
    auto varid = expr.varid();
    auto index = varid * 2 + 0;
    return mTermList[index];
  }
  else if ( expr.is_nega_literal() ) {
    auto varid = expr.varid();
    auto index = varid * 2 + 1;
    if ( mTermList[index].is_invalid() ) {
      mTermList[index] = new_not(string{}, mTermList[index - 1]);
    }
    return mTermList[index];
  }
  // 定数はありえない．
  ASSERT_COND( expr.is_op() );

  vector<BnNode> new_fanin_list;
  new_fanin_list.reserve(expr.operand_num());
  for ( auto& opr: expr.operand_list() ) {
    auto inode = decomp_expr(opr);
    new_fanin_list.push_back(inode);
  }
  auto node_type = BnNodeType::None;
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

  auto node = new_logic_primitive({}, node_type, new_fanin_list);
  return node;
}

END_NAMESPACE_YM_BNET
