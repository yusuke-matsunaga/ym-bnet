
/// @file WriterBase.cc
/// @brief WriterBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "WriterBase.h"
#include "ym/BnNetwork.h"
#include "ym/Range.h"
#include "ym/NameMgr.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
WriterBase::WriterBase(
  const BnNetwork& network
) : mNetwork{network},
    mNameArray(network.node_num()),
    mDataArray(network.node_num(), false)
{
}

// @brief ノード名の解決を行う．
void
WriterBase::init_name_array(
  const string& prefix,
  const string& suffix
)
{
  // ノード名を管理するクラス
  NameMgr name_mgr{prefix, suffix};

  // ノード名のハッシュ
  unordered_set<string> name_hash;

  // もともと与えられた名前があればそれを使う．
  // ただし重複のチェックを行う．

  // 外部ポート名
  for ( auto port: mNetwork.port_list() ) {
    const string& name = port.name();
    if ( name == string{} ) {
      // 名無しの場合はスキップ
      continue;
    }

    auto nb = port.bit_width();
    if ( nb == 1 ) {
      // 1ビットポートならポート名をノード名にする．
      auto node = port.bit(0);
      reg_node_name(node, name, name_hash, name_mgr);
    }
    else {
      // 多ビットポートの場合は '[' <ビット番号> ']' を後ろにつける．
      for ( SizeType b: Range(nb) ) {
	auto node = port.bit(b);
	ostringstream buf;
	buf << name << "[" << b << "]";
	reg_node_name(node, buf.str(), name_hash, name_mgr);
      }
    }
  }

  // FF名
  for ( auto dff: mNetwork.dff_list() ) {
    if ( dff.is_dff() || dff.is_latch() ) {
      const string& name = dff.name();
      if ( name == string{} ) {
	// 名無しの場合はスキップ
	continue;
      }
      auto node = dff.data_out();
      reg_node_name(node, name, name_hash, name_mgr);
    }
  }

  // 外部入力ノード名
  for ( auto node: mNetwork.primary_input_list() ) {
    const string& name = node.name();
    reg_node_name(node, name, name_hash, name_mgr);
  }

  // FFの出力ノード名
  for ( auto dff: mNetwork.dff_list() ) {
    auto node = dff.data_out();
    const string& name = node.name();
    reg_node_name(node, name, name_hash, name_mgr);
  }

  // 外部出力ノード名
  for ( auto node: mNetwork.primary_output_list() ) {
    const string& name = node.name();
    if ( name == string{} ) {
      auto src_node = node.output_src();
      set_node_name(node, src_node.name());
    }
    else {
      reg_node_name(node, name, name_hash, name_mgr);
    }
  }

  // FF の入力ノード名
  for ( auto dff: mNetwork.dff_list() ) {
    auto node = dff.data_in();
    const string& name = node.name();
    reg_node_name(node, name, name_hash, name_mgr);
  }

  // 論理ノード名
  for ( auto node: mNetwork.logic_list() ) {
    const string& name = node.name();
    reg_node_name(node, name, name_hash, name_mgr);
  }

  // 名無しのノードに名前を与える．
  for ( auto node: mNetwork.all_node_list() ) {
    if ( node_name(node) == string{} ) {
      string name = name_mgr.new_name(true);
      set_node_name(node, name);
    }
  }

  // 外部出力ノードのファンインの名前を出力ノードの名前に付け替える．
  for ( auto node: mNetwork.primary_output_list() ) {
    auto src_node = node.output_src();
    if ( !src_node.is_input() ) {
      set_node_name(src_node, node_name(node));
    }
  }

  // FFの入力ノードの名前をそのファンインの名前に付け替える．
  for ( auto dff: mNetwork.dff_list() ) {
    auto node= dff.data_in();
    auto src_node = node.output_src();
    set_node_name(node, node_name(src_node));
  }

  // データ系のノードに印をつける．
  for ( auto node: mNetwork.output_list() ) {
    if ( node.is_port_output() || node.is_data_in() ) {
      mark_tfi(node.output_src());
    }
  }

  // 特例としてどこにもファンアウトしていないノードはデータ系とみなす．
  // 正確にはクロック系ではないとみなす．
  for ( auto node: mNetwork.all_node_list() ) {
    if ( !node.is_output() && node.fanout_num() == 0 ) {
      mark_tfi(node);
    }
  }
}

// @brief ノード名の登録を行う．
void
WriterBase::reg_node_name(
  BnNode node,
  const string& name,
  unordered_set<string>& name_hash,
  NameMgr& name_mgr
)
{
  if ( node_name(node) != string{} ) {
    // すでに名前がついていた．
    return;
  }
  if ( name == string{} ) {
    // 名前がなかった．
    return;
  }
  if ( name_hash.count(name) > 0 ) {
    // 名前が重複していた．
    return;
  }

  // 名前を登録する．
  name_mgr.add(name.c_str());
  name_hash.insert(name);
  set_node_name(node, name);
}

// @brief TFI のノードに印をつける．
// @param[in] node_id ノード番号
void
WriterBase::mark_tfi(
  BnNode node
)
{
  if ( mDataArray[node.id() - 1] ) {
    // すでに印がついていた．
    return;
  }
  mDataArray[node.id() - 1] = true;

  for ( auto inode: node.fanin_list() ) {
    mark_tfi(inode);
  }
}

END_NAMESPACE_YM_BNET
