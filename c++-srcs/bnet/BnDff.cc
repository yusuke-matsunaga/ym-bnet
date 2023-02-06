
/// @file BnDff.cc
/// @brief BnDff の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnDff.h"
#include "BnNetworkImpl.h"
#include "BnDffImpl.h"


BEGIN_NAMESPACE_YM_BNET

// @brief 名前を返す．
string
BnDff::name() const
{
  auto dff = mNetwork->_dff(mId);
  return dff->name();
}

// @brief タイプを返す．
BnDffType
BnDff::type() const
{
  auto dff = mNetwork->_dff(mId);
  return dff->type();
}

// @brief DFF タイプの時 true を返す．
bool
BnDff::is_dff() const
{
  auto dff = mNetwork->_dff(mId);
  return dff->is_dff();
}

// @brief ラッチタイプの時 true を返す．
bool
BnDff::is_latch() const
{
  auto dff = mNetwork->_dff(mId);
  return dff->is_latch();
}

// @brief DFF/ラッチセルタイプの時 true を返す．
bool
BnDff::is_cell() const
{
  auto dff = mNetwork->_dff(mId);
  return dff->is_cell();
}

// @brief 入力端子のノードを返す．
BnNode
BnDff::data_in() const
{
  auto dff = mNetwork->_dff(mId);
  return BnNode{mNetwork, dff->data_in()};
}

// @brief 出力端子のノードを返す．
BnNode
BnDff::data_out() const
{
  auto dff = mNetwork->_dff(mId);
  return BnNode{mNetwork, dff->data_out()};
}

// @brief クロック端子のノードを返す．
BnNode
BnDff::clock() const
{
  auto dff = mNetwork->_dff(mId);
  return BnNode{mNetwork, dff->clock()};
}

// @brief クリア端子のノードを返す．
BnNode
BnDff::clear() const
{
  auto dff = mNetwork->_dff(mId);
  return BnNode{mNetwork, dff->clear()};
}

// @brief プリセット端子のノードを返す．
BnNode
BnDff::preset() const
{
  auto dff = mNetwork->_dff(mId);
  return BnNode{mNetwork, dff->preset()};
}

// @brief クリアとプリセットが衝突した場合の挙動
BnCPV
BnDff::clear_preset_value() const
{
  auto dff = mNetwork->_dff(mId);
  return dff->clear_preset_value();
}

// @brief セルに割り当てられている場合のセル番号を返す．
SizeType
BnDff::cell_id() const
{
  auto dff = mNetwork->_dff(mId);
  return dff->cell_id();
}

// @brief セルに割り当てられている場合の入力端子数を返す．
SizeType
BnDff::cell_input_num() const
{
  auto dff = mNetwork->_dff(mId);
  return dff->cell_input_num();
}

// @brief セルに割り当てられている場合の入力端子に対応するノードを返す．
BnNode
BnDff::cell_input(
  SizeType pos
) const
{
  auto dff = mNetwork->_dff(mId);
  return BnNode{mNetwork, dff->cell_input(pos)};
}

// @brief セルに割り当てられている場合の出力端子数を返す．
SizeType
BnDff::cell_output_num() const
{
  auto dff = mNetwork->_dff(mId);
  return dff->cell_output_num();
}

// @brief セルに割り当てられている場合の出力端子に対応するノードを返す．
BnNode
BnDff::cell_output(
  SizeType pos
) const
{
  auto dff = mNetwork->_dff(mId);
  return BnNode{mNetwork, dff->cell_output(pos)};
}

// @brief 実装クラスを取り出す．
BnDffImpl*
BnDff::_impl()
{
  if ( mNetwork == nullptr ) {
    return nullptr;
  }
  return mNetwork->_dff(mId);
}

END_NAMESPACE_YM_BNET
