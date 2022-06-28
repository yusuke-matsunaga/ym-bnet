
/// @file BnNetworkImpl.cc
/// @brief BnNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BnNetworkImpl.h"
#include "ym/ClibCell.h"
#include "ym/ClibFFInfo.h"
#include "ym/ClibLatchInfo.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/Range.h"

#include "BnPortImpl.h"
#include "BnInputNode.h"
#include "BnOutputNode.h"
#include "BnLogicNode.h"
#include "BnDffImpl.h"
#include "BnLatchImpl.h"

#include "FuncAnalyzer.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief 内容をクリアする．
//
// コンストラクタ直後と同じ状態になる．
void
BnNetworkImpl::clear()
{
  for ( auto port_p: mPortList ) {
    delete port_p;
  }
  for ( auto dff_p: mDffList ) {
    delete dff_p;
  }
  for ( auto latch_p: mLatchList ) {
    delete latch_p;
  }
  for ( auto node_p: mNodeList ) {
    delete node_p;
  }

  mName = string();
  mPortList.clear();
  mDffList.clear();
  mLatchList.clear();
  mInputList.clear();
  mPrimaryInputList.clear();
  mOutputList.clear();
  mOutputSrcList.clear();
  mPrimaryOutputList.clear();
  mPrimaryOutputSrcList.clear();
  mLogicList.clear();
  mNodeList.clear();

  mSane = false;
}


// @brief 内容をコピーする．
void
BnNetworkImpl::copy(
  const BnNetworkImpl& src
)
{
  if ( &src == this ) {
    // 自分自身がソースならなにもしない．
    return;
  }

  clear();

  // セルライブラリの設定
  set_library(src.library());

  // ネットワーク名の設定
  set_name(src.name());

  // srcのノード番号をキーにしてノード番号を格納する配列
  vector<SizeType> id_map(src.node_num());

  // ポートの生成
  auto np = src.port_num();
  for ( auto i: Range(np) ) {
    auto& src_port = src.port(i);
    string port_name = src_port.name();

    // 各ビットの方向を求める．
    auto nb = src_port.bit_width();
    vector<BnDir> dirs(nb);
    for ( auto i: Range(nb) ) {
      auto id = src_port.bit(i);
      auto& node = src.node(id);
      if ( node.is_input() ) {
	dirs[i] = BnDir::INPUT;
      }
      else if ( node.is_output() ) {
	dirs[i] = BnDir::OUTPUT;
      }
      else {
	ASSERT_NOT_REACHED;
      }
    }

    // ポートの生成
    auto dst_port_id = new_port(port_name, dirs);
    ASSERT_COND( src_port.id() == dst_port_id );

    // 各ビットの対応関係を記録する．
    auto& dst_port = port(dst_port_id);
    for ( auto i: Range(nb) ) {
      auto src_id = src_port.bit(i);
      auto dst_id = dst_port.bit(i);
      id_map[src_id] = dst_id;
    }
  }

  // src の入力の対応するノードを input_list に入れる．
  auto input_num = src.input_num();
  vector<SizeType> input_list(input_num);
  for ( auto i: Range(input_num) ) {
    auto src_id = src.input_id(i);
    auto dst_id = id_map[src_id];
    input_list[i] = dst_id;
  }

  // src 本体をインポートする．
  vector<SizeType> output_list;
  import_subnetwork(src, input_list, output_list);

  // 出力端子のファンインの接続
  auto output_num = src.output_num();
  for ( auto i: Range(output_num) ) {
    auto src_id = src.output_id(i);
    auto src_fanin_id = src.output_src_id(i);

    auto dst_id = id_map[src_id];
    auto dst_fanin_id = output_list[i];
    connect(dst_fanin_id, dst_id, 0);
  }

  bool stat = wrap_up();

  ASSERT_COND( stat );
}

// @brief セルライブラリをセットする．
void
BnNetworkImpl::set_library(
  const ClibCellLibrary& library
)
{
  mCellLibrary = library;
}

// @brief ネットワーク名を設定する．
void
BnNetworkImpl::set_name(
  const string& name
)
{
  mName = name;
}

