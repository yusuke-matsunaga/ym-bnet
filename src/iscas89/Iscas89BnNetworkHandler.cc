
/// @file Iscas89BnNetworkHandler.cc
/// @brief Iscas89BnNetworkHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "Iscas89BnNetworkHandler.h"
#include "ym/BnNetwork.h"
#include "ym/BnNode.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
Iscas89BnNetworkHandler::Iscas89BnNetworkHandler(BnNetwork* network) :
  mNetwork(network)
{
}

// @brief デストラクタ
Iscas89BnNetworkHandler::~Iscas89BnNetworkHandler()
{
}

// @brief 初期化
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
Iscas89BnNetworkHandler::init()
{
  mNetwork->clear();
  mNetwork->set_model_name("iscas89");
  return true;
}

// @brief INPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 入力ピン名の ID 番号
// @param[in] name 入力ピン名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
Iscas89BnNetworkHandler::read_input(const FileRegion& loc,
				    ymuint name_id,
				    const char* name)
{
  bool stat = mNetwork->new_input(name_id, name);
  if ( stat ) {
    mNetwork->new_port(name, vector<ymuint>(1, name_id));
  }

  return stat;
}

// @brief OUTPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 出力ピン名の ID 番号
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
Iscas89BnNetworkHandler::read_output(const FileRegion& loc,
				     ymuint name_id,
				     const char* name)
{
  mNetwork->new_port(name, vector<ymuint>(1, name_id));

  return true;
}

// @brief ゲート文を読み込む．
// @param[in] loc ファイル位置
// @param[in] logic_type ゲートの型
// @param[in] oname_id 出力名の ID 番号
// @param[in] oname 出力名
// @param[in] iname_list 入力名のリスト
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
Iscas89BnNetworkHandler::read_gate(const FileRegion& loc,
				   BnLogicType logic_type,
				   ymuint oname_id,
				   const char* oname,
				   const vector<ymuint>& iname_list)
{
  mNetwork->new_primitive(oname_id, oname, iname_list, logic_type);

  return true;
}

// @brief D-FF用のゲート文を読み込む．
// @param[in] loc ファイル位置
// @param[in] oname_id 出力名の ID 番号
// @param[in] oname 出力名
// @param[in] iname_id 入力名の ID 番号
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
Iscas89BnNetworkHandler::read_dff(const FileRegion& loc,
				  ymuint oname_id,
				  const char* oname,
				  ymuint iname_id)
{
  mNetwork->new_dff(oname_id, oname, iname_id, ' ');

  return true;
}

// @brief 終了操作
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
Iscas89BnNetworkHandler::end()
{
  bool stat = mNetwork->wrap_up();

  return stat;
}

// @brief 通常終了時の処理
void
Iscas89BnNetworkHandler::normal_exit()
{
}

// @brief エラー終了時の処理
void
Iscas89BnNetworkHandler::error_exit()
{
  mNetwork->clear();
}

END_NAMESPACE_YM_BNET
