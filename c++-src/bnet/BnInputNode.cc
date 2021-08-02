
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
// クラス BnDffOutput
//////////////////////////////////////////////////////////////////////

// @brief DFFの出力端子の時 true を返す．
bool
BnDffOutput::is_dff_output() const
{
  return true;
}

// @brief 接続しているDFFの番号を返す．
//
// is_dff_input() || is_dff_output() || is_dff_xoutput() ||
// is_dff_clock() || is_dff_clear() || is_dff_preset()
// の時のみ意味を持つ．
SizeType
BnDffOutput::dff_id() const
{
  return mDffId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffXOutput
//////////////////////////////////////////////////////////////////////

// @brief DFFの反転出力端子の時 true を返す．
bool
BnDffXOutput::is_dff_xoutput() const
{
  return true;
}

// @brief 接続しているDFFの番号を返す．
//
// is_dff_input() || is_dff_output() || is_dff_xoutput() ||
// is_dff_clock() || is_dff_clear() || is_dff_preset()
// の時のみ意味を持つ．
SizeType
BnDffXOutput::dff_id() const
{
  return mDffId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchOutput
//////////////////////////////////////////////////////////////////////

// @brief ラッチの出力端子の時 true を返す．
bool
BnLatchOutput::is_latch_output() const
{
  return true;
}

// @brief 接続しているラッチの番号を返す．
//
// is_latch_input() || is_latch_output() || is_latch_xoutput() ||
// is_latch_enable() || is_latch_clear() || is_latch_preset()
// の時のみ意味を持つ．
SizeType
BnLatchOutput::latch_id() const
{
  return mLatchId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchXOutput
//////////////////////////////////////////////////////////////////////

// @brief ラッチの出力端子の時 true を返す．
bool
BnLatchXOutput::is_latch_xoutput() const
{
  return true;
}

// @brief 接続しているラッチの番号を返す．
//
// is_latch_input() || is_latch_output() || is_latch_xoutput() ||
// is_latch_enable() || is_latch_clear() || is_latch_preset()
// の時のみ意味を持つ．
SizeType
BnLatchXOutput::latch_id() const
{
  return mLatchId;
}

END_NAMESPACE_YM_BNET
