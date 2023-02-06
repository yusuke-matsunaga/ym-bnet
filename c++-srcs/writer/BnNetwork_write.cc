
/// @file BnNetwork_write.cc
/// @brief BnNetwork::write() の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"
#include "ym/BnNodeList.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

void
write_fanout(
  ostream& s,
  const BnNode& node
)
{
  s << "  fanout:";
  for ( auto onode: node.fanout_list() ) {
    s << " " << onode.id();
  }
  s << endl;
}

END_NONAMESPACE

// @brief 内容を出力する．
void
BnNetwork::write(
  ostream& s
) const
{
  s << "network name : " << name() << endl
    << endl;

  for ( auto port: port_list() ) {
    s << "port#" << port.id() << ": ";
    s << "(" << port.name() << ") : ";
    for ( auto i: Range(port.bit_width()) ) {
      s << " " << port.bit(i).id();
    }
    s << endl;
  }
  s << endl;

  for ( auto node: input_list() ) {
    ASSERT_COND( node.type() == BnNodeType::Input );
    s << "input: " << node.id()
      << "(" << node.name() << ")" << endl;
    write_fanout(s, node);
  }
  s << endl;

  for ( auto node: output_list() ) {
    s << "output: " << node.id()
      << "(" << node.name() << ")" << endl
      << "    input: " << node.output_src().id() << endl;
    write_fanout(s, node);
  }
  s << endl;

  for ( auto dff: dff_list() ) {
    s << "dff#" << dff.id()
      << "(" << dff.name() << ")";
    if ( dff.is_dff() || dff.is_latch() ) {
      if ( dff.is_dff() ) {
	s << "[DFF]";
      }
      else {
	s << "[LATCH]";
      }
      s << endl
	<< "    input:  " << dff.data_in().id() << endl
	<< "    output: " << dff.data_out().id() << endl
	<< "    clock:  " << dff.clock().id() << endl;
      if ( dff.clear().is_valid() ) {
	s << "    clear:  " << dff.clear().id() << endl;
      }
      if ( dff.preset().is_valid() ) {
	s << "    preset: " << dff.preset().id() << endl;
      }
    }
    else {
      s << "[CELL]" << endl;
      SizeType ni = dff.cell_input_num();
      for ( auto i: Range(ni) ) {
	s << "    input#" << i << ":  " << dff.cell_input(i).id() << endl;
      }
      SizeType no = dff.cell_output_num();
      for ( auto i: Range(no) ) {
	s << "    output#" << i << ": " << dff.cell_output(i).id() << endl;
      }
    }
    s << endl;
  }
  s << endl;

  for ( auto node: logic_list() ) {
    s << "logic: " << node.id()
      << "(" << node.name() << ")" << endl
      << "    fanins: ";
    for ( auto fanin: node.fanin_list() ) {
      s << " " << fanin.id();
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
	<< expr(node.expr_id());
      break;
    case BnNodeType::TvFunc:
      s << "func#" << node.func_id() << ": "
	<< func(node.func_id());
      break;
    case BnNodeType::Bdd:
      s << "BDD" << endl;
      node.bdd().display(s);
      break;
    default:
      ASSERT_NOT_REACHED;
    }
    s << endl;
    write_fanout(s, node);
  }

  s << endl;
}

END_NAMESPACE_YM_BNET
