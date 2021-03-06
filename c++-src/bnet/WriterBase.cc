
/// @file WriterBase.cc
/// @brief WriterBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "WriterBase.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnLatch.h"
#include "ym/BnNode.h"
#include "ym/Range.h"
#include "ym/NameMgr.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
WriterBase::WriterBase(const BnNetwork& network) :
  mNetwork(network),
  mNameArray(network.node_num()),
  mDataArray(network.node_num(), false)
{
}

// @brief ノード名の解決を行う．
// @param[in] prefix 自動生成名の接頭語
// @param[in] suffix 自動生成名の接尾語
void
WriterBase::init_name_array(const string& prefix,
			    const string& suffix)
{
  // ノード名を管理するクラス
  NameMgr name_mgr(prefix, suffix);

  // ノード名のハッシュ
  unordered_set<string> name_hash;

  // もともと与えられた名前があればそれを使う．
  // ただし重複のチェックを行う．

  // 外部ポート名
  for ( int i: Range(mNetwork.port_num()) ) {
    auto& port = mNetwork.port(i);
    const string& name = port.name();
    if ( name == string() ) {
      // 名無しの場合はスキップ
      continue;
    }

    int nb = port.bit_width();
    if ( nb == 1 ) {
      // 1ビットポートならポート名をノード名にする．
      int id = port.bit(0);
      reg_node_name(id, name, name_hash, name_mgr);
    }
    else {
      // 多ビットポートの場合は '[' <ビット番号> ']' を後ろにつける．
      for ( int b: Range(nb) ) {
	int id = port.bit(b);
	ostringstream buf;
	buf << name << "[" << b << "]";
	reg_node_name(id, buf.str(), name_hash, name_mgr);
      }
    }
  }

  // FF名
  for ( int i: Range(mNetwork.dff_num()) ) {
    auto& dff = mNetwork.dff(i);
    const string& name = dff.name();
    if ( name == string() ) {
      // 名無しの場合はスキップ
      continue;
    }
    int id = dff.output();
    reg_node_name(id, name, name_hash, name_mgr);
  }

  // ラッチ名
  for ( int i: Range(mNetwork.latch_num()) ) {
    auto& latch = mNetwork.latch(i);
    const string& name = latch.name();
    if ( name == string() ) {
      // 名無しの場合はスキップ
      continue;
    }
    int id = latch.output();
    reg_node_name(id, name, name_hash, name_mgr);
  }

  // 外部入力ノード名
  for ( int id: mNetwork.primary_input_id_list() ) {
    auto& node = mNetwork.node(id);
    const string& name = node.name();
    reg_node_name(id, name, name_hash, name_mgr);
  }

  // FFの出力ノード名
  for ( int i: Range(mNetwork.dff_num()) ) {
    auto& dff = mNetwork.dff(i);
    int id = dff.output();
    auto& node = mNetwork.node(id);
    const string& name = node.name();
    reg_node_name(id, name, name_hash, name_mgr);
  }

  // ラッチの出力ノード名
  for ( int i: Range(mNetwork.latch_num()) ) {
    auto& latch = mNetwork.latch(i);
    int id = latch.output();
    auto& node = mNetwork.node(id);
    const string& name = node.name();
    reg_node_name(id, name, name_hash, name_mgr);
  }

  // 外部出力ノード名
  for ( int id: mNetwork.primary_output_id_list() ) {
    auto& node = mNetwork.node(id);
    const string& name = node.name();
    reg_node_name(id, name, name_hash, name_mgr);
  }

  // FF の入力ノード名
  for ( int i: Range(mNetwork.dff_num()) ) {
    auto& dff = mNetwork.dff(i);
    int id = dff.input();
    auto& node = mNetwork.node(id);
    const string& name = node.name();
    reg_node_name(id, name, name_hash, name_mgr);
  }

  // ラッチの入力ノード名
  for ( int i: Range(mNetwork.latch_num()) ) {
    auto& latch = mNetwork.latch(i);
    int id = latch.input();
    auto& node = mNetwork.node(id);
    const string& name = node.name();
    reg_node_name(id, name, name_hash, name_mgr);
  }

  // 論理ノード名
  for ( int id: mNetwork.logic_id_list() ) {
    auto& node = mNetwork.node(id);
    const string& name = node.name();
    reg_node_name(id, name, name_hash, name_mgr);
  }

  // 名無しのノードに名前を与える．
  for ( int id: Range(mNetwork.node_num()) ) {
    if ( mNameArray[id] == string() ) {
      string name = name_mgr.new_name(true);
      mNameArray[id] = name;
    }
  }

  // 外部出力ノードのファンインの名前を出力ノードの名前に付け替える．
  for ( int id: mNetwork.primary_output_id_list() ) {
    auto& node = mNetwork.node(id);
    int src_id = node.fanin_id(0);
    auto& src_node = mNetwork.node(src_id);
    if ( !src_node.is_input() ) {
      mNameArray[src_id] = mNameArray[id];
    }
  }

  // FFの入力ノードの名前をそのファンインの名前に付け替える．
  for ( int i: Range(mNetwork.dff_num()) ) {
    auto& dff = mNetwork.dff(i);
    int id = dff.input();
    auto& node = mNetwork.node(id);
    int src_id = node.fanin_id(0);
    mNameArray[id] = mNameArray[src_id];
  }

  // ラッチの入力ノードの名前をそのファンインの名前に付け替える．
  for ( int i: Range(mNetwork.latch_num()) ) {
    auto& latch = mNetwork.latch(i);
    int id = latch.input();
    auto& node = mNetwork.node(id);
    int src_id = node.fanin_id(0);
    mNameArray[id] = mNameArray[src_id];
  }

  // データ系のノードに印をつける．
  for ( int id: mNetwork.output_id_list() ) {
    auto& node = mNetwork.node(id);
    if ( node.is_port_output() || node.is_dff_input() ) {
      mark_tfi(node.fanin_id(0));
    }
  }

  // 特例としてどこにもファンアウトしていないノードはデータ系とみなす．
  // 正確にはクロック系ではないとみなす．
  for ( int id: Range(mNetwork.node_num()) ) {
    auto& node = mNetwork.node(id);
    if ( node.is_output() ) {
      continue;
    }
    if ( node.fanout_num() == 0 ) {
      mark_tfi(id);
    }
  }
}

// @brief ノード名の登録を行う．
// @param[in] node_id ノード番号
// @param[in] name 登録する名前
// @param[in] name_hash ノード名のハッシュ
// @param[in] name_mgr ノード名を管理するクラス
void
WriterBase::reg_node_name(int node_id,
			  const string& name,
			  unordered_set<string>& name_hash,
			  NameMgr& name_mgr)
{
  if ( mNameArray[node_id] != string() ) {
    // すでに名前がついていた．
    return;
  }
  if ( name == string() ) {
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
  mNameArray[node_id] = name;
}

// @brief TFI のノードに印をつける．
// @param[in] node_id ノード番号
void
WriterBase::mark_tfi(int node_id)
{
  if ( mDataArray[node_id] ) {
    // すでに印がついていた．
    return;
  }
  mDataArray[node_id] = true;

  auto& node = mNetwork.node(node_id);
  for ( int iid: node.fanin_id_list() ) {
    mark_tfi(iid);
  }
}

END_NAMESPACE_YM_BNET
