
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
BnInputNode::BnInputNode(ymuint id,
			 const string& name,
			 ymuint input_id) :
  BnNodeImpl(id, name),
  mInputId(input_id)
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
  return kBnInput;
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
// node = BnNetwork::input(id) の時 node->input_id() = id となる．
ymuint
BnInputNode::input_id() const
{
  return mInputId;
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
BnPortInput::BnPortInput(ymuint id,
			 const string& name,
			 ymuint input_id,
			 ymuint port_id,
			 ymuint port_bit) :
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
ymuint
BnPortInput::port_id() const
{
  return mPortId;
}

// @brief 接続しているポート中のビット番号を返す．
//
// is_port_input() || is_port_output() の時のみ意味を持つ．
ymuint
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
BnDffOutput::BnDffOutput(ymuint id,
			 const string& name,
			 ymuint input_id,
			 ymuint dff_id) :
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
// is_dff_input() || is_dff_output() || is_dff_clock() || is_dff_clear() || is_dff_preset()
// の時のみ意味を持つ．
ymuint
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
BnDffXOutput::BnDffXOutput(ymuint id,
			   const string& name,
			   ymuint input_id,
			   ymuint dff_id) :
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
// is_dff_input() || is_dff_output() || is_dff_clock() || is_dff_clear() || is_dff_preset()
// の時のみ意味を持つ．
ymuint
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
BnLatchOutput::BnLatchOutput(ymuint id,
			     const string& name,
			     ymuint input_id,
			     ymuint latch_id) :
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
// is_latch_input() || is_latch_output() || is_latch_enable() || is_latch_clear() || is_latch_preset()
// の時のみ意味を持つ．
ymuint
BnLatchOutput::latch_id() const
{
  return mLatchId;
}

END_NAMESPACE_YM_BNET
