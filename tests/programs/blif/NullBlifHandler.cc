
/// @file NullBlifHandler.cc
/// @brief NullBlifHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "NullBlifHandler.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] parser
NullBlifHandler::NullBlifHandler(
  BlifParser& parser
) : BlifHandler(parser)
{
}

// @brief デストラクタ
NullBlifHandler::~NullBlifHandler()
{
}

// @brief 初期化
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::init()
{
  return true;
}

// @brief セルライブラリの設定
// @param[in] library セルライブラリ
//
// この関数が呼ばれないこともある．
void
NullBlifHandler::set_cell_library(
  const ClibCellLibrary& library
)
{
}

// @brief .model 文の処理
// @param[in] loc1 .model の位置情報
// @param[in] loc2 文字列の位置情報
// @param[in] name 文字列
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::model(
  const FileRegion& loc1,
  const FileRegion& loc2,
  const string& name
)
{
  return true;
}

// @brief .inputs 文中の文字列の処理
// @param[in] name_id 文字列のID番号
// @param[in] name 入力ピン名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::inputs_elem(
  SizeType name_id,
  const string& name
)
{
  return true;
}

// @brief .outputs 文中の文字列の処理
// @param[in] name_id 文字列のID番号
// @param[in] name 出力ピン名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::outputs_elem(
  SizeType name_id,
  const string& name
)
{
  return true;
}

// @brief .names 文の処理
// @param[in] onode_id 出力ノードのID番号
// @param[in] oname 出力名
// @param[in] inode_id_array 各識別子のID番号の配列
// @param[in] cover カバー
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::names(
  SizeType onode_id,
  const string& oname,
  const vector<SizeType>& inode_id_array,
  SizeType cover_id)
{
  return true;
}

// @brief .gate 文の処理
// @param[in] onode_id 出力ノードのID番号
// @param[in] oname 出力名
// @param[in] inode_id_array 入力ノードのID番号の配列
// @param[in] cell_id セル番号
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::gate(
  SizeType onode_id,
  const string& oname,
  const vector<SizeType>& inode_id_array,
  int cell_id)
{
  return true;
}

// @brief .latch 文の処理
// @param[in] onode_id 出力ノードのID番号
// @param[in] oname 出力名
// @param[in] inode_id 入力ノードのID番号
// @param[in] loc4 リセット値の位置情報
// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::latch(
  SizeType onode_id,
  const string& oname,
  SizeType inode_id,
  const FileRegion& loc4,
  char rval)
{
  return true;
}

// @brief .end 文の処理
// @param[in] loc 位置情報
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::end(
  const FileRegion& loc
)
{
  return true;
}

// @brief 通常終了時の処理
void
NullBlifHandler::normal_exit()
{
}

// @brief エラー終了時の処理
void
NullBlifHandler::error_exit()
{
}

END_NAMESPACE_YM_BNET
