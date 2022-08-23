
/// @file BnNetworkImpl.cc
/// @brief BnNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BnNetworkImpl.h"
#include "ym/ClibCell.h"
#include "ym/ClibPin.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/Range.h"

#include "BnPortImpl.h"
#include "BnInputNode.h"
#include "BnOutputNode.h"
#include "BnLogicNode.h"
#include "BnDffImpl.h"

#include "FuncAnalyzer.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

inline
BnCPV
conv(
  ClibCPV clib_cpv
)
{
  switch ( clib_cpv ) {
  case ClibCPV::L: return BnCPV::L;
  case ClibCPV::H: return BnCPV::H;
  case ClibCPV::N: return BnCPV::N;
  case ClibCPV::T: return BnCPV::T;
  case ClibCPV::X: return BnCPV::X;
  default: ASSERT_NOT_REACHED;
  }
  return BnCPV::X;
}

END_NONAMESPACE

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
  for ( auto node_p: mNodeList ) {
    delete node_p;
  }

  mName = string();
  mPortList.clear();
  mDffList.clear();
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

// @brief 論理セルを追加する．
SizeType
BnNetworkImpl::new_logic_cell(
  const string& node_name,
  SizeType cell_id,
  const vector<SizeType>& fanin_id_list
)
{
  auto& cell = mCellLibrary.cell(cell_id);
  if ( cell.type() != ClibCellType::Logic || cell.output_num() != 1 ) {
    return BNET_NULLID;
  }

  Expr expr = cell.logic_expr(0);
  auto id = new_expr(node_name, expr, fanin_id_list);
  return id;
}

// @brief 既存の論理ノードを変更する最も低レベルの関数
void
BnNetworkImpl::_change_logic(
  SizeType node_id,
  BnNodeType logic_type,
  SizeType expr_id,
  SizeType func_id,
  const Bdd& bdd,
  const vector<SizeType>& fanin_id_list
)
{
  auto old_node = _node_p(node_id);
  SizeType nfi = fanin_id_list.size();
  auto new_node = _new_logic(old_node->name(), nfi, logic_type,
			     expr_id, func_id, bdd);
  _set_node(new_node, node_id);
  delete old_node;
  connect_fanins(node_id, fanin_id_list);

}

// @brief 論理ノードを作る最も低レベルの関数
SizeType
BnNetworkImpl::_reg_logic(
  const string& name,
  BnNodeType logic_type,
  SizeType expr_id,
  SizeType func_id,
  const Bdd& bdd,
  const vector<SizeType>& fanin_id_list
)
{
  SizeType nfi = fanin_id_list.size();
  auto node = _new_logic(name, nfi, logic_type,
			 expr_id, func_id, bdd);
  _reg_node(node);
  connect_fanins(node->id(), fanin_id_list);
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
  const Bdd& bdd
)
{
  BnNodeImpl* node = nullptr;
  if ( logic_type == BnNodeType::Expr ) {
    node = new BnExprNode{name, fanin_num, expr_id};
  }
  else if ( logic_type == BnNodeType::TvFunc ) {
    node = new BnTvNode{name, fanin_num, func_id};
  }
  else if ( logic_type == BnNodeType::Bdd ) {
    auto new_bdd = mBddMgr.copy(bdd);
    node = new BnBddNode{name, fanin_num, bdd};
  }
  else {
    node = new BnPrimNode{name, fanin_num, logic_type};
  }
  ASSERT_COND( node != nullptr );

  return node;
}

#if 0
// @brief 論理式型の論理ノードを追加する．
SizeType
BnNetworkImpl::new_expr(
  const string& node_name,
  const Expr& expr,
  const vector<SizeType>& fanin_id_list
)
{
  unordered_map<VarId, VarId> varmap;
  vector<SizeType> local_inputs;
  SizeType ni = fanin_id_list.size();
  for ( SizeType i = 0; i < ni; ++ i ) {
    if ( expr.literal_num(VarId{i}) > 0 ) {
      auto lid = local_inputs.size();
      varmap.emplace(VarId{i}, VarId{lid});
      SizeType inode = fanin_id_list[i];
      local_inputs.push_back(inode);
    }
  }
  auto local_expr = expr.remap_var(varmap);
  auto id = new_expr(node_name, local_expr);
  connect_fanins(id, local_inputs);
  return id;
}
#endif

