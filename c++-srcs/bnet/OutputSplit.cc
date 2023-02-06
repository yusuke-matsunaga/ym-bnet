
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
  BnNode node,
  unordered_set<SizeType>& marks,
  vector<SizeType>& node_list,
  vector<SizeType>& input_list
)
{
  if ( marks.count(node.id()) ) {
    // 処理済み
    return;
  }
  marks.emplace(node.id());
  if ( node.is_input() ) {
    input_list.push_back(node.id());
  }
  else {
    for ( auto inode: node.fanin_list() ) {
      get_support_sub(network, inode,
		      marks, node_list, input_list);
    }
    node_list.push_back(node.id());
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
  auto output = src_network.output_node(output_pos);
  unordered_set<SizeType> marks;
  vector<SizeType> node_list;
  vector<SizeType> input_list;
  get_support_sub(src_network, output.output_src(),
		  marks, node_list, input_list);

  clear();
  mNodeMap.clear();

  // 入力を複製する．
  for ( auto src_id: input_list ) {
    auto src_node = src_network.node(src_id);
    auto dst_port = new_input_port(src_node.name());
    auto dst_node = dst_port.bit(0);
    mNodeMap.put(src_id, dst_node);
  }

  // 論理ノードを複製する．
  for ( auto src_id: node_list ) {
    auto src_node = src_network.node(src_id);
    auto dst_node = copy_logic(src_node, mNodeMap);
    mNodeMap.put(src_id, dst_node);
  }

  // 出力ノードを複製する．
  {
    auto src_node = output.output_src();
    SizeType src_id = src_node.id();
    ASSERT_COND( mNodeMap.is_in(src_id) );
    auto dst_inode = mNodeMap.get(src_id);
    auto dst_port = new_output_port(output.name());
    auto dst_node = dst_port.bit(0);
    set_output_src(dst_node, dst_inode);
  }
}

END_NAMESPACE_YM_BNET
