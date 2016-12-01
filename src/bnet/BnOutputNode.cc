
/// @file BnOutputNode.cc
/// @brief BnOutputNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnOutputNode.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnOutputNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] output_id 出力番号
BnOutputNode::BnOutputNode(ymuint id,
			   const string& name,
			   ymuint output_id)
  BnNodeImpl(id, name),
  mOutputId(output_id),
  mFanin(kBnNullId)
{
}

// @brief デストラクタ
BnOutputNode::~BnOutputNode()
{
}

// @brief タイプを返す．
BnNodeType
BnOutputNode::type() const
{
  return kBnOutput;
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
// node = BnNetwork::output(id) の時，node->output_id() = id となる．
ymuint
BnOutputNode::output_id() const
{
  return mOutputId;
}

// @brief 入力のノード番号を返す．
//
// is_output() == false の時の動作は不定
ymuint
BnOutputNode::fanin() const
{
  return mFanin;
}

// @brief ファンインを設定する．
// @param[in] ipos 入力位置
// @param[in] fanin_id ファンインのノード番号
void
BnOutputNode::set_fanin(ymuint ipos,
			ymuint fanin_id)
{
  ASSERT_COND( ipos == 0 );
  mFanin = fanin_id;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPortOutput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] input 入力のノード番号
// @param[in] output_id 出力番号
// @param[in] port_id ポート番号
// @param[in] port_bit ポート中のビット位置
BnPortOutput::BnPortOutput(ymuint id,
			   const string& name,
			   ymuint output_id,
			   ymuint port_id,
			   ymuint port_bit) :
  BnOutputNode(id, name, output_id),
  mPortId(port_id),
  mPortBit(port_bit)
{
}

// @brief デストラクタ
BnPortOutput::~BnPortOutput()
{
}

// @brief 外部出力端子の時に true を返す．
bool
BnPortOutput::is_port_output() const
{
  return true;
}

// @brief 接続しているポート番号を返す．
//
// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
ymuint
BnPortOutput::port_id() const
{
  return mPortId;
}

// @brief 接続しているポート中のビット番号を返す．
//
// is_port_input() || is_port_output() の時のみ意味を持つ．
ymuint
BnPortOutput::port_bit() const
{
  return mPortBit;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffControl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] input 入力のノード番号
// @param[in] output_id 出力番号
// @param[in] dff_id DFF番号
BnDffControl::BnDffControl(ymuint id,
			   const string& name,
			   ymuint output_id,
			   ymuint dff_id) :
  BnOutputNode(id, name, output_id),
  mDffId(dff_id)
{
}

// @brief デストラクタ
BnDffControl::~BnDffControl()
{
}

// @brief 接続しているDFFの番号を返す．
//
// is_dff_input() || is_dff_output() || is_dff_clock() || is_dff_clear() || is_dff_preset()
// の時のみ意味を持つ．
ymuint
BnDffControl::dff_id() const
{
  return mDffId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] output_id 出力番号
// @param[in] dff_id DFF番号
BnDffInput::BnDffInput(ymuint id,
		       const string& name,
		       ymuint output_id,
		       ymuint dff_id) :
  BnDffControl(id, name, output_id, dff_id)
{
}

// @brief デストラクタ
BnDffInput::~BnDffInput()
{
}

// @brie DFFの入力端子の時に true を返す．
bool
BnDffInput::is_dff_input() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffClock
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] output_id 出力番号
// @param[in] dff_id DFF番号
BnDffClock::BnDffClock(ymuint id,
		       const string& name,
		       ymuint output_id,
		       ymuint dff_id) :
  BnDffControl(id, name, output_id, dff_id)
{
}

// @brief デストラクタ
BnDffClock::~BnDffClock()
{
}

// @brief DFFのクロック端子の時に true を返す．
bool
BnDffClock::is_dff_clock() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffClear
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] output_id 出力番号
// @param[in] dff_id DFF番号
BnDffClear::BnDffClear(ymuint id,
		       const string& name,
		       ymuint output_id,
		       ymuint dff_id) :
  BnDffControl(id, name, output_id, dff_id)
{
}

// @brief デストラクタ
BnDffClear::~BnDffClear()
{
}

// @brief DFFのクリア端子の時に true を返す．
bool
BnDffClear::is_dff_clear() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffPreset
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] output_id 出力番号
// @param[in] dff_id DFF番号
BnDffPreset::BnDffPreset(ymuint id,
			 const string& name,
			 ymuint output_id,
			 ymuint dff_id) :
  BnDffControl(id, name, output_id, dff_id)
{
}

// @brief デストラクタ
BnDffPreset::~BnDffPreset()
{
}

// @brief DFFのプリセット端子の時に true を返す．
bool
BnDffPreset::is_dff_preset() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchControl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] output_id 出力番号
// @param[in] latch_id ラッチ番号
BnLatchControl::BnLatchControl(ymuint id,
			       const string& name,
			       ymuint output_id,
			       ymuint latch_id) :
  BnOutputNode(id, name, output_id),
  mLatchId(latch_id)
{
}

// @brief デストラクタ
BnLatchControl::~BnLatchControl()
{
}

// @brief 接続しているラッチの番号を返す．
//
// is_latch_input() || is_latch_output() || is_latch_enable() || is_latch_clear() || is_latch_preset()
// の時のみ意味を持つ．
ymuint
BnLatchControl::latch_id() const
{
  return mLatchId;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] output_id 出力番号
// @param[in] latch_id ラッチ番号
BnLatchInput::BnLatchInput(ymuint id,
			   const string& name,
			   ymuint output_id,
			   ymuint latch_id) :
  BnLatchControl(id, name, output_id, latch_id)
{
}

// @brief デストラクタ
BnLatchInput::~BnLatchInput()
{
}

// @brie ラッチの入力端子の時に true を返す．
bool
BnLatchInput::is_latch_input() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchEnable
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] output_id 出力番号
// @param[in] latch_id ラッチ番号
BnLatchEnable::BnLatchEnable(ymuint id,
			     const string& name,
			     ymuint output_id,
			     ymuint latch_id) :
  BnLatchControl(id, name, output_id, latch_id)
{
}

// @brief デストラクタ
BnLatchEnable::~BnLatchEnable()
{
}

// @brief ラッチのイネーブル端子の時に true を返す．
bool
BnLatchEnable::is_latch_enable() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchClear
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] output_id 出力番号
// @param[in] latch_id ラッチ番号
BnLatchClear::BnLatchClear(ymuint id,
			   const string& name,
			   ymuint output_id,
			   ymuint latch_id) :
  BnLatchControl(id, name, output_id, latch_id)
{
}

// @brief デストラクタ
BnLatchClear::~BnLatchClear()
{
}

// @brief ラッチのクリア端子の時に true を返す．
bool
BnLatchClear::is_latch_clear() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLatchPreset
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] output_id 出力番号
// @param[in] latch_id ラッチ番号
BnLatchPreset::BnLatchPreset(ymuint id,
			     const string& name,
			     ymuint output_id,
			     ymuint latch_id) :
  BnLatchControl(id, name, output_id, latch_id)
{
}

// @brief デストラクタ
BnLatchPreset::~BnLatchPreset()
{
}

// @brief ラッチのプリセット端子の時に true を返す．
bool
BnLatchPreset::is_latch_preset() const
{
  return true;
}

END_NAMESPACE_YM_BNET