// @brief DFFを追加する．
SizeType
BnNetworkImpl::new_dff(
  const string& name,
  bool has_clear,
  bool has_preset,
  BnCPV cpv
)
{
  auto dff_id = mDffList.size();

  // 入力端子
  auto input_id = _new_data_in(name, dff_id);

  // 出力端子
  auto output_id = _new_data_out(name, dff_id);

  // クロック端子
  auto clock_id = _new_clock(name, dff_id);

  // クリア端子
  auto clear_id = _new_clear(name, dff_id, has_clear);

  // プリセット端子
  auto preset_id = _new_preset(name, dff_id, has_preset);

  auto dff = new BnDffImpl{dff_id, name, input_id, output_id,
			   clock_id, clear_id, preset_id, cpv};
  mDffList.push_back(dff);

  return dff->id();
}

// @brief ラッチを追加する．
SizeType
BnNetworkImpl::new_latch(
  const string& name,
  bool has_clear,
  bool has_preset,
  BnCPV cpv
)
{
  auto dff_id = mDffList.size();

  // 入力端子
  auto input_id = _new_data_in(name, dff_id);

  // 出力端子
  auto output_id = _new_data_out(name, dff_id);

  // イネーブル端子
  auto enable_id = _new_clock(name, dff_id);

  // クリア端子
  auto clear_id = _new_clear(name, dff_id, has_clear);

  // プリセット端子
  auto preset_id = _new_preset(name, dff_id, has_preset);

  auto dff = new BnLatchImpl{dff_id, name, input_id, output_id,
			     enable_id, clear_id, preset_id, cpv};
  mDffList.push_back(dff);

  return dff_id;
}

// @brief セルの情報を持ったDFFを追加する．
SizeType
BnNetworkImpl::new_dff_cell(
  const string& name,
  SizeType cell_id
)
{
  auto dff_id = mDffList.size();

  const ClibCell& cell = mCellLibrary.cell(cell_id);
  if ( !cell.is_ff() ) {
    ostringstream buf;
    buf << "BnNetwork::new_dff_cell(): "
	<< cell.name() << " is not a latch cell.";
    throw BnetError{buf.str()};
  }
  if ( cell.inout_num() > 0 ) {
    ostringstream buf;
    buf << "BnNetwork::new_dff_cell(): "
	<< cell.name() << " has inout pins.";
    throw BnetError{buf.str()};
  }

  SizeType ni = cell.input_num();
  vector<SizeType> input_list(ni);
  for ( SizeType i = 0; i < ni; ++ i ) {
    input_list[i] = _new_cell_input(name, dff_id, i);
  }

  SizeType no = cell.output_num();
  vector<SizeType> output_list(no);
  for ( SizeType i = 0; i < no; ++ i ) {
    output_list[i] = _new_cell_output(name, dff_id, i);
  }

  auto dff = new BnDffCell{dff_id, name, cell_id, input_list, output_list};
  mDffList.push_back(dff);

  return dff_id;
}

// @brief data_in ノードを作る．
SizeType
BnNetworkImpl::_new_data_in(
  const string& dff_name,
  SizeType dff_id
)
{
  ostringstream buf;
  buf << dff_name << ".input";
  auto iname = buf.str();
  auto node = new BnDataIn{iname, dff_id};
  _reg_output(node);
  return node->id();
}

// @brief データ出力ノードを作る．
SizeType
BnNetworkImpl::_new_data_out(
  const string& dff_name,
  SizeType dff_id
)
{
  ostringstream buf;
  buf << dff_name << ".output";
  auto oname = buf.str();
  auto node = new BnDataOut{oname, dff_id};
  _reg_input(node);
  return node->id();
}

