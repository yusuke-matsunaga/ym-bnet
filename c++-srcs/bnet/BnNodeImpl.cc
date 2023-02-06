
/// @file BnNodeImpl.cc
/// @brief BnNodeImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "BnNodeImpl.h"
#include "ym/Bdd.h"


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

// @brief プリミティブ型の論理ノードの時 true を返す．
bool
BnNodeImpl::is_primitive_logic() const
{
  return false;
}

// @brief 入力番号を返す．
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
SizeType
BnNodeImpl::output_pos() const
{
  ASSERT_NOT_REACHED;
  return -1;
}

// @brief ソースノードのノード番号を返す．
SizeType
BnNodeImpl::output_src() const
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
  return -1;
}

// @brief 接続しているポート番号を返す．
SizeType
BnNodeImpl::port_id() const
{
  ASSERT_NOT_REACHED;
  return -1;
}

// @brief 接続しているポート中のビット番号を返す．
SizeType
BnNodeImpl::port_bit() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 接続しているDFFの番号を返す．
SizeType
BnNodeImpl::dff_id() const
{
  ASSERT_NOT_REACHED;
  return -1;
}

// @brief ファンイン数を得る．
SizeType
BnNodeImpl::fanin_num() const
{
  return 0;
}

// @brief ファンインのノード番号を返す．
SizeType
BnNodeImpl::fanin_id(
  SizeType pos
) const
{
  ASSERT_NOT_REACHED;
  return BNET_NULLID;
}

// @brief ファンインのノード番号のリストを返す．
const vector<SizeType>&
BnNodeImpl::fanin_id_list() const
{
  static vector<SizeType> dummy;
  return dummy;
}

// @brief 論理式番号を返す．
SizeType
BnNodeImpl::expr_id() const
{
  ASSERT_NOT_REACHED;
  return -1;
}

// @brief 関数番号を返す．
SizeType
BnNodeImpl::func_id() const
{
  ASSERT_NOT_REACHED;
  return -1;
}

// @brief Bdd を返す．
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

// @brief ソースノードを設定する．
void
BnNodeImpl::set_output_src(
  SizeType node_id
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
void
BnNodeImpl::set_fanin(
  SizeType ipos,
  SizeType fanin_id)
{
  ASSERT_NOT_REACHED;
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
