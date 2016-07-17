
/// @file BlifBnNetworkHandler.cc
/// @brief BlifBnNetworkHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BlifBnNetworkHandler.h"
#include "ym/BnBuilder.h"
#include "ym/BlifCover.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] builder ビルダーオブジェクト
// @param[in] clock_name クロック端子名
// @param[in] reset_name リセット端子名
BlifBnNetworkHandler::BlifBnNetworkHandler(BnBuilder* builder,
					   const string& clock_name,
					   const string& reset_name) :
  mBuilder(builder),
  mClockName(clock_name),
  mResetName(reset_name)
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

  mNeedClock = false;
  mNeedReset = false;

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
  ymuint node_id = mBuilder->add_input(name);
  mBuilder->add_port(name, node_id);

  mIdMap.add(name_id, node_id);
  mNodeInfoMap.add(node_id, NodeInfo());

  return true;
}

// @brief .output 文の読み込み
// @param[in] name_id 文字列のID番号
// @param[in] name 出力ピン名
bool
BlifBnNetworkHandler::outputs_elem(ymuint name_id,
				   const char* name)
{
  ymuint node_id = mBuilder->add_output(name);
  mBuilder->add_port(name, node_id);

  mNodeInfoMap.add(node_id, NodeInfo(vector<ymuint>(1, name_id)));

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
  ymuint node_id = mBuilder->add_expr(oname, inode_id_array.size(), cover->expr());
  mIdMap.add(oname_id, node_id);

  mNodeInfoMap.add(node_id, NodeInfo(inode_id_array));

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
  ymuint node_id = mBuilder->add_cell(oname, cell);
  mIdMap.add(oname_id, node_id);
  mNodeInfoMap.add(node_id, NodeInfo(inode_id_array));

  return true;
}

// @brief .latch 文の処理
// @param[in] oname_id 出力ノードのID番号
// @param[in] oname 出力名
// @param[in] inode_id 入力ノードのID番号
// @param[in] loc4 リセット値の位置情報
// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::latch(ymuint oname_id,
			    const char* oname,
			    ymuint inode_id,
			    const FileRegion& loc4,
			    char rval)
{
  // ラッチの出力に対応する入力ノードを作る．
  ymuint onode_id = mBuilder->add_input(oname);
  mIdMap.add(oname_id, onode_id);
  mNodeInfoMap.add(onode_id, NodeInfo());

  ymuint latch_id = mBuilder->add_dff(oname);
  mBuilder->set_dff_output(latch_id, onode_id);

  // この時点では実際の入力ノードができていないかも知れないので情報を記録しておく．
  ASSERT_COND( latch_id == mLatchInfoList.size() );
  mLatchInfoList.push_back(LatchInfo(inode_id, rval));

  mNeedClock = true;

  if ( rval == '0' || rval == '1' ) {
    mNeedReset = true;
  }

  return true;
}

// @brief 終了処理
// @param[in] loc 位置情報
bool
BlifBnNetworkHandler::end(const FileRegion& loc)
{
  ymuint clock_id = 0;
  if ( mNeedClock ) {
    // クロック端子を作る．
    clock_id = mBuilder->add_input(mClockName);
    mBuilder->add_port(mClockName, clock_id);
    mNodeInfoMap.add(clock_id, NodeInfo());
  }

  ymuint reset_id = 0;
  if ( mNeedReset ) {
    // クリア端子を作る．
    reset_id = mBuilder->add_input(mResetName);
    mBuilder->add_port(mResetName, reset_id);
    mNodeInfoMap.add(reset_id, NodeInfo());
  }

  // 論理ノードのファンインを設定する．
  for (ymuint node_id = 0; node_id < mBuilder->node_num(); ++ node_id) {
    NodeInfo node_info;
    bool stat = mNodeInfoMap.find(node_id, node_info);
    if ( !stat ) {
      cout << "error!: Node#" << node_id << " not found" << endl;
      continue;
    }
    ASSERT_COND( stat );
    ymuint ni = node_info.mInameIdArray.size();
    for (ymuint i = 0; i < ni; ++ i) {
      ymuint inode_id;
      bool stat1 = mIdMap.find(node_info.mInameIdArray[i], inode_id);
      ASSERT_COND( stat1 );
      mBuilder->set_fanin(node_id, i, inode_id);
    }
  }

  // DFFノードの入力を設定する．
  for (ymuint dff_id = 0; dff_id < mBuilder->dff_num(); ++ dff_id) {
    const LatchInfo& latch_info = mLatchInfoList[dff_id];
    ymuint inode_id;
    bool stat = mIdMap.find(latch_info.mInameId, inode_id);
    ASSERT_COND( stat );

    // まず外部出力ノードを作る．
    string oname = id2str(latch_info.mInameId);
    ymuint onode_id = mBuilder->add_output(oname);
    mBuilder->set_output_input(onode_id, inode_id);

    mBuilder->set_dff_input(dff_id, onode_id);

    mBuilder->set_dff_clock(dff_id, clock_id);

    if ( latch_info.mResetVal == '0' ) {
      mBuilder->set_dff_clear(dff_id, reset_id);
    }
    else if ( latch_info.mResetVal == '1' ) {
      mBuilder->set_dff_preset(dff_id, reset_id);
    }
  }

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
  mBuilder->clear();
}

END_NAMESPACE_YM_BNET
