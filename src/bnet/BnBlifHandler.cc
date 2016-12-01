
/// @file BnBlifHandler.cc
/// @brief BnBlifHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnBlifHandler.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"
#include "ym/BlifCover.h"
#include "ym/Cell.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] network ネットワーク
// @param[in] clock_name クロック端子名
// @param[in] reset_name リセット端子名
BnBlifHandler::BnBlifHandler(BnNetwork* network,
			     const string& clock_name,
			     const string& reset_name) :
  mNetwork(network),
  mClockName(clock_name),
  mResetName(reset_name)
{
}

// @brief デストラクタ
BnBlifHandler::~BnBlifHandler()
{
}

// @brief 初期化
bool
BnBlifHandler::init()
{
  mNetwork->clear();

  mIdMap.clear();
  mFaninInfoMap.clear();

  mClockId = 0;
  mResetId = 0;

  return true;
}

// @brief .model 文の読み込み
// @param[in] loc 位置情報
// @param[in] name model名
bool
BnBlifHandler::model(const FileRegion& loc1,
		     const FileRegion& loc2,
		     const char* name)
{
  mNetwork->set_name(name);

  return true;
}

// @brief .input 文の読み込み
// @param[in] loc 位置情報
// @param[in] name_id 文字列のID番号
// @param[in] name 入力ピン名
bool
BnBlifHandler::inputs_elem(ymuint name_id,
			   const char* name)
{
  BnPort* port = mNetwork->new_input_port(name);
  ymuint id = port->bit(0);
  mIdMap.add(name_id, id);

  return true;
}

// @brief .output 文の読み込み
// @param[in] name_id 文字列のID番号
// @param[in] name 出力ピン名
bool
BnBlifHandler::outputs_elem(ymuint name_id,
			    const char* name)
{
  BnPort* port = mNetwork->new_output_port(name);
  ymuint id = port->bit(0);

  mFaninInfoMap.add(id, vector<ymuint>(1, name_id));

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
BnBlifHandler::names(ymuint oname_id,
		     const char* oname,
		     const vector<ymuint>& inode_id_array,
		     ymuint cover_id)
{
  const BlifCover* cover = id2cover(cover_id);
  const Expr& expr = cover->expr();

  ymuint ni = inode_id_array.size();
  ASSERT_COND( ni == expr.input_size() );
  ymuint node_id = mNetwork->new_expr(oname, ni, expr);
  mIdMap.add(oname_id, node_id);

  mFaninInfoMap.add(node_id, inode_id_array);

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
BnBlifHandler::gate(ymuint oname_id,
		    const char* oname,
		    const vector<ymuint>& inode_id_array,
		    const Cell* cell)
{
  const Expr& expr = cell->logic_expr(0);
  ymuint ni = inode_id_array.size();
  ASSERT_COND( ni == expr.input_size() );
  ymuint node_id = mNetwork->new_expr(oname, ni, expr, cell);
  mIdMap.add(oname_id, node_id);

  mFaninInfoMap.add(node_id, inode_id_array);

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
BnBlifHandler::latch(ymuint oname_id,
		     const char* oname,
		     ymuint iname_id,
		     const FileRegion& loc4,
		     char rval)
{
  bool has_clear = false;
  bool has_preset = false;
  if ( rval == '0' ) {
    has_clear = true;
  }
  else if ( rval == '1' ) {
    has_preset = true;
  }
  BnDff* dff = mNetwork->new_dff(oname, has_clear, has_preset);

  ymuint output_id = dff->output();
  mIdMap.add(oname_id, output_id);

  ymuint input_id = dff->input();
  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  mFaninInfoMap.add(input_id, vector<ymuint>(1, iname_id));

  if ( mClockId == kBnNullId ) {
    // クロックのポートを作る．
    BnPort* clock_port = mNetwork->new_input_port(mClockName);
    // クロックの入力ノード番号を記録する．
    mClockId = clock_port->bit(0);
  }

  // クロック入力とdffのクロック端子を結びつける．
  ymuint clock_id = dff->clock();
  mNetwork->connect(mClockId, clock_id, 0);

  if ( has_clear || has_preset ) {
    if ( mResetId == kBnNullId ) {
      // リセット端子のポートを作る．
      BnPort* reset_port = mNetwork->new_input_port(mResetName);
      // リセット端子の入力ノードを記録する．
      mResetId = reset_port->bit(0);
    }
  }
  if ( has_clear ) {
    // リセット入力とクリア端子を結びつける．
    ymuint clear_id = dff->clear();
    mNetwork->connect(mResetId, clear_id, 0);
  }
  else if ( has_preset ) {
    // リセット入力とプリセット端子を結びつける．
    ymuint preset_id = dff->preset();
    mNetwork->connect(mResetId, preset_id, 0);
  }

  return true;
}

// @brief 終了処理
// @param[in] loc 位置情報
bool
BnBlifHandler::end(const FileRegion& loc)
{
  // ノードのファンインを設定する．
  for (ymuint node_id = 1; node_id <= mNetwork->node_num(); ++ node_id) {
    vector<ymuint> fanin_info;
    bool stat = mFaninInfoMap.find(node_id, fanin_info);
    if ( !stat ) {
      continue;
    }

    const BnNode* node = mNetwork->node(node_id);
    if ( node->is_logic() ) {
      ymuint ni = fanin_info.size();
      for (ymuint i = 0; i < ni; ++ i) {
	ymuint inode_id;
	bool stat1 = mIdMap.find(fanin_info[i], inode_id);
	ASSERT_COND( stat1 );
	mNetwork->connect(inode_id, node_id, i);
      }
    }
    else if ( node->is_output() ) {
      ymuint iname_id = fanin_info[0];
      ymuint inode_id;
      bool stat1 = mIdMap.find(iname_id, inode_id);
      ASSERT_COND( stat1 );
      mNetwork->connect(inode_id, node_id, 0);
    }
  }

  bool stat = mNetwork->wrap_up();

  return stat;
}

// @brief 通常終了時の処理
void
BnBlifHandler::normal_exit()
{
}

// @brief エラー終了時の処理
void
BnBlifHandler::error_exit()
{
  mNetwork->clear();
}

END_NAMESPACE_YM_BNET