// @brief 部分回路を追加する．
void
BnNetworkImpl::import_subnetwork(
  const BnNetworkImpl& src_network,
  const vector<SizeType>& input_list,
  vector<SizeType>& output_list
)
{
  ASSERT_COND( src_network.mSane );

  auto input_num = src_network.input_num();
  ASSERT_COND( input_list.size() == input_num );

  auto output_num = src_network.output_num();
  output_list.clear();
  output_list.reserve(output_num);

  // src_network のノード番号をキーにして生成したノード番号を入れる配列
  vector<SizeType> id_map(src_network.node_num());

  // src_network の入力と input_list の対応関係を id_map に入れる．
  for ( auto i: Range(input_num) ) {
    auto src_id = src_network.input_id(i);
    auto dst_id = input_list[i];
    id_map[src_id] = dst_id;
  }

  // DFFを作る．
  for ( auto src_dff_p: src_network.mDffList ) {
    _copy_dff(*src_dff_p, id_map);
  }

  // ラッチを作る．
  for ( auto src_latch_p: src_network.mLatchList ) {
    _copy_latch(*src_latch_p, id_map);
  }

  // 論理ノードの生成
  for ( auto src_id: src_network.logic_id_list() ) {
    auto& src_node = src_network.node(src_id);
    auto dst_id = _copy_logic(src_node, src_network, id_map);
  }

  // src_network の外部出力のファンインに対応するノード番号を
  // output_list に入れる．
  for ( auto src_id: src_network.output_src_id_list() ) {
    auto dst_id = id_map[src_id];
    output_list.push_back(dst_id);
  }
}

// @brief DFFを複製する．
SizeType
BnNetworkImpl::_copy_dff(
  const BnDff& src_dff,
  vector<SizeType>& id_map
)
{
  string dff_name = src_dff.name();
  bool has_clear = src_dff.clear() != BNET_NULLID;
  bool has_preset = src_dff.preset() != BNET_NULLID;
  auto dst_id = _new_dff(dff_name, has_clear, has_preset);
  auto& dst_dff = dff(dst_id);

  // 各端子の対応関係を記録する．
  {
    auto src_id = src_dff.input();
    auto dst_id = dst_dff.input();
    id_map[src_id] = dst_id;
  }
  {
    auto src_id = src_dff.output();
    auto dst_id = dst_dff.output();
    id_map[src_id] = dst_id;
  }
  {
    auto src_id = src_dff.clock();
    auto dst_id = dst_dff.clock();
    id_map[src_id] = dst_id;
  }
  if ( has_clear ) {
    auto src_id = src_dff.clear();
    auto dst_id = dst_dff.clear();
    id_map[src_id] = dst_id;
  }
  if ( has_preset ) {
    auto src_id = src_dff.preset();
    auto dst_id = dst_dff.preset();
    id_map[src_id] = dst_id;
  }

  return dst_id;
}

// @brief ラッチを複製する．
SizeType
BnNetworkImpl::_copy_latch(
  const BnLatch& src_latch,
  vector<SizeType>& id_map
)
{
  string latch_name = src_latch.name();
  bool has_clear = src_latch.clear() != BNET_NULLID;
  bool has_preset = src_latch.preset() != BNET_NULLID;
  auto dst_id = _new_latch(latch_name, has_clear, has_preset);
  auto& dst_latch = latch(dst_id);

  // 各端子の対応関係を記録する．
  {
    auto src_id = src_latch.input();
    auto dst_id = dst_latch.input();
    id_map[src_id] = dst_id;
  }
  {
    auto src_id = src_latch.output();
    auto dst_id = dst_latch.output();
    id_map[src_id] = dst_id;
  }
  {
    auto src_id = src_latch.enable();
    auto dst_id = dst_latch.enable();
    id_map[src_id] = dst_id;
  }
  if ( has_clear ) {
    auto src_id = src_latch.clear();
    auto dst_id = dst_latch.clear();
    id_map[src_id] = dst_id;
  }
  if ( has_preset ) {
    auto src_id = src_latch.preset();
    auto dst_id = dst_latch.preset();
    id_map[src_id] = dst_id;
  }

  return dst_id;
}

// @brief 論理ノードを複製する．
SizeType
BnNetworkImpl::_copy_logic(
  const BnNode& src_node,
  const BnNetworkImpl& src_network,
  vector<SizeType>& id_map
)
{
  ASSERT_COND( src_node.is_logic() );

  auto nfi = src_node.fanin_num();
  string name = src_node.name();
  BnNodeType logic_type = src_node.type();
  auto cell_id = src_node.cell_id();
  SizeType expr_id = BNET_NULLID;
  SizeType func_id = BNET_NULLID;
  if ( logic_type == BnNodeType::Expr ) {
    const Expr& expr = src_network.expr(src_node.expr_id());
    expr_id = _reg_expr(expr);
  }
  else if ( logic_type == BnNodeType::TvFunc ) {
    const TvFunc& func = src_network.func(src_node.func_id());
    func_id = _reg_tv(func);
  }
  auto dst_id = _reg_logic(name, nfi, logic_type, expr_id, func_id, cell_id);
  id_map[src_node.id()] = dst_id;

  vector<SizeType> fanin_id_list(nfi);
  for ( auto i: Range(nfi) ) {
    auto src_iid = src_node.fanin_id(i);
    auto iid = id_map[src_iid];
    fanin_id_list[i] = iid;
  }
  connect_fanins(dst_id, fanin_id_list);

  return dst_id;
}

