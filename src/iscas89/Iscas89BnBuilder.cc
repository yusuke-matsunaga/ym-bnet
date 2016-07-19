
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
  _clear();

  _set_model_name("iscas89_network");

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
  // DFF 情報を生成する．
  DffInfo& dff_info = _add_dff(oname);

  // DFF の出力(BnNode的には入力ノード)を生成する．
  ymuint input_id = _add_input(oname);

  dff_info.mOutput = input_id;

  mIdMap.add(oname_id, input_id);

  // DFF の入力(BnNode的には出力ノード)を生成する．
  ymuint output_id = _add_output(iname, 0);

  dff_info.mInput = output_id;

  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  mFaninInfoMap.add(output_id, vector<ymuint>(1, iname_id));

  if ( mClockId == 0 ) {
    // クロック端子を作る．
    ymuint node_id = _add_input(mClockName);

    // クロックのポートを作る．
    _add_port(mClockName, node_id);

    // クロック端子の外部出力を作る．
    mClockId = _add_output(mClockName, node_id);
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
  ymuint id = _add_input(name);

  mIdMap.add(name_id, id);

  _add_port(name, id);

  mSane = false;
}

// @brief 外部出力ノードを追加する．
// @param[in] name_id 名前のID番号
// @param[in] name 名前
void
Iscas89BnBuilder::add_output(ymuint name_id,
			     const string& name)
{
  ymuint id = _add_output(name, 0);

  mFaninInfoMap.add(id, vector<ymuint>(1, name_id));

  _add_port(name, id);

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
  ymuint id = _add_primitive(oname, logic_type, ni);

  mIdMap.add(oname_id, id);

  mFaninInfoMap.add(id, inode_id_array);

  mSane = false;
}

// @brief 最終処理を行う．
// @retval true 正しく設定されている．
// @retval false エラーが起こった．
bool
Iscas89BnBuilder::wrap_up()
{
  if ( mSane ) {
    return true;
  }

  // ノードのファンインを設定する．
  for (ymuint node_id = 1; node_id <= node_num(); ++ node_id) {
    vector<ymuint> fanin_info;
    bool stat = mFaninInfoMap.find(node_id, fanin_info);
    if ( !stat ) {
      continue;
    }

    NodeInfo& node_info = node(node_id);
    if ( node_info.mType == BnNode::kLogic ) {
      ymuint ni = fanin_info.size();
      for (ymuint i = 0; i < ni; ++ i) {
	ymuint inode_id;
	bool stat1 = mIdMap.find(fanin_info[i], inode_id);
	ASSERT_COND( stat1 );
	_connect(inode_id, node_id, i);
      }
    }
    else if ( node_info.mType == BnNode::kOutput ) {
      ymuint iname_id = fanin_info[0];
      ymuint inode_id;
      bool stat1 = mIdMap.find(iname_id, inode_id);
      ASSERT_COND( stat1 );
      _connect(inode_id, node_id, 0);
    }
  }

  mSane = sanity_check();

  return mSane;

}

END_NAMESPACE_YM_BNET
