﻿
/// @file BlifBnNetworkHandler.cc
/// @brief BlifBnNetworkHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BlifBnNetworkHandler.h"
#include "BnNetworkImpl.h"
#include "ym/BnNode.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
BlifBnNetworkHandler::BlifBnNetworkHandler() :
  mNetwork(nullptr)
{
}

// @brief デストラクタ
BlifBnNetworkHandler::~BlifBnNetworkHandler()
{
}

// @brief 読み込む対象のネットワークを設定する．
void
BlifBnNetworkHandler::set(BnNetworkImpl* network)
{
  mNetwork = network;
}

// @brief 初期化
bool
BlifBnNetworkHandler::init()
{
  mNetwork->clear();

  return true;
}

// @brief .model 文の読み込み
// @param[in] loc 位置情報
// @param[in] name model名
bool
BlifBnNetworkHandler::model(const FileRegion& loc1,
			    const FileRegion& loc2,
			    const char* name)
{
  mNetwork->set_model(name);

  return true;
}

// @brief .input 文の読み込み
// @param[in] loc 位置情報
// @param[in] name 入力ノード名
bool
BlifBnNetworkHandler::inputs_elem(ymuint name_id)
{
  mNetwork->new_input(name_id, id2str(name_id));

  return true;
}

// @brief .output 文の読み込み
// @param[in] loc 位置情報
// @param[in] name 出力ノード名
bool
BlifBnNetworkHandler::outputs_elem(ymuint name_id)
{
  mNetwork->new_output(name_id);

  return true;
}

// @brief .names 文の処理
// @param[in] onode_id ノード名のID番号
// @param[in] inode_id_array ファンイン各のID番号の配列
// @param[in] cover_id カバーID
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::names(ymuint onode_id,
			    const vector<ymuint>& inode_id_array,
			    ymuint cover_id)
{
  mNetwork->new_logic(onode_id, id2str(onode_id), inode_id_array, cover_id);

  return true;
}

// @brief .gate 文の処理
// @param[in] onode_id 出力ノードのID番号
// @param[in] cell セル
// @param[in] inode_id_array 入力ノードのID番号の配列
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::gate(ymuint onode_id,
			   const vector<ymuint>& inode_id_array,
			   const Cell* cell)
{
  mNetwork->new_logic(onode_id, id2str(onode_id), inode_id_array, cell);

  return true;
}

// @brief .latch 文の処理
// @param[in] onode_id 出力ノードのID番号
// @param[in] inode_id 入力ノードのID番号
// @param[in] loc4 リセット値の位置情報
// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::latch(ymuint onode_id,
			    ymuint inode_id,
			    const FileRegion& loc4,
			    char rval)
{
  mNetwork->new_latch(onode_id, id2str(onode_id), inode_id, rval);

  return true;
}

// @brief カバー情報の登録
// @param[in] cover_id カバーのID番号
// @param[in] input_num 入力数
// @param[in] cube_num キューブ数
// @param[in] ipat_str 入力パタン文字列
// @param[in] opat 出力パタン
void
BlifBnNetworkHandler::cover(ymuint cover_id,
			    ymuint input_num,
			    ymuint cube_num,
			    const string& ipat_str,
			    BlifPat opat)
{
  mNetwork->new_cover(cover_id, input_num, cube_num, ipat_str, opat);
}

// @brief 終了処理
// @param[in] loc 位置情報
bool
BlifBnNetworkHandler::end(const FileRegion& loc)
{
  return true;
}

// @brief 通常終了時の処理
void
BlifBnNetworkHandler::normal_exit()
{
}

// @brief エラー終了時の処理
void
BlifBnNetworkHandler::error_exit()
{
  mNetwork->clear();
}

END_NAMESPACE_YM_BNET
