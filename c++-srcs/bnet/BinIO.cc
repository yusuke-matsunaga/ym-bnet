
/// @file BinIO.cc
/// @brief BinIO の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "BinIO.h"
#include "ym/BnNetwork.h"
#include "ym/Range.h"
#include "BnNetworkImpl.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

// シグネチャ
const char* BNET_SIG{"ym_bnet1.0"};

END_NONAMESPACE

// @brief BnNetwork の内容をダンプする．
void
BinIO::dump(
  BinEnc& s,
  const BnNetwork& network
)
{
  // シグネチャ
  s.write_signature(BNET_SIG);

  // 名前
  s.write_string(network.name());

  // 論理式
  SizeType ne = network.expr_num();
  s.write_vint(ne);
  for ( SizeType i = 0; i < ne; ++ i ) {
    network.expr(i).dump(s);
  }

  // 真理値表型の関数
  SizeType nf = network.func_num();
  s.write_vint(nf);
  for ( SizeType i = 0; i < nf; ++ i ) {
    network.func(i).dump(s);
  }

  // BDD
  mBddMap.clear();
  {
    vector<Bdd> bdd_list;
    for ( auto& node: network.logic_list() ) {
      if ( node.type() == BnNodeType::Bdd ) {
	auto bdd = node.bdd();
	if ( mBddMap.count(bdd) == 0 ) {
	  SizeType bdd_id = bdd_list.size();
	  bdd_list.push_back(bdd);
	  mBddMap.emplace(bdd, bdd_id);
	}
      }
    }
    Bdd::dump(s, bdd_list);
  }

  // ポート
  SizeType np = network.port_num();
  s.write_vint(np);
  for ( auto& port: network.port_list() ) {
    s.write_string(port.name());
    SizeType nb = port.bit_width();
    s.write_vint(nb);
    for ( auto i: Range(nb) ) {
      SizeType id = port.bit(i);
      auto& node = network.node(id);
      if ( node.is_input() ) {
	s.write_8(0);
      }
      else if ( node.is_output() ) {
	s.write_8(1);
      }
      else {
	ASSERT_NOT_REACHED;
      }
      s.write_vint(port.bit(i));
    }
  }

  // D-FF
  SizeType ndff = network.dff_num();
  s.write_vint(ndff);
  for ( auto& dff: network.dff_list() ) {
    dump_dff(s, dff);
  }

  // 論理ノード
  SizeType nl = network.logic_num();
  s.write_vint(nl);
  for ( auto& node: network.logic_list() ) {
    dump_logic(s, node);
  }

  // 出力ノード
  SizeType no = network.output_num();
  s.write_vint(no);
  for ( auto& node: network.output_list() ) {
    s.write_vint(node.id());
    s.write_vint(node.fanin_id(0));
  }
}

// DFFの内容をバイナリダンプする．
void
BinIO::dump_dff(
  BinEnc& s,
  const BnDff& dff
)
{
  s.write_string(dff.name());
  switch ( dff.type() ) {
  case BnDffType::None:  s.write_8(0); break;
  case BnDffType::Dff:   s.write_8(1); break;
  case BnDffType::Latch: s.write_8(2); break;
  case BnDffType::Cell:  s.write_8(3); break;
  }
  if ( dff.is_dff() || dff.is_latch() ) {
    s.write_vint(dff.data_in());
    s.write_vint(dff.data_out());
    s.write_vint(dff.clock());
    s.write_vint(dff.clear());
    s.write_vint(dff.preset());
    // clear_preset_value()
  }
  else if ( dff.is_cell() ) {
    s.write_vint(dff.cell_id());
    SizeType ni = dff.cell_input_num();
    s.write_vint(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      s.write_vint(dff.cell_input(i));
    }
    SizeType no = dff.cell_output_num();
    s.write_vint(no);
    for ( SizeType i = 0; i < no; ++ i ) {
      s.write_vint(dff.cell_output(i));
    }
  }
}

