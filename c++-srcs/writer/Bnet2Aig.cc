
/// @file Bnet2Aig.cc
/// @brief Bnet2Aig の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "AigWriter.h"
#include "ym/BnNetwork.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"


BEGIN_NAMESPACE_YM_BNET

// @brief BnNetwork の内容を変換する．
// @return 変換できない場合は false を返す．
bool
AigWriter::conv_from_bnet(
  const BnNetwork& src_network
)
{
  SizeType I = src_network.input_num();
  SizeType L = src_network.dff_num();
  SizeType O = src_network.output_num();

  // Latchタイプ，Cellタイプの BnDff を持つとき変換不能
  for ( SizeType i = 0; i < L; ++ i ) {
    const auto& dff = src_network.dff(i);
    if ( dff.type() != BnDffType::Dff ) {
      return false;
    }
  }
  // TvFuncタイプ，Bddタイプの論理ノードを持つ時変換不能
  for ( auto id: src_network.logic_id_list() ) {
    auto& node = src_network.node(id);
    if ( node.type() == BnNodeType::TvFunc ||
	 node.type() == BnNodeType::Bdd ) {
      return false;
    }
  }

  initialize(I, L, O);

  // ノード番号をキーにしてリテラルを格納する辞書
  unordered_map<SizeType, SizeType> lit_map;

  // 入力ノードを登録する．
  for ( SizeType i = 0; i < I; ++ i ) {
    lit_map.emplace(src_network.input_id(i), (i + 1) * 2);
  }
  // ラッチノード(DFFの出力ノード)を登録する．
  for ( SizeType i = 0; i < L; ++ i ) {
    const auto& dff = src_network.dff(i);
    lit_map.emplace(dff.data_out(), (i + I + 1) * 2);
  }
  // AND ノードを生成する．
  for ( auto id: src_network.logic_id_list() ) {
    const auto& node = src_network.node(id);
    SizeType ni = node.fanin_num();
    // ファンインのリテラルのリスト
    vector<SizeType> fanin_list(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto fid = node.fanin_id(i);
      ASSERT_COND( lit_map.count(fid) > 0 );
      auto ilit = lit_map.at(fid);
      fanin_list[i] = ilit;
    }
    auto olit = make_bnnode(node,  src_network, fanin_list);
    lit_map.emplace(id, olit);
  }
  // ラッチのソースを設定する．
  for ( SizeType i = 0; i < L; ++ i ) {
    const auto& dff = src_network.dff(i);
    auto src_id = dff.data_in();
    ASSERT_COND( lit_map.count(src_id) > 0 );
    auto src = lit_map.at(src_id);
    set_latch_src(i, src);
  }
  // 出力のソースを設定する．
  for ( SizeType i = 0; i < O; ++ i ) {
    auto src_id = src_network.output_src_id(i);
    ASSERT_COND( lit_map.count(src_id) > 0 );
    auto src = lit_map.at(src_id);
    set_output_src(i, src);
  }
  // 入力のシンボル名を設定する．
  for ( SizeType i = 0; i < I; ++ i ) {
    const auto& node = src_network.node(src_network.input_id(i));
    auto name = node.name();
    if ( name != string{} ) {
      set_input_symbol(i, name);
    }
  }
  // ラッチのシンボル名を設定する．
  for ( SizeType i = 0; i < L; ++ i ) {
    const auto& dff = src_network.dff(i);
    auto name = dff.name();
    if ( name != string{} ) {
      set_latch_symbol(i, name);
    }
  }
  // 出力のシンボル名を設定する．
  for ( SizeType i = 0; i < O; ++ i ) {
    const auto& node = src_network.node(src_network.output_id(i));
    auto name = node.name();
    if ( name != string{} ) {
      set_output_symbol(i, name);
    }
  }
  return true;
}

// BnNode の内容を AIG に変換する．
SizeType
AigWriter::make_bnnode(
  const BnNode& node,
  const BnNetwork& network,
  const vector<SizeType>& fanin_lits
)
{
  switch ( node.type() ) {
  case BnNodeType::C0:   // 定数0
    return 0;
  case BnNodeType::C1:   // 定数1
    return 1;
  case BnNodeType::Buff: // バッファ
    return fanin_lits[0];
  case BnNodeType::Not:  // インバータ
    return fanin_lits[0] ^ 1;
  case BnNodeType::And:  // AND
    return make_and(fanin_lits);
  case BnNodeType::Nand: // NAND
    return make_nand(fanin_lits);
  case BnNodeType::Or:   // OR
    return make_or(fanin_lits);
  case BnNodeType::Nor:  // NOR
    return make_nor(fanin_lits);
  case BnNodeType::Xor:  // XOR
    return make_xor(fanin_lits);
  case BnNodeType::Xnor: // XNOR
    return make_xnor(fanin_lits);
  case BnNodeType::Expr: // 論理式
    return make_expr(network.expr(node.expr_id()), fanin_lits);
  default:
    ASSERT_NOT_REACHED;
  }
  return static_cast<SizeType>(-1);
}

// @brief 内容を aig 形式で出力する．
void
BnNetwork::write_aig(
  const string& filename,
  const string& comment
) const
{
  ofstream s{filename};
  if ( s ) {
    write_aig(s, comment);
  }
}

// @brief 内容を aag (ascii aig) 形式で出力する．
void
BnNetwork::write_aag(
  const string& filename,
  const string& comment
) const
{
  ofstream s{filename};
  if ( s ) {
    write_aag(s, comment);
  }
}

// @brief 内容を aig 形式で出力する．
void
BnNetwork::write_aig(
  ostream& s,
  const string& comment
) const
{
  AigWriter aig;
  if ( aig.conv_from_bnet(*this) ) {
    aig.write_aig(s, comment);
  }
  else {
    cerr << "Cannot convert to aig." << endl;
    return;
  }
}

// @brief 内容を aag (ascii aig) 形式で出力する．
void
BnNetwork::write_aag(
  ostream& s,
  const string& comment
) const
{
  AigWriter aig;
  if ( aig.conv_from_bnet(*this) ) {
    aig.write_aag(s, comment);
  }
  else {
    cerr << "Cannot convert to aag." << endl;
    return;
  }
}

END_NAMESPACE_YM_BNET
