
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


//////////////////////////////////////////////////////////////////////
// クラス BnPortInput
//////////////////////////////////////////////////////////////////////

// @brief 外部入力端子の時 true を返す．
bool
BnPortInput::is_port_input() const
{
  return true;
}

// @brief 接続しているポート番号を返す．
//
// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
SizeType
BnPortInput::port_id() const
{
  return mPortId;
}

// @brief 接続しているポート中のビット番号を返す．
//
// is_port_input() || is_port_output() の時のみ意味を持つ．
SizeType
BnPortInput::port_bit() const
{
  return mPortBit;
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

// @brief DFFの出力端子の時 true を返す．
bool
BnDataOut::is_data_out() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnCellOutput
//////////////////////////////////////////////////////////////////////

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