// @brief クロック端子ノードを作る．
SizeType
BnNetworkImpl::_new_clock(
  const string& dff_name,
  SizeType dff_id
)
{
  ostringstream buf;
  buf << dff_name << ".clock";
  auto oname = buf.str();
  auto node = new BnClock{oname, dff_id};
  _reg_output(node);
  return node->id();
}

// @brief クリア端子ノードを作る．
SizeType
BnNetworkImpl::_new_clear(
  const string& dff_name,
  SizeType dff_id,
  bool has_clear
)
{
  if ( has_clear ) {
    ostringstream buf;
    buf << dff_name << ".clear";
    auto oname = buf.str();
    auto node = new BnClear{oname, dff_id};
    _reg_output(node);
    return node->id();
  }
  return BNET_NULLID;
}

// @brief プリセット端子ノードを作る．
SizeType
BnNetworkImpl::_new_preset(
  const string& dff_name,
  SizeType dff_id,
  bool has_clear
)
{
  if ( has_clear ) {
    ostringstream buf;
    buf << dff_name << ".preset";
    auto oname = buf.str();
    auto node = new BnPreset{oname, dff_id};
    _reg_output(node);
    return node->id();
  }
  return BNET_NULLID;
}

// @brief DFFセルの入力端子を作る．
SizeType
BnNetworkImpl::_new_cell_input(
  const string& dff_name,
  SizeType dff_id,
  SizeType pos
)
{
  ostringstream buf;
  buf << dff_name << ".input" << (pos + 1);
  auto oname = buf.str();
  auto node = new BnCellInput{oname, dff_id, pos};
  _reg_output(node);
  return node->id();
}

// @brief DFFセルの出力端子を作る．
SizeType
BnNetworkImpl::_new_cell_output(
  const string& dff_name,
  SizeType dff_id,
  SizeType pos
)
{
  ostringstream buf;
  buf << dff_name << ".output" << (pos + 1);
  auto oname = buf.str();
  auto node = new BnCellOutput{oname, dff_id, pos};
  _reg_input(node);
  return node->id();
}

// @brief 入出力混合のポートを作る．
SizeType
BnNetworkImpl::new_port(
  const string& port_name,
  const vector<BnDir>& dir_vect
)
{
  if ( mPortDict.count(port_name) > 0 ) {
    ostringstream buf;
    buf << "Error in BnNetwork::new_port(): "
	<< "'" << port_name << "' is already in use.";
    throw BnetError{buf.str()};
  }

  auto port_id = mPortList.size();
  auto bit_width = dir_vect.size();
  vector<SizeType> bits(bit_width);
  for ( SizeType i: Range(bit_width) ) {
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
      auto node = new BnPortInput{node_name, port_id, i};
      _reg_primary_input(node);
      bits[i] = node->id();
    }
    else { // BnDir::OUTPUT
      auto node = new BnPortOutput{node_name, port_id, i};
      _reg_primary_output(node);
      bits[i] = node->id();
    }
  }

  BnPort* port;
  if ( bit_width == 1 ) {
    port = new BnPort1{port_id, port_name, bits[0]};
  }
  else {
    port = new BnPortN{port_id, port_name, bits};
  }
  mPortList.push_back(port);
  mPortDict.emplace(port_name, port);

  return port_id;
}

// @brief ノードを複製する．
SizeType
BnNetworkImpl::dup_logic(
  const string& node_name,
  SizeType src_id,
  const vector<SizeType>& fanin_id_list
)
{
  auto src_node = _node_p(src_id);
  ASSERT_COND( src_node->fanin_num() == fanin_id_list.size() );
  auto logic_type = src_node->type();
  auto expr_id = src_node->expr_id();
  auto func_id = src_node->func_id();
  auto bdd = src_node->bdd();
  auto id = _reg_logic(node_name, logic_type, expr_id, func_id, bdd,
		       fanin_id_list);
  return id;
}

