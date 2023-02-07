
/// @file BnNode.cc
/// @brief BnNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNode.h"
#include "ym/BnNodeList.h"
#include "ym/Bdd.h"
#include "BnNetworkImpl.h"
#include "BnNodeImpl.h"


BEGIN_NAMESPACE_YM_BNET

// @brief 名前を返す．
string
BnNode::name() const
{
  auto node = mNetwork->_node(mId);
  return node->name();
}

// @brief ノードのタイプを返す．
BnNodeType
BnNode::type() const
{
  auto node = mNetwork->_node(mId);
  return node->type();
}

/// @brief 入力タイプの時 true を返す．
bool
BnNode::is_input() const
{
  auto node = mNetwork->_node(mId);
  return node->is_input();
}

// @brief 出力タイプの時 true を返す．
bool
BnNode::is_output() const
{
  auto node = mNetwork->_node(mId);
  return node->is_output();
}

// @brief 論理ノードの時 true を返す．
bool
BnNode::is_logic() const
{
  auto node = mNetwork->_node(mId);
  return node->is_logic();
}

// @brief ファンアウト数を得る．
SizeType
BnNode::fanout_num() const
{
  auto node = mNetwork->_node(mId);
  return node->fanout_num();
}

/// @brief ファンアウトのノードを返す．
BnNode
BnNode::fanout(
  SizeType pos ///< [in] 位置番号 ( 0 <= pos < fanout_num() )
) const
{
  auto node = mNetwork->_node(mId);
  SizeType id = node->fanout_id(pos);
  return BnNode{mNetwork, id};
}

// @brief ファンアウトのノードのリストを返す．
BnNodeList
BnNode::fanout_list() const
{
  auto node = mNetwork->_node(mId);
  return BnNodeList{mNetwork, node->fanout_id_list()};
}

// @brief 入力番号を返す．
SizeType
BnNode::input_pos() const
{
  auto node = mNetwork->_node(mId);
  return node->input_pos();
}

// @brief 外部入力端子の時 true を返す．
bool
BnNode::is_port_input() const
{
  auto node = mNetwork->_node(mId);
  return node->is_port_input();
}

// @brief 外部入力番号を返す．
SizeType
BnNode::primary_input_pos() const
{
  auto node = mNetwork->_node(mId);
  return node->primary_input_pos();
}

// @brief DFF/ラッチのデータ出力端子の時 true を返す．
bool
BnNode::is_data_out() const
{
  auto node = mNetwork->_node(mId);
  return node->is_data_out();
}

// @brief DFFセルの出力端子の時 true を返す．
bool
BnNode::is_cell_output() const
{
  auto node = mNetwork->_node(mId);
  return node->is_cell_output();
}

// @brief DFFセルの出力ピン番号を返す．
SizeType
BnNode::cell_output_pos() const
{
  auto node = mNetwork->_node(mId);
  return node->cell_output_pos();
}

// @brief 出力番号を返す．
SizeType
BnNode::output_pos() const
{
  auto node = mNetwork->_node(mId);
  return node->output_pos();
}

// @brief ソースノードを返す．
BnNode
BnNode::output_src() const
{
  auto node = mNetwork->_node(mId);
  return BnNode{mNetwork, node->output_src()};
}

// @brief 外部出力端子の時に true を返す．
bool
BnNode::is_port_output() const
{
  auto node = mNetwork->_node(mId);
  return node->is_port_output();
}

// @brief 外部出力端子番号を返す．
SizeType
BnNode::primary_output_pos() const
{
  auto node = mNetwork->_node(mId);
  return node->primary_output_pos();
}

// @brief DFF/ラッチののデータ入力端子の時に true を返す．
bool
BnNode::is_data_in() const
{
  auto node = mNetwork->_node(mId);
  return node->is_data_in();
}

// @brief DFF/ラッチのクロック/イネーブル端子の時に true を返す．
bool
BnNode::is_clock() const
{
  auto node = mNetwork->_node(mId);
  return node->is_clock();
}

// @brief DFF/ラッチのクリア端子の時に true を返す．
bool
BnNode::is_clear() const
{
  auto node = mNetwork->_node(mId);
  return node->is_clear();
}

// @brief DFF/ラッチのプリセット端子の時に true を返す．
bool
BnNode::is_preset() const
{
  auto node = mNetwork->_node(mId);
  return node->is_preset();
}

// @brief DFF/ラッチセルの入力端子の時 true を返す．
bool
BnNode::is_cell_input() const
{
  auto node = mNetwork->_node(mId);
  return node->is_cell_input();
}

// @brief DFF/ラッチセルの入力ピン番号を返す．
SizeType
BnNode::cell_input_pos() const
{
  auto node = mNetwork->_node(mId);
  return node->cell_input_pos();
}

// @brief 接続しているポート番号を返す．
SizeType
BnNode::port_id() const
{
  auto node = mNetwork->_node(mId);
  return node->port_id();
}

// @brief 接続しているポート中のビット番号を返す．
SizeType
BnNode::port_bit() const
{
  auto node = mNetwork->_node(mId);
  return node->port_bit();
}

// @brief 接続しているDFFの番号を返す．
SizeType
BnNode::dff_id() const
{
  auto node = mNetwork->_node(mId);
  return node->dff_id();
}

// @brief ファンイン数を得る．
SizeType
BnNode::fanin_num() const
{
  auto node = mNetwork->_node(mId);
  return node->fanin_num();
}

// @brief ファンインのノード番号を返す．
SizeType
BnNode::fanin_id(
  SizeType pos
) const
{
  auto node = mNetwork->_node(mId);
  return node->fanin_id(pos);
}

// @brief ファンインのノードを返す．
BnNode
BnNode::fanin(
  SizeType pos
) const
{
  auto node = mNetwork->_node(mId);
  return BnNode{mNetwork, node->fanin_id(pos)};
}

// @brief ファンインのノードのリストを返す．
BnNodeList
BnNode::fanin_list() const
{
  auto node = mNetwork->_node(mId);
  return BnNodeList{mNetwork, node->fanin_id_list()};
}

// @brief 組み込み型を返す．
PrimType
BnNode::primitive_type() const
{
  auto node = mNetwork->_node(mId);
  return node->primitive_type();
}

// @brief 論理式番号を返す．
SizeType
BnNode::expr_id() const
{
  auto node = mNetwork->_node(mId);
  return node->expr_id();
}

// @brief 論理式を返す．
Expr
BnNode::expr() const
{
  SizeType id = expr_id();
  return mNetwork->expr(id);
}

// @brief 関数番号を返す．
SizeType
BnNode::func_id() const
{
  auto node = mNetwork->_node(mId);
  return node->func_id();
}

// @brief 関数を返す．
const TvFunc&
BnNode::func() const
{
  SizeType id = func_id();
  return mNetwork->func(id);
}

// @brief Bdd を返す．
Bdd
BnNode::bdd() const
{
  auto node = mNetwork->_node(mId);
  return node->bdd();
}

// @brief セル番号を返す．
SizeType
BnNode::cell_id() const
{
  auto node = mNetwork->_node(mId);
  return node->cell_id();
}

// @brief 実装クラスを取り出す．
BnNodeImpl*
BnNode::_impl()
{
  return mNetwork->_node(mId);
}

END_NAMESPACE_YM_BNET
