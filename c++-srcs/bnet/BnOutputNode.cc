
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

// @brief ファンイン数を得る．
SizeType
BnOutputNode::fanin_num() const
{
  return 1;
}

// @brief ファンインのノード番号を返す．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
SizeType
BnOutputNode::fanin_id(
  SizeType pos
) const
{
  ASSERT_COND( pos == 0 );

  return mFanin;
}

// @brief ファンインのノード番号のリストを返す．
vector<SizeType>
BnOutputNode::fanin_id_list() const
{
  return vector<SizeType>{mFanin};
}

// @brief ファンインを設定する．
// @param[in] ipos 入力位置
// @param[in] fanin_id ファンインのノード番号
void
BnOutputNode::set_fanin(
  SizeType ipos,
  SizeType fanin_id
)
{
  ASSERT_COND( ipos == 0 );
  mFanin = fanin_id;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPortOutput
//////////////////////////////////////////////////////////////////////

// @brief 外部出力端子の時に true を返す．
bool
BnPortOutput::is_port_output() const
{
  return true;
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

// @brie DFFの入力端子の時に true を返す．
bool
BnDataIn::is_data_in() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnClock
//////////////////////////////////////////////////////////////////////

// @brie DFFのクロック端子の時に true を返す．
bool
BnClock::is_clock() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnClear
//////////////////////////////////////////////////////////////////////

// @brie DFFのクリア端子の時に true を返す．
bool
BnClear::is_clear() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPreset
//////////////////////////////////////////////////////////////////////

// @brie DFFのプリセット端子の時に true を返す．
bool
BnPreset::is_preset() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnCellInput
//////////////////////////////////////////////////////////////////////

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
