
/// @file WriterBase.cc
/// @brief WriterBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "WriterBase.h"
#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/Range.h"
#include "ym/NameMgr.h"
#include "ym/HashSet.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
WriterBase::WriterBase(const BnNetwork& network) :
  mNetwork(network),
  mNameArray(network.node_num()),
  mDataArray(network.node_num(), false)
{
}

// @brief デストラクタ
WriterBase::~WriterBase()
{
}

// @brief ノード名の解決を行う．
// @param[in] prefix 自動生成名の接頭語
// @param[in] suffix 自動生成名の接尾語
void
WriterBase::init_name_array(const string& prefix,
			    const string& suffix)
{
  // ノード名の自動生成が必要なノード番号のリスト
  vector<int> node_list;
  node_list.reserve(mNetwork.node_num());

  // ノード名を管理するクラス
  NameMgr name_mgr(prefix, suffix);

  // ノード名のハッシュ
  HashSet<string> name_hash;

  // もともと与えられた名前があればそれを使う．
  // ただし重複のチェックを行う．
  for ( int id: Range(mNetwork.node_num()) ) {
    auto& node = mNetwork.node(id);
    if ( node.is_output() ) {
      // 外部出力ノードは無視
      continue;
    }
    string name = node.name();
    if ( name == string() ) {
      node_list.push_back(id);
    }
    else if ( name_hash.check(name) ) {
      // 名前が重複していた．
      node_list.push_back(id);
    }
    else {
      // 名前を登録する．
      name_mgr.add(name.c_str());
      name_hash.add(name);
      mNameArray[id] = name;
    }
  }

  // node_list のノードに名前を与える．
  for ( int id: node_list ) {
    string name = name_mgr.new_name(true);
    mNameArray[id] = name;
  }

  // 出力ノードの名前をファンインの名前に付け替える．
  for ( int i: Range(mNetwork.output_num()) ) {
    int id = mNetwork.output_id(i);
    int src_id = mNetwork.output_src_id(i);
    mNameArray[id] = mNameArray[src_id];
  }

  // データ系のノードに印をつける．
  for ( int id: mNetwork.output_id_list() ) {
    auto& node = mNetwork.node(id);
    if ( node.is_port_output() ) {
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
