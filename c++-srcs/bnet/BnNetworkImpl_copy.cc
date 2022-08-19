
/// @file BnNetworkImpl_copy.cc
/// @brief BnNetworkImpl のコピー関係の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "BnNetworkImpl.h"
#include "BnPortImpl.h"
#include "BnDffImpl.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief 内容をコピーする．
void
BnNetworkImpl::copy(
  const BnNetworkImpl& src
)
{
  if ( &src == this ) {
    // 自分自身がソースならなにもしない．
    return;
  }

  clear();

  // セルライブラリの設定
  set_library(src.library());

  // ネットワーク名の設定
  set_name(src.name());

  // srcのノード番号をキーにしてノード番号を格納する配列
  vector<SizeType> id_map(src.node_num());

  // ポートの生成
  auto np = src.port_num();
  for ( auto i: Range(np) ) {
    auto& src_port = src.port(i);
    string port_name = src_port.name();

    // 各ビットの方向を求める．
    auto nb = src_port.bit_width();
    vector<BnDir> dirs(nb);
    for ( auto i: Range(nb) ) {
      auto id = src_port.bit(i);
      auto& node = src.node(id);
      if ( node.is_input() ) {
	dirs[i] = BnDir::INPUT;
      }
      else if ( node.is_output() ) {
	dirs[i] = BnDir::OUTPUT;
      }
      else {
	ASSERT_NOT_REACHED;
      }
    }

    // ポートの生成
    auto dst_port_id = new_port(port_name, dirs);
    ASSERT_COND( src_port.id() == dst_port_id );

    // 各ビットの対応関係を記録する．
    auto& dst_port = port(dst_port_id);
    for ( auto i: Range(nb) ) {
      auto src_id = src_port.bit(i);
      auto dst_id = dst_port.bit(i);
      id_map[src_id] = dst_id;
    }
  }

  // src の入力の対応するノードを input_list に入れる．
  auto input_num = src.input_num();
  vector<SizeType> input_list(input_num);
  for ( auto i: Range(input_num) ) {
    auto src_id = src.input_id(i);
    auto dst_id = id_map[src_id];
    input_list[i] = dst_id;
  }

  // src 本体をインポートする．
  auto output_list = import_subnetwork(src, input_list);

  // 出力端子のファンインの接続
  auto output_num = src.output_num();
  for ( auto i: Range(output_num) ) {
    auto src_id = src.output_id(i);
    auto src_fanin_id = src.output_src_id(i);

    auto dst_id = id_map[src_id];
    auto dst_fanin_id = output_list[i];
    set_output(dst_id, dst_fanin_id);
  }

  bool stat = wrap_up();

  ASSERT_COND( stat );
}

// @brief 部分回路を追加する．
vector<SizeType>
BnNetworkImpl::import_subnetwork(
  const BnNetworkImpl& src_network,
  const vector<SizeType>& input_list
)
{
  ASSERT_COND( src_network.mSane );

  auto input_num = src_network.input_num();
  ASSERT_COND( input_list.size() == input_num );

  auto output_num = src_network.output_num();
  vector<SizeType> output_list;
  output_list.reserve(output_num);

  // src_network のノード番号をキーにして生成したノード番号を入れる配列
  vector<SizeType> id_map(src_network.node_num());

  // src_network の入力と input_list の対応関係を id_map に入れる．
  for ( auto i: Range(input_num) ) {
    auto src_id = src_network.input_id(i);
    auto dst_id = input_list[i];
    id_map[src_id] = dst_id;
  }

  // DFFを作る．
  for ( auto src_dff_p: src_network.mDffList ) {
    _copy_dff(*src_dff_p, id_map);
  }

  // 論理ノードの生成
  for ( auto src_id: src_network.logic_id_list() ) {
    auto& src_node = src_network.node(src_id);
    auto dst_id = _copy_logic(src_node, src_network, id_map);
  }

  // src_network の外部出力のファンインに対応するノード番号を
  // output_list に入れる．
  for ( auto src_id: src_network.output_src_id_list() ) {
    auto dst_id = id_map[src_id];
    output_list.push_back(dst_id);
  }

  return output_list;
}

