
/// @file read_aig.cc
/// @brief read_aig の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "AigReader.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

// Aig を BnNetwork に変換する．
BnNetwork
aig2bnet(
  const AigReader& aig,
  const string& clock_name,
  const string& reset_name
)
{
  // 結果のネットワーク
  BnNetwork network;

  // AIG のリテラルと BnNetwork のノード番号の対応表
  unordered_map<SizeType, SizeType> lit_map;

  SizeType ni = aig.I();
  SizeType no = aig.O();
  SizeType nl = aig.L();
  SizeType na = aig.A();

  // 入力ポートの生成
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto name = aig.input_symbol(i);
    if ( name == string{} ) {
      ostringstream buf;
      buf << "i" << i;
      name = buf.str();
    }
    auto id = network.new_input_port(name);
    const auto& port = network.port(id);
    auto node_id = port.bit(0);
    auto lit = aig.input(i);
    lit_map.emplace(lit, node_id);
  }

  // 出力ポートの生成
  vector<SizeType> output_list(no);
  for ( SizeType i = 0; i < no; ++ i ) {
    auto name = aig.output_symbol(i);
    if ( name == string{} ) {
      ostringstream buf;
      buf << "o" << i;
      name = buf.str();
    }
    auto id = network.new_output_port(name);
    const auto& port = network.port(id);
    auto node_id = port.bit(0);
    output_list[i] = node_id;
  }

  // ラッチの生成
  vector<SizeType> latch_list(nl);
  SizeType clock_id{BNET_NULLID};
  SizeType reset_id{BNET_NULLID};
  if ( nl > 0 ) {
    // クロック入力の生成
    auto clock_port_id = network.new_input_port(clock_name);
    const auto& clock_port = network.port(clock_port_id);
    clock_id = clock_port.bit(0);
    // リセット入力の生成
    auto reset_port_id = network.new_input_port(reset_name);
    const auto& reset_port = network.port(reset_port_id);
    reset_id = reset_port.bit(0);
  }
  for ( SizeType i = 0; i < nl; ++ i ) {
    auto name = aig.latch_symbol(i);
    if ( name == string{} ) {
      ostringstream buf;
      buf << "l" << i;
      name = buf.str();
    }
    auto id = network.new_dff(name, true);
    const auto& dff = network.dff(id);
    network.set_output(dff.clock(), clock_id);
    network.set_output(dff.clear(), reset_id);
    auto node_id = dff.data_out();
    auto lit = aig.latch(i);
    lit_map.emplace(lit, node_id);
    latch_list[i] = dff.data_in();
  }

  // 必要とされている極性を調べる．
  vector<bool> req_map((ni + nl + na + 1) * 2, false);
  for ( SizeType i = 0; i < no; ++ i ) {
    auto src = aig.output_src(i);
    req_map[src] = true;
  }
  for ( SizeType i = 0; i < nl; ++ i ) {
    auto src = aig.latch_src(i);
    req_map[src] = true;
  }

  // 入力の否定に出力やラッチ入力が直接接続している場合は
  // インバータが必要
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto lit = aig.input(i);
    auto lit1 = lit ^ 1UL;
    if ( req_map[lit1] ) {
      auto src_id = lit_map.at(lit);
      auto id1 = network.new_not(string{}, src_id);
      lit_map.emplace(lit1, id1);
    }
  }

  // ANDノードの生成
  for ( SizeType i = 0; i < na; ++ i ) {
    ostringstream buf;
    buf << "a" << i;
    auto src1 = aig.and_src1(i);
    auto src2 = aig.and_src2(i);

    auto l1 = Expr::make_posi_literal(VarId{0});
    if ( lit_map.count(src1) == 0 ) {
      l1 = ~l1;
      src1 ^= 1UL;
    }
    ASSERT_COND( lit_map.count(src1) > 0 );
    SizeType i1 = lit_map.at(src1);

    auto l2 = Expr::make_posi_literal(VarId{1});
    if ( lit_map.count(src2) == 0 ) {
      l2 = ~l2;
      src2 ^= 1UL;
    }
    ASSERT_COND( lit_map.count(src2) > 0 );
    SizeType i2 = lit_map.at(src2);

    auto expr = l1 & l2;
    auto lit = aig.and_node(i);
    auto lit1 = lit ^ 1UL;
    if ( !req_map[lit] && req_map[lit1] ) {
      expr = ~expr;
      auto id1 = network.new_logic(buf.str(), expr, {i1, i2});
      lit_map.emplace(lit1, id1);
    }
    else {
      auto id = network.new_logic(buf.str(), expr, {i1, i2});
      lit_map.emplace(lit, id);
      if ( req_map[lit1] ) {
	auto id1 = network.new_not(string{}, id);
	lit_map.emplace(lit1, id1);
      }
    }
  }

  // 出力の接続
  for ( SizeType i = 0; i < no; ++ i ) {
    auto src_lit = aig.output_src(i);
    ASSERT_COND ( lit_map.count(src_lit) >  0 );
    auto src_id = lit_map.at(src_lit);
    network.set_output(output_list[i], src_id);
  }

  // ラッチの入力の接続
  for ( SizeType i = 0; i < nl; ++ i ) {
    auto src_lit = aig.latch_src(i);
    ASSERT_COND( lit_map.count(src_lit) > 0 );
    auto src_id = lit_map.at(src_lit);
    network.set_output(latch_list[i], src_id);
  }
  network.wrap_up();

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
  AigReader aig;
  if ( !aig.read_aag(filename) ) {
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
  AigReader aig;
  if ( !aig.read_aig(filename) ) {
    throw BnetError{"Error in read_aag"};
  }

  return aig2bnet(aig, clock_name, reset_name);
}

END_NAMESPACE_YM_BNET