// @brief 既存の論理ノードを変更する最も低レベルの関数
void
BnNetworkImpl::_change_logic(
  SizeType node_id,
  SizeType fanin_num,
  BnNodeType logic_type,
  SizeType expr_id,
  SizeType func_id,
  int cell_id
)
{
  ASSERT_COND( node_id >= 0 && node_id < mNodeList.size() );

  auto old_node = mNodeList[node_id];
  auto new_node = _new_logic(old_node->name(), fanin_num,
			     logic_type, expr_id, func_id, cell_id);
  mNodeList[node_id] = new_node;
  new_node->mId = node_id;
  delete old_node;

}

// @brief 論理ノードを作る最も低レベルの関数
SizeType
BnNetworkImpl::_reg_logic(
  const string& name,
  SizeType fanin_num,
  BnNodeType logic_type,
  SizeType expr_id,
  SizeType func_id,
  int cell_id
)
{
  auto node = _new_logic(name, fanin_num, logic_type, expr_id, func_id, cell_id);
  _reg_node(node);
  return node->id();
}

// @brief 論理ノードを作る最も低レベルの関数
BnNodeImpl*
BnNetworkImpl::_new_logic(
  const string& name,
  SizeType fanin_num,
  BnNodeType logic_type,
  SizeType expr_id,
  SizeType func_id,
  int cell_id
)
{
  BnNodeImpl* node = nullptr;
  if ( logic_type == BnNodeType::Expr ) {
    node = new BnExprNode(name, fanin_num, expr_id, cell_id);
  }
  else if ( logic_type == BnNodeType::TvFunc ) {
    node = new BnTvNode(name, fanin_num, func_id, cell_id);
  }
  else {
    node = new BnPrimNode(name, fanin_num, logic_type, cell_id);
  }
  ASSERT_COND( node != nullptr );

  return node;
}

// @brief DFFを追加する．
SizeType
BnNetworkImpl::new_dff(
  const string& name,
  bool has_clear,
  bool has_preset
)
{
  return _new_dff(name, has_clear, has_preset);
}

// @brief セルの情報を持ったDFFを追加する．
// @return 生成したDFF番号を返す．
//
// - 名前の重複に関しては感知しない．
// - FFセルでない場合には -1 を返す．
SizeType
BnNetworkImpl::new_dff(
  const string& name, ///< [in] DFF名
  int cell_id         ///< [in] 対応するセル番号
)
{
#if 0
  const ClibCell& cell = mCellLibrary.cell(cell_id);
  if ( !cell.is_ff() ) {
    return -1;
  }
  if ( cell.inout_num() > 0 ) {
    return -1;
  }

  SizeType ni = cell.input_num();
  SizeType no = cell.output_num();
  vector<Expr> output_exprs(no);
  for ( SizeType i = 0; i < no; ++ i ) {
    if ( !cell.has_logic(i) || cell.has_tristate(i) ) {
      return -1;
    }
    output_expr[i] = cell.logic_expr(i);
  }
  auto next_state_expr = cell.next_state_expr();
  auto clock_expr = cell.clock_expr();
  auto clear_expr = cell.clear_expr();
  auto preset_expr = cell.preset_expr();
  return _new_dff(name, ni, output_exprs,
		  next_state_expr,
		  clock_expr,
		  clear_expr,
		  preset_expr,
		  cell_id);
#endif
}

// @brief ラッチを追加する．
SizeType
BnNetworkImpl::new_latch(
  const string& name,
  bool has_clear,
  bool has_preset
)
{
  return _new_latch(name, has_clear, has_preset);
}

// @brief セルの情報を持ったラッチを追加する．
SizeType
BnNetworkImpl::new_latch(
  const string& name,
  int cell_id
)
{
#if 0
  const ClibCell& cell = mCellLibrary.cell(cell_id);
  if ( !cell.is_latch() ) {
    return -1;
  }
  if ( cell.inout_num() > 0 ) {
    return -1;
  }

  SizeType ni = cell.input_num();
  SizeType no = cell.output_num();
  vector<Expr> output_exprs(no);
  for ( SizeType i = 0; i < no; ++ i ) {
    if ( !cell.has_logic(i) || cell.has_tristate(i) ) {
      return -1;
    }
    output_expr[i] = cell.logic_expr(i);
  }
  auto data_in_expr = cell.data_in_expr();
  auto enable_expr = cell.enable_expr();
  auto clear_expr = cell.clear_expr();
  auto preset_expr = cell.preset_expr();
  return _new_latch(name, ni, output_exprs,
		    data_in_expr,
		    enable_expr,
		    clear_expr,
		    preset_expr,
		    cell_id);
#endif
}

