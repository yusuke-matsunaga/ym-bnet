
/// @file write_iscas89.cc
/// @brief write_iscas89 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/BnDff.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/Range.h"
#include <fstream>


BEGIN_NAMESPACE_YM_BNET

// @brief 内容を ISCAS89(.bench) 形式で出力する．
// @param[in] filename 出力先のファイル名
// @param[in] network ネットワーク
//
// ポートの情報は無視される．
void
BnNetwork::write_iscas89(const string& filename) const
{
  ofstream ofs(filename);
  if ( ofs ) {
    write_iscas89(ofs);
  }
}

// @brief 内容を ISCAS89(.bench) 形式で出力する．
// @param[in] s 出力先のストリーム
// @param[in] network ネットワーク
//
// ポートの情報は無視される．
void
BnNetwork::write_iscas89(ostream& s) const
{
  // 個々のノードが単純なゲートか調べる．
  bool ng = false;
  for ( auto id: logic_id_list() ) {
    auto& node = this->node(id);
    switch ( node.type() ) {
    case BnNodeType::C0:
    case BnNodeType::C1:
    case BnNodeType::Buff:
    case BnNodeType::Not:
    case BnNodeType::And:
    case BnNodeType::Nand:
    case BnNodeType::Or:
    case BnNodeType::Nor:
    case BnNodeType::Xor:
    case BnNodeType::Xnor:
      break;
    default:
      // 上記以外は .bench では受け付けない．
      ng = true;
      break;
    }
  }
  if ( ng ) {
    BnNetwork network(*this);
    network.simple_decomp();
    network.write_iscas89(s);
    return;
  }

  // INPUT 文の出力
  int count = 0;
  for ( auto id: input_id_list() ) {
    s << "INPUT(" << node(id).name() << ")" << endl;
  }
  s << endl;

  // OUTPUT 文の出力
  for ( auto id: output_id_list() ) {
    s << "OUTPUT(" << node(id).name() << ")" << endl;
  }
  s << endl;

  // DFF 文の出力
  for ( auto id: Range(dff_num()) ) {
    auto& dff = this->dff(id);
    auto& dff_out = node(dff.output());
    auto& dff_in = node(dff.input());
    s << dff_out.name() << " = DFF(" << dff_in.name() << ")" << endl;
  }
  s << endl;

  // ゲート文の出力
  for ( auto id: logic_id_list() ) {
    auto& node = this->node(id);
    s << node.name() << " = ";
    switch ( node.type() ) {
    case BnNodeType::C0:   s << "CONST0"; break;
    case BnNodeType::C1:   s << "CONST1"; break;
    case BnNodeType::Buff: s << "BUFF"; break;
    case BnNodeType::Not:  s << "NOT"; break;
    case BnNodeType::And:  s << "AND"; break;
    case BnNodeType::Nand: s << "NAND"; break;
    case BnNodeType::Or:   s << "OR"; break;
    case BnNodeType::Nor:  s << "NOR"; break;
    case BnNodeType::Xor:  s << "XOR"; break;
    case BnNodeType::Xnor: s << "XNOR"; break;
    default: ASSERT_NOT_REACHED; break;
    }
    if ( node.fanin_num() > 0 ) {
      s << "(";
      const char* comma = "";
      for ( auto iid: node.fanin_id_list() ) {
	s << comma << this->node(iid).name();
	comma = ", ";
	  }
      s << ")" << endl;
    }
  }
}

END_NAMESPACE_YM_BNET
