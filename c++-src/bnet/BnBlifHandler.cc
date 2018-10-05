
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
#include "ym/ClibCell.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] parser パーサー
// @param[in] network ネットワーク
// @param[in] clock_name クロック端子名
// @param[in] reset_name リセット端子名
BnBlifHandler::BnBlifHandler(BlifParser& parser,
			     BnNetwork* network,
			     const string& clock_name,
			     const string& reset_name) :
  BlifHandler(parser),
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

  mClockId = kBnNullId;
  mResetId = kBnNullId;

  return true;
}

// @brief セルライブラリの設定
// @param[in] library セルライブラリ
//
// この関数が呼ばれないこともある．
void
BnBlifHandler::set_cell_library(const ClibCellLibrary& library)
{
  mNetwork->set_library(library);
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
BnBlifHandler::inputs_elem(int name_id,
			   const char* name)
{
  auto port_id = mNetwork->new_input_port(name);
  auto port = mNetwork->port(port_id);
  int id = port->bit(0);
  mIdMap.add(name_id, id);

  return true;
}

// @brief .output 文の読み込み
// @param[in] name_id 文字列のID番号
// @param[in] name 出力ピン名
bool
BnBlifHandler::outputs_elem(int name_id,
			    const char* name)
{
  auto port_id = mNetwork->new_output_port(name);
  auto port = mNetwork->port(port_id);
  int id = port->bit(0);
  mFaninInfoMap.add(id, vector<int>({name_id}));

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
BnBlifHandler::names(int oname_id,
		     const char* oname,
		     const vector<int>& inode_id_array,
		     int cover_id)
{
  const BlifCover& cover = id2cover(cover_id);
  const Expr& expr = cover.expr();

  int ni = inode_id_array.size();
  ASSERT_COND( ni == expr.input_size() );
  int node_id = mNetwork->new_expr(oname, ni, expr);
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
BnBlifHandler::gate(int oname_id,
		    const char* oname,
		    const vector<int>& inode_id_array,
		    const ClibCell* cell)
{
  int ni = inode_id_array.size();
  ASSERT_COND( ni == cell->input_num() );
  int node_id = mNetwork->new_logic_cell(oname, cell->name());
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
BnBlifHandler::latch(int oname_id,
		     const char* oname,
		     int iname_id,
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
  bool has_xoutput = false;
  int dff_id = mNetwork->new_dff(oname, has_xoutput, has_clear, has_preset);
  const BnDff* dff = mNetwork->dff(dff_id);

  int output_id = dff->output();
  mIdMap.add(oname_id, output_id);

  int input_id = dff->input();
  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  mFaninInfoMap.add(input_id, vector<int>(1, iname_id));

  if ( mClockId == kBnNullId ) {
    // クロックのポートを作る．
    auto port_id = mNetwork->new_input_port(mClockName);
    auto clock_port = mNetwork->port(port_id);
    // クロックの入力ノード番号を記録する．
    mClockId = clock_port->bit(0);
  }

  // クロック入力とdffのクロック端子を結びつける．
  int clock_id = dff->clock();
  mNetwork->connect(mClockId, clock_id, 0);

  if ( has_clear || has_preset ) {
    if ( mResetId == kBnNullId ) {
      // リセット端子のポートを作る．
      auto port_id = mNetwork->new_input_port(mResetName);
      auto reset_port = mNetwork->port(port_id);
      // リセット端子の入力ノードを記録する．
      mResetId = reset_port->bit(0);
    }
  }
  if ( has_clear ) {
    // リセット入力とクリア端子を結びつける．
    int clear_id = dff->clear();
    mNetwork->connect(mResetId, clear_id, 0);
  }
  else if ( has_preset ) {
    // リセット入力とプリセット端子を結びつける．
    int preset_id = dff->preset();
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
  for ( int node_id = 1; node_id <= mNetwork->node_num(); ++ node_id ) {
    vector<int> fanin_info;
    bool stat = mFaninInfoMap.find(node_id, fanin_info);
    if ( !stat ) {
      continue;
    }

    const BnNode* node = mNetwork->node(node_id);
    if ( node->is_logic() ) {
      int ni = fanin_info.size();
      for ( int i = 0; i < ni; ++ i ) {
	int inode_id;
	bool stat1 = mIdMap.find(fanin_info[i], inode_id);
	ASSERT_COND( stat1 );
	mNetwork->connect(inode_id, node_id, i);
      }
    }
    else if ( node->is_output() ) {
      int iname_id = fanin_info[0];
      int inode_id;
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
