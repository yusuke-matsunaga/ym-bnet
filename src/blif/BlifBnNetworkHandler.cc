
/// @file BlifBnNetworkHandler.cc
/// @brief BlifBnNetworkHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BlifBnNetworkHandler.h"
#include "ym/BnNetwork.h"
#include "ym/BlifCover.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] builder ビルダーオブジェクト
BlifBnNetworkHandler::BlifBnNetworkHandler(BnNetwork* network) :
  mNetwork(network)
{
}

// @brief デストラクタ
BlifBnNetworkHandler::~BlifBnNetworkHandler()
{
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
  mNetwork->set_model_name(name);

  return true;
}

// @brief .input 文の読み込み
// @param[in] loc 位置情報
// @param[in] name_id 文字列のID番号
// @param[in] name 入力ピン名
bool
BlifBnNetworkHandler::inputs_elem(ymuint name_id,
				  const char* name)
{
  bool stat = mNetwork->new_input(name_id, name);
  if ( stat ) {
    mNetwork->new_port(name, vector<ymuint>(1, name_id));
  }

  return stat;
}

// @brief .output 文の読み込み
// @param[in] loc 位置情報
// @param[in] name 出力ノード名
// @param[in] name 出力ピン名
bool
BlifBnNetworkHandler::outputs_elem(ymuint name_id,
				   const char* name)
{
  mNetwork->new_port(name, vector<ymuint>(1, name_id));

  return true;
}

// @brief .names 文の処理
// @param[in] onode_id ノード名のID番号
// @param[in] oname 出力名
// @param[in] inode_id_array ファンイン各のID番号の配列
// @param[in] cover_id カバーID
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::names(ymuint onode_id,
			    const char* oname,
			    const vector<ymuint>& inode_id_array,
			    ymuint cover_id)
{
  const BlifCover* cover = id2cover(cover_id);
  BnLogicType logic_type = cover->logic_type();

  bool stat = false;
  if ( logic_type == kBnLt_EXPR ) {
    stat = mNetwork->new_expr(onode_id, oname, inode_id_array, cover->expr());
  }
  else {
    stat = mNetwork->new_primitive(onode_id, oname, inode_id_array, logic_type);
  }

  return stat;
}

// @brief .gate 文の処理
// @param[in] onode_id 出力ノードのID番号
// @param[in] oname 出力名
// @param[in] cell セル
// @param[in] inode_id_array 入力ノードのID番号の配列
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::gate(ymuint onode_id,
			   const char* oname,
			   const vector<ymuint>& inode_id_array,
			   const Cell* cell)
{
  bool stat = mNetwork->new_cell(onode_id, oname, inode_id_array, cell);

  return stat;
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
BlifBnNetworkHandler::latch(ymuint onode_id,
			    const char* oname,
			    ymuint inode_id,
			    const FileRegion& loc4,
			    char rval)
{
  mNetwork->new_dff(onode_id, oname, inode_id, rval);

  return true;
}

// @brief 終了処理
// @param[in] loc 位置情報
bool
BlifBnNetworkHandler::end(const FileRegion& loc)
{
  bool stat = mNetwork->wrap_up();
  return stat;
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
