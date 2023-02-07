
/// @file read_aig.cc
/// @brief read_aig の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "Aig2Bnet.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/AigModel.h"


BEGIN_NAMESPACE_YM_BNET

// Aig を BnNetwork に変換する．
void
Aig2Bnet::conv(
  const AigModel& aig,
  const string& clock_name,
  const string& reset_name
)
{
  mLitMap.clear();

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
    auto port = new_input_port(name);
    auto node = port.bit(0);
    auto lit = aig.input(i);
    mLitMap.emplace(lit, node);
  }

  // 出力ポートの生成
  vector<BnNode> output_list(no);
  for ( SizeType i = 0; i < no; ++ i ) {
    auto name = aig.output_symbol(i);
    if ( name == string{} ) {
      ostringstream buf;
      buf << "o" << i;
      name = buf.str();
    }
    auto port = new_output_port(name);
    output_list[i] = port.bit(0);
  }

  // ラッチの生成
  vector<BnNode> latch_list(nl);
  BnNode clock_node;
  BnNode reset_node;
  if ( nl > 0 ) {
    // クロック入力の生成
    auto clock_port = new_input_port(clock_name);
    clock_node = clock_port.bit(0);
    // リセット入力の生成
    auto reset_port = new_input_port(reset_name);
    reset_node = reset_port.bit(0);
  }
  for ( SizeType i = 0; i < nl; ++ i ) {
    auto name = aig.latch_symbol(i);
    if ( name == string{} ) {
      ostringstream buf;
      buf << "l" << i;
      name = buf.str();
    }
    auto dff = new_dff(name, true);
    set_output_src(dff.clock(), clock_node);
    set_output_src(dff.clear(), reset_node);
    auto node = dff.data_out();
    auto lit = aig.latch(i);
    mLitMap.emplace(lit, node);
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
      auto src_node = mLitMap.at(lit);
      auto node1 = new_not(string{}, src_node);
      mLitMap.emplace(lit1, node1);
    }
  }

  // ANDノードの生成
  for ( SizeType i = 0; i < na; ++ i ) {
    ostringstream buf;
    buf << "a" << i;
    auto src1 = aig.and_src1(i);
    auto src2 = aig.and_src2(i);

    auto l1 = Expr::make_posi_literal(0);
    if ( mLitMap.count(src1) == 0 ) {
      l1 = ~l1;
      src1 ^= 1UL;
    }
    ASSERT_COND( mLitMap.count(src1) > 0 );
    auto i1 = mLitMap.at(src1);

    auto l2 = Expr::make_posi_literal(1);
    if ( mLitMap.count(src2) == 0 ) {
      l2 = ~l2;
      src2 ^= 1UL;
    }
    ASSERT_COND( mLitMap.count(src2) > 0 );
    auto i2 = mLitMap.at(src2);

    auto expr = l1 & l2;
    auto lit = aig.and_node(i);
    auto lit1 = lit ^ 1UL;
    if ( !req_map[lit] && req_map[lit1] ) {
      expr = ~expr;
      auto node1 = new_logic_expr(buf.str(), expr, {i1, i2});
      mLitMap.emplace(lit1, node1);
    }
    else {
      auto node = new_logic_expr(buf.str(), expr, {i1, i2});
      mLitMap.emplace(lit, node);
      if ( req_map[lit1] ) {
	auto node1 = new_not(string{}, node);
	mLitMap.emplace(lit1, node1);
      }
    }
  }

  // 出力の接続
  for ( SizeType i = 0; i < no; ++ i ) {
    auto src_lit = aig.output_src(i);
    ASSERT_COND ( mLitMap.count(src_lit) >  0 );
    auto src_node = mLitMap.at(src_lit);
    set_output_src(output_list[i], src_node);
  }

  // ラッチの入力の接続
  for ( SizeType i = 0; i < nl; ++ i ) {
    auto src_lit = aig.latch_src(i);
    ASSERT_COND( mLitMap.count(src_lit) > 0 );
    auto src_node = mLitMap.at(src_lit);
    set_output_src(latch_list[i], src_node);
  }
}

// @brief .aag 形式のファイルを読み込む．
BnNetwork
BnNetwork::read_aag(
  const string& filename,
  const string& clock_name,
  const string& reset_name
)
{
  AigModel aig;
  if ( !aig.read_aag(filename) ) {
    ostringstream buff;
    buff << "Error in read_aag(\"" << filename << "\"";
    throw std::invalid_argument{buff.str()};
  }

  Aig2Bnet op;
  op.conv(aig, clock_name, reset_name);
  return BnNetwork{std::move(op)};
}

// @brief .aig 形式のファイルを読み込む．
BnNetwork
BnNetwork::read_aig(
  const string& filename,
  const string& clock_name,
  const string& reset_name
)
{
  AigModel aig;
  if ( !aig.read_aig(filename) ) {
    ostringstream buff;
    buff << "Error in read_aig(\"" << filename << "\"";
    throw std::invalid_argument{buff.str()};
  }

  Aig2Bnet op;
  op.conv(aig, clock_name, reset_name);
  return BnNetwork{std::move(op)};
}

END_NAMESPACE_YM_BNET
