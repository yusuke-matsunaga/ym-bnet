
/// @file BnNetworkImpl_copy.cc
/// @brief BnNetworkImpl のコピー関係の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "BnNetworkImpl.h"
#include "BnPortImpl.h"
#include "BnDffImpl.h"
#include "ym/Bdd.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief 内容をコピーする．
void
BnNetworkImpl::copy(
  const BnNetworkImpl* src
)
{
  if ( src == this ) {
    // 自分自身がソースならなにもしない．
    return;
  }

  unordered_map<SizeType, SizeType> id_map;

  clear();

  // セルライブラリの設定
  set_library(src->library());

  // ネットワーク名の設定
  set_name(src->name());

  // ポートの生成
  for ( auto src_port_p: src->mPortList ) {
    copy_port(src_port_p, src, id_map);
  }

  // DFFを作る．
  for ( auto src_dff_p: src->mDffList ) {
    copy_dff(src_dff_p, id_map);
  }

  // 論理ノードの生成
  for ( auto src_id: src->logic_id_list() ) {
    auto src_node = src->_node(src_id);
    copy_logic(src_node, src, id_map);
  }

  // 出力端子のファンインの接続
  for ( auto src_id: src->output_id_list() ) {
    auto src_node = src->_node(src_id);
    copy_output(src_node, id_map);
  }

  wrap_up();
}

// @brief ポートの情報のみをコピーする．
void
BnNetworkImpl::make_skelton_copy(
  const BnNetworkImpl* src,
  unordered_map<SizeType, SizeType>& id_map
)
{
  clear();

  // セルライブラリの設定
  set_library(src->library());

  // ネットワーク名の設定
  set_name(src->name());

  // ポートの生成
  for ( auto src_port_p: src->mPortList ) {
    copy_port(src_port_p, src, id_map);
  }
}

// @brief 部分回路を追加する．
vector<SizeType>
BnNetworkImpl::import_subnetwork(
  const BnNetworkImpl* src_network,
  const vector<SizeType>& input_list
)
{
  ASSERT_COND( src_network->mSane );

  auto input_num = src_network->primary_input_num();
  ASSERT_COND( input_list.size() == input_num );

  auto output_num = src_network->primary_output_num();
  vector<SizeType> output_list;
  output_list.reserve(output_num);

  // src_network のノード番号をキーにして生成したノード番号を入れる配列
  unordered_map<SizeType, SizeType> id_map;

  // src_network の外部入力と input_list の対応関係を id_map に入れる．
  for ( auto i: Range(input_num) ) {
    auto src_id = src_network->primary_input_id(i);
    auto dst_id = input_list[i];
    id_map.emplace(src_id, dst_id);
  }

  // DFFを作る．
  for ( auto src_dff_p: src_network->mDffList ) {
    copy_dff(src_dff_p, id_map);
  }

  // 論理ノードの生成
  for ( auto src_id: src_network->logic_id_list() ) {
    auto src_node = src_network->_node(src_id);
    copy_logic(src_node, src_network, id_map);
  }

  // src_network の外部出力のファンインに対応するノード番号を
  // output_list に入れる．
  for ( auto src_id: src_network->primary_output_id_list() ) {
    auto src_node = src_network->_node(src_id);
    auto src_iid = src_node->output_src();
    ASSERT_COND( id_map.count(src_iid) > 0 );
    auto dst_iid = id_map.at(src_iid);
    output_list.push_back(dst_iid);
  }

  return output_list;
}

// @brief ポートの情報をコピーする．
SizeType
BnNetworkImpl::copy_port(
  const BnPortImpl* src_port,
  const BnNetworkImpl* src_network,
  unordered_map<SizeType, SizeType>& id_map
)
{
  string port_name = src_port->name();

  // 各ビットの方向を求める．
  auto nb = src_port->bit_width();
  vector<BnDir> dirs(nb);
  for ( auto i: Range(nb) ) {
    auto id = src_port->bit(i);
    auto node = src_network->_node(id);
    if ( node->is_input() ) {
      dirs[i] = BnDir::INPUT;
    }
    else if ( node->is_output() ) {
      dirs[i] = BnDir::OUTPUT;
    }
    else {
      ASSERT_NOT_REACHED;
    }
  }

  // ポートの生成
  auto dst_port_id = new_port(port_name, dirs);
  ASSERT_COND( src_port->id() == dst_port_id );

  // 各ビットの対応関係を記録する．
  auto dst_port = _port(dst_port_id);
  for ( auto i: Range(nb) ) {
    auto src_id = src_port->bit(i);
    auto dst_id = dst_port->bit(i);
    id_map.emplace(src_id, dst_id);
  }

  return dst_port_id;
}

