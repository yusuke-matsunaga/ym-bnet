
/// @file read_aig.cc
/// @brief read_aig の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/Aig.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

BnNetwork
aig2bnet(
  const Aig& aig,
  const string& clock_name,
  const string& reset_name
)
{
  BnNetwork network;

  unordered_map<SizeType, SizeType> lit_map;

  // 入力ポートの生成
  SizeType ni = aig.I();
  for ( SizeType i = 0; i < ni; ++ i ) {
    ostringstream buf;
    buf << "i" << i;
    auto id = network.new_input_port(buf.str());
    const auto& port = network.port(id);
    auto node_id = port.bit(0);
    auto lit = aig.input(i);
    lit_map.emplace(lit, node_id);
  }

  // 出力ポートの生成
  SizeType no = aig.O();
  vector<SizeType> output_list(no);
  for ( SizeType i = 0; i < no; ++ i ) {
    ostringstream buf;
    buf << "o" << i;
    auto id = network.new_output_port(buf.str());
    const auto& port = network.port(id);
    auto node_id = port.bit(0);
    output_list[i] = node_id;
  }

  // ラッチの生成
  SizeType nl = aig.L();
  vector<SizeType> latch_list(nl);
  if ( nl > 0 ) {
    // クロック入力の生成
    auto clock_port_id = network.new_input_port(clock_name);
    const auto& clock_port = network.port(clock_port_id);
    // リセット入力の生成
    auto reset_port_id = network.new_input_port(reset_name);
    const auto& reset_port = network.port(reset_port_id);
  }
  for ( SizeType i = 0; i < nl; ++ i ) {
    ostringstream buf;
    buf << "l" << i;
    auto id = network.new_dff(buf.str(), true);
    const auto& dff = network.dff(id);
    auto node_id = dff.output();
    auto lit = aig.latch(i);
    lit_map.emplace(lit, node_id);
    latch_list[i] = dff.input();
  }

  // ANDノードの生成
  SizeType na = aig.A();
  for ( SizeType i = 0; i < na; ++ i ) {
    ostringstream buf;
    buf << "a" << i;
    auto src1 = aig.and_src1(i);
    auto src2 = aig.and_src2(i);
    auto l1 = Expr::make_posi_literal(VarId{0});
    auto l2 = Expr::make_posi_literal(VarId{1});
    if ( src1 & 1 ) {
      l1 = ~l1;
      src1 &= ~1UL;
    }
    if ( src2 & 1 ) {
      l2 = ~l2;
      src2 &= ~1UL;
    }
    if ( lit_map.count(src1) == 0 ) {
      cerr << src1 << " is not defined" << endl;
      abort();
    }
    SizeType i1 = lit_map.at(src1);
    if ( lit_map.count(src2) == 0 ) {
      cerr << src2 << " is not defined" << endl;
      abort();
    }
    SizeType i2 = lit_map.at(src2);
    auto expr = l1 & l2;
    auto id = network.new_logic(buf.str(), expr, {i1, i2});
    auto lit = aig.and_node(i);
    lit_map.emplace(lit, id);
  }

  // 出力の接続
  for ( SizeType i = 0; i < no; ++ i ) {
    auto src_lit = aig.output_src(i);
    if ( lit_map.count(src_lit) == 0 ) {
      cerr << src_lit << " is not defined" << endl;
      abort();
    }
    auto src_id = lit_map.at(src_lit);
    network.connect(src_id, output_list[i], 0);
  }

  // ラッチの入力の接続
  for ( SizeType i = 0; i < nl; ++ i ) {
    auto src_lit = aig.latch_src(i);
    if ( lit_map.count(src_lit) == 0 ) {
      cerr << src_lit << " is not defined" << endl;
      abort();
    }
    auto src_id = lit_map.at(src_lit);
    network.connect(src_id, latch_list[i], 0);
  }

  return network;
}

END_NONAMESPACE

// @brief .aag 形式のファイルを読み込む．
BnNetwork
BnNetwork::read_aag(
  const string& filename,
  const string& clock_name,
  const string& reset_name
)
{
  Aig aig;
  if ( !aig.read_aig(filename) ) {
    throw BnetError{"Error in read_aag"};
  }

  return aig2bnet(aig, clock_name, reset_name);
}

// @brief .aig 形式のファイルを読み込む．
BnNetwork
BnNetwork::read_aig(
  const string& filename,
  const string& clock_name,
  const string& reset_name
)
{
  Aig aig;
  if ( !aig.read_aig(filename) ) {
    throw BnetError{"Error in read_aag"};
  }

  return aig2bnet(aig, clock_name, reset_name);
}

END_NAMESPACE_YM_BNET
