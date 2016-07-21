
/// @file BnIscas89Handler.cc
/// @brief BnIscas89Handler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnIscas89Handler.h"
#include "ym/BnBuilder.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] builder ビルダーオブジェクト
// @param[in] clock_name クロック端子名
BnIscas89Handler::BnIscas89Handler(BnBuilder* builder,
				   const string& clock_name) :
  mBuilder(builder),
  mClockName(clock_name)
{
}

// @brief デストラクタ
BnIscas89Handler::~BnIscas89Handler()
{
}

// @brief 初期化
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::init()
{
  mBuilder->clear();

  mBuilder->set_model_name("iscas89_network");

  mIdMap.clear();
  mFaninInfoMap.clear();

  mClockId = 0;

  return true;
}

// @brief INPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 入力ピン名の ID 番号
// @param[in] name 入力ピン名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::read_input(const FileRegion& loc,
			     ymuint name_id,
			     const char* name)
{
  ymuint id = mBuilder->add_input(name);
  mIdMap.add(name_id, id);

  mBuilder->add_port(name, id);

  return true;
}

// @brief OUTPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 出力ピン名の ID 番号
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::read_output(const FileRegion& loc,
			      ymuint name_id,
			      const char* name)
{
  ymuint id = mBuilder->add_output(name, 0);

  mFaninInfoMap.add(id, vector<ymuint>(1, name_id));

  mBuilder->add_port(name, id);

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
BnIscas89Handler::read_gate(const FileRegion& loc,
			    BnLogicType logic_type,
			    ymuint oname_id,
			    const char* oname,
			    const vector<ymuint>& iname_list)
{
  ymuint ni = iname_list.size();
  ymuint id = mBuilder->add_primitive(oname, logic_type, ni);
  mIdMap.add(oname_id, id);

  mFaninInfoMap.add(id, iname_list);

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
BnIscas89Handler::read_dff(const FileRegion& loc,
			   ymuint oname_id,
			   const char* oname,
			   ymuint iname_id)
{
  // DFF 情報を生成する．
  BnBuilder::DffInfo& dff_info = mBuilder->add_dff(oname);

  // DFF の出力(BnNode的には入力ノード)を生成する．
  ymuint input_id = mBuilder->add_input(oname);
  mIdMap.add(oname_id, input_id);

  dff_info.mOutput = input_id;

  // DFF の入力(BnNode的には出力ノード)を生成する．
  ymuint output_id = mBuilder->add_output(id2str(iname_id), 0);

  dff_info.mInput = output_id;

  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  mFaninInfoMap.add(output_id, vector<ymuint>(1, iname_id));

  if ( mClockId == 0 ) {
    // クロック端子を作る．
    ymuint node_id = mBuilder->add_input(mClockName);

    // クロックのポートを作る．
    mBuilder->add_port(mClockName, node_id);

    // クロック端子の外部出力を作る．
    mClockId = mBuilder->add_output(mClockName, node_id);
  }

  dff_info.mClock = mClockId;

  return true;
}

// @brief 終了操作
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::end()
{
  // ノードのファンインを設定する．
  for (ymuint node_id = 1; node_id <= mBuilder->node_num(); ++ node_id) {
    vector<ymuint> fanin_info;
    bool stat = mFaninInfoMap.find(node_id, fanin_info);
    if ( !stat ) {
      continue;
    }

    const BnBuilder::NodeInfo& node_info = mBuilder->node(node_id);
    if ( node_info.mType == kBnLogic ) {
      ymuint ni = fanin_info.size();
      for (ymuint i = 0; i < ni; ++ i) {
	ymuint inode_id;
	bool stat1 = mIdMap.find(fanin_info[i], inode_id);
	ASSERT_COND( stat1 );
	mBuilder->connect(inode_id, node_id, i);
      }
    }
    else if ( node_info.mType == kBnOutput ) {
      ymuint iname_id = fanin_info[0];
      ymuint inode_id;
      bool stat1 = mIdMap.find(iname_id, inode_id);
      ASSERT_COND( stat1 );
      mBuilder->connect(inode_id, node_id, 0);
    }
  }

  bool stat = mBuilder->wrap_up();

  return stat;
}

// @brief 通常終了時の処理
void
BnIscas89Handler::normal_exit()
{
}

// @brief エラー終了時の処理
void
BnIscas89Handler::error_exit()
{
  mBuilder->clear();
}

END_NAMESPACE_YM_BNET
