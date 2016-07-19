
/// @file BlifBnNetworkHandler.cc
/// @brief BlifBnNetworkHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BlifBnNetworkHandler.h"
#include "BlifBnBuilder.h"
#include "ym/BlifCover.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] builder ビルダーオブジェクト
BlifBnNetworkHandler::BlifBnNetworkHandler(BlifBnBuilder* builder) :
  mBuilder(builder)
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
  mBuilder->clear();

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
  mBuilder->set_model_name(name);

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
  mBuilder->add_input(name_id, name);

  return true;
}

// @brief .output 文の読み込み
// @param[in] name_id 文字列のID番号
// @param[in] name 出力ピン名
bool
BlifBnNetworkHandler::outputs_elem(ymuint name_id,
				   const char* name)
{
  mBuilder->add_output(name_id, name);

  return true;
}

// @brief .names 文の処理
// @param[in] oname_id ノード名のID番号
// @param[in] oname 出力名
// @param[in] inode_id_array ファンイン各のID番号の配列
// @param[in] cover_id カバーID
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::names(ymuint oname_id,
			    const char* oname,
			    const vector<ymuint>& inode_id_array,
			    ymuint cover_id)
{
  const BlifCover* cover = id2cover(cover_id);
  const Expr& expr = cover->expr();

  mBuilder->add_expr(oname_id, oname, inode_id_array, expr);

  return true;
}

// @brief .gate 文の処理
// @param[in] oname_id 出力ノードのID番号
// @param[in] oname 出力名
// @param[in] cell セル
// @param[in] inode_id_array 入力ノードのID番号の配列
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::gate(ymuint oname_id,
			   const char* oname,
			   const vector<ymuint>& inode_id_array,
			   const Cell* cell)
{
  mBuilder->add_cell(oname_id, oname, inode_id_array, cell);

  return true;
}

// @brief .latch 文の処理
// @param[in] oname_id 出力ノードのID番号
// @param[in] oname 出力名
// @param[in] iname_id 入力ノードのID番号
// @param[in] loc4 リセット値の位置情報
// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::latch(ymuint oname_id,
			    const char* oname,
			    ymuint iname_id,
			    const FileRegion& loc4,
			    char rval)
{
  mBuilder->add_dff(oname_id, oname, iname_id, id2str(iname_id), rval);

  return true;
}

// @brief 終了処理
// @param[in] loc 位置情報
bool
BlifBnNetworkHandler::end(const FileRegion& loc)
{

  bool stat = mBuilder->wrap_up();

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
  mBuilder->clear();
}

END_NAMESPACE_YM_BNET
