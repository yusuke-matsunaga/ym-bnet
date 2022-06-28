
/// @file BnNetwork.cc
/// @brief BnNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/BnNodeType.h"
#include "ym/Range.h"
#include "BnNetworkImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
//
// 空の状態で初期化される．
BnNetwork::BnNetwork(
) : mImpl{new BnNetworkImpl()}
{
}

// @brief コピーコンストラクタ
BnNetwork::BnNetwork(
  const BnNetwork& src
) : mImpl{new BnNetworkImpl()}
{
  mImpl->copy(*src.mImpl);
}

// @brief ムーブコンストラクタ
BnNetwork::BnNetwork(
  BnNetwork&& src
) : mImpl{std::move(src.mImpl)}
{
}

// @brief コピー代入演算子
BnNetwork&
BnNetwork::operator=(
  const BnNetwork& src
)
{
  mImpl->copy(*src.mImpl);

  return *this;
}

// @brief ムーブ代入演算子
BnNetwork&
BnNetwork::operator=(
  BnNetwork&& src
)
{
  swap(mImpl, src.mImpl);

  return *this;
}

// @brief デストラクタ
BnNetwork::~BnNetwork()
{
  clear();
}

// @brief 内容をクリアする．
//
// コンストラクタ直後と同じ状態になる．
void
BnNetwork::clear()
{
  if ( mImpl ) {
    mImpl->clear();
  }
}

// @brief 内容をコピーする．
void
BnNetwork::copy(
  const BnNetwork& src
)
{
  if ( &src == this ) {
    // 自分自身がソースならなにもしない．
    return;
  }

  ASSERT_COND( mImpl != nullptr );

  mImpl->copy(*src.mImpl);
}

// @brief 内容をムーブする．
void
BnNetwork::move(
  BnNetwork&& src
)
{
  swap(mImpl, src.mImpl);
}

// @brief セルライブラリをセットする．
void
BnNetwork::set_library(
  const ClibCellLibrary& library
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->set_library(library);
}

// @brief ネットワーク名を設定する．
void
BnNetwork::set_name(
  const string& name
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->set_name(name);
}

// @brief 1ビットの入力ポートを作る．
SizeType
BnNetwork::new_input_port(
  const string& port_name
)
{
  ASSERT_COND( mImpl != nullptr );

  //return mImpl->new_port(port_name, vector<BnDir>({BnDir::INPUT}));
  return mImpl->new_port(port_name, {BnDir::INPUT});
}

// @brief 多ビットの入力ポートを作る．
SizeType
BnNetwork::new_input_port(
  const string& port_name,
  SizeType bit_width
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_port(port_name, vector<BnDir>(bit_width, BnDir::INPUT));
}

// @brief 1ビットの出力ポートを作る．
SizeType
BnNetwork::new_output_port(
  const string& port_name
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_port(port_name, {BnDir::OUTPUT});
}

// @brief 多ビットの出力ポートを作る．
SizeType
BnNetwork::new_output_port(
  const string& port_name,
  SizeType bit_width
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_port(port_name, vector<BnDir>(bit_width, BnDir::OUTPUT));
}

// @brief 入出力混合のポートを作る．
SizeType
BnNetwork::new_port(
  const string& port_name,
  const vector<BnDir>& dir_vect
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_port(port_name, dir_vect);
}

// @brief DFFを追加する．
SizeType
BnNetwork::new_dff(
  const string& name,
  bool has_clear,
  bool has_preset
)
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->new_dff(name, has_clear, has_preset);
}

// @brief セルの情報を持ったDFFを追加する．
SizeType
BnNetwork::new_dff(
  const string& name,
  int cell_id
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_dff(name, cell_id);
}

// @brief ラッチを追加する．
SizeType
BnNetwork::new_latch(
  const string& name,
  bool has_clear,
  bool has_preset
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_latch(name, has_clear, has_preset);
}

// @brief セルの情報を持ったラッチを追加する．
SizeType
BnNetwork::new_latch(
  const string& name,
  int cell_id
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_latch(name, cell_id);
}

// @brief プリミティブ型の論理ノードを追加する．
SizeType
BnNetwork::new_logic(
  const string& node_name,
  BnNodeType logic_type,
  SizeType ni
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_primitive(node_name, ni, logic_type);
}

// @brief プリミティブ型の論理ノードを追加する．
SizeType
BnNetwork::new_logic(
  const string& node_name,
  BnNodeType logic_type,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_primitive(node_name, logic_type, fanin_id_list);
}

