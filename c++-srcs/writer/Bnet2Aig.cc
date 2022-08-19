
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

BEGIN_NONAMESPACE

// TvFunc を AIG に変換する．
SizeType
make_tv(
  AigWriter& aig,
  const TvFunc& func,
  const vector<SizeType>& fanin_lits
)
{
  auto expr = func.MWC_expr();
  return aig.make_expr(expr, fanin_lits);
}

// BnNode の内容を AIG に変換する．
SizeType
make_aig(
  AigWriter& aig,
  const BnNetwork& bnet,
  const BnNode& node,
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
    return aig.make_and(fanin_lits);
  case BnNodeType::Nand: // NAND
    return aig.make_nand(fanin_lits);
  case BnNodeType::Or:   // OR
    return aig.make_or(fanin_lits);
  case BnNodeType::Nor:  // NOR
    return aig.make_nor(fanin_lits);
  case BnNodeType::Xor:  // XOR
    return aig.make_xor(fanin_lits);
  case BnNodeType::Xnor: // XNOR
    return aig.make_xnor(fanin_lits);
  case BnNodeType::Expr: // 論理式
    return aig.make_expr(bnet.expr(node.expr_id()), fanin_lits);
  case BnNodeType::TvFunc: // 真理値表
    return make_tv(aig, bnet.func(node.func_id()), fanin_lits);
  default:
    ASSERT_NOT_REACHED;
  }
  return static_cast<SizeType>(-1);
}

// BnNetwork の情報を AigWriter に変換する．
void
bnet2aig(
  AigWriter& aig,
  const BnNetwork& bnet,
  const string& comment
)
{
  SizeType I = bnet.input_num();
  SizeType L = bnet.dff_num();
  SizeType O = bnet.output_num();
  aig.initialize(I, L, O);

  // ノード番号をキーにしてリテラルを格納する辞書
  unordered_map<SizeType, SizeType> lit_map;

  // 入力ノードを登録する．
  for ( SizeType i = 0; i < I; ++ i ) {
    lit_map.emplace(bnet.input_id(i), (i + 1) * 2);
  }
  // ラッチノード(DFFの出力ノード)を登録する．
  for ( SizeType i = 0; i < L; ++ i ) {
    const auto& dff = bnet.dff(i);
    lit_map.emplace(dff.data_out(), (i + I + 1) * 2);
  }
  // AND ノードを生成する．
  for ( auto id: bnet.logic_id_list() ) {
    const auto& node = bnet.node(id);
    SizeType ni = node.fanin_num();
    // ファンインのリテラルのリスト
    vector<SizeType> fanin_list(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto fid = node.fanin_id(i);
      ASSERT_COND( lit_map.count(fid) > 0 );
      auto ilit = lit_map.at(fid);
      fanin_list[i] = ilit;
    }
    auto olit = make_aig(aig, bnet, node, fanin_list);
    lit_map.emplace(id, olit);
  }
  // ラッチのソースを設定する．
  for ( SizeType i = 0; i < L; ++ i ) {
    const auto& dff = bnet.dff(i);
    auto src_id = dff.data_in();
    ASSERT_COND( lit_map.count(src_id) > 0 );
    auto src = lit_map.at(src_id);
    aig.set_latch_src(i, src);
  }
  // 出力のソースを設定する．
  for ( SizeType i = 0; i < O; ++ i ) {
    auto src_id = bnet.output_src_id(i);
    ASSERT_COND( lit_map.count(src_id) > 0 );
    auto src = lit_map.at(src_id);
    aig.set_output_src(i, src);
  }
  // 入力のシンボル名を設定する．
  for ( SizeType i = 0; i < I; ++ i ) {
    const auto& node = bnet.node(bnet.input_id(i));
    auto name = node.name();
    if ( name != string{} ) {
      aig.set_input_symbol(i, name);
    }
  }
  // ラッチのシンボル名を設定する．
  for ( SizeType i = 0; i < L; ++ i ) {
    const auto& dff = bnet.dff(i);
    auto name = dff.name();
    if ( name != string{} ) {
      aig.set_latch_symbol(i, name);
    }
  }
  // 出力のシンボル名を設定する．
  for ( SizeType i = 0; i < O; ++ i ) {
    const auto& node = bnet.node(bnet.output_id(i));
    auto name = node.name();
    if ( name != string{} ) {
      aig.set_output_symbol(i, name);
    }
  }
  // コメントを設定する．
  if ( comment != string{} ) {
    aig.set_comment(comment);
  }
}

END_NONAMESPACE


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
  if ( !is_concrete() ) {
    cerr << "Cannot convert to aig." << endl;
    return;
  }

  AigWriter aig;
  bnet2aig(aig, *this, comment);
  aig.write_aig(s);
}

// @brief 内容を aag (ascii aig) 形式で出力する．
void
BnNetwork::write_aag(
  ostream& s,
  const string& comment
) const
{
  if ( !is_concrete() ) {
    cerr << "Cannot convert to aag." << endl;
    return;
  }

  AigWriter aig;
  bnet2aig(aig, *this, comment);
  aig.write_aag(s);
}

END_NAMESPACE_YM_BNET
