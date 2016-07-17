
/// @file Iscas89BnNetworkHandler.cc
/// @brief Iscas89BnNetworkHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "Iscas89BnNetworkHandler.h"
#include "ym/BnBuilder.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] builder ビルダーオブジェクト
// @param[in] clock_name クロック端子名
Iscas89BnNetworkHandler::Iscas89BnNetworkHandler(BnBuilder* builder,
						 const string& clock_name) :
  mBuilder(builder),
  mClockName(clock_name)
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
  mBuilder->clear();
  mBuilder->set_model_name("iscas89");

  mIdMap.clear();
  mNodeInfoMap.clear();
  mLatchInfoList.clear();

  mNeedClock = false;

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
  ymuint node_id = mBuilder->add_input(name);
  mBuilder->add_port(name, node_id);

  mIdMap.add(name_id, node_id);
  mNodeInfoMap.add(node_id, NodeInfo());

  return true;
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
  ymuint node_id = mBuilder->add_output(name);
  mBuilder->add_port(name, node_id);

  mNodeInfoMap.add(node_id, NodeInfo(vector<ymuint>(1, name_id)));

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
  ymuint node_id = mBuilder->add_primitive(oname, iname_list.size(), logic_type);
  mIdMap.add(oname_id, node_id);

  mNodeInfoMap.add(node_id, NodeInfo(iname_list));

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
  // DFF の出力に対応する入力ノードを作る．
  ymuint onode_id = mBuilder->add_input(oname);
  mIdMap.add(oname_id, onode_id);
  mNodeInfoMap.add(onode_id, NodeInfo());

  ymuint dff_id = mBuilder->add_dff(oname);
  mBuilder->set_dff_output(dff_id, onode_id);

  ASSERT_COND( dff_id == mLatchInfoList.size() );
  mLatchInfoList.push_back(LatchInfo(iname_id));

  mNeedClock = true;

  return true;
}

// @brief 終了操作
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
Iscas89BnNetworkHandler::end()
{
  ymuint clock_id = 0;
  if ( mNeedClock ) {
    // クロック端子を作る．
    clock_id = mBuilder->add_input(mClockName);
    mBuilder->add_port(mClockName, clock_id);
    mNodeInfoMap.add(clock_id, NodeInfo());
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
  }

  return true;
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
  mBuilder->clear();
}

END_NAMESPACE_YM_BNET
