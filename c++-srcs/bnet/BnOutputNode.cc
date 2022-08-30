
/// @file BnOutputNode.cc
/// @brief BnOutputNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "BnOutputNode.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnOutputNode
//////////////////////////////////////////////////////////////////////

// @brief タイプを返す．
BnNodeType
BnOutputNode::type() const
{
  return BnNodeType::Output;
}

// @brief 外部出力ノードの時 true を返す．
bool
BnOutputNode::is_output() const
{
  return true;
}

// @brief 出力番号を返す．
//
// is_output() == false の時の動作は不定<br>
// node_id = BnNetwork::output_id(pos) の時，node->output_pos() = pos となる．
SizeType
BnOutputNode::output_pos() const
{
  return mOutputPos;
}

// @brief ソースノードのノード番号を返す．
SizeType
BnOutputNode::output_src() const
{
  return mFanin;
}

// @brief 出力位置を設定する．
void
BnOutputNode::set_output_pos(
  SizeType opos
)
{
  mOutputPos = opos;
}

// @brief ファンインを設定する．
void
BnOutputNode::set_output_src(
  SizeType fanin_id
)
{
  mFanin = fanin_id;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPortOutput
//////////////////////////////////////////////////////////////////////

// @brief 外部出力端子のノードを作る．
BnNodeImpl*
BnNodeImpl::new_port_output(
  const string& name,
  SizeType port_id,
  SizeType port_bit
)
{
  return new BnPortOutput{name, port_id, port_bit};
}

// @brief 外部出力端子の時に true を返す．
bool
BnPortOutput::is_port_output() const
{
  return true;
}

// @brief 外部出力端子番号を返す．
SizeType
BnPortOutput::primary_output_pos() const
{
  return mPrimaryOutputPos;
}

// @brief 接続しているポート番号を返す．
//
// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
SizeType
BnPortOutput::port_id() const
{
  return mPortId;
}

// @brief 接続しているポート中のビット番号を返す．
//
// is_port_input() || is_port_output() の時のみ意味を持つ．
SizeType
BnPortOutput::port_bit() const
{
  return mPortBit;
}

// @brief 外部出力端子番号を設定する．
void
BnPortOutput::set_primary_output_pos(
  SizeType pos
)
{
  mPrimaryOutputPos = pos;
}


//////////////////////////////////////////////////////////////////////
// クラス BnControl
//////////////////////////////////////////////////////////////////////

// @brief 接続しているDFFの番号を返す．
SizeType
BnControl::dff_id() const
{
  return mDffId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDataIn
//////////////////////////////////////////////////////////////////////

// @brief DFFの入力端子のノードを作る．
BnNodeImpl*
BnNodeImpl::new_data_in(
  const string& name,
  SizeType dff_id
)
{
  return new BnDataIn{name, dff_id};
}

// @brie DFFの入力端子の時に true を返す．
bool
BnDataIn::is_data_in() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnClock
//////////////////////////////////////////////////////////////////////

// @brief DFFのクロック端子のノードを作る．
BnNodeImpl*
BnNodeImpl::new_clock(
  const string& name,
  SizeType dff_id
)
{
  return new BnClock{name, dff_id};
}

// @brie DFFのクロック端子の時に true を返す．
bool
BnClock::is_clock() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnClear
//////////////////////////////////////////////////////////////////////

// @brief DFFのクリア端子のノードを作る．
BnNodeImpl*
BnNodeImpl::new_clear(
  const string& name,
  SizeType dff_id
)
{
  return new BnClear{name, dff_id};
}

// @brie DFFのクリア端子の時に true を返す．
bool
BnClear::is_clear() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPreset
//////////////////////////////////////////////////////////////////////

// @brief DFFのプリセット端子のノードを作る．
BnNodeImpl*
BnNodeImpl::new_preset(
  const string& name,
  SizeType dff_id
)
{
  return new BnPreset{name, dff_id};
}

// @brie DFFのプリセット端子の時に true を返す．
bool
BnPreset::is_preset() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnCellInput
//////////////////////////////////////////////////////////////////////

// @brief DFFセルの入力端子のノードを作る．
BnNodeImpl*
BnNodeImpl::new_cell_input(
  const string& name,
  SizeType dff_id,
  SizeType iid
)
{
  return new BnCellInput{name, dff_id, iid};
}

// @brief DFFセルの入力端子の時 true を返す．
bool
BnCellInput::is_cell_input() const
{
  return true;
}

// @brief DFFセルの入力ピン番号を返す．
SizeType
BnCellInput::cell_input_pos() const
{
  return mIid;
}

END_NAMESPACE_YM_BNET