// @brief DFFを複製する．
SizeType
BnNetworkImpl::copy_dff(
  const BnDffImpl* src_dff,
  unordered_map<SizeType, SizeType>& id_map
)
{
  string dff_name = src_dff->name();
  SizeType dst_id{BNET_NULLID};
  if ( src_dff->is_dff() || src_dff->is_latch() ) {
    bool has_clear = src_dff->clear() != BNET_NULLID;
    bool has_preset = src_dff->preset() != BNET_NULLID;
    auto cpv = src_dff->clear_preset_value();
    if ( src_dff->is_dff() ) {
      dst_id = new_dff(dff_name, has_clear, has_preset, cpv);
    }
    else {
      dst_id = new_latch(dff_name, has_clear, has_preset, cpv);
    }
    auto dst_dff = _dff(dst_id);

    // 各端子の対応関係を記録する．
    {
      auto src_id = src_dff->data_in();
      auto dst_id = dst_dff->data_in();
      id_map.emplace(src_id, dst_id);
    }
    {
      auto src_id = src_dff->data_out();
      auto dst_id = dst_dff->data_out();
      id_map.emplace(src_id, dst_id);
    }
    {
      auto src_id = src_dff->clock();
      auto dst_id = dst_dff->clock();
      id_map.emplace(src_id, dst_id);
    }
    if ( has_clear ) {
      auto src_id = src_dff->clear();
      auto dst_id = dst_dff->clear();
      id_map.emplace(src_id, dst_id);
    }
    if ( has_preset ) {
      auto src_id = src_dff->preset();
      auto dst_id = dst_dff->preset();
      id_map.emplace(src_id, dst_id);
    }
  }
  else if ( src_dff->is_cell() ) {
    SizeType cell_id = src_dff->cell_id();
    dst_id = new_dff_cell(dff_name, cell_id);
    auto dst_dff = _dff(dst_id);
    SizeType ni = src_dff->cell_input_num();
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto src_id = src_dff->cell_input(i);
      auto dst_id = dst_dff->cell_input(i);
      id_map.emplace(src_id, dst_id);
    }
    SizeType no = src_dff->cell_output_num();
    for ( SizeType i = 0; i < no; ++ i ) {
      auto src_id = src_dff->cell_output(i);
      auto dst_id = dst_dff->cell_output(i);
      id_map.emplace(src_id, dst_id);
    }
  }

  return dst_id;
}

// @brief 論理ノードを複製する．
SizeType
BnNetworkImpl::copy_logic(
  const BnNodeImpl* src_node,
  const BnNetworkImpl* src_network,
  unordered_map<SizeType, SizeType>& id_map
)
{
  ASSERT_COND( src_node->is_logic() );

  auto nfi = src_node->fanin_num();
  string name = src_node->name();
  auto node_type = src_node->type();
  vector<SizeType> fanin_id_list(nfi);
  for ( auto i: Range(nfi) ) {
    auto src_iid = src_node->fanin_id(i);
    ASSERT_COND( id_map.count(src_iid) > 0 );
    auto iid = id_map.at(src_iid);
    fanin_id_list[i] = iid;
  }
  SizeType dst_id{BNET_NULLID};
  if ( node_type == BnNodeType::Expr ) {
    auto expr = src_network->expr(src_node->expr_id());
    dst_id = new_logic_expr(name, expr, fanin_id_list);
  }
  else if ( node_type == BnNodeType::TvFunc ) {
    auto& func = src_network->func(src_node->func_id());
    dst_id = new_logic_tv(name, func, fanin_id_list);
  }
  else if ( node_type == BnNodeType::Bdd ) {
    dst_id = new_logic_bdd(name, src_node->bdd(), fanin_id_list);
  }
  else if ( node_type == BnNodeType::Cell ) {
    dst_id = new_logic_cell(name, src_node->cell_id(), fanin_id_list);
  }
  else if ( node_type == BnNodeType::Prim ) {
    auto prim_type = src_node->primitive_type();
    dst_id = new_logic_primitive(name, prim_type, fanin_id_list);
  }
  ASSERT_COND( _check_node_id(dst_id) );
  id_map.emplace(src_node->id(), dst_id);

  return dst_id;
}

// @brief 出力ノードを複製する．
void
BnNetworkImpl::copy_output(
  const BnNodeImpl* src_node,
  unordered_map<SizeType, SizeType>& id_map
)
{
  ASSERT_COND( src_node->is_output() );
  auto src_id = src_node->id();
  ASSERT_COND( id_map.count(src_id) > 0 );
  auto dst_id = id_map.at(src_id);
  auto dst_node = _node(dst_id);
  auto src_iid = src_node->output_src();
  if ( id_map.count(src_iid) == 0 ) {
    cout << src_iid << " not found" << endl;
    abort();
  }
  ASSERT_COND( id_map.count(src_iid) > 0 );
  auto dst_fanin_id = id_map.at(src_iid);
  set_output_src(dst_node, dst_fanin_id);
}

END_NAMESPACE_YM_BNET