// @brief 論理式型の論理ノードを追加する．
SizeType
BnNetwork::new_logic(
  const string& node_name,
  const Expr& expr,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  SizeType id = mImpl->new_expr(node_name, expr);
  if ( id != BNET_NULLID && !fanin_id_list.empty() ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief 真理値表型の論理ノードを追加する．
SizeType
BnNetwork::new_logic(
  const string& node_name,
  const TvFunc& tv,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  SizeType id = mImpl->new_tv(node_name, tv);
  if ( id != BNET_NULLID && !fanin_id_list.empty() ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief 論理セルを追加する．
SizeType
BnNetwork::new_logic(
  const string& node_name,
  int cell_id,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  SizeType id = mImpl->new_cell(node_name, cell_id);
  if ( id != BNET_NULLID && !fanin_id_list.empty() ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief 与えられたノードと同型の論理ノードを追加する．
SizeType
BnNetwork::dup_logic(
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
BnNetwork::new_c0(
  const string& node_name
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_primitive(node_name, 1, BnNodeType::C0);
}

// @brief C1型(定数1)の論理ノードを追加する．
SizeType
BnNetwork::new_c1(
  const string& node_name
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_primitive(node_name, 1, BnNodeType::C1);
}

// @brief Buff型の論理ノードを追加する．
SizeType
BnNetwork::new_buff(
  const string& node_name,
  SizeType fanin_id
)
{
  ASSERT_COND( mImpl != nullptr );

  SizeType id = mImpl->new_primitive(node_name, 1, BnNodeType::Buff);
  if ( fanin_id != BNET_NULLID ) {
    connect(fanin_id, id, 0);
  }
  return id;
}

// @brief Not型の論理ノードを追加する．
SizeType
BnNetwork::new_not(
  const string& node_name,
  SizeType fanin_id
)
{
  ASSERT_COND( mImpl != nullptr );

  SizeType id = mImpl->new_primitive(node_name, 1, BnNodeType::Not);
  if ( fanin_id != BNET_NULLID ) {
    connect(fanin_id, id, 0);
  }
  return id;
}

// @brief And型の論理ノードを追加する．
SizeType
BnNetwork::new_and(
  const string& node_name,
  SizeType ni
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_primitive(node_name, ni, BnNodeType::And);
}

// @brief AND型の論理ノードを追加する．
SizeType
BnNetwork::new_and(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  SizeType ni = fanin_id_list.size();
  SizeType id = mImpl->new_primitive(node_name, ni, BnNodeType::And);
  if ( id != BNET_NULLID ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief Nand型の論理ノードを追加する．
SizeType
BnNetwork::new_nand(
  const string& node_name,
  SizeType ni
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_primitive(node_name, ni, BnNodeType::Nand);
}

// @brief NAND型の論理ノードを追加する．
SizeType
BnNetwork::new_nand(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  SizeType ni = fanin_id_list.size();
  SizeType id = mImpl->new_primitive(node_name, ni, BnNodeType::Nand);
  if ( id != BNET_NULLID ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief Or型の論理ノードを追加する．
SizeType
BnNetwork::new_or(
  const string& node_name,
  SizeType ni
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_primitive(node_name, ni, BnNodeType::Or);
}

// @brief OR型の論理ノードを追加する．
SizeType
BnNetwork::new_or(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  auto ni = fanin_id_list.size();
  auto id = mImpl->new_primitive(node_name, ni, BnNodeType::Or);
  if ( id != BNET_NULLID ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief Nor型の論理ノードを追加する．
SizeType
BnNetwork::new_nor(
  const string& node_name,
  SizeType ni
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_primitive(node_name, ni, BnNodeType::Nor);
}

// @brief NOR型の論理ノードを追加する．
SizeType
BnNetwork::new_nor(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  SizeType ni = fanin_id_list.size();
  SizeType id = mImpl->new_primitive(node_name, ni, BnNodeType::Nor);
  if ( id != BNET_NULLID ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief Xor型の論理ノードを追加する．
SizeType
BnNetwork::new_xor(
  const string& node_name,
  SizeType ni
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_primitive(node_name, ni, BnNodeType::Xor);
}

// @brief XOR型の論理ノードを追加する．
SizeType
BnNetwork::new_xor(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  SizeType ni = fanin_id_list.size();
  SizeType id = mImpl->new_primitive(node_name, ni, BnNodeType::Xor);
  if ( id != BNET_NULLID ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief Xnor型の論理ノードを追加する．
SizeType
BnNetwork::new_xnor(
  const string& node_name,
  SizeType ni
)
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->new_primitive(node_name, ni, BnNodeType::Xnor);
}

// @brief XNOR型の論理ノードを追加する．
SizeType
BnNetwork::new_xnor(
  const string& node_name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  SizeType ni = fanin_id_list.size();
  SizeType id = mImpl->new_primitive(node_name, ni, BnNodeType::Xnor);
  if ( id != BNET_NULLID ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief プリミティブ型の論理ノードに変更する．
void
BnNetwork::change_primitive(
  SizeType id,
  BnNodeType logic_type,
  SizeType ni
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->change_primitive(id, logic_type, ni);
}

// @brief プリミティブ型の論理ノードに変更する．
void
BnNetwork::change_primitive(
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
BnNetwork::change_expr(
  SizeType id,
  const Expr& expr
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->change_expr(id, expr);
}

// @brief 論理式型の論理ノードに変更する．
void
BnNetwork::change_expr(
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
BnNetwork::change_tv(
  SizeType id,
  const TvFunc& tv
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->change_tv(id, tv);
}

// @brief 真理値表型の論理ノードに変更する．
void
BnNetwork::change_tv(
  SizeType id,
  const TvFunc& tv,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->change_tv(id, tv, fanin_id_list);
}

// @brief 論理セルに変更する．
void
BnNetwork::change_cell(
  SizeType id,
  int cell_id
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->change_cell(id, cell_id);
}

// @brief 論理セルに変更する．
void
BnNetwork::change_cell(
  SizeType id,
  int cell_id,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->change_cell(id, cell_id, fanin_id_list);
}

// @brief 部分回路を追加する．
void
BnNetwork::import_subnetwork(
  const BnNetwork& src_network,
  const vector<SizeType>& input_list,
  vector<SizeType>& output_list
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->import_subnetwork(*src_network.mImpl, input_list, output_list);
}

// @brief 単純なノードに分解する．
void
BnNetwork::simple_decomp()
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->simple_decomp();
}

// @brief ノード間を接続する．
void
BnNetwork::connect(
  SizeType src_id,
  SizeType dst_id,
  SizeType ipos
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->connect(src_id, dst_id, ipos);
}

// @brief ファンアウトをつなぎ替える．
void
BnNetwork::substitute_fanout(
  SizeType old_id,
  SizeType new_id
)
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->substitute_fanout(old_id, new_id);
}

// @brief 整合性のチェックを行う．
bool
BnNetwork::wrap_up()
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->wrap_up();
}

// @brief ファンインの接続を行う．
void
BnNetwork::connect_fanins(
  SizeType id,
  const vector<SizeType>& fanin_id_list
)
{
  SizeType ni = fanin_id_list.size();
  ASSERT_COND( mImpl->node(id).fanin_num() == ni );
  for ( SizeType i: Range(ni) ) {
    SizeType iid = fanin_id_list[i];
    mImpl->connect(iid, id, i);
  }
}

// @brief ネットワーク名を得る．
string
BnNetwork::name() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->name();
}

// @brief 関連するセルライブラリを得る．
const ClibCellLibrary&
BnNetwork::library() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->library();
}

// @brief ポート数を得る．
SizeType
BnNetwork::port_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->port_num();
}

// @brief ポートの情報を得る．
const BnPort&
BnNetwork::port(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->port(pos);
}

// @brief DFF数を得る．
SizeType
BnNetwork::dff_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->dff_num();
}

// @brief DFFを得る．
const BnDff&
BnNetwork::dff(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->dff(pos);
}

// @brief ラッチ数を得る．
SizeType
BnNetwork::latch_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->latch_num();
}

// @brief ラッチを得る．
const BnLatch&
BnNetwork::latch(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->latch(pos);
}

// @brief ノード数を得る．
SizeType
BnNetwork::node_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->node_num();
}

// @brief ノードを得る．
const BnNode&
BnNetwork::node(
  SizeType id
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->node(id);
}

// @brief 入力数を得る．
SizeType
BnNetwork::input_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->input_num();
}

// @brief 入力ノードのノード番号を得る．
SizeType
BnNetwork::input_id(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->input_id(pos);
}

// @brief 入力ノードのノード番号のリストを得る．
const vector<SizeType>&
BnNetwork::input_id_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->input_id_list();
}

// @brief 外部入力ノードのノード番号のリストを得る．
const vector<SizeType>&
BnNetwork::primary_input_id_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->primary_input_id_list();
}

// @brief 出力数を得る．
SizeType
BnNetwork::output_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output_num();
}

// @brief 出力ノードのノード番号を得る．
SizeType
BnNetwork::output_id(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output_id(pos);
}

// @brief 出力ノードのノード番号のリストを得る．
const vector<SizeType>&
BnNetwork::output_id_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output_id_list();
}

// @brief 出力ノードのソースノード番号を得る．
SizeType
BnNetwork::output_src_id(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output_src_id(pos);
}

// @brief 出力ノードのソースノード番号のリストを得る．
const vector<SizeType>&
BnNetwork::output_src_id_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output_src_id_list();
}

// @brief 外部出力ノードのノード番号のリストを得る．
const vector<SizeType>&
BnNetwork::primary_output_id_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->primary_output_id_list();
}

// @brief 外部出力ノードのソースノード番号のリストを得る．
const vector<SizeType>&
BnNetwork::primary_output_src_id_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->primary_output_src_id_list();
}

// @brief 論理ノード数を得る．
SizeType
BnNetwork::logic_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->logic_num();
}

// @brief 論理ノードのノード番号を得る．
SizeType
BnNetwork::logic_id(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->logic_id(pos);
}

// @brief 論理ノードのノード番号のリストを得る．
const vector<SizeType>&
BnNetwork::logic_id_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->logic_id_list();
}

// @brief 関数の数を得る．
SizeType
BnNetwork::func_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->func_num();
}

