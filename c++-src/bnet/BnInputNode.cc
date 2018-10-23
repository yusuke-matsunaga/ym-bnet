
/// @file BnInputNode.cc
/// @brief BnInputNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnInputNode.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnInputNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] input_id 入力番号
BnInputNode::BnInputNode(int id,
			 const string& name,
			 int input_id) :
  BnNodeImpl(id, name),
  mInputPos(input_id)
{
}

// @brief デストラクタ
BnInputNode::~BnInputNode()
{
}

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
int
BnInputNode::input_pos() const
{
  return mInputPos;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPortInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] input_id 入力番号
// @param[in] port_id ポート番号
// @param[in] port_bit ポート中のビット位置
BnPortInput::BnPortInput(int id,
			 const string& name,
			 int input_id,
			 int port_id,
			 int port_bit) :
  BnInputNode(id, name, input_id),
  mPortId(port_id),
  mPortBit(port_bit)
{
}

// @brief デストラクタ
BnPortInput::~BnPortInput()
{
}

// @brief 外部入力端子の時 true を返す．
bool
BnPortInput::is_port_input() const
{
  return true;
}

// @brief 接続しているポート番号を返す．
//
// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
int
BnPortInput::port_id() const
{
  return mPortId;
}

// @brief 接続しているポート中のビット番号を返す．
//
// is_port_input() || is_port_output() の時のみ意味を持つ．
int
BnPortInput::port_bit() const
{
  return mPortBit;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffOutput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] input_id 入力番号
// @param[in] dff_id DFF番号
BnDffOutput::BnDffOutput(int id,
			 const string& name,
			 int input_id,
			 int dff_id) :
  BnInputNode(id, name, input_id),
  mDffId(dff_id)
{
}

// @brief デストラクタ
BnDffOutput::~BnDffOutput()
{
}

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
int
BnDffOutput::dff_id() const
{
  return mDffId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffXOutput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] input_id 入力番号
// @param[in] dff_id DFF番号
BnDffXOutput::BnDffXOutput(int id,
			   const string& name,
			   int input_id,
			   int dff_id) :
  BnInputNode(id, name, input_id),
  mDffId(dff_id)
{
}

// @brief デストラクタ
BnDffXOutput::~BnDffXOutput()
{
}

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
int
BnDffXOutput::dff_id() const
{
  return mDffId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchOutput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] input_id 入力番号
// @param[in] latch_id ラッチ番号
BnLatchOutput::BnLatchOutput(int id,
			     const string& name,
			     int input_id,
			     int latch_id) :
  BnInputNode(id, name, input_id),
  mLatchId(latch_id)
{
}

// @brief デストラクタ
BnLatchOutput::~BnLatchOutput()
{
}

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
int
BnLatchOutput::latch_id() const
{
  return mLatchId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchXOutput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] input_id 入力番号
// @param[in] latch_id ラッチ番号
BnLatchXOutput::BnLatchXOutput(int id,
			       const string& name,
			       int input_id,
			       int latch_id) :
  BnInputNode(id, name, input_id),
  mLatchId(latch_id)
{
}

// @brief デストラクタ
BnLatchXOutput::~BnLatchXOutput()
{
}

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
int
BnLatchXOutput::latch_id() const
{
  return mLatchId;
}

END_NAMESPACE_YM_BNET