// @brief 各ノードがプリミティブ型になるように分解する．
void
BnNetworkImpl::simple_decomp()
{
  // 分解するノードのリストを作る．
  vector<BnNodeImpl*> node_list;
  node_list.reserve(mNodeList.size());
  for ( auto node: mNodeList ) {
    if ( node->type() == BnNodeType::Expr ||
	 node->type() == BnNodeType::TvFunc ) {
      node_list.push_back(node);
    }
  }

  // node_list の各ノードを分解する．
  // 直接 mNodeList で for ループを回さない理由は
  // 分解の途中で mNodeList が変化するため．
  for ( auto node: node_list ) {
    if ( node->type() == BnNodeType::Expr ) {
      const Expr& expr = mExprList[node->expr_id()];
      SizeType ni = expr.input_size();
      vector<SizeType> term_list(ni * 2, BNET_NULLID);
      for ( auto i: Range(ni) ) {
	auto iid = node->fanin_id(i);
	term_list[i * 2 + 0] = iid;
      }
      _decomp_expr(node->id(), expr, term_list);
    }
    else { // BnNodeType::TvFunc
      _decomp_tvfunc(node->id(), func(node->func_id()), node->fanin_id_list());
    }
  }

  mSane = false;

  wrap_up();
}

// @brief 論理式型のノードを分解する．
SizeType
BnNetworkImpl::_decomp_expr(
  SizeType id,
  const Expr& expr,
  vector<SizeType>& term_list
)
{
  if ( expr.is_posi_literal() ) {
    VarId varid = expr.varid();
    auto index = varid.val() * 2 + 0;
    return term_list[index];
  }
  else if ( expr.is_nega_literal() ) {
    VarId varid = expr.varid();
    auto index = varid.val() * 2 + 1;
    if ( term_list[index] == BNET_NULLID ) {
      term_list[index] = new_primitive(string(), BnNodeType::Not,
				       vector<SizeType>{ term_list[index - 1] });
    }
    return term_list[index];
  }
  // 定数はありえない．
  ASSERT_COND( expr.is_op() );

  auto nc = expr.child_num();
  vector<SizeType> new_fanin_list(nc);
  for ( auto i: Range(nc) ) {
    auto iid = _decomp_expr(BNET_NULLID, expr.child(i), term_list);
    new_fanin_list[i] = iid;
  }
  BnNodeType node_type{BnNodeType::None};
  if ( expr.is_and() ) {
    node_type = BnNodeType::And;
  }
  else if ( expr.is_or() ) {
    node_type = BnNodeType::Or;
  }
  else if ( expr.is_xor() ) {
    node_type = BnNodeType::Xor;
  }
  else {
    ASSERT_NOT_REACHED;
  }

  if ( id == BNET_NULLID ) {
    id = new_primitive(string(), node_type, new_fanin_list);
  }
  else {
    change_primitive(id, node_type, new_fanin_list);
  }
  return id;
}

// @brief 真理値表型のノードを分解する．
SizeType
BnNetworkImpl::_decomp_tvfunc(
  SizeType id,
  const TvFunc& func,
  const vector<SizeType>& fanin_id_list
)
{
  return id;
}

// @brief 入出力混合のポートを作る．
SizeType
BnNetworkImpl::new_port(
  const string& port_name,
  const vector<BnDir>& dir_vect
)
{
  auto port_id = mPortList.size();
  auto bit_width = dir_vect.size();
  vector<SizeType> bits(bit_width);
  for ( auto i: Range(bit_width) ) {
    auto node_id = mNodeList.size();
    bits[i] = node_id;
    string node_name;
    if ( bit_width > 1 ) {
      ostringstream buf;
      buf << port_name << "." << i;
      node_name = buf.str();
    }
    else {
      node_name = port_name;
    }
    if ( dir_vect[i] == BnDir::INPUT ) {
      auto node = new BnPortInput(node_name, port_id, i);
      _reg_primary_input(node);
    }
    else { // BnDir::OUTPUT
      auto node = new BnPortOutput(node_name, port_id, i);
      _reg_primary_output(node);
    }
  }

  BnPort* port;
  if ( bit_width == 1 ) {
    port = new BnPort1(port_id, port_name, bits[0]);
  }
  else {
    port = new BnPortN(port_id, port_name, bits);
  }
  mPortList.push_back(port);

  return port_id;
}

