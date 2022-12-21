
/// @file OutputSplit.cc
/// @brief OutputSplit の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "OutputSplit.h"


BEGIN_NAMESPACE_YM_BNET

// @brief 1つの出力に関係したノードのみからなるネットワークを返す．
BnNetwork
BnNetwork::output_split(
  SizeType output_id
) const
{
  OutputSplit op;
  op.split(*this, output_id);
  return BnNetwork{std::move(op)};
}


BEGIN_NONAMESPACE

void
get_support_sub(
  const BnNetwork& network,
  SizeType node_id,
  unordered_set<SizeType>& marks,
  vector<SizeType>& node_list,
  vector<SizeType>& input_list
)
{
  if ( marks.count(node_id) ) {
    // 処理済み
    return;
  }
  marks.emplace(node_id);
  auto& node = network.node(node_id);
  if ( node.is_input() ) {
    input_list.push_back(node_id);
  }
  else {
    SizeType ni = node.fanin_num();
    for ( SizeType i = 0; i < ni; ++ i ) {
      get_support_sub(network, node.fanin_id(i),
		      marks, node_list, input_list);
    }
    node_list.push_back(node_id);
  }
}

END_NONAMESPACE

// @brief 処理を行う本体
void
OutputSplit::split(
  const BnNetwork& src_network,
  SizeType output_pos
)
{
  // 関係するノードに印をつける．
  auto& output = src_network.output_node(output_pos);
  unordered_set<SizeType> marks;
  vector<SizeType> node_list;
  vector<SizeType> input_list;
  get_support_sub(src_network, output.output_src(),
		  marks, node_list, input_list);

  clear();
  mIdMap.clear();

  // 入力を複製する．
  for ( auto src_id: input_list ) {
    auto& src_node = src_network.node(src_id);
    auto port_id = new_input_port(src_node.name());
    auto& dst_port = port(port_id);
    auto dst_id = dst_port.bit(0);
    mIdMap.emplace(src_id, dst_id);
  }

  // 論理ノードを複製する．
  for ( auto src_id: node_list ) {
    auto& src_node = src_network.node(src_id);
    auto name = src_node.name();
    auto dst_id = copy_logic(src_node, src_network, mIdMap);
    mIdMap.emplace(src_id, dst_id);
  }

  // 出力ノードを複製する．
  {
    auto src_id = output.output_src();
    ASSERT_COND( mIdMap.count(src_id) > 0 );
    auto dst_id = mIdMap.at(src_id);
    auto port_id = new_output_port(output.name());
    auto& dst_port = port(port_id);
    auto dst_oid = dst_port.bit(0);
    set_output_src(dst_oid, dst_id);
  }
}

END_NAMESPACE_YM_BNET
