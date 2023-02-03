
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
  mClockId = BNET_NULLID;

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
    ASSERT_COND( mIdMap.count(src_id) > 0 );
    auto inode_id = mIdMap.at(src_id);
    mNetwork.set_output_src(id, inode_id);
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
  auto port_id = mNetwork.new_input_port(oname);
  const auto& port = mNetwork.port(port_id);
  auto id = port.bit(0);
  mIdMap.emplace(src_id, id);
}

// @brief 出力の設定を行う．
void
Bench2Bnet::set_output(
  SizeType src_id
)
{
  auto name = mModel.node_name(src_id);
  string name1;
  if ( mNetwork.find_port(name) == static_cast<SizeType>(-1) ) {
    name1 = name;
  }
  auto port_id = mNetwork.new_output_port(name1);
  auto& port = mNetwork.port(port_id);
  auto id = port.bit(0);
  ASSERT_COND( mIdMap.count(src_id) > 0 );
  auto inode_id = mIdMap.at(src_id);
  mNetwork.set_output_src(id, inode_id);
}

// @brief DFF を作る．
void
Bench2Bnet::make_dff(
  SizeType src_id
)
{
  auto oname = mModel.node_name(src_id);

  // この形式ではクロック以外の制御端子はない．
  SizeType dff_id = mNetwork.new_dff(oname);
  const BnDff& dff = mNetwork.dff(dff_id);

  SizeType output_id = dff.data_out();
  mIdMap.emplace(src_id, output_id);

  SizeType input_id = dff.data_in();
  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  auto inode_id = mModel.node_input(src_id);
  mOutputMap.emplace(input_id, inode_id);

  if ( mClockId == BNET_NULLID ) {
    // クロックのポートを作る．
    auto port_id = mNetwork.new_input_port(mClockName);
    auto& clock_port = mNetwork.port(port_id);
    // クロックの入力ノード番号を記録する．
    mClockId = clock_port.bit(0);
  }

  // クロック入力とdffのクロック端子を結びつける．
  mNetwork.set_output_src(dff.clock(), mClockId);
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
  vector<SizeType> fanin_id_list;
  fanin_id_list.reserve(ni);
  for ( auto iid: mModel.node_fanin_list(src_id) ) {
    ASSERT_COND( mIdMap.count(iid) > 0 );
    fanin_id_list.push_back(mIdMap.at(iid));
  }

  auto type = mModel.node_gate_type(src_id);
  auto gtype = BnNodeType::None;
  switch ( type ) {
  case Iscas89Gate::C0:   gtype = BnNodeType::C0; break;
  case Iscas89Gate::C1:   gtype = BnNodeType::C1; break;
  case Iscas89Gate::Buff: gtype = BnNodeType::Buff; break;
  case Iscas89Gate::Not:  gtype = BnNodeType::Not; break;
  case Iscas89Gate::And:  gtype = BnNodeType::And; break;
  case Iscas89Gate::Nand: gtype = BnNodeType::Nand; break;
  case Iscas89Gate::Or:   gtype = BnNodeType::Or; break;
  case Iscas89Gate::Nor:  gtype = BnNodeType::Nor; break;
  case Iscas89Gate::Xor:  gtype = BnNodeType::Xor; break;
  case Iscas89Gate::Xnor: gtype = BnNodeType::Xor; break;
  case Iscas89Gate::Mux: break;
  default: ASSERT_NOT_REACHED; break;
  }
  SizeType id;
  if ( type == Iscas89Gate::Mux ) {
    id = make_mux(oname, fanin_id_list);
  }
  else {
    id = mNetwork.new_logic_primitive(oname, gtype, fanin_id_list);
  }
  mIdMap.emplace(src_id, id);
}

SizeType
Bench2Bnet::make_mux(
  const string& oname,
  const vector<SizeType>& fanin_id_list
)
{
  SizeType ni = fanin_id_list.size();
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
  auto id = mNetwork.new_logic_expr(oname, mux_expr, fanin_id_list);
  return id;
}

END_NAMESPACE_YM_BNET