// @brief DFFを追加する共通の処理を行う関数
SizeType
BnNetworkImpl::_new_dff(
  const string& name,
  bool has_clear,
  bool has_preset
)
{
  auto dff_id = mDffList.size();

  // 入力端子
  SizeType input_id{BNET_NULLID};
  {
    ostringstream buf;
    buf << name << ".input";
    auto iname = buf.str();
    auto node = new BnDffInput(iname, dff_id);
    _reg_output(node);
    input_id = node->id();
  }

  // 出力端子
  SizeType output_id{BNET_NULLID};
  {
    ostringstream buf;
    buf << name << ".output";
    auto oname = buf.str();
    auto node = new BnDffOutput(oname, dff_id);
    _reg_input(node);
    output_id = node->id();
  }

  // クロック端子
  SizeType clock_id{BNET_NULLID};
  {
    ostringstream buf;
    buf << name << ".clock";
    auto oname = buf.str();
    auto node = new BnDffClock(oname, dff_id);
    _reg_output(node);
    clock_id = node->id();
  }

  // クリア端子
  SizeType clear_id{BNET_NULLID};
  if ( has_clear ) {
    ostringstream buf;
    buf << name << ".clear";
    auto oname = buf.str();
    auto node = new BnDffClear(oname, dff_id);
    _reg_output(node);
    clear_id = node->id();
  }

  // プリセット端子
  SizeType preset_id{BNET_NULLID};
  if ( has_preset ) {
    ostringstream buf;
    buf << name << ".preset";
    auto oname = buf.str();
    auto node = new BnDffPreset(oname, dff_id);
    _reg_output(node);
    clear_id = node->id();
  }

  auto dff = new BnDffImpl(dff_id, name, input_id, output_id,
			   clock_id, clear_id, preset_id);
  mDffList.push_back(dff);

  return dff_id;
}

// @brief ラッチを追加する共通の処理を行う関数
SizeType
BnNetworkImpl::_new_latch(
  const string& name,
  bool has_clear,
  bool has_preset
)
{
  auto latch_id = mLatchList.size();

  // 入力端子
  SizeType input_id{BNET_NULLID};
  {
    ostringstream buf;
    buf << name << ".input";
    auto iname = buf.str();
    auto node = new BnLatchInput(iname, latch_id);
    _reg_output(node);
    input_id = node->id();
  }

  // 出力端子
  SizeType output_id{BNET_NULLID};
  {
    ostringstream buf;
    buf << name << ".output";
    auto oname = buf.str();
    auto node = new BnLatchOutput(oname, latch_id);
    _reg_input(node);
    output_id = node->id();
  }

  // イネーブル端子
  SizeType enable_id{BNET_NULLID};
  {
    ostringstream buf;
    buf << name << ".enable";
    auto oname = buf.str();
    auto node = new BnLatchEnable(oname, latch_id);
    _reg_output(node);
    enable_id = node->id();
  }

  // クリア端子
  SizeType clear_id{BNET_NULLID};
  if ( has_clear ) {
    ostringstream buf;
    buf << name << ".clear";
    auto oname = buf.str();
    auto node = new BnLatchClear(oname, latch_id);
    _reg_output(node);
    clear_id = node->id();
  }

  // プリセット端子
  SizeType preset_id{BNET_NULLID};
  if ( has_preset ) {
    ostringstream buf;
    buf << name << ".preset";
    auto oname = buf.str();
    auto node = new BnLatchPreset(oname, latch_id);
    _reg_output(node);
    clear_id = node->id();
  }

  auto latch = new BnLatchImpl(latch_id, name, input_id, output_id,
			       enable_id, clear_id, preset_id);
  mLatchList.push_back(latch);

  return latch_id;
}

// @brief ノードを複製する．
SizeType
BnNetworkImpl::dup_logic(
  const string& node_name,
  SizeType src_id
)
{
  auto src_node = mNodeList[src_id];
  auto nfi = src_node->fanin_num();
  auto logic_type = src_node->type();
  auto expr_id = src_node->expr_id();
  auto func_id = src_node->func_id();
  auto cell_id = src_node->cell_id();
  auto id = _reg_logic(node_name, nfi, logic_type, expr_id, func_id, cell_id);
  return id;
}

