
/// @file BnIscas89Handler.cc
/// @brief BnIscas89Handler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnIscas89Handler.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] parser パーサー
// @param[in] network 設定対象のネットワーク
// @param[in] clock_name クロック端子名
BnIscas89Handler::BnIscas89Handler(Iscas89Parser& parser,
				   BnNetwork* network,
				   const string& clock_name) :
  Iscas89Handler(parser),
  mNetwork(network),
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
  mNetwork->clear();

  mNetwork->set_name("iscas89_network");

  mIdMap.clear();
  mFaninInfoMap.clear();

  mClockId = kBnNullId;

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
			     int name_id,
			     const char* name)
{
  BnPort* port = mNetwork->new_input_port(name);
  int id = port->bit(0);
  mIdMap.add(name_id, id);

  return true;
}

// @brief OUTPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 出力ピン名の ID 番号
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::read_output(const FileRegion& loc,
			      int name_id,
			      const char* name)
{
  BnPort* port = mNetwork->new_output_port(name);
  int id = port->bit(0);

  add_fanin_info(id, vector<int>(1, name_id));

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
			    BnNodeType logic_type,
			    int oname_id,
			    const char* oname,
			    const vector<int>& iname_list)
{
  int ni = iname_list.size();
  int id = mNetwork->new_primitive(oname, ni, logic_type);
  mIdMap.add(oname_id, id);

  add_fanin_info(id, iname_list);

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
			   int oname_id,
			   const char* oname,
			   int iname_id)
{
  // この形式ではクロック以外の制御端子はない．

  BnDff* dff = mNetwork->new_dff(oname);

  int output_id = dff->output();
  mIdMap.add(oname_id, output_id);

  int input_id = dff->input();
  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  add_fanin_info(input_id, vector<int>(1, iname_id));

  if ( mClockId == kBnNullId ) {
    // クロックのポートを作る．
    BnPort* clock_port = mNetwork->new_input_port(mClockName);
    // クロックの入力ノード番号を記録する．
    mClockId = clock_port->bit(0);
  }

  // クロック入力とdffのクロック端子を結びつける．
  int clock_id = dff->clock();
  mNetwork->connect(mClockId, clock_id, 0);

  return true;
}

// @brief 終了操作
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::end()
{
  // ノードのファンインを設定する．
  for ( int node_id = 1; node_id <= mNetwork->node_num(); ++ node_id ) {
    if ( !mFaninInfoMap.check(node_id) ) {
      continue;
    }
    const FaninInfo& fanin_info = mFaninInfoMap[node_id];

    const BnNode* node = mNetwork->node(node_id);
    if ( node->is_logic() ) {
      int ni = fanin_info.fanin_num();
      for ( int i = 0; i < ni; ++ i ) {
	int inode_id;
	bool stat1 = mIdMap.find(fanin_info.fanin(i), inode_id);
	ASSERT_COND( stat1 );
	mNetwork->connect(inode_id, node_id, i);
      }
    }
    else if ( node->is_output() ) {
      int iname_id = fanin_info.fanin(0);
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
BnIscas89Handler::normal_exit()
{
}

// @brief エラー終了時の処理
void
BnIscas89Handler::error_exit()
{
  mNetwork->clear();
}

// @brief ファンイン情報を追加する．
// @param[in] id ID番号
// @param[in] fanin_list ファンイン番号のリスト
void
BnIscas89Handler::add_fanin_info(int id,
				 const vector<int>& fanin_list)
{
  mFaninInfoMap.add(id, FaninInfo(fanin_list));
}

END_NAMESPACE_YM_BNET
