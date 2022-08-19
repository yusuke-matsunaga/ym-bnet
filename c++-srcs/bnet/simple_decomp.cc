
/// @file simple_decomp.cc
/// @brief simple_decomp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

class Decomp
{
public:

  Decomp(
    BnNetwork& network,
    const BnNode& node
  );

  ~Decomp() = default;


public:

  /// @brief 論理式を分解する．
  SizeType
  decomp_expr(
    SizeType id,
    const Expr& expr
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  BnNetwork& mNetwork;

  // 入力のノード番号
  vector<SizeType> mTermList;

};


// @brief 単純なノードに分解する．
void
BnNetwork::simple_decomp()
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
    const Expr& expr = this->expr(node.expr_id());
    SizeType ni = expr.input_size();
    Decomp decomp{*this, node};
    decomp.decomp_expr(id, expr);
  }

  wrap_up();
}

Decomp::Decomp(
  BnNetwork& network,
  const BnNode& node
) : mNetwork{network},
    mTermList(node.fanin_num(), BNET_NULLID)
{
  SizeType ni = node.fanin_num();
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto iid = node.fanin_id(i);
    mTermList[i * 2 + 0] = iid;
  }
}

// @brief 論理式型のノードを分解する．
SizeType
Decomp::decomp_expr(
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
      mTermList[index] = mNetwork.new_not(string(),
					  mTermList[index - 1]);
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
    id = mNetwork.new_logic(string(), node_type, new_fanin_list);
  }
  else {
    mNetwork.change_primitive(id, node_type, new_fanin_list);
  }
  return id;
}

END_NAMESPACE_YM_BNET
