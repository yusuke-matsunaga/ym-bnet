
/// @file BnNetwork_dump_restore.cc
/// @brief BnNetwork::dump(), restore() の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/Range.h"
#include "BnNetworkImpl.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

// シグネチャ
const char* BNET_SIG{"ym_bnet1.0"};

END_NONAMESPACE


// @brief 内容を独自形式でバイナリダンプする．
void
BnNetwork::dump(
  BinEnc& s
) const
{
  // シグネチャ
  s.write_signature(BNET_SIG);

  // 名前
  s.write_string(name());

  // 論理式
  SizeType ne = expr_num();
  s.write_vint(ne);
  for ( SizeType i = 0; i < ne; ++ i ) {
    const auto& expr = this->expr(i);
    expr.dump(s);
  }

  // 真理値表型の関数
  SizeType nf = func_num();
  s.write_vint(nf);
  for ( SizeType i = 0; i < nf; ++ i ) {
    auto& func = this->func(i);
    func.dump(s);
  }

  // BDD
  unordered_map<Bdd, SizeType> bdd_map;
  {
    vector<Bdd> bdd_list;
    for ( auto id: logic_id_list() ) {
      auto& node = this->node(id);
      if ( node.type() == BnNodeType::Bdd ) {
	auto bdd = node.bdd();
	if ( bdd_map.count(bdd) == 0 ) {
	  SizeType bdd_id = bdd_list.size();
	  bdd_list.push_back(bdd);
	  bdd_map.emplace(bdd, bdd_id);
	}
      }
    }
    Bdd::dump(s, bdd_list);
  }

  // ポート
  SizeType np = port_num();
  s.write_vint(np);
  for ( SizeType i = 0; i < np; ++ i ) {
    auto& port = this->port(i);
    s.write_string(port.name());
    SizeType nb = port.bit_width();
    s.write_vint(nb);
    for ( auto i: Range(nb) ) {
      SizeType id = port.bit(i);
      auto& node = this->node(id);
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
#warning "TODO: type() 別のコード"
  SizeType ndff = dff_num();
  s.write_vint(ndff);
  for ( SizeType i = 0; i < ndff; ++ i ) {
    auto& dff = this->dff(i);
    s.write_string(dff.name());
    s.write_vint(dff.data_in());
    s.write_vint(dff.data_out());
    s.write_vint(dff.clock());
    s.write_vint(dff.clear());
    s.write_vint(dff.preset());
  }

  // 論理ノード
  SizeType nl = logic_num();
  s.write_vint(nl);
  for ( auto id: logic_id_list() ) {
    auto& node = this->node(id);
    s.write_vint(id);
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
	SizeType id = bdd_map.at(node.bdd());
	s.write_vint(id);
      }
      break;
    default:
      ASSERT_NOT_REACHED;
    }
    //s.write_vint(node.cell_id());
  }

  // 出力ノード
  SizeType no = output_num();
  s.write_vint(no);
  for ( auto id: output_id_list() ) {
    auto& node = this->node(id);
    s.write_vint(id);
    s.write_vint(node.fanin_id(0));
  }
}

// @brief バイナリダンプされた内容を復元する．
BnNetwork
BnNetwork::restore(
  BinDec& s
)
{
  // シグネチャ
  if ( !s.read_signature(BNET_SIG) ) {
    throw BnetError{"BnNetwork::restore(): Wrong signature."};
  }

  // 結果を格納するネットワーク
  BnNetwork network;

  // 名前
  auto name = s.read_string();
  network.set_name(name);

  // 論理式
  SizeType ne = s.read_vint();
  vector<Expr> expr_list(ne);
  for ( SizeType i = 0; i < ne; ++ i ) {
    expr_list[i] = Expr::restore(s);
  }

  // 真理値表型の関数
  SizeType nf = s.read_vint();
  vector<TvFunc> func_list(nf);
  for ( SizeType i = 0; i < nf; ++ i ) {
    func_list[i].restore(s);
  }

  // Bdd
  vector<Bdd> bdd_list = network.mImpl->restore_bdds(s);

  // ノード番号の対応表
  unordered_map<SizeType, SizeType> node_map;

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
    SizeType id = network.new_port(name, dir_vect);
    auto& port = network.port(id);
    for ( SizeType j = 0; j < nb; ++ j ) {
      SizeType dst_id = port.bit(j);
      node_map[id_list[j]] = dst_id;
    }
  }

  // D-FF
#warning "TODO: type() 別のコード"
  SizeType ndff = s.read_vint();
  for ( SizeType i = 0; i < ndff; ++ i ) {
    auto name = s.read_string();
    SizeType src_input_id = s.read_vint();
    SizeType src_output_id = s.read_vint();
    SizeType src_clock_id = s.read_vint();
    SizeType src_clear_id = s.read_vint();
    bool has_clear = src_clear_id != BNET_NULLID;
    SizeType src_preset_id = s.read_vint();
    bool has_preset = src_preset_id != BNET_NULLID;
    SizeType id = network.new_dff(name, has_clear, has_preset);
    auto& dff = network.dff(id);
    node_map[src_input_id] = dff.data_in();
    node_map[src_output_id] = dff.data_out();
    node_map[src_clock_id] = dff.clock();
    if ( has_clear ) {
      node_map[src_clear_id] = dff.clear();
    }
    if ( has_preset ) {
      node_map[src_preset_id] = dff.preset();
    }
  }

  // 論理ノード
  SizeType nl = s.read_vint();
  for ( SizeType i = 0; i < nl; ++ i ) {
    SizeType id = s.read_vint();
    auto name = s.read_string();
    SizeType nfi = s.read_vint();
    vector<SizeType> fanin_id_list(nfi);
    for ( SizeType j = 0; j < nfi; ++ j ) {
      SizeType src_id = s.read_vint();
      ASSERT_COND( node_map.count(src_id) > 0 );
      fanin_id_list[j] = node_map.at(src_id);
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
      node_id = network.new_logic(name, type, fanin_id_list);
    }
    else if ( type_code == 11 ) {
      // Expr
      SizeType id = s.read_vint();
      auto& expr = expr_list[id];
      node_id = network.new_logic(name, expr, fanin_id_list);
    }
    else if ( type_code == 12 ) {
      // TvFunc
      SizeType id = s.read_vint();
      auto& func = func_list[id];
      node_id = network.new_logic(name, func, fanin_id_list);
    }
    else if ( type_code == 13 ) {
      // Bdd
      SizeType id = s.read_vint();
      auto bdd = bdd_list[id];
      node_id = network.new_logic(name, bdd, fanin_id_list);
    }
    node_map[id] = node_id;
  }

  // 出力ノード
  SizeType no = s.read_vint();
  for ( SizeType i = 0; i < no; ++ i ) {
    SizeType src_output_id = s.read_vint();
    ASSERT_COND( node_map.count(src_output_id) > 0 );
    SizeType src_input_id = s.read_vint();
    if ( src_input_id != BNET_NULLID ) {
      network.set_output(node_map[src_output_id], node_map[src_input_id]);
    }
  }

  network.wrap_up();

  return network;
}

END_NAMESPACE_YM_BNET
