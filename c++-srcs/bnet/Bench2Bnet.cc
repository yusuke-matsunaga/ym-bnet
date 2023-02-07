﻿
/// @file Bench2Bnet.cc
/// @brief Bench2Bnet の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Bench2Bnet.h"
#include "ym/Iscas89Model.h"
#include "ym/BnNetwork.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief blif ファイルを読み込む．
BnNetwork
BnNetwork::read_iscas89(
  const string& filename,
  const string& clock_name
)
{
  Iscas89Model model;
  bool stat = model.read(filename);
  if ( !stat ) {
    ostringstream buff;
    buff << "Error in read_iscas89(\"" << filename << "\"";
    throw std::invalid_argument{buff.str()};
  }

  auto _clock_name = clock_name;
  if ( _clock_name == string{} ) {
    _clock_name = "clock";
  }

  Bench2Bnet conv{model, _clock_name};

  return conv.get_network();
}


//////////////////////////////////////////////////////////////////////
// クラス Bench2Bnet
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Bench2Bnet::Bench2Bnet(
  const Iscas89Model& model,
  const string& clock_name
) : mModel{model},
    mClockName{clock_name}
{
  mNetwork.set_name("iscas89_network");

  for ( SizeType src_id: model.input_list() ) {
    make_input(src_id);
  }
  for ( SizeType src_id: model.dff_list() ) {
    make_dff(src_id);
  }
  for ( SizeType src_id: model.gate_list() ) {
    make_gate(src_id);
  }
  for ( auto src_id: model.output_list() ) {
    set_output(src_id);
  }
  for ( auto& p: mOutputMap ) {
    auto id = p.first;
    auto src_id = p.second;
    ASSERT_COND( mNodeMap.count(src_id) > 0 );
    auto inode = mNodeMap.at(src_id);
    auto onode = mNetwork.node(id);
    mNetwork.set_output_src(onode, inode);
  }
}

// @brief 結果のネットワークを取り出す．
BnNetwork
Bench2Bnet::get_network()
{
  return BnNetwork{std::move(mNetwork)};
}

// @brief 入力ノードを作る．
void
Bench2Bnet::make_input(
  SizeType src_id
)
{
  auto oname = mModel.node_name(src_id);
  auto port = mNetwork.new_input_port(oname);
  auto node = port.bit(0);
  mNodeMap.emplace(src_id, node);
}

// @brief 出力の設定を行う．
void
Bench2Bnet::set_output(
  SizeType src_id
)
{
  auto name = mModel.node_name(src_id);
  string name1;
  if ( mNetwork.find_port(name).is_invalid() ) {
    name1 = name;
  }
  auto port = mNetwork.new_output_port(name1);
  auto onode = port.bit(0);
  ASSERT_COND( mNodeMap.count(src_id) > 0 );
  auto inode = mNodeMap.at(src_id);
  mNetwork.set_output_src(onode, inode);
}

// @brief DFF を作る．
void
Bench2Bnet::make_dff(
  SizeType src_id
)
{
  auto oname = mModel.node_name(src_id);

  // この形式ではクロック以外の制御端子はない．
  auto dff = mNetwork.new_dff(oname);

  auto output = dff.data_out();
  mNodeMap.emplace(src_id, output);

  auto input = dff.data_in();
  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  auto inode_id = mModel.node_input(src_id);
  mOutputMap.emplace(input.id(), inode_id);

  if ( mClock.is_invalid() ) {
    // クロックのポートを作る．
    auto clock_port = mNetwork.new_input_port(mClockName);
    // クロックの入力ノード番号を記録する．
    mClock = clock_port.bit(0);
  }

  // クロック入力とdffのクロック端子を結びつける．
  mNetwork.set_output_src(dff.clock(), mClock);
}

// @brief ゲートを作る．
void
Bench2Bnet::make_gate(
  SizeType src_id
)
{
  auto oname = mModel.node_name(src_id);

  // ファンインのノードを作る．
  SizeType ni = mModel.node_fanin_list(src_id).size();
  vector<BnNode> fanin_list;
  fanin_list.reserve(ni);
  for ( auto iid: mModel.node_fanin_list(src_id) ) {
    ASSERT_COND( mNodeMap.count(iid) > 0 );
    fanin_list.push_back(mNodeMap.at(iid));
  }

  auto gate_type = mModel.node_gate_type(src_id);
  auto node = mNetwork.new_logic_primitive(oname, gate_type, fanin_list);

  mNodeMap.emplace(src_id, node);
}

#if 0
BnNode
Bench2Bnet::make_mux(
  const string& oname,
  const vector<BnNode>& fanin_list
)
{
  SizeType ni = fanin_list.size();
  SizeType nc = 0;
  SizeType nd = 1;
  while ( nc + nd < ni ) {
    ++ nc;
    nd <<= 1;
  }
  ASSERT_COND( nc + nd == ni );

  vector<Expr> cinputs(nc);
  for ( SizeType var: Range(nc) ) {
    cinputs[var] = Expr::make_posi_literal(var);
  }
  vector<Expr> dinputs(nd);
  for ( SizeType i: Range(nd) ) {
    dinputs[i] = Expr::make_posi_literal(i + nc);
  }

  vector<Expr> or_fanins(nd);
  for ( SizeType p: Range(nd) ) {
    vector<Expr> and_fanins(nc + 1);
    for ( SizeType i: Range(nc) ) {
      if ( p & (1 << i) ) {
	and_fanins[i] = cinputs[i];
      }
      else {
	and_fanins[i] = ~cinputs[i];
      }
    }
    and_fanins[nc] = dinputs[p];
    or_fanins[p] = Expr::make_and(and_fanins);
  }
  auto mux_expr = Expr::make_or(or_fanins);
  auto node = mNetwork.new_logic_expr(oname, mux_expr, fanin_list);
  return node;
}
#endif

END_NAMESPACE_YM_BNET
