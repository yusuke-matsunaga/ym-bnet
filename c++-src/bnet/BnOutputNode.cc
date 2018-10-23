
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
BnOutputNode::BnOutputNode(int id,
			   const string& name,
			   int output_id) :
  BnNodeImpl(id, name),
  mOutputPos(output_id),
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
int
BnOutputNode::output_pos() const
{
  return mOutputPos;
}

// @brief ファンイン数を得る．
int
BnOutputNode::fanin_num() const
{
  return 1;
}

// @brief ファンインのノード番号を返す．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
int
BnOutputNode::fanin_id(int pos) const
{
  ASSERT_COND( pos == 0 );

  return mFanin;
}

#if 0
// @brief ファンインのノード番号のリストを返す．
Array<int>
BnOutputNode::fanin_id_list() const
{
  return Array<int>(&mFanin, 0, 1);
}
#else
const vector<int>&
BnOutputNode::fanin_id_list() const
{
  static vector<int> dummy_fanin(1);
  dummy_fanin[0] = mFanin;
  return dummy_fanin;
}
#endif

// @brief ファンインを設定する．
// @param[in] ipos 入力位置
// @param[in] fanin_id ファンインのノード番号
void
BnOutputNode::set_fanin(int ipos,
			int fanin_id)
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
BnPortOutput::BnPortOutput(int id,
			   const string& name,
			   int output_id,
			   int port_id,
			   int port_bit) :
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
int
BnPortOutput::port_id() const
{
  return mPortId;
}

// @brief 接続しているポート中のビット番号を返す．
//
// is_port_input() || is_port_output() の時のみ意味を持つ．
int
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
BnDffControl::BnDffControl(int id,
			   const string& name,
			   int output_id,
			   int dff_id) :
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
int
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
BnDffInput::BnDffInput(int id,
		       const string& name,
		       int output_id,
		       int dff_id) :
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
BnDffClock::BnDffClock(int id,
		       const string& name,
		       int output_id,
		       int dff_id) :
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
BnDffClear::BnDffClear(int id,
		       const string& name,
		       int output_id,
		       int dff_id) :
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
BnDffPreset::BnDffPreset(int id,
			 const string& name,
			 int output_id,
			 int dff_id) :
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
BnLatchControl::BnLatchControl(int id,
			       const string& name,
			       int output_id,
			       int latch_id) :
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
int
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
BnLatchInput::BnLatchInput(int id,
			   const string& name,
			   int output_id,
			   int latch_id) :
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
BnLatchEnable::BnLatchEnable(int id,
			     const string& name,
			     int output_id,
			     int latch_id) :
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
BnLatchClear::BnLatchClear(int id,
			   const string& name,
			   int output_id,
			   int latch_id) :
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
BnLatchPreset::BnLatchPreset(int id,
			     const string& name,
			     int output_id,
			     int latch_id) :
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