// @brief DFFを複製する．
SizeType
BnNetworkImpl::_copy_dff(
  const BnDff& src_dff,
  vector<SizeType>& id_map
)
{
  string dff_name = src_dff.name();
  bool has_clear = src_dff.clear() != BNET_NULLID;
  bool has_preset = src_dff.preset() != BNET_NULLID;
  auto cpv = src_dff.clear_preset_value();
  auto dst_id = new_dff(dff_name, has_clear, has_preset, cpv);
  auto& dst_dff = dff(dst_id);

#warning "TODO: type() 別のコード"
  // 各端子の対応関係を記録する．
  {
    auto src_id = src_dff.data_in();
    auto dst_id = dst_dff.data_in();
    id_map[src_id] = dst_id;
  }
  {
    auto src_id = src_dff.data_out();
    auto dst_id = dst_dff.data_out();
    id_map[src_id] = dst_id;
  }
  {
    auto src_id = src_dff.clock();
    auto dst_id = dst_dff.clock();
    id_map[src_id] = dst_id;
  }
  if ( has_clear ) {
    auto src_id = src_dff.clear();
    auto dst_id = dst_dff.clear();
    id_map[src_id] = dst_id;
  }
  if ( has_preset ) {
    auto src_id = src_dff.preset();
    auto dst_id = dst_dff.preset();
    id_map[src_id] = dst_id;
  }

  return dst_dff.id();
}

#if 0
// @brief ラッチを複製する．
SizeType
BnNetworkImpl::_copy_latch(
  const BnLatch& src_latch,
  vector<SizeType>& id_map
)
{
  string latch_name = src_latch.name();
  bool has_clear = src_latch.clear() != BNET_NULLID;
  bool has_preset = src_latch.preset() != BNET_NULLID;
  auto cpv = src_latch.clear_preset_value();
  auto& dst_latch = _new_latch(latch_name, has_clear, has_preset, cpv);

  // 各端子の対応関係を記録する．
  {
    auto src_id = src_latch.input();
    auto dst_id = dst_latch.input();
    id_map[src_id] = dst_id;
  }
  {
    auto src_id = src_latch.output();
    auto dst_id = dst_latch.output();
    id_map[src_id] = dst_id;
  }
  {
    auto src_id = src_latch.enable();
    auto dst_id = dst_latch.enable();
    id_map[src_id] = dst_id;
  }
  if ( has_clear ) {
    auto src_id = src_latch.clear();
    auto dst_id = dst_latch.clear();
    id_map[src_id] = dst_id;
  }
  if ( has_preset ) {
    auto src_id = src_latch.preset();
    auto dst_id = dst_latch.preset();
    id_map[src_id] = dst_id;
  }

  return dst_latch.id();
}
#endif

// @brief 論理ノードを複製する．
SizeType
BnNetworkImpl::_copy_logic(
  const BnNode& src_node,
  const BnNetworkImpl& src_network,
  vector<SizeType>& id_map
)
{
  ASSERT_COND( src_node.is_logic() );

  auto nfi = src_node.fanin_num();
  string name = src_node.name();
  BnNodeType logic_type = src_node.type();
  SizeType expr_id = BNET_NULLID;
  SizeType func_id = BNET_NULLID;
  Bdd bdd = Bdd::invalid();
  if ( logic_type == BnNodeType::Expr ) {
    const Expr& expr = src_network.expr(src_node.expr_id());
    expr_id = _reg_expr(expr);
  }
  else if ( logic_type == BnNodeType::TvFunc ) {
    const TvFunc& func = src_network.func(src_node.func_id());
    func_id = _reg_tv(func);
  }
  else if ( logic_type == BnNodeType::Bdd ) {
    bdd = mBddMgr.copy(src_node.bdd());
  }
  vector<SizeType> fanin_id_list(nfi);
  for ( auto i: Range(nfi) ) {
    auto src_iid = src_node.fanin_id(i);
    auto iid = id_map[src_iid];
    fanin_id_list[i] = iid;
  }
  auto dst_id = _reg_logic(name, logic_type,
			   expr_id, func_id, bdd,
			   fanin_id_list);
  id_map[src_node.id()] = dst_id;

  return dst_id;
}

END_NAMESPACE_YM_BNET