// 論理ノードの内容をバイナリダンプする．
void
BinIO::dump_logic(
  BinEnc& s,
  const BnNode& node
)
{
  s.write_vint(node.id());
  s.write_string(node.name());
  SizeType nfi = node.fanin_num();
  s.write_vint(nfi);
  for ( auto fanin_id: node.fanin_id_list() ) {
    s.write_vint(fanin_id);
  }
  switch ( node.type() ) {
  case BnNodeType::None:
    s.write_8(0);
    break;
  case BnNodeType::C0:
    s.write_8(1);
    break;
  case BnNodeType::C1:
    s.write_8(2);
    break;
  case BnNodeType::Buff:
    s.write_8(3);
    break;
  case BnNodeType::Not:
    s.write_8(4);
    break;
  case BnNodeType::And:
    s.write_8(5);
    break;
  case BnNodeType::Nand:
    s.write_8(6);
    break;
  case BnNodeType::Or:
    s.write_8(7);
    break;
  case BnNodeType::Nor:
    s.write_8(8);
    break;
  case BnNodeType::Xor:
    s.write_8(9);
    break;
  case BnNodeType::Xnor:
    s.write_8(10);
    break;
  case BnNodeType::Expr:
    s.write_8(11);
    s.write_vint(node.expr_id());
    break;
  case BnNodeType::TvFunc:
    s.write_8(12);
    s.write_vint(node.func_id());
    break;
  case BnNodeType::Bdd:
    s.write_8(13);
    {
      SizeType id = mBddMap.at(node.bdd());
      s.write_vint(id);
    }
    break;
  case BnNodeType::Cell:
    s.write_8(14);
    s.write_vint(node.cell_id());
    break;
  default:
    ASSERT_NOT_REACHED;
  }
}

// @brief BnNetwork を復元する．
void
BinIO::restore(
  BinDec& s,
  BnNetworkImpl* network_impl
)
{
  // シグネチャ
  if ( !s.read_signature(BNET_SIG) ) {
    throw BnetError{"BnNetwork::restore(): Wrong signature."};
  }

  // 名前
  auto name = s.read_string();
  network_impl->set_name(name);

  // 論理式
  SizeType ne = s.read_vint();
  mExprList.clear();
  mExprList.resize(ne);
  for ( SizeType i = 0; i < ne; ++ i ) {
    mExprList[i] = Expr::restore(s);
  }

  // 真理値表型の関数
  SizeType nf = s.read_vint();
  mFuncList.clear();
  mFuncList.resize(nf);
  for ( SizeType i = 0; i < nf; ++ i ) {
    mFuncList[i].restore(s);
  }

  // Bdd
  mBddList = network_impl->restore_bdds(s);

  mNodeMap.clear();

  // ポート
  SizeType np = s.read_vint();
  for ( SizeType i = 0; i < np; ++ i ) {
    auto name = s.read_string();
    SizeType nb = s.read_vint();
    vector<BnDir> dir_vect(nb);
    vector<SizeType> id_list(nb);
    for ( SizeType j = 0; j < nb; ++ j ) {
      ymuint8 dir = s.read_8();
      dir_vect[j] = static_cast<BnDir>(dir);
      id_list[j] = s.read_vint();
    }
    SizeType id = network_impl->new_port(name, dir_vect);
    auto& port = network_impl->port(id);
    for ( SizeType j = 0; j < nb; ++ j ) {
      SizeType dst_id = port.bit(j);
      mNodeMap[id_list[j]] = dst_id;
    }
  }

  // D-FF
  SizeType ndff = s.read_vint();
  for ( SizeType i = 0; i < ndff; ++ i ) {
    restore_dff(s, network_impl);
  }

  // 論理ノード
  SizeType nl = s.read_vint();
  for ( SizeType i = 0; i < nl; ++ i ) {
    restore_logic(s, network_impl);
  }

  // 出力ノード
  SizeType no = s.read_vint();
  for ( SizeType i = 0; i < no; ++ i ) {
    SizeType src_output_id = s.read_vint();
    ASSERT_COND( mNodeMap.count(src_output_id) > 0 );
    SizeType src_input_id = s.read_vint();
    if ( src_input_id != BNET_NULLID ) {
      network_impl->set_output_src(mNodeMap[src_output_id], mNodeMap[src_input_id]);
    }
  }

  network_impl->wrap_up();
}