// @brief ファンアウトをつなぎ替える．
void
BnNetworkImpl::substitute_fanout(
  SizeType old_id,
  SizeType new_id
)
{
  ASSERT_COND( _check_node_id(new_id) );

  // old_id のファンアウトのリストをコピーする．
  auto old_node = _node_p(old_id);
  vector<SizeType> fo_list{old_node->fanout_id_list()};
  for ( auto dst: fo_list ) {
    auto dst_node = _node_p(dst);
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
  auto node = _node_p(id);
  auto ni = node->fanin_num();

  ASSERT_COND( fanin_id_list.size() == ni );

  for ( auto i: Range(ni) ) {
    node->set_fanin(i, fanin_id_list[i]);
  }

  mSane = true;
}

// @brief 出力ノードのファンインを設定する．
void
BnNetworkImpl::set_output(
  SizeType output_id,
  SizeType src_id
)
{
  ASSERT_COND( _check_node_id(src_id) );

  auto dst_node = _node_p(output_id);
  dst_node->set_fanin(0, src_id);

  mSane = false;
}

BEGIN_NONAMESPACE

inline
string
port_name(
  const BnPort* port,
  SizeType bit_pos
)
{
  ostringstream buf;
  buf << "Port#" << port->id()
      << "(" << port->name() << ").bit["
      << bit_pos << "]";
  return buf.str();
}

inline
string
dff_name(
  const BnDff* dff
)
{
  ostringstream buf;
  buf << "DFF#" << dff->id()
      << "(" << dff->name() << ")";
  return buf.str();
}

inline
string
node_fanin_name(
  const BnNodeImpl* node,
  SizeType ipos
)
{
  ostringstream buf;
  buf << "NODE#" << node->id()
      << "(" << node->name() << ").fanin["
      << ipos << "]";
  return buf.str();
}

END_NONAMESPACE

// @brief 整合性のチェックを行う．
void
BnNetworkImpl::wrap_up()
{
  if ( mSane ) {
    return;
  }

  bool error = false;

  // ポートのチェック
  for ( auto port_p: mPortList ) {
    for ( auto i: Range(port_p->bit_width()) ) {
      auto id = port_p->bit(i);
      if ( id == BNET_NULLID ) {
	cerr << port_name(port_p, i)
	     << " is not set" << endl;
	error = true;
      }
      else if ( !_check_node_id(id) ) {
	cerr << port_name(port_p, i)
	     << " is not valid" << endl;
	error = true;
      }
    }
  }

  // DFF のチェック
  for ( auto dff_p: mDffList ) {
    if ( dff_p->is_dff() || dff_p->is_latch() ) {
      { // data_in
	auto id1 = dff_p->data_in();
	if ( id1 == BNET_NULLID ) {
	  cerr << dff_name(dff_p)
	       << ".data_in is not set" << endl;
	  error = true;
	}
	else if ( !_check_node_id(id1) ) {
	  cerr << dff_name(dff_p)
	       << ".data_in is not valid" << endl;
	  error = true;
	}
      }
      { // data_out
	auto id2 = dff_p->data_out();
	if ( id2 == BNET_NULLID ) {
	  cerr << dff_name(dff_p)
	       << ".data_out is not set" << endl;
	  error = true;
	}
	else if ( !_check_node_id(id2) ) {
	  cerr << dff_name(dff_p)
	       << ".data_out is not valid" << endl;
	  error = true;
	}
      }
      { // clock
	auto id3 = dff_p->clock();
	if ( id3 == BNET_NULLID ) {
	  cerr << dff_name(dff_p)
	       << ".clock is not set" << endl;
	  error = true;
	}
	else if ( !_check_node_id(id3) ) {
	  cerr << dff_name(dff_p)
	       << ".clock is not valid" << endl;
	  error = true;
	}
      }
      { // clear
	auto id4 = dff_p->clear();
	if ( id4 != BNET_NULLID && !_check_node_id(id4) ) {
	  cerr << dff_name(dff_p)
	       << ".clear is not valid" << endl;
	  error = true;
	}
      }
      { // preset
	auto id5 = dff_p->preset();
	if ( id5 != BNET_NULLID && !_check_node_id(id5) ) {
	  cerr << dff_name(dff_p)
	       << ".preset is not valid" << endl;
	  error = true;
	}
      }
    }
    else if ( dff_p->is_cell() ) {
      SizeType ni = dff_p->cell_input_num();
      for ( SizeType i = 0; i < ni; ++ i ) {
	auto id = dff_p->cell_input(i);
	if ( id == BNET_NULLID ) {
	  cerr << dff_name(dff_p)
	       << ".input" << i << " is not set" << endl;
	  error = true;
	}
	else if ( !_check_node_id(id) ) {
	  cerr << dff_name(dff_p)
	       << ".input" << i << " is not valid" << endl;
	  error = true;
	}
      }
      SizeType no = dff_p->cell_output_num();
      for ( SizeType i = 0; i < no; ++ i ) {
	auto id = dff_p->cell_output(i);
	if ( id == BNET_NULLID ) {
	  cerr << dff_name(dff_p)
	       << ".output" << i << " is not set" << endl;
	  error = true;
	}
	else if ( !_check_node_id(id) ) {
	  cerr << dff_name(dff_p)
	       << ".output" << i << " is not valid" << endl;
	  error = true;
	}
      }
    }
  }

  // ノードのチェック
  for ( auto node_p: mNodeList ) {
    for ( auto i: Range(node_p->fanin_num()) ) {
      auto id = node_p->fanin_id(i);
      if ( id == BNET_NULLID ) {
	if ( node_p->is_logic() ) {
	  cerr << node_fanin_name(node_p, i)
	       << " is not set" << endl;
	  error = true;
	}
      }
      else if ( !_check_node_id(id) ) {
	cerr << node_fanin_name(node_p, i)
	     << " is not valid" << endl;
	error = true;
      }
    }
  }

  ASSERT_COND( !error );

  // 各ノードのファンアウトリストの作成
  for ( auto node_p: mNodeList ) {
    node_p->clear_fanout();
  }
  for ( auto node_p: mNodeList ) {
    for ( auto id: node_p->fanin_id_list() ) {
      if ( id != BNET_NULLID ) {
	auto src_node_p = _node_p(id);
	src_node_p->add_fanout(node_p->id());
      }
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
    auto node_p = _node_p(id);
    if ( node_p->is_logic() ) {
      mLogicList.push_back(id);
    }
    for ( auto oid: node_p->fanout_id_list() ) {
      if ( mark[oid] ) {
	continue;
      }
      auto onode_p = _node_p(oid);
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
    auto node_p = _node_p(oid);
    auto iid = node_p->fanin_id(0);
    mOutputSrcList[i] = iid;
  }

  // mPrimaryOutputSrcList を作る．
  mPrimaryOutputSrcList.clear();
  mPrimaryOutputSrcList.resize(mPrimaryOutputList.size());
  for ( auto i: Range(mPrimaryOutputList.size()) ) {
    auto oid = mPrimaryOutputList[i];
    auto node_p = _node_p(oid);
    auto iid = node_p->fanin_id(0);
    mPrimaryOutputSrcList[i] = iid;
  }

  mSane = true;
}

// @brief 実装可能な構造を持っている時 true を返す．
bool
BnNetworkImpl::is_concrete() const
{
  for ( auto id: logic_id_list() ) {
    auto node_p = _node_p(id);
    switch ( node_p->type() ) {
    case BnNodeType::TvFunc:
    case BnNodeType::Bdd:
      return false;
    default:
      break;
    }
  }
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

END_NAMESPACE_YM_BNET
