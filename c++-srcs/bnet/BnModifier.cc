
/// @file BnModifier.cc
/// @brief BnModifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnModifier.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnNode.h"
#include "BnNetworkImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnModifier
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BnModifier::BnModifier(
)
{
}

// @brief BnNetwork からのムーブコンストラクタ
BnModifier::BnModifier(
  BnNetwork&& src
)
{
  mImpl.swap(src.mImpl);
}

// @brief BnNetwork からのムーブ代入演算
BnModifier&
BnModifier::operator=(
  BnNetwork&& src
)
{
  mImpl.swap(src.mImpl);
  return *this;
}

// @brief デストラクタ
BnModifier::~BnModifier()
{
}

// @brief 内容をクリアする．
//
// コンストラクタ直後と同じ状態になる．
void
BnModifier::clear()
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->clear();
}

// @brief セルライブラリをセットする．
void
BnModifier::set_library(
  const ClibCellLibrary& library
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->set_library(library);
}

// @brief ネットワーク名を設定する．
void
BnModifier::set_name(
  const string& name
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->set_name(name);
}

// @brief 1ビットの入力ポートを作る．
BnPort
BnModifier::new_input_port(
  const string& port_name
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_port(port_name, {BnDir::INPUT});
  return BnPort{mImpl.get(), id};
}

// @brief 多ビットの入力ポートを作る．
BnPort
BnModifier::new_input_port(
  const string& port_name,
  SizeType bit_width
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_port(port_name,
			    vector<BnDir>(bit_width, BnDir::INPUT));
  return BnPort{mImpl.get(), id};
}

// @brief 1ビットの出力ポートを作る．
BnPort
BnModifier::new_output_port(
  const string& port_name
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_port(port_name, {BnDir::OUTPUT});
  return BnPort{mImpl.get(), id};
}

// @brief 多ビットの出力ポートを作る．
BnPort
BnModifier::new_output_port(
  const string& port_name,
  SizeType bit_width
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_port(port_name,
			    vector<BnDir>(bit_width, BnDir::OUTPUT));
  return BnPort{mImpl.get(), id};
}

// @brief 入出力混合のポートを作る．
BnPort
BnModifier::new_port(
  const string& port_name,
  const vector<BnDir>& dir_vect
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_port(port_name, dir_vect);
  return BnPort{mImpl.get(), id};
}

// @brief DFFを追加する．
BnDff
BnModifier::new_dff(
  const string& name,
  bool has_clear,
  bool has_preset,
  BnCPV cpv
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_dff(name, has_clear, has_preset, cpv);
  return BnDff{mImpl.get(), id};
}

// @brief セルの情報を持ったDFFを追加する．
BnDff
BnModifier::new_dff_cell(
  const string& name,
  SizeType cell_id
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_dff_cell(name, cell_id);
  return BnDff{mImpl.get(), id};
}

// @brief ラッチを追加する．
BnDff
BnModifier::new_latch(
  const string& name,
  bool has_clear,
  bool has_preset,
  BnCPV cpv
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_latch(name, has_clear, has_preset, cpv);
  return BnDff{mImpl.get(), id};
}

// @brief プリミティブ型の論理ノードを追加する．
BnNode
BnModifier::new_logic_primitive(
  const string& node_name,
  BnNodeType logic_type,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  auto id = mImpl->new_logic_primitive(node_name, logic_type, fanin_id_list);
  return BnNode{mImpl.get(), id};
}

// @brief 論理式型の論理ノードを追加する．
BnNode
BnModifier::new_logic_expr(
  const string& node_name,
  const Expr& expr,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  auto id = mImpl->new_logic_expr(node_name, expr, fanin_id_list);
  return BnNode{mImpl.get(), id};
}

// @brief 真理値表型の論理ノードを追加する．
BnNode
BnModifier::new_logic_tv(
  const string& node_name,
  const TvFunc& tv,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  auto id = mImpl->new_logic_tv(node_name, tv, fanin_id_list);
  return BnNode{mImpl.get(), id};
}

