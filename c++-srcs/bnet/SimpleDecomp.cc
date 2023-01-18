
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
  unordered_map<SizeType, SizeType> id_map;

  make_skelton_copy(src_network, id_map);

  // DFFをコピーする．
  for ( auto& src_dff: src_network.dff_list() ) {
    copy_dff(src_dff, id_map);
  }

  // 論理ノードを分解しつつコピーする．
  for ( auto& src_node: src_network.logic_list() ) {
    SizeType dst_id;
    if ( src_node.type() == BnNodeType::Expr ) {
      SizeType ni = src_node.fanin_num();
      mTermList.clear();
      mTermList.resize(ni * 2, BNET_NULLID);
      for ( SizeType i = 0; i < ni; ++ i ) {
	auto src_iid = src_node.fanin_id(i);
	ASSERT_COND( id_map.count(src_iid) > 0 );
	auto dst_iid = id_map.at(src_iid);
	mTermList[i * 2 + 0] = dst_iid;
      }
      const Expr& expr = src_network.expr(src_node.expr_id());
      dst_id = decomp_expr(expr);
    }
    else {
      dst_id = copy_logic(src_node, src_network, id_map);
    }
    id_map.emplace(src_node.id(), dst_id);
  }

  // 出力のファンインをセットする．
  for ( auto& src_node: src_network.output_list() ) {
    copy_output(src_node, id_map);
  }
}

// @brief 論理式型のノードを分解する．
SizeType
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
    if ( mTermList[index] == BNET_NULLID ) {
      mTermList[index] = new_not(string{}, mTermList[index - 1]);
    }
    return mTermList[index];
  }
  // 定数はありえない．
  ASSERT_COND( expr.is_op() );

  vector<SizeType> new_fanin_list;
  new_fanin_list.reserve(expr.operand_num());
  for ( auto& opr: expr.operand_list() ) {
    auto iid = decomp_expr(opr);
    new_fanin_list.push_back(iid);
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

  auto id = new_logic_primitive({}, node_type, new_fanin_list);

  return id;
}

END_NAMESPACE_YM_BNET
