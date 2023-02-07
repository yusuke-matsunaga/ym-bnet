
/// @file Iscas89Writer.cc
/// @brief Iscas89Writer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "Iscas89Writer.h"
#include "ym/BnNetwork.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"


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
  ofstream ofs{filename};
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
  // Latchタイプ，CellタイプのDFFノードを持つとき変換不能
  for ( auto dff: dff_list() ) {
    if ( dff.type() != BnDffType::Dff ) {
      cerr << "Cannot convert to iscas89(.bench)" << endl;
      return;
    }
  }

  // TvFuncタイプ，Bddタイプ，Cellタイプの論理ノードを持つ時変換不能
  for ( auto node: logic_list() ) {
    if ( node.type() == BnNodeType::TvFunc ||
	 node.type() == BnNodeType::Bdd ||
	 node.type() == BnNodeType::Cell ) {
      cerr << "Cannot convert to iscas89(.bench)" << endl;
      return;
    }
  }

  // 個々のノードが単純なゲートか調べる．
  bool need_decomp = false;
  for ( auto node: logic_list() ) {
    if ( node.type() != BnNodeType::Prim ) {
      need_decomp = true;
      break;
    }
  }
  if ( need_decomp ) {
    // iscas89 フォーマットに合うように変形する
    auto network = simple_decomp();
    Iscas89Writer writer{network, prefix, suffix};
    writer(s);
    return;
  }
  else {
    Iscas89Writer writer{*this, prefix, suffix};
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
  for ( auto node: network().primary_input_list() ) {
    if ( is_data(node) ) {
      s << "INPUT(" << node_name(node) << ")" << endl;
    }
  }
  s << endl;

  // OUTPUT 文の出力
  for ( auto node: network().primary_output_list() ) {
    auto src_node = node.output_src();
    s << "OUTPUT(" << node_name(src_node) << ")" << endl;
  }
  s << endl;

  // DFF 文の出力
  for ( auto dff: network().dff_list() ) {
    s << node_name(dff.data_out())
      << " = DFF(" << node_name(dff.data_in()) << ")" << endl;
  }
  s << endl;

  // 出力用の追加の BUFF 文
  for ( auto node: network().primary_output_list() ) {
    string name = node_name(node);
    auto src_node = node.output_src();
    string src_name = node_name(src_node);
    if ( name != src_name ) {
      s << name << " = BUFF(" << src_name << ")" << endl;
    }
  }

  // ゲート文の出力
  for ( auto node: network().logic_list() ) {
    if ( !is_data(node) ) {
      continue;
    }
    ASSERT_COND( node.type() == BnNodeType::Prim );
    s << node_name(node) << " = ";
    switch ( node.primitive_type() ) {
    case PrimType::C0:   s << "CONST0"; break;
    case PrimType::C1:   s << "CONST1"; break;
    case PrimType::Buff: s << "BUFF"; break;
    case PrimType::Not:  s << "NOT"; break;
    case PrimType::And:  s << "AND"; break;
    case PrimType::Nand: s << "NAND"; break;
    case PrimType::Or:   s << "OR"; break;
    case PrimType::Nor:  s << "NOR"; break;
    case PrimType::Xor:  s << "XOR"; break;
    case PrimType::Xnor: s << "XNOR"; break;
    default: ASSERT_NOT_REACHED; break;
    }
    if ( node.fanin_num() > 0 ) {
      s << "(";
      const char* comma = "";
      for ( auto inode: node.fanin_list() ) {
	s << comma << node_name(inode);
	comma = ", ";
      }
      s << ")";
    }
    s << endl;
  }
}

END_NAMESPACE_YM_BNET
