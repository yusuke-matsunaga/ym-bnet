
/// @file BnModifier.cc
/// @brief BnModifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021 Yusuke Matsunaga
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
SizeType
BnModifier::new_input_port(
  const string& port_name
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_port(port_name, {BnDir::INPUT});
}

// @brief 多ビットの入力ポートを作る．
SizeType
BnModifier::new_input_port(
  const string& port_name,
  SizeType bit_width
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_port(port_name,
			 vector<BnDir>(bit_width, BnDir::INPUT));
}

// @brief 1ビットの出力ポートを作る．
SizeType
BnModifier::new_output_port(
  const string& port_name
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_port(port_name, {BnDir::OUTPUT});
}

// @brief 多ビットの出力ポートを作る．
SizeType
BnModifier::new_output_port(
  const string& port_name,
  SizeType bit_width
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_port(port_name,
			 vector<BnDir>(bit_width, BnDir::OUTPUT));
}

// @brief 入出力混合のポートを作る．
SizeType
BnModifier::new_port(
  const string& port_name,
  const vector<BnDir>& dir_vect
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_port(port_name, dir_vect);
}

// @brief DFFを追加する．
SizeType
BnModifier::new_dff(
  const string& name,
  bool has_clear,
  bool has_preset,
  BnCPV cpv
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_dff(name, has_clear, has_preset, cpv);
}

// @brief セルの情報を持ったDFFを追加する．
SizeType
BnModifier::new_dff_cell(
  const string& name,
  SizeType cell_id
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_dff_cell(name, cell_id);
}

// @brief ラッチを追加する．
SizeType
BnModifier::new_latch(
  const string& name,
  bool has_clear,
  bool has_preset,
  BnCPV cpv
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_latch(name, has_clear, has_preset, cpv);
}

// @brief プリミティブ型の論理ノードを追加する．
SizeType
BnModifier::new_logic_primitive(
  const string& node_name,
  BnNodeType logic_type,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_primitive(node_name, logic_type, fanin_id_list);
}

// @brief 論理式型の論理ノードを追加する．
SizeType
BnModifier::new_logic_expr(
  const string& node_name,
  const Expr& expr,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_expr(node_name, expr, fanin_id_list);
}

// @brief 真理値表型の論理ノードを追加する．
SizeType
BnModifier::new_logic_tv(
  const string& node_name,
  const TvFunc& tv,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_tv(node_name, tv, fanin_id_list);
}

// @brief BDD型の論理ノードを追加する．
SizeType
BnModifier::new_logic_bdd(
  const string& node_name,
  const Bdd& bdd,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_bdd(node_name, bdd, fanin_id_list);
}

// @brief 論理セルを追加する．
SizeType
BnModifier::new_logic_cell(
  const string& node_name,
  SizeType cell_id,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_cell(node_name, cell_id, fanin_id_list);
}

// @brief 与えられたノードと同型の論理ノードを追加する．
SizeType
BnModifier::dup_logic(
  const string& node_name,
  SizeType node_id,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->dup_logic(node_name, node_id, fanin_id_list);
}

// @brief C0型(定数０)の論理ノードを追加する．
SizeType
BnModifier::new_c0(
  const string& node_name
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_primitive(node_name, BnNodeType::C0, {});
}

// @brief C1型(定数1)の論理ノードを追加する．
SizeType
BnModifier::new_c1(
  const string& node_name
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_primitive(node_name, BnNodeType::C1, {});
}

// @brief Buff型の論理ノードを追加する．
SizeType
BnModifier::new_buff(
  const string& node_name,
  SizeType fanin_id
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_primitive(node_name, BnNodeType::Buff, {fanin_id});
}

// @brief Not型の論理ノードを追加する．
SizeType
BnModifier::new_not(
  const string& node_name,
  SizeType fanin_id
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_primitive(node_name, BnNodeType::Not, {fanin_id});
}

// @brief AND型の論理ノードを追加する．
SizeType
BnModifier::new_and(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_primitive(node_name, BnNodeType::And, fanin_id_list);

}

// @brief NAND型の論理ノードを追加する．
SizeType
BnModifier::new_nand(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_primitive(node_name, BnNodeType::Nand, fanin_id_list);
}

// @brief OR型の論理ノードを追加する．
SizeType
BnModifier::new_or(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_primitive(node_name, BnNodeType::Or, fanin_id_list);
}

// @brief NOR型の論理ノードを追加する．
SizeType
BnModifier::new_nor(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_primitive(node_name, BnNodeType::Nor, fanin_id_list);
}

// @brief XOR型の論理ノードを追加する．
SizeType
BnModifier::new_xor(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_primitive(node_name, BnNodeType::Xor, fanin_id_list);
}

// @brief XNOR型の論理ノードを追加する．
SizeType
BnModifier::new_xnor(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_logic_primitive(node_name, BnNodeType::Xnor, fanin_id_list);
}

// @brief プリミティブ型の論理ノードに変更する．
void
BnModifier::change_primitive(
  SizeType id,
  BnNodeType logic_type,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  mImpl->change_primitive(id, logic_type, fanin_id_list);
}

// @brief 論理式型の論理ノードに変更する．
void
BnModifier::change_expr(
  SizeType id,
  const Expr& expr,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  mImpl->change_expr(id, expr, fanin_id_list);
}

// @brief 真理値表型の論理ノードに変更する．
void
BnModifier::change_tv(
  SizeType id,
  const TvFunc& tv,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );
  mImpl->change_tv(id, tv, fanin_id_list);
}

// @brief ポートの情報のみコピーする．
unordered_map<SizeType, SizeType>
BnModifier::make_skelton_copy(
  const BnNetwork& src_network
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->make_skelton_copy(*src_network.mImpl);
}

// @brief 部分回路を追加する．
vector<SizeType>
BnModifier::import_subnetwork(
  const BnNetwork& src_network,
  const vector<SizeType>& input_list
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->import_subnetwork(*src_network.mImpl, input_list);
}

// @brief 出力ノードのファンインを設定する．
void
BnModifier::set_output_src(
  SizeType output_id,
  SizeType src_id
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->set_output_src(output_id, src_id);
}

// @brief ファンアウトをつなぎ替える．
void
BnModifier::substitute_fanout(
  SizeType old_id,
  SizeType new_id
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->substitute_fanout(old_id, new_id);
}

END_NAMESPACE_YM_BNET