// @brief DFFを復元する．
void
BinIO::restore_dff(
  BinDec& s,
  BnNetworkImpl* network_impl
)
{
  auto name = s.read_string();
  SizeType type = s.read_vint();
  if ( type == 1 || type == 2 ) {
    SizeType src_input_id = s.read_vint();
    SizeType src_output_id = s.read_vint();
    SizeType src_clock_id = s.read_vint();
    SizeType src_clear_id = s.read_vint();
    bool has_clear = src_clear_id != BNET_NULLID;
    SizeType src_preset_id = s.read_vint();
    bool has_preset = src_preset_id != BNET_NULLID;
    // clear_preset_value()
    SizeType id;
    if ( type == 0 ) {
      id = network_impl->new_dff(name, has_clear, has_preset);
    }
    else {
      id = network_impl->new_latch(name, has_clear, has_preset);
    }
    auto& dff = network_impl->dff(id);
    mNodeMap[src_input_id] = dff.data_in();
    mNodeMap[src_output_id] = dff.data_out();
    mNodeMap[src_clock_id] = dff.clock();
    if ( has_clear ) {
      mNodeMap[src_clear_id] = dff.clear();
    }
    if ( has_preset ) {
      mNodeMap[src_preset_id] = dff.preset();
    }
  }
  else if ( type == 3 ) {
    SizeType cell_id = s.read_vint();
    SizeType id = network_impl->new_dff_cell(name, cell_id);
    auto& dff = network_impl->dff(id);
    SizeType ni = s.read_vint();
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto src_id = s.read_vint();
      mNodeMap[src_id] = dff.cell_input(i);
    }
    SizeType no = s.read_vint();
    for ( SizeType i = 0; i < no; ++ i ) {
      auto src_id = s.read_vint();
      mNodeMap[src_id] = dff.cell_output(i);
    }
  }
}

// @brief 論理ノードを復元する．
void
BinIO::restore_logic(
  BinDec& s,
  BnNetworkImpl* network_impl
)
{
  SizeType id = s.read_vint();
  auto name = s.read_string();
  SizeType nfi = s.read_vint();
  vector<SizeType> fanin_id_list(nfi);
  for ( SizeType j = 0; j < nfi; ++ j ) {
    SizeType src_id = s.read_vint();
    ASSERT_COND( mNodeMap.count(src_id) > 0 );
    fanin_id_list[j] = mNodeMap.at(src_id);
  }
  ymuint8 type_code = s.read_8();
  BnNodeType type{BnNodeType::None};
  SizeType node_id = BNET_NULLID;
  switch ( type_code ) {
  case 0: // None:
    ASSERT_NOT_REACHED;
    break;
  case 1: type = BnNodeType::C0; break;
  case 2: type = BnNodeType::C1; break;
  case 3: type = BnNodeType::Buff; break;
  case 4: type = BnNodeType::Not; break;
  case 5: type = BnNodeType::And; break;
  case 6: type = BnNodeType::Nand; break;
  case 7: type = BnNodeType::Or; break;
  case 8: type = BnNodeType::Nor; break;
  case 9: type = BnNodeType::Xor; break;
  case 10: type = BnNodeType::Xnor; break;
  default: break;
  }
  if ( type != BnNodeType::None ) {
    node_id = network_impl->new_logic_primitive(name, type, fanin_id_list);
  }
  else if ( type_code == 11 ) {
    // Expr
    SizeType id = s.read_vint();
    ASSERT_COND( id < mExprList.size() );
    auto& expr = mExprList[id];
    node_id = network_impl->new_logic_expr(name, expr, fanin_id_list);
  }
  else if ( type_code == 12 ) {
    // TvFunc
    SizeType id = s.read_vint();
    auto& func = mFuncList[id];
    node_id = network_impl->new_logic_tv(name, func, fanin_id_list);
  }
  else if ( type_code == 13 ) {
    // Bdd
    SizeType id = s.read_vint();
    auto bdd = mBddList[id];
    node_id = network_impl->new_logic_bdd(name, bdd, fanin_id_list);
  }
  else if ( type_code == 14 ) {
    // Cell
    SizeType cell_id = s.read_vint();
    node_id = network_impl->new_logic_cell(name, cell_id, fanin_id_list);
  }
  mNodeMap[id] = node_id;
}


//////////////////////////////////////////////////////////////////////
// BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief 内容を独自形式でバイナリダンプする．
void
BnNetwork::dump(
  BinEnc& s
) const
{
  BinIO bio;
  bio.dump(s, *this);
}

// @brief バイナリダンプされた内容を復元する．
BnNetwork
BnNetwork::restore(
  BinDec& s
)
{
  BinIO bio;
  BnNetwork network;
  bio.restore(s, network.mImpl.get());
  return network;
}

END_NAMESPACE_YM_BNET