// @brief ファンアウトをつなぎ替える．
void
BnNetworkImpl::substitute_fanout(
  SizeType old_id,
  SizeType new_id
)
{
  ASSERT_COND( old_id >= 0 && old_id < mNodeList.size() );
  ASSERT_COND( new_id >= 0 && new_id < mNodeList.size() );

  // old_id のファンアウトのリストをコピーする．
  BnNodeImpl* old_node = mNodeList[old_id];
  vector<SizeType> fo_list{old_node->fanout_id_list()};
  for ( auto dst: fo_list ) {
    BnNodeImpl* dst_node = mNodeList[dst];
    // old_id のファンインを探す．
    SizeType ipos = dst_node->fanin_num() + 1;
    if ( dst_node->is_output() ) {
      ASSERT_COND( dst_node->fanin_id(0) == old_id );
      ipos = 0;
    }
    else {
      for ( auto i: Range(dst_node->fanin_num()) ) {
	if ( dst_node->fanin_id(i) == old_id ) {
	  ipos = i;
	  break;
	}
      }
    }
    ASSERT_COND( ipos < dst_node->fanin_num() );
    dst_node->set_fanin(ipos, new_id);
  }

  mSane = false;
}

// @brief ファンインをつなぎ替える．
void
BnNetworkImpl::connect_fanins(
  SizeType id,
  const vector<SizeType>& fanin_id_list
)
{
  ASSERT_COND( id >= 0 && id < mNodeList.size() );

  BnNodeImpl* node = mNodeList[id];
  auto ni = node->fanin_num();

  ASSERT_COND( fanin_id_list.size() == ni );

  for ( auto i: Range(ni) ) {
    node->set_fanin(i, fanin_id_list[i]);
  }

  mSane = true;
}

// @brief ノード間を接続する．
void
BnNetworkImpl::connect(
  SizeType src_id,
  SizeType dst_id,
  SizeType ipos
)
{
  ASSERT_COND( src_id >= 0 && src_id < mNodeList.size() );
  ASSERT_COND( dst_id >= 0 && dst_id < mNodeList.size() );

  BnNodeImpl* dst_node = mNodeList[dst_id];
  dst_node->set_fanin(ipos, src_id);

  mSane = false;
}

