
/// @file BnInputNode.cc
/// @brief BnInputNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BnInputNode.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnInputNode
//////////////////////////////////////////////////////////////////////

// @brief タイプを返す．
BnNodeType
BnInputNode::type() const
{
  return BnNodeType::Input;
}

// @brief 外部入力の時 true を返す．
bool
BnInputNode::is_input() const
{
  return true;
}

// @brief 入力番号を返す．
//
// is_input() == false の時の動作は不定<br>
// node_id = BnNetwork::input_id(pos) の時 node->input_pos() = pos となる．
SizeType
BnInputNode::input_pos() const
{
  return mInputPos;
}

// @brief 入力位置を設定する．
void
BnInputNode::set_input_pos(
  SizeType ipos
)
{
  mInputPos = ipos;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPortInput
//////////////////////////////////////////////////////////////////////

// @brief 外部入力端子のノードを作る．
BnNodeImpl*
BnNodeImpl::new_port_input(
  const string& name,
  SizeType port_id,
  SizeType port_bit
)
{
  return new BnPortInput{name, port_id, port_bit};
}

// @brief 外部入力端子の時 true を返す．
bool
BnPortInput::is_port_input() const
{
  return true;
}

// @brief 外部入力番号を返す．
SizeType
BnPortInput::primary_input_pos() const
{
  return mPrimaryInputPos;
}

// @brief 接続しているポート番号を返す．
SizeType
BnPortInput::port_id() const
{
  return mPortId;
}

// @brief 接続しているポート中のビット番号を返す．
SizeType
BnPortInput::port_bit() const
{
  return mPortBit;
}

// @brief 外部入力端子番号を設定する．
void
BnPortInput::set_primary_input_pos(
  SizeType pos
)
{
  mPrimaryInputPos = pos;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffOut
//////////////////////////////////////////////////////////////////////

// @brief 接続しているDFFの番号を返す．
SizeType
BnDffOut::dff_id() const
{
  return mDffId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDataOut
//////////////////////////////////////////////////////////////////////

// @brief DFFの出力端子のノードを作る．
BnNodeImpl*
BnNodeImpl::new_data_out(
  const string& name,
  SizeType dff_id
)
{
  return new BnDataOut{name, dff_id};
}

// @brief DFFの出力端子の時 true を返す．
bool
BnDataOut::is_data_out() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnCellOutput
//////////////////////////////////////////////////////////////////////

// @brief DFFセルの出力端子のノードを作る．
BnNodeImpl*
BnNodeImpl::new_cell_output(
  const string& name,
  SizeType dff_id,
  SizeType oid
)
{
  return new BnCellOutput{name, dff_id, oid};
}

// @brief DFFセルの出力端子の時 true を返す．
bool
BnCellOutput::is_cell_output() const
{
  return true;
}

// @brief DFFセルの出力ピン番号を返す．
SizeType
BnCellOutput::cell_output_pos() const
{
  return mOid;
}

END_NAMESPACE_YM_BNET
