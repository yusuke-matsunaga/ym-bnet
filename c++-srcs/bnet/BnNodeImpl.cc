
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
BnNodeImpl::BnNodeImpl(
  const string& name
) : mName{name}
{
}

// @brief デストラクタ
BnNodeImpl::~BnNodeImpl()
{
}

// @brief ID を返す．
SizeType
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
SizeType
BnNodeImpl::fanout_num() const
{
  return mFanoutList.size();
}

// @brief ファンアウトのノード番号を返す．
SizeType
BnNodeImpl::fanout_id(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < fanout_num() );

  return mFanoutList[pos];
}

// @brief ファンアウトのノード番号のリストを返す．
vector<SizeType>
BnNodeImpl::fanout_id_list() const
{
  return mFanoutList;
}

// @brief 入力番号を返す．
//
// is_input() == false の時の動作は不定<br>
// node_id = BnNetwork::input_id(pos) の時 node->input_pos() = pos となる．
SizeType
BnNodeImpl::input_pos() const
{
  ASSERT_NOT_REACHED;
  return BNET_NULLID;
}

// @brief 外部入力端子の時 true を返す．
bool
BnNodeImpl::is_port_input() const
{
  return false;
}

// @brief 外部入力番号を返す．
SizeType
BnNodeImpl::primary_input_pos() const
{
  ASSERT_NOT_REACHED;
  return BNET_NULLID;
}

// @brief DFFの出力端子の時 true を返す．
bool
BnNodeImpl::is_data_out() const
{
  return false;
}

// @brief DFFセルの出力端子の時 true を返す．
bool
BnNodeImpl::is_cell_output() const
{
  return false;
}

// @brief DFFセルの出力ピン番号を返す．
SizeType
BnNodeImpl::cell_output_pos() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 出力番号を返す．
//
// is_output() == false の時の動作は不定<br>
// node_id = BnNetwork::output_id(pos) の時，node->output_pos() = pos となる．
SizeType
BnNodeImpl::output_pos() const
{
  ASSERT_NOT_REACHED;
  return BNET_NULLID;
}

// @brief 外部出力端子の時に true を返す．
bool
BnNodeImpl::is_port_output() const
{
  return false;
}

// @brief 外部出力端子番号を返す．
SizeType
BnNodeImpl::primary_output_pos() const
{
  ASSERT_NOT_REACHED;
  return BNET_NULLID;
}

// @brie DFFの入力端子の時に true を返す．
bool
BnNodeImpl::is_data_in() const
{
  return false;
}

// @brie DFFのクロック端子の時に true を返す．
bool
BnNodeImpl::is_clock() const
{
  return false;
}

// @brie DFFのクリア端子の時に true を返す．
bool
BnNodeImpl::is_clear() const
{
  return false;
}

// @brie DFFのプリセット端子の時に true を返す．
bool
BnNodeImpl::is_preset() const
{
  return false;
}

// @brief DFFセルの入力端子の時 true を返す．
bool
BnNodeImpl::is_cell_input() const
{
  return false;
}

// @brief DFFセルの入力ピン番号を返す．
SizeType
BnNodeImpl::cell_input_pos() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 接続しているポート番号を返す．
//
// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
SizeType
BnNodeImpl::port_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 接続しているポート中のビット番号を返す．
//
// is_port_input() || is_port_output() の時のみ意味を持つ．
SizeType
BnNodeImpl::port_bit() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 接続しているDFFの番号を返す．
//
// is_dff_input() || is_dff_output()
// の時のみ意味を持つ．
SizeType
BnNodeImpl::dff_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief ファンイン数を得る．
SizeType
BnNodeImpl::fanin_num() const
{
  return 0;
}

// @brief ファンインのノード番号を返す．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
SizeType
BnNodeImpl::fanin_id(
  SizeType pos
) const
{
  ASSERT_NOT_REACHED;
  return BNET_NULLID;
}

// @brief ファンインのノード番号のリストを返す．
vector<SizeType>
BnNodeImpl::fanin_id_list() const
{
  return {};
}

// @brief 論理式番号を返す．
//
// logic_type() == BnNodeType::Expr の時のみ意味を持つ．
// 論理式番号は同じ BnNetwork 内で唯一となるもの．
SizeType
BnNodeImpl::expr_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 関数番号を返す．
//
// type() == BnNodeType::TvFunc の時のみ意味を持つ．
// 関数番号は同じ BnNetwork 内で唯一となるもの．
SizeType
BnNodeImpl::func_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief Bdd を返す．
//
// - type() == Bdd の時のみ意味を持つ．
Bdd
BnNodeImpl::bdd() const
{
  ASSERT_NOT_REACHED;
  return Bdd::invalid();
}

// @brief セル番号を返す．
SizeType
BnNodeImpl::cell_id() const
{
  ASSERT_NOT_REACHED;
  return CLIB_NULLID;
}

// @brief 入力位置を設定する．
void
BnNodeImpl::set_input_pos(
  SizeType ipos
)
{
  ASSERT_NOT_REACHED;
}

// @brief 外部入力端子番号を設定する．
void
BnNodeImpl::set_primary_input_pos(
  SizeType pos
)
{
  ASSERT_NOT_REACHED;
}

// @brief 出力位置を設定する．
void
BnNodeImpl::set_output_pos(
  SizeType opos
)
{
  ASSERT_NOT_REACHED;
}

// @brief 外部出力端子番号を設定する．
void
BnNodeImpl::set_primary_output_pos(
  SizeType pos
)
{
  ASSERT_NOT_REACHED;
}

// @brief ファンインを設定する．
// @param[in] ipos 入力位置
// @param[in] fanin_id ファンインのノード番号
void
BnNodeImpl::set_fanin(
  SizeType ipos,
  SizeType fanin_id)
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
BnNodeImpl::add_fanout(
  SizeType node_id
)
{
  mFanoutList.push_back(node_id);
}

// @brief 自分と同じタイプのノードを作る．
BnNodeImpl*
BnNodeImpl::duplicate(
  const string& name,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

END_NAMESPACE_YM_BNET