// @brief 整合性のチェックを行う．
bool
BnNetworkImpl::wrap_up()
{
  if ( mSane ) {
    return true;
  }

  bool error = false;

  // ポートのチェック
  for ( auto port_p: mPortList ) {
    for ( auto i: Range(port_p->bit_width()) ) {
      auto id = port_p->bit(i);
      if ( id == BNET_NULLID || id >= node_num() ) {
	cerr << "Port#" << port_p->id() << "(" << port_p->name() << ").bit["
	     << i << "] is not set" << endl;
	error = true;
      }
    }
  }

  // DFF のチェック
  for ( auto dff_p: mDffList ) {
    auto id1 = dff_p->input();
    if ( id1 == BNET_NULLID ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").input is not set" << endl;
      error = true;
    }
    else if ( id1 >= node_num() ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").input is not valid" << endl;
      error = true;
    }
    auto id2 = dff_p->output();
    if ( id2 == BNET_NULLID ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").output is not set" << endl;
      error = true;
    }
    else if ( id2 >= node_num() ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").output is not valid" << endl;
      error = true;
    }
    auto id3 = dff_p->clock();
    if ( id3 == BNET_NULLID ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").clock is not set" << endl;
      error = true;
    }
    else if ( id3 >= node_num() ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").clock is not valid" << endl;
      error = true;
    }
    auto id4 = dff_p->clear();
    if ( id4 != BNET_NULLID && id4 >= node_num() ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").clear is not valid" << endl;
      error = true;
    }
    auto id5 = dff_p->preset();
    if ( id5 != BNET_NULLID && id5 >= node_num() ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").preset is not valid" << endl;
      error = true;
    }
  }

  // ラッチのチェック
  for ( auto latch_p: mLatchList ) {
    auto id1 = latch_p->input();
    if ( id1 == BNET_NULLID ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").input is not set" << endl;
      error = true;
    }
    else if ( id1 >= node_num() ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").input is not valid" << endl;
      error = true;
    }
    auto id2 = latch_p->output();
    if ( id2 == BNET_NULLID ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").output is not set" << endl;
      error = true;
    }
    else if ( id2 >= node_num() ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").output is not valid" << endl;
      error = true;
    }
    auto id3 = latch_p->enable();
    if ( id3 == BNET_NULLID ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").enable is not set" << endl;
      error = true;
    }
    else if ( id3 >= node_num() ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").enable is not valid" << endl;
      error = true;
    }
    auto id4 = latch_p->clear();
    if ( id4 != BNET_NULLID && id4 >= node_num() ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").clear is not valid" << endl;
      error = true;
    }
    auto id5 = latch_p->preset();
    if ( id5 != BNET_NULLID && id5 >= node_num() ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").preset is not valid" << endl;
      error = true;
    }
  }

  // ノードのチェック
  for ( auto node_p: mNodeList ) {
    for ( auto i: Range(node_p->fanin_num()) ) {
      auto id = node_p->fanin_id(i);
      if ( id == BNET_NULLID ) {
	cerr << "NODE#" << node_p->id() << "(" << node_p->name() << ").fanin["
	     << i << "] is not set" << endl;
	error = true;
      }
      else if ( id < 0 || id >= node_num() ) {
	cerr << "NODE#" << node_p->id() << "(" << node_p->name() << ").fanin["
	     << i << "] is not valid" << endl;
	error = true;
      }
    }
  }

  if ( error ) {
    return false;
  }

  // 各ノードのファンアウトリストの作成
  for ( auto node_p: mNodeList ) {
    node_p->clear_fanout();
  }
  for ( auto node_p: mNodeList ) {
    for ( auto id: node_p->fanin_id_list() ) {
      auto src_node_p = mNodeList[id];
      src_node_p->add_fanout(node_p->id());
    }
  }

  // 論理ノードをトポロジカル順にソートする．

  // ノード番号を入れるキュー
  vector<SizeType> queue;
  queue.reserve(node_num());

  // キューの印を表すマーク配列
  vector<bool> mark(node_num(), false);

  // 入力ノードをキューに積む．
  for ( auto id: input_id_list() ) {
    queue.push_back(id);
    mark[id] = true;
  }

  // 定数ノードをキューに積む．
  for ( auto node: mNodeList ) {
    if ( node->is_logic() && node->fanin_num() == 0 ) {
      int id = node->id();
      queue.push_back(id);
      mark[id] = true;
    }
  }

  mLogicList.clear();
  mLogicList.reserve(node_num() - input_num() - output_num());

  // キューからノードを取り出してファンアウト先のノードをキューに積む．
  for ( auto rpos = 0; rpos < queue.size(); ++ rpos ) {
    auto id = queue[rpos];
    auto node_p = mNodeList[id];
    if ( node_p->is_logic() ) {
      mLogicList.push_back(id);
    }
    for ( auto oid: node_p->fanout_id_list() ) {
      if ( mark[oid] ) {
	continue;
      }
      auto onode_p = mNodeList[oid];
      bool ready = true;
      for ( auto iid: onode_p->fanin_id_list() ) {
	if ( !mark[iid] ) {
	  ready = false;
	  break;
	}
      }
      if ( ready ) {
	queue.push_back(oid);
	mark[oid] = true;
      }
    }
  }

  // mOutputSrcList を作る．
  mOutputSrcList.clear();
  mOutputSrcList.resize(mOutputList.size());
  for ( auto i: Range(mOutputList.size()) ) {
    auto oid = mOutputList[i];
    auto node_p = mNodeList[oid];
    auto iid = node_p->fanin_id(0);
    mOutputSrcList[i] = iid;
  }

  // mPrimaryOutputSrcList を作る．
  mPrimaryOutputSrcList.clear();
  mPrimaryOutputSrcList.resize(mPrimaryOutputList.size());
  for ( auto i: Range(mPrimaryOutputList.size()) ) {
    auto oid = mPrimaryOutputList[i];
    auto node_p = mNodeList[oid];
    auto iid = node_p->fanin_id(0);
    mPrimaryOutputSrcList[i] = iid;
  }

  mSane = true;

  return true;
}

// @brief 論理式を解析する．
tuple<SizeType, BnNodeType, SizeType>
BnNetworkImpl::_analyze_expr(
  const Expr& expr
)
{
  auto ni = expr.input_size();
  BnNodeType logic_type = FuncAnalyzer::analyze(expr);
  SizeType expr_id = BNET_NULLID;
  if ( logic_type == BnNodeType::Expr ) {
    expr_id = _reg_expr(expr);
  }
  return make_tuple(ni, logic_type, expr_id);
}

// @brief 論理式を登録する．
SizeType
BnNetworkImpl::_reg_expr(
  const Expr& expr
)
{
  auto ni = expr.input_size();
  SizeType expr_id = 0;
  if ( ni <= 10 ) {
    // 10入力以下の場合は一旦 TvFunc に変換する．
    TvFunc tv = expr.make_tv(ni);
    if ( mExprMap.count(tv) == 0 ) {
      // 新たに登録する．
      expr_id = mExprList.size();
      mExprList.push_back(expr);
      mExprMap[tv] = expr_id;
    }
    else {
      expr_id = mExprMap[tv];
    }
    return expr_id;
  }
  else {
    // 11入力以上は常に新規に登録する．
    expr_id = mExprList.size();
    mExprList.push_back(expr);
  }
  return expr_id;
}

