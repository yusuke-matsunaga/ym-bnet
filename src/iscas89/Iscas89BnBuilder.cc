
/// @file Iscas89BnBuilder.cc
/// @brief Iscas89BnBuilder の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "Iscas89BnBuilder.h"

#include "ym/Iscas89Parser.h"
#include "Iscas89BnNetworkHandler.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス Iscas89BnBuilder
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] clock_name クロック端子名
Iscas89BnBuilder::Iscas89BnBuilder(const string& clock_name) :
  mClockName(clock_name)
{
}

// @brief デストラクタ
Iscas89BnBuilder::~Iscas89BnBuilder()
{
}

// @brief iscas89(.bench) 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @retval true 正常に読み込めた
// @retval false 読み込み中にエラーが起こった．
bool
Iscas89BnBuilder::read_blif(const string& filename)
{
  Iscas89BnNetworkHandler* handler = new Iscas89BnNetworkHandler(this);

  Iscas89Parser parser;
  parser.add_handler(handler);

  bool stat = parser.read(filename);

  return stat;
}

// @brief 内容をクリアする．
//
// コンストラクタ直後と同じ状態になる．
void
Iscas89BnBuilder::clear()
{
  mName = "iscas89_network";

  mPortInfoList.clear();
  mDffInfoList.clear();
  mNodeInfoList.clear();
  mIdMap.clear();
  mFaninInfoMap.clear();

  mNeedClock = false;

  mSane = false;
}

// @brief DFFを追加する．
// @param[in] oname_id DFF名のID番号
// @param[in] oname DFF名
// @param[in] iname_id 入力名のID番号
// @param[in] iname 入力名
void
Iscas89BnBuilder::add_dff(ymuint oname_id,
			  const string& oname,
			  ymuint iname_id,
			  const string& iname)
{
  // DFF 情報を生成
  ymuint id = mDffInfoList.size();
  mDffInfoList.push_back(DffInfo(oname));

  DffInfo& dff_info = mDffInfoList[id];

  // DFF の出力(BnNode的には入力ノード)を生成
  mNodeInfoList.push_back(NodeInfo(oname));
  ymuint input_id = mNodeInfoList.size();

  dff_info.mOutput = input_id;

  mIdMap.add(oname_id, input_id);

  // DFF の入力(BnNode的には出力ノード)を生成
  mNodeInfoList.push_back(NodeInfo(iname));
  ymuint output_id = mNodeInfoList.size();

  dff_info.mInput = output_id;

  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  mFaninInfoMap.add(output_id, vector<ymuint>(1, iname_id));

  mNeedClock = true;

  mSane = false;

  return id;
}

// @brief 外部入力ノードを追加する．
// @param[in] name_id 名前のID番号
// @param[in] name 名前
void
Iscas89BnBuilder::add_input(ymuint name_id,
			    const string& name)
{
  mNodeInfoList.push_back(NodeInfo(name));
  ymuint id = mNodeInfoList.size();
  mIdMap.add(name_id, id);

  mPortInfoList.push_back(PortInfo(name, id));

  mSane = false;
}

// @brief 外部出力ノードを追加する．
// @param[in] name_id 名前のID番号
// @param[in] name 名前
void
Iscas89BnBuilder::add_output(ymuint name_id,
			     const string& name)
{
  mNodeInfoList.push_back(NodeInfo(name, 0));
  ymuint id = mNodeInfoList.size();

  mFaninInfoMap.add(id, vector<ymuint>(1, name_id));

  mPortInfoList.push_back(PortInfo(name, id));

  mSane = false;

  return id;
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] oname_id 出力名のID番号
// @param[in] oname 出力名
// @param[in] inode_id_array ファンインのノード番号のリスト
// @param[in] logic_type 論理型
void
Iscas89BnBuilder::add_expr(ymuint oname_id,
			   const string& oname,
			   const vector<ymuint>& inode_id_array,
			   BnLogicType logic_type)
{
  ymuint ni = inode_id_array.size();
  mNodeInfoList.push_back(NodeInfo(oname, ni, logic_type));
  ymuint id = mNodeInfoList.size();
  mIdMap.add(oname_id, id);

  mFaninInfoMap.add(id, inode_id_array);

  mSane = false;
}