// @brief 関数番号から関数を得る．
const TvFunc&
BnNetwork::func(
  SizeType func_id
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->func(func_id);
}

// @brief 論理式の数を得る．
SizeType
BnNetwork::expr_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->expr_num();
}

// @brief 論理式番号から論理式を得る．
Expr
BnNetwork::expr(
  SizeType expr_id
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->expr(expr_id);
}

// @brief 内容を出力する．
void
BnNetwork::write(
  ostream& s
) const
{
  ASSERT_COND( mImpl != nullptr );

  mImpl->write(s);
}


//////////////////////////////////////////////////////////////////////
// BnNodeType
//////////////////////////////////////////////////////////////////////

// @relates BnNodeType
ostream&
operator<<(
  ostream& s,
  BnNodeType type
)
{
  switch ( type ) {
  case BnNodeType::None:   s << "None"; break;
  case BnNodeType::Input:  s << "Input"; break;
  case BnNodeType::Output: s << "Output"; break;
  case BnNodeType::C0:     s << "C0"; break;
  case BnNodeType::C1:     s << "C1"; break;
  case BnNodeType::Buff:   s << "Buff"; break;
  case BnNodeType::Not:    s << "Not"; break;
  case BnNodeType::And:    s << "And"; break;
  case BnNodeType::Nand:   s << "Nand"; break;
  case BnNodeType::Or:     s << "Or"; break;
  case BnNodeType::Nor:    s << "Nor"; break;
  case BnNodeType::Xor:    s << "Xor"; break;
  case BnNodeType::Xnor:   s << "Xnor"; break;
  case BnNodeType::Expr:   s << "Expr"; break;
  case BnNodeType::TvFunc: s << "TvFunc"; break;
  defult: ASSERT_NOT_REACHED; break;
  }
  return s;
}

