
/// @file BnetWriter
/// @brief BnetWriter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "BnetWriter.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnLatch.h"
#include "ym/BnNode.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

// @brief 内容を出力する．
void
BnNetwork::write(
  ostream& s
) const
{
  BnetWriter writer;

  writer(s, *this);
}


//////////////////////////////////////////////////////////////////////
// クラス BnetWriter
//////////////////////////////////////////////////////////////////////

// @brief 内容を出力する．
void
BnetWriter::operator()(
  ostream& s,
  const BnNetwork& network
) const
{
  s << "network name : " << network.name() << endl
    << endl;

  SizeType np = network.port_num();
  for ( SizeType i = 0; i < np; ++ i ) {
    auto& port = network.port(i);
    s << "port#" << port.id() << ": ";
    s << "(" << port.name() << ") : ";
    for ( auto i: Range(port.bit_width()) ) {
      s << " " << port.bit(i);
    }
    s << endl;
  }
  s << endl;

  for ( auto id: network.input_id_list() ) {
    auto& node = network.node(id);
    ASSERT_COND( node.type() == BnNodeType::Input );
    s << "input: " << node.id()
      << "(" << node.name() << ")" << endl;
  }
  s << endl;

  for ( auto id: network.output_id_list() ) {
    auto& node = network.node(id);
    s << "output: " << node.id()
      << "(" << node.name() << ")" << endl
      << "    input: " << node.fanin_id(0) << endl;
  }
  s << endl;

  SizeType ndff = network.dff_num();
  for ( SizeType i = 0; i < ndff; ++ i ) {
    auto& dff = network.dff(i);
    s << "dff#" << dff.id()
      << "(" << dff.name() << ")" << endl
      << "    input:  " << dff.input() << endl
      << "    output: " << dff.output() << endl
      << "    clock:  " << dff.clock() << endl;
    if ( dff.clear() != BNET_NULLID ) {
      s << "    clear:  " << dff.clear() << endl;
    }
    if ( dff.preset() != BNET_NULLID ) {
      s << "    preset: " << dff.preset() << endl;
    }
    s << endl;
  }
  s << endl;

  SizeType nlatch = network.latch_num();
  for ( SizeType i = 0; i < nlatch; ++ i ) {
    auto& latch = network.latch(i);
    s << "latch#" << latch.id()
      << "(" << latch.name() << ")" << endl
      << "    input:  " << latch.input() << endl
      << "    output: " << latch.output() << endl
      << "    enable: " << latch.enable() << endl;
    if ( latch.clear() != BNET_NULLID ) {
      s << "    clear:  " << latch.clear() << endl;
    }
    if ( latch.preset() != BNET_NULLID ) {
      s << "    preset: " << latch.preset() << endl;
    }
  }
  s << endl;

  for ( auto id: network.logic_id_list() ) {
    auto& node = network.node(id);
    s << "logic: " << id
      << "(" << node.name() << ")" << endl
      << "    fanins: ";
    for ( auto fanin_id: node.fanin_id_list() ) {
    s << " " << fanin_id;
    }
    s << endl;
    s << "    ";
    switch ( node.type() ) {
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
      s << "expr#" << node.expr_id() << ": "
	<< network.expr(node.expr_id());
      break;
    case BnNodeType::TvFunc:
      s << "func#" << node.func_id() << ": "
	<< network.func(node.func_id());
      break;
    case BnNodeType::Bdd:
      s << "BDD" << endl;
      node.bdd().display(s);
      break;
    default:
      ASSERT_NOT_REACHED;
    }
    s << endl;
    int cell_id = node.cell_id();
    if ( cell_id != BNET_NULLID ) {
      const ClibCell& cell = network.library().cell(cell_id);
      s << "    cell: " << cell.name() << endl;
    }
    s << endl;
  }

  s << endl;
}

END_NAMESPACE_YM_BNET