// @brief BDD型の論理ノードを追加する．
BnNode
BnModifier::new_logic_bdd(
  const string& node_name,
  const Bdd& bdd,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  auto id = mImpl->new_logic_bdd(node_name, bdd, fanin_id_list);
  return BnNode{mImpl.get(), id};
}

// @brief 論理セルを追加する．
BnNode
BnModifier::new_logic_cell(
  const string& node_name,
  SizeType cell_id,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  auto id = mImpl->new_logic_cell(node_name, cell_id, fanin_id_list);
  return BnNode{mImpl.get(), id};
}

// @brief C0型(定数０)の論理ノードを追加する．
BnNode
BnModifier::new_c0(
  const string& node_name
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_logic_primitive(node_name, BnNodeType::C0, {});
  return BnNode{mImpl.get(), id};
}

// @brief C1型(定数1)の論理ノードを追加する．
BnNode
BnModifier::new_c1(
  const string& node_name
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_logic_primitive(node_name, BnNodeType::C1, {});
  return BnNode{mImpl.get(), id};
}

// @brief Buff型の論理ノードを追加する．
BnNode
BnModifier::new_buff(
  const string& node_name,
  BnNode fanin
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_logic_primitive(node_name, BnNodeType::Buff, {fanin.id()});
  return BnNode{mImpl.get(), id};
}

// @brief Not型の論理ノードを追加する．
BnNode
BnModifier::new_not(
  const string& node_name,
  BnNode fanin
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id = mImpl->new_logic_primitive(node_name, BnNodeType::Not, {fanin.id()});
  return BnNode{mImpl.get(), id};
}

// @brief AND型の論理ノードを追加する．
BnNode
BnModifier::new_and(
  const string& node_name,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  auto id = mImpl->new_logic_primitive(node_name, BnNodeType::And, fanin_id_list);
  return BnNode{mImpl.get(), id};
}

// @brief NAND型の論理ノードを追加する．
BnNode
BnModifier::new_nand(
  const string& node_name,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  auto id = mImpl->new_logic_primitive(node_name, BnNodeType::Nand, fanin_id_list);
  return BnNode{mImpl.get(), id};
}

// @brief OR型の論理ノードを追加する．
BnNode
BnModifier::new_or(
  const string& node_name,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  auto id = mImpl->new_logic_primitive(node_name, BnNodeType::Or, fanin_id_list);
  return BnNode{mImpl.get(), id};
}

// @brief NOR型の論理ノードを追加する．
BnNode
BnModifier::new_nor(
  const string& node_name,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  auto id = mImpl->new_logic_primitive(node_name, BnNodeType::Nor, fanin_id_list);
  return BnNode{mImpl.get(), id};
}

// @brief XOR型の論理ノードを追加する．
BnNode
BnModifier::new_xor(
  const string& node_name,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  auto id = mImpl->new_logic_primitive(node_name, BnNodeType::Xor, fanin_id_list);
  return BnNode{mImpl.get(), id};
}

// @brief XNOR型の論理ノードを追加する．
BnNode
BnModifier::new_xnor(
  const string& node_name,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  auto id = mImpl->new_logic_primitive(node_name, BnNodeType::Xnor, fanin_id_list);
  return BnNode{mImpl.get(), id};
}

// @brief プリミティブ型の論理ノードに変更する．
void
BnModifier::change_primitive(
  BnNode node,
  BnNodeType logic_type,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  mImpl->change_primitive(node.id(), logic_type, fanin_id_list);
}

// @brief 論理式型の論理ノードに変更する．
void
BnModifier::change_expr(
  BnNode node,
  const Expr& expr,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  mImpl->change_expr(node.id(), expr, fanin_id_list);
}

