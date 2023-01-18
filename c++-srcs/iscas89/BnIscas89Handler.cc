
/// @file BnIscas89Handler.cc
/// @brief BnIscas89Handler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "BnIscas89Handler.h"
#include "Iscas89Parser.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"
#include "ym/BnNodeList.h"
#include "ym/MsgMgr.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief blif ファイルを読み込む．
BnNetwork
BnNetwork::read_iscas89(
  const string& filename,
  const string& clock_name
)
{
  Iscas89Parser parser;
  string _clock_name{clock_name};
  if ( _clock_name == string{} ) {
    _clock_name = "clock";
  }
  BnIscas89Handler handler{parser, _clock_name};

  bool stat = parser.read(filename);
  if ( !stat ) {
    ostringstream buff;
    buff << "Error in read_iscas89(\"" << filename << "\"";
    throw std::invalid_argument{buff.str()};
  }

  return handler.get_network();
}


//////////////////////////////////////////////////////////////////////
// クラス BnIscas89Handler
//////////////////////////////////////////////////////////////////////

// @brief 結果のネットワークを取り出す．
BnNetwork
BnIscas89Handler::get_network()
{
  return BnNetwork{std::move(mNetwork)};
}

// @brief 初期化
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::init()
{
  mNetwork.clear();

  mNetwork.set_name("iscas89_network");

  mIdMap.clear();
  mNodeMap.clear();
  mOutputMap.clear();

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
  string name1;
  if ( mNetwork.find_port(name) == static_cast<SizeType>(-1) ) {
    name1 = name;
  }
  auto port_id = mNetwork.new_output_port(name1);
  auto& port = mNetwork.port(port_id);
  SizeType id = port.bit(0);
  mOutputMap.emplace(id, name_id);
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
  mNodeMap.emplace(oname_id, NodeInfo{oname, logic_type, false, iname_list});
  return true;
}

// @brief ゲート文(MUX)を読み込む．
bool
BnIscas89Handler::read_mux(
  const FileRegion& loc,
  SizeType oname_id,
  const string& oname,
  const vector<SizeType>& iname_list)
{
  mNodeMap.emplace(oname_id, NodeInfo{oname, BnNodeType::C0, true, iname_list});
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

  SizeType output_id = dff.data_out();
  mIdMap[oname_id] = output_id;

  SizeType input_id = dff.data_in();
  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  mOutputMap.emplace(input_id, iname_id);

  if ( mClockId == BNET_NULLID ) {
    // クロックのポートを作る．
    auto port_id = mNetwork.new_input_port(mClockName);
    auto& clock_port = mNetwork.port(port_id);
    // クロックの入力ノード番号を記録する．
    mClockId = clock_port.bit(0);
  }

  // クロック入力とdffのクロック端子を結びつける．
  mNetwork.set_output_src(dff.clock(), mClockId);

  return true;
}

// @brief 終了操作
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BnIscas89Handler::end()
{
  // 出力ノードのファンインをセットする．
  // 入力側のノードはドミノ式に生成される．
  for ( auto& node: mNetwork.output_list() ) {
    if ( node.is_clock() || node.is_clear() || node.is_preset() ) {
      continue;
    }
    ASSERT_COND( node.output_src() == BNET_NULLID );
    auto id = node.id();
    ASSERT_COND( mOutputMap.count(id) > 0 );
    auto name_id = mOutputMap.at(id);
    auto inode_id = make_node(name_id);
    if ( inode_id == BNET_NULLID ) {
      return false;
    }
    mNetwork.set_output_src(id, inode_id);
  }
  return true;
}

// @brief name_id のノードを生成する．
SizeType
BnIscas89Handler::make_node(
  SizeType name_id
)
{
  if ( mIdMap.count(name_id) > 0 ) {
    return mIdMap.at(name_id);
  }
  if ( mNodeMap.count(name_id) > 0 ) {
    auto& node_info = mNodeMap.at(name_id);
    // ファンインのノードを作る．
    SizeType ni = node_info.iname_id_list.size();
    vector<SizeType> fanin_id_list(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto inode = make_node(node_info.iname_id_list[i]);
      if ( inode == BNET_NULLID ) {
	// エラー
	return BNET_NULLID;
      }
      fanin_id_list[i] = inode;
    }
    SizeType id;
    if ( node_info.is_mux ) {
      id = make_mux(node_info.oname, fanin_id_list);
    }
    else {
      id = mNetwork.new_logic_primitive(node_info.oname, node_info.gate_type,
					fanin_id_list);
    }
    mIdMap.emplace(name_id, id);
    return id;
  }
  return BNET_NULLID;
}

SizeType
BnIscas89Handler::make_mux(
  const string& oname,
  const vector<SizeType>& fanin_id_list
)
{
  SizeType ni = fanin_id_list.size();
  SizeType nc = 0;
  SizeType nd = 1;
  while ( nc + nd < ni ) {
    ++ nc;
    nd <<= 1;
  }
  ASSERT_COND( nc + nd == ni );

  vector<Expr> cinputs(nc);
  for ( SizeType var: Range(nc) ) {
    cinputs[var] = Expr::make_posi_literal(var);
  }
  vector<Expr> dinputs(nd);
  for ( SizeType i: Range(nd) ) {
    dinputs[i] = Expr::make_posi_literal(i + nc);
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
  auto id = mNetwork.new_logic_expr(oname, mux_expr, fanin_id_list);
  return id;
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

END_NAMESPACE_YM_BNET
