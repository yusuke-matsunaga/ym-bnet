﻿
/// @file BnNodeImpl.cc
/// @brief BnNodeImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BnNodeImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNodeImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
BnNodeImpl::BnNodeImpl(
  int id,
  const string& name
) : mId(id),
    mName(name)
{
}

// @brief デストラクタ
BnNodeImpl::~BnNodeImpl()
{
  // mFanoutList は BnNetworkImpl が管理している．
}

// @brief ID を返す．
int
BnNodeImpl::id() const
{
  return mId;
}

// @brief 名前を返す．
string
BnNodeImpl::name() const
{
  return mName;
}

// @brief 外部入力の時 true を返す．
bool
BnNodeImpl::is_input() const
{
  return false;
}

// @brief 外部出力の時 true を返す．
bool
BnNodeImpl::is_output() const
{
  return false;
}

// @brief 論理ノードの時 true を返す．
bool
BnNodeImpl::is_logic() const
{
  return false;
}

// @brief ファンアウト数を得る．
int
BnNodeImpl::fanout_num() const
{
  return mFanoutList.size();
}

// @brief ファンアウトのノード番号を返す．
int
BnNodeImpl::fanout_id(
  int pos
) const
{
  ASSERT_COND( pos >= 0 && pos < fanout_num() );

  return mFanoutList[pos];
}

// @brief ファンアウトのノード番号のリストを返す．
vector<int>
BnNodeImpl::fanout_id_list() const
{
  return mFanoutList;
}

// @brief 入力番号を返す．
//
// is_input() == false の時の動作は不定<br>
// node_id = BnNetwork::input_id(pos) の時 node->input_pos() = pos となる．
int
BnNodeImpl::input_pos() const
{
  ASSERT_NOT_REACHED;
  return -1;
}

// @brief 外部入力端子の時 true を返す．
bool
BnNodeImpl::is_port_input() const
{
  return false;
}

// @brief DFFの出力端子の時 true を返す．
bool
BnNodeImpl::is_dff_output() const
{
  return false;
}

// @brief DFFの出力端子の時 true を返す．
bool
BnNodeImpl::is_dff_xoutput() const
{
  return false;
}

// @brief ラッチの出力端子の時 true を返す．
bool
BnNodeImpl::is_latch_output() const
{
  return false;
}

// @brief ラッチの反転出力端子の時 true を返す．
bool
BnNodeImpl::is_latch_xoutput() const
{
  return false;
}

// @brief 出力番号を返す．
//
// is_output() == false の時の動作は不定<br>
// node_id = BnNetwork::output_id(pos) の時，node->output_pos() = pos となる．
int
BnNodeImpl::output_pos() const
{
  ASSERT_NOT_REACHED;
  return -1;
}

// @brief 外部出力端子の時に true を返す．
bool
BnNodeImpl::is_port_output() const
{
  return false;
}

// @brie DFFの入力端子の時に true を返す．
bool
BnNodeImpl::is_dff_input() const
{
  return false;
}

// @brief DFFのクロック端子の時に true を返す．
bool
BnNodeImpl::is_dff_clock() const
{
  return false;
}

// @brief DFFのクリア端子の時に true を返す．
bool
BnNodeImpl::is_dff_clear() const
{
  return false;
}

// @brief DFFのプリセット端子の時に true を返す．
bool
BnNodeImpl::is_dff_preset() const
{
  return false;
}

// @brief ラッチの入力端子の時に true を返す．
bool
BnNodeImpl::is_latch_input() const
{
  return false;
}

// @brief ラッチのイネーブル端子の時に true を返す．
bool
BnNodeImpl::is_latch_enable() const
{
  return false;
}

// @brief ラッチのクリア端子の時に true を返す．
bool
BnNodeImpl::is_latch_clear() const
{
  return false;
}

// @brief ラッチのプリセット端子の時に true を返す．
bool
BnNodeImpl::is_latch_preset() const
{
  return false;
}

// @brief 接続しているポート番号を返す．
//
// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
int
BnNodeImpl::port_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 接続しているポート中のビット番号を返す．
//
// is_port_input() || is_port_output() の時のみ意味を持つ．
int
BnNodeImpl::port_bit() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 接続しているDFFの番号を返す．
//
// is_dff_input() || is_dff_output() || is_dff_xoutput() ||
// is_dff_clock() || is_dff_clear() || is_dff_preset()
// の時のみ意味を持つ．
int
BnNodeImpl::dff_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 接続しているラッチの番号を返す．
//
// is_latch_input() || is_latch_output() || is_latch_xoutput() ||
// is_latch_enable() || is_latch_clear() || is_latch_preset()
// の時のみ意味を持つ．
int
BnNodeImpl::latch_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief ファンイン数を得る．
int
BnNodeImpl::fanin_num() const
{
  return 0;
}

// @brief ファンインのノード番号を返す．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
int
BnNodeImpl::fanin_id(int pos) const
{
  ASSERT_NOT_REACHED;
  return kBnNullId;
}

// @brief ファンインのノード番号のリストを返す．
vector<int>
BnNodeImpl::fanin_id_list() const
{
  return {};
}

// @brief 論理式番号を返す．
//
// logic_type() == BnNodeType::Expr の時のみ意味を持つ．
// 論理式番号は同じ BnNetwork 内で唯一となるもの．
int
BnNodeImpl::expr_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 関数番号を返す．
//
// type() == BnNodeType::TvFunc の時のみ意味を持つ．
// 関数番号は同じ BnNetwork 内で唯一となるもの．
int
BnNodeImpl::func_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief セル番号を返す．
//
// is_logic() == false の時の動作は不定
int
BnNodeImpl::cell_id() const
{
  ASSERT_NOT_REACHED;
  return -1;
}

// @brief ファンインを設定する．
// @param[in] ipos 入力位置
// @param[in] fanin_id ファンインのノード番号
void
BnNodeImpl::set_fanin(int ipos,
		      int fanin_id)
{
  ASSERT_NOT_REACHED;
}

// @brief ファンアウトリストをクリアする．
void
BnNodeImpl::clear_fanout()
{
  mFanoutList.clear();
}

// @brief ファンアウトを追加する．
// @param[in] node_id ノード番号
void
BnNodeImpl::add_fanout(int node_id)
{
  mFanoutList.push_back(node_id);
}

END_NAMESPACE_YM_BNET