// @brief 真理値表型の論理ノードに変更する．
void
BnModifier::change_tv(
  BnNode node,
  const TvFunc& tv,
  const vector<BnNode>& fanin_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto fanin_id_list = make_id_list(fanin_list);
  mImpl->change_tv(node.id(), tv, fanin_id_list);
}

// @brief ポートの情報のみコピーする．
void
BnModifier::make_skelton_copy(
  const BnNetwork& src_network,
  unordered_map<SizeType, BnNode>& node_map
)
{
  ASSERT_COND( mImpl != nullptr );

  auto id_map = make_id_map(node_map);
  mImpl->make_skelton_copy(src_network.mImpl.get(), id_map);
}

// @brief DFFをコピーする．
BnDff
BnModifier::copy_dff(
  BnDff src_dff,
  unordered_map<SizeType, BnNode>& node_map
)
{
  ASSERT_COND( mImpl != nullptr );

  auto dff_impl = src_dff._impl();
  auto id_map = make_id_map(node_map);
  auto id = mImpl->copy_dff(dff_impl, id_map);
  return BnDff{mImpl.get(), id};
}

// @brief 論理ノードをコピーする．
// @brief ノードを返す．
BnNode
BnModifier::copy_logic(
  BnNode src_node,
  unordered_map<SizeType, BnNode>& node_map
)
{
  ASSERT_COND( mImpl != nullptr );

  auto src_impl = src_node._impl();
  auto src_network = src_node._network();
  auto id_map = make_id_map(node_map);
  auto id = mImpl->copy_logic(src_impl, src_network, id_map);
  return BnNode{mImpl.get(), id};
}

// @brief 出力ノードを複製する．
void
BnModifier::copy_output(
  BnNode src_node,
  unordered_map<SizeType, BnNode>& node_map
)
{
  ASSERT_COND( mImpl != nullptr );
  auto src_impl = src_node._impl();
  auto id_map = make_id_map(node_map);
  return mImpl->copy_output(src_impl, id_map);
}

// @brief 部分回路を追加する．
vector<BnNode>
BnModifier::import_subnetwork(
  const BnNetwork& src_network,
  const vector<BnNode>& input_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto input_id_list = make_id_list(input_list);
  auto output_id_list = mImpl->import_subnetwork(src_network.mImpl.get(), input_id_list);
  return make_node_list(output_id_list);
}

// @brief 出力ノードのファンインを設定する．
void
BnModifier::set_output_src(
  BnNode onode,
  BnNode src_node
)
{
  ASSERT_COND( mImpl != nullptr );
  auto onode_impl = onode._impl();
  mImpl->set_output_src(onode_impl, src_node.id());
}

// @brief ファンアウトをつなぎ替える．
void
BnModifier::substitute_fanout(
  BnNode old_node,
  BnNode new_node
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->substitute_fanout(old_node.id(), new_node.id());
}

// @brief ノード番号のリストを作る．
vector<SizeType>
BnModifier::make_id_list(
  const vector<BnNode>& node_list
)
{
  vector<SizeType> id_list;
  id_list.reserve(node_list.size());
  for ( auto node: node_list ) {
    id_list.push_back(node.id());
  }
  return id_list;
}

// @brief ノードのリストを作る．
vector<BnNode>
BnModifier::make_node_list(
  const vector<SizeType>& id_list
)
{
  vector<BnNode> node_list;
  node_list.reserve(id_list.size());
  for ( SizeType id: id_list ) {
    node_list.push_back(BnNode{mImpl.get(), id});
  }
  return node_list;
}

// @brief ノード番号の辞書を作る．
unordered_map<SizeType, SizeType>
BnModifier::make_id_map(
  const unordered_map<SizeType, BnNode>& node_map
)
{
  unordered_map<SizeType, SizeType> id_map;
  for ( auto& p: node_map ) {
    auto key = p.first;
    auto node = p.second;
    id_map.emplace(key, node.id());
  }
  return id_map;
}

END_NAMESPACE_YM_BNET
