
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
Iscas89BnBuilder::read_iscas89(const string& filename)
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

  mClockId = 0;

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

  if ( mClockId == 0 ) {
    // クロック端子を作る．
    mNodeInfoList.push_back(NodeInfo(mClockName));
    ymuint node_id = mNodeInfoList.size();

    // クロックのポートを作る．
    mPortInfoList.push_back(PortInfo(mClockName, node_id));

    // クロック端子の外部出力を作る．
    mNodeInfoList.push_back(NodeInfo(mClockName, 0));
    mClockId = mNodeInfoList.size();
    mNodeInfoList[mClockId - 1].mInodeList[0] = node_id;
  }

  dff_info.mClock = mClockId;

  mSane = false;
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
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] oname_id 出力名のID番号
// @param[in] oname 出力名
// @param[in] inode_id_array ファンインのノード番号のリスト
// @param[in] logic_type 論理型
void
Iscas89BnBuilder::add_primitive(ymuint oname_id,
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

  // 論理ノードのファンインを設定する．
  for (ymuint node_id = 1; node_id <= node_num(); ++ node_id) {
    vector<ymuint> fanin_info;
    bool stat = mFaninInfoMap.find(node_id, fanin_info);
    if ( !stat ) {
      continue;
    }

    NodeInfo& bnode_info = mNodeInfoList[node_id - 1];
    if ( bnode_info.mType == BnNode::kLogic ) {
      ymuint ni = fanin_info.size();
      for (ymuint i = 0; i < ni; ++ i) {
	ymuint inode_id;
	bool stat1 = mIdMap.find(fanin_info[i], inode_id);
	ASSERT_COND( stat1 );
	bnode_info.mInodeList[i] = inode_id;
      }
    }
    else if ( bnode_info.mType == BnNode::kOutput ) {
      ymuint iname_id = fanin_info[0];
      ymuint inode_id;
      bool stat1 = mIdMap.find(iname_id, inode_id);
      ASSERT_COND( stat1 );
      bnode_info.mInodeList[0] = inode_id;
    }
  }

  bool error = false;

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
