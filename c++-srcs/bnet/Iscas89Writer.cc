
/// @file Iscas89Writer.cc
/// @brief Iscas89Writer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "Iscas89Writer.h"
#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/BnDff.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief 内容を ISCAS89(.bench) 形式で出力する．
void
BnNetwork::write_iscas89(
  const string& filename,
  const string& prefix,
  const string& suffix
) const
{
  ofstream ofs(filename);
  if ( ofs ) {
    write_iscas89(ofs, prefix, suffix);
  }
}

// @brief 内容を ISCAS89(.bench) 形式で出力する．
void
BnNetwork::write_iscas89(
  ostream& s,
  const string& prefix,
  const string& suffix
) const
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
    // iscas89 フォーマットに合うように変形する
    BnNetwork network(*this);
    network.simple_decomp();
    Iscas89Writer writer(network, prefix, suffix);
    writer(s);
    return;
  }
  else {
    Iscas89Writer writer(*this, prefix, suffix);
    writer(s);
  }
}


//////////////////////////////////////////////////////////////////////
// クラス Iscas89Writer
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Iscas89Writer::Iscas89Writer(
  const BnNetwork& network,
  const string& prefix,
  const string& suffix
) : WriterBase{network}
{
  string _prefix{prefix};
  if ( _prefix == string{} ) {
    _prefix = "__node";
  }
  string _suffix{suffix};
  if ( _suffix == string{} ) {
    _suffix = "";
  }
  init_name_array(_prefix, _suffix);
}

// @brief blif 形式で出力する．
void
Iscas89Writer::operator()(
  ostream& s
)
{
  // INPUT 文の出力
  int count = 0;
  for ( int id: network().primary_input_id_list() ) {
    if ( is_data(id) ) {
      s << "INPUT(" << node_name(id) << ")" << endl;
    }
  }
  s << endl;

  // OUTPUT 文の出力
  for ( int id: network().primary_output_src_id_list() ) {
    s << "OUTPUT(" << node_name(id) << ")" << endl;
  }
  s << endl;

  // DFF 文の出力
  for ( auto id: Range(network().dff_num()) ) {
    auto& dff = network().dff(id);
    s << node_name(dff.output()) << " = DFF(" << node_name(dff.input()) << ")" << endl;
  }
  s << endl;

  // 出力用の追加の BUFF 文
  for ( int id: network().primary_output_id_list() ) {
    auto& node = network().node(id);
    int src_id = node.fanin_id(0);
    string src_name = node_name(src_id);
    string name = node_name(id);
    if ( name != src_name ) {
      s << name << " = BUFF(" << src_name << ")" << endl;
    }
  }

  // ゲート文の出力
  for ( auto id: network().logic_id_list() ) {
    if ( !is_data(id) ) {
      continue;
    }
    auto& node = network().node(id);
    s << node_name(id) << " = ";
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
	s << comma << node_name(iid);
	comma = ", ";
      }
      s << ")";
    }
    s << endl;
  }
}

END_NAMESPACE_YM_BNET
