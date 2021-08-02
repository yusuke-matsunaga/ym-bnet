
/// @file BnIscas89Handler.cc
/// @brief BnIscas89Handler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BnIscas89Handler.h"
#include "ym/Iscas89Parser.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"
#include "ym/MsgMgr.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief blif ファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] clock_name クロック端子名
// @return ネットワークを返す．
BnNetwork
BnNetwork::read_iscas89(
  const string& filename,
  const string& clock_name
)
{
  BnNetwork network;

  Iscas89Parser parser;
  BnIscas89Handler handler(parser, network, clock_name);

  bool stat = parser.read(filename);
  if ( !stat ) {
    network.clear();
  }

  return network;
}


//////////////////////////////////////////////////////////////////////
// クラス BnIscas89Handler
//////////////////////////////////////////////////////////////////////

// @brief 初期化
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::init()
{
  mNetwork.clear();

  mNetwork.set_name("iscas89_network");

  mIdMap.clear();
  mFaninInfoMap.clear();

  mClockId = BNET_NULLID;

  return true;
}

// @brief INPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 入力ピン名の ID 番号
// @param[in] name 入力ピン名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::read_input(
  const FileRegion& loc,
  SizeType name_id,
  const string& name
)
{
  auto port_id = mNetwork.new_input_port(name);
  auto& port = mNetwork.port(port_id);
  SizeType id = port.bit(0);
  mIdMap[name_id] = id;

  return true;
}

// @brief OUTPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 出力ピン名の ID 番号
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::read_output(
  const FileRegion& loc,
  SizeType name_id,
  const string& name
)
{
  auto port_id = mNetwork.new_output_port(name);
  auto& port = mNetwork.port(port_id);
  SizeType id = port.bit(0);
  add_fanin_info(id, name_id);
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
BnIscas89Handler::read_gate(
  const FileRegion& loc,
  BnNodeType logic_type,
  SizeType oname_id,
  const string& oname,
  const vector<SizeType>& iname_list
)
{
  SizeType ni = iname_list.size();
  SizeType id = mNetwork.new_logic(oname, logic_type, ni);
  mIdMap[oname_id] = id;

  add_fanin_info(id, iname_list);

  return true;
}

// @brief ゲート文(MUX)を読み込む．
// @param[in] loc ファイル位置
// @param[in] oname_id 出力名の ID 番号
// @param[in] oname 出力名
// @param[in] iname_list 入力名のリスト
// @retval true 処理が成功した．
// @retval false エラーが起こった．
//
// 入力数のチェックは済んでいるものとする．
bool
BnIscas89Handler::read_mux(
  const FileRegion& loc,
  SizeType oname_id,
  const string& oname,
  const vector<SizeType>& iname_list)
{
  SizeType ni = iname_list.size();
  SizeType nc = 0;
  SizeType nd = 1;
  while ( nc + nd < ni ) {
    ++ nc;
    nd <<= 1;
  }
  ASSERT_COND( nc + nd == ni );

  vector<Expr> cinputs(nc);
  for ( SizeType i: Range(nc) ) {
    cinputs[i] = Expr::make_posi_literal(VarId(i));
  }
  vector<Expr> dinputs(nd);
  for ( SizeType i: Range(nd) ) {
    dinputs[i] = Expr::make_posi_literal(VarId(i + nc));
  }

  vector<Expr> or_fanins(nd);
  for ( SizeType p: Range(nd) ) {
    vector<Expr> and_fanins(nc + 1);
    for ( SizeType i: Range(nc) ) {
      if ( p & (1 << i) ) {
	and_fanins[i] = cinputs[i];
      }
      else {
	and_fanins[i] = ~cinputs[i];
      }
    }
    and_fanins[nc] = dinputs[p];
    or_fanins[p] = Expr::make_and(and_fanins);
  }
  Expr mux_expr = Expr::make_or(or_fanins);
  SizeType id = mNetwork.new_logic(oname, mux_expr);

  mIdMap[oname_id] = id;

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
BnIscas89Handler::read_dff(
  const FileRegion& loc,
  SizeType oname_id,
  const string& oname,
  SizeType iname_id)
{
  // この形式ではクロック以外の制御端子はない．

  SizeType dff_id = mNetwork.new_dff(oname);
  const BnDff& dff = mNetwork.dff(dff_id);

  SizeType output_id = dff.output();
  mIdMap[oname_id] = output_id;

  SizeType input_id = dff.input();
  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  add_fanin_info(input_id, iname_id);

  if ( mClockId == BNET_NULLID ) {
    // クロックのポートを作る．
    auto port_id = mNetwork.new_input_port(mClockName);
    auto& clock_port = mNetwork.port(port_id);
    // クロックの入力ノード番号を記録する．
    mClockId = clock_port.bit(0);
  }

  // クロック入力とdffのクロック端子を結びつける．
  SizeType clock_id = dff.clock();
  mNetwork.connect(mClockId, clock_id, 0);

  return true;
}

// @brief 終了操作
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::end()
{
  // ノードのファンインを設定する．
  for ( SizeType node_id = 1; node_id <= mNetwork.node_num(); ++ node_id ) {
    if ( mFaninInfoMap.count(node_id) == 0 ) {
      continue;
    }
    const auto& fanin_info = mFaninInfoMap[node_id];

    const BnNode& node = mNetwork.node(node_id);
    if ( node.is_logic() ) {
      SizeType ni = fanin_info.size();
      for ( SizeType i: Range(ni) ) {
	SizeType iname_id = fanin_info[i];
	if ( mIdMap.count(iname_id) == 0 ) {
	  ostringstream buf;
	  buf << id2str(iname_id) << " not found" << endl;
	  MsgMgr::put_msg(__FILE__, __LINE__, MsgType::Error,
			  "ISCAS89_PARSER", buf.str());
	  return false;
	}
	SizeType inode_id = mIdMap.at(iname_id);
	mNetwork.connect(inode_id, node_id, i);
      }
    }
    else if ( node.is_output() ) {
      SizeType iname_id = fanin_info[0];
      if ( mIdMap.count(iname_id) == 0 ) {
	ostringstream buf;
	buf << id2str(iname_id) << " not found" << endl;
	MsgMgr::put_msg(__FILE__, __LINE__, MsgType::Error,
			"ISCAS89_PARSER", buf.str());
	return false;
      }
      SizeType inode_id = mIdMap.at(iname_id);
      mNetwork.connect(inode_id, node_id, 0);
    }
  }
  bool stat = mNetwork.wrap_up();
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
  mNetwork.clear();
}

// @brief ファンイン情報を追加する．
// @param[in] id ID番号
// @param[in] fanin ファンイン番号
void
BnIscas89Handler::add_fanin_info(
  SizeType id,
  SizeType fanin)
{
  mFaninInfoMap[id] = vector<SizeType>{fanin};
}

// @brief ファンイン情報を追加する．
// @param[in] id ID番号
// @param[in] fanin_list ファンイン番号のリスト
void
BnIscas89Handler::add_fanin_info(
  SizeType id,
  const vector<SizeType>& fanin_list)
{
  mFaninInfoMap[id] = fanin_list;
}

END_NAMESPACE_YM_BNET