// cython 用の変換関数
int
__bnnodetype_to_int(BnNodeType type)
{
  switch ( type ) {
  case BnNodeType::None:   return 0;
  case BnNodeType::Input:  return 1;
  case BnNodeType::Output: return 2;
  case BnNodeType::C0:     return 3;
  case BnNodeType::C1:     return 4;
  case BnNodeType::Buff:   return 5;
  case BnNodeType::Not:    return 6;
  case BnNodeType::And:    return 7;
  case BnNodeType::Nand:   return 8;
  case BnNodeType::Or:     return 9;
  case BnNodeType::Nor:    return 10;
  case BnNodeType::Xor:    return 11;
  case BnNodeType::Xnor:   return 12;
  case BnNodeType::Expr:   return 13;
  case BnNodeType::TvFunc: return 14;
  defult: ASSERT_NOT_REACHED; break;
  }
  return 0;
}

BnNodeType
__int_to_bnnodetype(int val)
{
  static BnNodeType tbl[] {
    BnNodeType::None,
    BnNodeType::Input,
    BnNodeType::Output,
    BnNodeType::C0,
    BnNodeType::C1,
    BnNodeType::Buff,
    BnNodeType::Not,
    BnNodeType::And,
    BnNodeType::Nand,
    BnNodeType::Or,
    BnNodeType::Nor,
    BnNodeType::Xor,
    BnNodeType::Xnor,
    BnNodeType::Expr,
    BnNodeType::TvFunc
  };

  ASSERT_COND( val >= 0 && val < 15 );
  return tbl[val];
}

END_NAMESPACE_YM_BNET
