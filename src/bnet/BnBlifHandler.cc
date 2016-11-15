
/// @file BnBlifHandler.cc
/// @brief BnBlifHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnBlifHandler.h"
#include "ym/BnNetwork.h"
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
  ymuint id = mNetwork->new_input(name);
  mIdMap.add(name_id, id);

  // 関連するポートを作る．
  mNetwork->new_port(name, id);

  return true;
}

// @brief .output 文の読み込み
// @param[in] name_id 文字列のID番号
// @param[in] name 出力ピン名
bool
BnBlifHandler::outputs_elem(ymuint name_id,
			    const char* name)
{
  ymuint id = mNetwork->new_output(name);

  mFaninInfoMap.add(id, vector<ymuint>(1, name_id));

  // 関連するポートを作る．
  mNetwork->new_port(name, id);

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
  // DFF の出力(BnNode的には入力ノード)を生成
  ymuint output_id = mNetwork->new_input(oname);
  mIdMap.add(oname_id, output_id);

  // DFF の入力(BnNode的には出力ノード)を生成
  ymuint input_id = mNetwork->new_output(id2str(iname_id));

  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  mFaninInfoMap.add(input_id, vector<ymuint>(1, iname_id));

  if ( mClockId == 0 ) {
    // クロック端子を作る．
    ymuint node_id = mNetwork->new_input(mClockName);

    // クロックのポートを作る．
    mNetwork->new_port(mClockName, node_id);

    // クロック端子の外部出力を作る．
    mClockId = mNetwork->new_output(mClockName, node_id);
  }

  ymuint clear_id = kBnNullId;
  ymuint preset_id = kBnNullId;
  if ( rval == '0' || rval == '1' ) {
    // リセット端子を作る．
    ymuint node_id = mNetwork->new_input(mResetName);

    // リセット端子のポートを作る．
    mNetwork->new_port(mResetName, node_id);

    // リセット端子の外部出力を作る．
    mResetId = mNetwork->new_output(mResetName, node_id);

    if ( rval == 0 ) {
      clear_id = mResetId;
    }
    else if ( rval == 1 ) {
      preset_id = mResetId;
    }
  }

  // DFF 情報を生成する．
  mNetwork->new_dff(oname, input_id, output_id, mClockId, clear_id, preset_id);

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