// @brief 真理値表を登録する．
SizeType
BnNetworkImpl::_reg_tv(
  const TvFunc& tv
)
{
  // 同じ関数が登録されていないか調べる．
  SizeType func_id;
  if ( mFuncMap.count(tv) == 0 ) {
    // 新たに登録する．
    func_id = mFuncList.size();
    mFuncList.push_back(tv);
    mFuncMap[tv] = func_id;
  }
  else {
    func_id = mFuncMap[tv];
  }
  return func_id;
}

// @brief 内容を出力する．
void
BnNetworkImpl::write(
  ostream& s
) const
{
  s << "network name : " << name() << endl
    << endl;

  for ( auto port_p: mPortList ) {
    s << "port#" << port_p->id() << ": ";
    s << "(" << port_p->name() << ") : ";
    for ( auto i: Range(port_p->bit_width()) ) {
      s << " " << port_p->bit(i);
    }
    s << endl;
  }
  s << endl;

  for ( auto id: input_id_list() ) {
    auto& node = this->node(id);
    ASSERT_COND( node.type() == BnNodeType::Input );
    s << "input: " << node.id()
      << "(" << node.name() << ")" << endl;
  }
  s << endl;

  for ( auto id: output_id_list() ) {
    auto& node_p = mNodeList[id];
    ASSERT_COND( node_p != nullptr );
    ASSERT_COND( node_p->type() == BnNodeType::Output );
    s << "output: " << node_p->id()
      << "(" << node_p->name() << ")" << endl
      << "    input: " << node_p->fanin_id(0) << endl;
  }
  s << endl;

  for ( auto dff_p: mDffList ) {
    ASSERT_COND( dff_p != nullptr );
    s << "dff#" << dff_p->id()
      << "(" << dff_p->name() << ")" << endl
      << "    input:  " << dff_p->input() << endl
      << "    output: " << dff_p->output() << endl
      << "    clock:  " << dff_p->clock() << endl;
    if ( dff_p->clear() != BNET_NULLID ) {
      s << "    clear:  " << dff_p->clear() << endl;
    }
    if ( dff_p->preset() != BNET_NULLID ) {
      s << "    preset: " << dff_p->preset() << endl;
    }
    s << endl;
  }
  s << endl;

  for ( auto latch_p: mLatchList ) {
    ASSERT_COND( latch_p != nullptr );
    s << "latch#" << latch_p->id()
      << "(" << latch_p->name() << ")" << endl
      << "    input:  " << latch_p->input() << endl
      << "    output: " << latch_p->output() << endl
      << "    enable: " << latch_p->enable() << endl;
    if ( latch_p->clear() != BNET_NULLID ) {
      s << "    clear:  " << latch_p->clear() << endl;
    }
    if ( latch_p->preset() != BNET_NULLID ) {
      s << "    preset: " << latch_p->preset() << endl;
    }
  }
  s << endl;

  for ( auto id: logic_id_list() ) {
    auto node_p = mNodeList[id];
    ASSERT_COND( node_p != nullptr );
    ASSERT_COND( node_p->is_logic() );
    s << "logic: " << id
      << "(" << node_p->name() << ")" << endl
      << "    fanins: ";
    for ( auto fanin_id: node_p->fanin_id_list() ) {
    s << " " << fanin_id;
    }
    s << endl;
    s << "    ";
    switch ( node_p->type() ) {
    case BnNodeType::None:
      s << "NONE";
      break;
    case BnNodeType::C0:
      s << "C0";
      break;
    case BnNodeType::C1:
      s << "C1";
      break;
    case BnNodeType::Buff:
      s << "BUFF";
      break;
    case BnNodeType::Not:
      s << "NOT";
      break;
    case BnNodeType::And:
      s << "AND";
      break;
    case BnNodeType::Nand:
      s << "NAND";
      break;
    case BnNodeType::Or:
      s << "OR";
      break;
    case BnNodeType::Nor:
      s << "NOR";
      break;
    case BnNodeType::Xor:
      s << "XOR";
      break;
    case BnNodeType::Xnor:
      s << "XNOR";
      break;
    case BnNodeType::Expr:
      s << "expr#" << node_p->expr_id() << ": " << expr(node_p->expr_id());
      break;
    case BnNodeType::TvFunc:
      s << "func#" << node_p->func_id() << ": " << func(node_p->func_id());
      break;
    default:
      ASSERT_NOT_REACHED;
    }
    s << endl;
    int cell_id = node_p->cell_id();
    if ( cell_id != BNET_NULLID ) {
      const ClibCell& cell = mCellLibrary.cell(cell_id);
      s << "    cell: " << cell.name() << endl;
    }
    s << endl;
  }

  s << endl;
}

END_NAMESPACE_YM_BNET