// @brief 整合性のチェックを行う．
// @return チェック結果を返す．
//
// チェック項目は以下の通り
// - model_name() が設定されているか？
//   設定されていない場合にはデフォルト値を設定する．
//   エラーとはならない．
// - 各ノードのファンインが設定されているか？
bool
Iscas89BnBuilder::sanity_check()
{
  if ( mSane ) {
    return true;
  }

  if ( name() == string() ) {
    // name が設定されていない．
    set_model_name("network");
  }
  ymuint clock_id = 0;
  if ( mNeedClock ) {
    // クロック端子を作る．
    ymuint node_id = mBuilder->add_input(mClockName);
    mBuilder->add_port(mClockName, node_id);

    // クロック端子の外部出力を作る．
    clock_id = mBuilder->add_output(mClockName);
    mBuilder->set_output_input(clock_id, node_id);
  }

  ymuint reset_id = 0;
  if ( mNeedReset ) {
    // クリア端子を作る．
    ymuint node_id = mBuilder->add_input(mResetName);
    mBuilder->add_port(mResetName, node_id);

    // クリア端子の外部出力を作る．
    reset_id = mBuilder->add_output(mResetName);
    mBuilder->set_output_input(reset_id, node_id);
  }

  // 論理ノードのファンインを設定する．
  for (ymuint node_id = 1; node_id <= mBuilder->node_num(); ++ node_id) {
    NodeInfo node_info;
    bool stat = mNodeInfoMap.find(node_id, node_info);
    if ( !stat ) {
      continue;
    }

    const BnBuilder::NodeInfo bnode_info = const_cast<const BnBuilder*>(mBuilder)->node(node_id);
    if ( bnode_info.mType == BnNode::kLogic ) {
      ymuint ni = node_info.mInameIdArray.size();
      for (ymuint i = 0; i < ni; ++ i) {
	ymuint inode_id;
	bool stat1 = mIdMap.find(node_info.mInameIdArray[i], inode_id);
	ASSERT_COND( stat1 );
	mBuilder->set_fanin(node_id, i, inode_id);
      }
    }
    else if ( bnode_info.mType == BnNode::kOutput ) {
      ymuint iname_id = node_info.mInameIdArray[0];
      ymuint inode_id;
      bool stat1 = mIdMap.find(iname_id, inode_id);
      ASSERT_COND( stat1 );
      mBuilder->set_output_input(node_id, inode_id);
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

    // DFF の入力を設定する．
    mBuilder->set_dff_input(dff_id, onode_id);

    // DFF のクロックを設定する．
    mBuilder->set_dff_clock(dff_id, clock_id);

    // DFF のリセット(クリア or プリセット)を設定する．
    if ( latch_info.mResetVal == '0' ) {
      mBuilder->set_dff_clear(dff_id, reset_id);
    }
    else if ( latch_info.mResetVal == '1' ) {
      mBuilder->set_dff_preset(dff_id, reset_id);
    }
  }

  bool error = false;

  // DFF の入力，出力，クロックにノードが設定されているか調べる．
  for (ymuint i = 0; i < dff_num(); ++ i) {
    const DffInfo& dff_info = dff(i);
    if ( dff_info.mInput == 0 ) {
      cerr << "Dff(" << dff_info.mName << ").mInput is not set" << endl;
      error = true;
    }
    if ( dff_info.mOutput == 0 ) {
      cerr << "Dff(" << dff_info.mName << ").mOutput is not set" << endl;
      error = true;
    }
    if ( dff_info.mClock == 0 ) {
      cerr << "Dff(" << dff_info.mName << ").mClock is not set" << endl;
      error = true;
    }
  }

  // ラッチの入力，出力，イネーブルにノードが設定されているか調べる．
  for (ymuint i = 0; i < latch_num(); ++ i) {
    const LatchInfo& latch_info = latch(i);
    if ( latch_info.mInput == 0 ) {
      cerr << "Latch(" << latch_info.mName << ").mInput is not set" << endl;
      error = true;
    }
    if ( latch_info.mOutput == 0 ) {
      cerr << "Latch(" << latch_info.mName << ").mOutput is not set" << endl;
      error = true;
    }
    if ( latch_info.mEnable== 0 ) {
      cerr << "Latch(" << latch_info.mName << ").mEnable is not set" << endl;
      error = true;
    }
  }

  // ノードにファンインが設定されているか調べる．
  for (ymuint i = 1; i <= node_num(); ++ i) {
    const NodeInfo& node_info = node(i);
    ymuint ni = node_info.mInodeList.size();
    for (ymuint j = 0; j < ni; ++ j) {
      if ( node_info.mInodeList[j] == 0 ) {
	cerr << "Node(" << node_info.mName << ").mInodeList[" << j << "] is not set" << endl;
	error = true;
      }
    }
  }

  if ( !error ) {
    mSane = true;
  }

  return mSane;

}

// @brief 名前を得る．
string
Iscas89BnBuilder::name() const
{
  return mName;
}

// @brief ポート数を得る．
ymuint
Iscas89BnBuilder::port_num() const
{
  return mPortInfoList.size();
}

// @brief ポート情報を得る．
// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
const Iscas89BnBuilder::PortInfo&
Iscas89BnBuilder::port(ymuint pos) const
{
  ASSERT_COND( pos < port_num() );
  return mPortInfoList[pos];
}

// @brief DFF数を得る．
ymuint
Iscas89BnBuilder::dff_num() const
{
  return mDffInfoList.size();
}

// @brief DFF情報を得る．
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
const BnBuilder::DffInfo&
Iscas89BnBuilder::dff(ymuint pos) const
{
  ASSERT_COND( pos < dff_num() );
  return mDffInfoList[pos];
}

// @brief ラッチ数を得る．
ymuint
Iscas89BnBuilder::latch_num() const
{
  return 0;
}

// @brief ラッチ情報を得る．
// @param[in] pos 位置番号 ( 0 <= pos < latch_num() )
const BnBuilder::LatchInfo&
Iscas89BnBuilder::latch(ymuint pos) const
{
  ASSERT_NOT_REACHED;
  return LatchInfo();
}

// @brief ノード数を得る．
ymuint
Iscas89BnBuilder::node_num() const
{
  return mNodeInfoList.size();
}

// @brief ノード情報を得る．
// @param[in] id ノード番号
const BnBuilder::NodeInfo&
Iscas89BnBuilder::node(ymuint id) const
{
  ASSERT_COND( id > 0 && id <= node_num() );
  return mNodeInfoList[id - 1];
}

// @brief ノード情報を得る．
// @param[in] id ノード番号 ( 0 < id <= node_num() )
//
// ノード番号 0 は不正な値として予約されている．
BnBuilder::NodeInfo&
Iscas89BnBuilder::node(ymuint id)
{
  ASSERT_COND( id > 0 && id <= node_num() );
  return mNodeInfoList[id - 1];
}

END_NAMESPACE_YM_BNET
