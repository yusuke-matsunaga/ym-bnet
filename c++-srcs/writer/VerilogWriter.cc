
/// @file VerilogWriter.cc
/// @brief VerilogWriter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "VerilogWriter.h"
#include "ym/BnNetwork.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/ClibPin.h"
#include "ym/NameMgr.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief 内容を Verilog-HDL 形式で出力する．
void
BnNetwork::write_verilog(
  ostream& s,
  const string& port_prefix,
  const string& port_suffix,
  const string& node_prefix,
  const string& node_suffix,
  const string& instance_prefix,
  const string& instance_suffix
) const
{
  if ( !is_concrete() ) {
    cerr << "Cannot convert to Verilog-HDL" << endl;
    return;
  }

  VerilogWriter writer(*this,
		       port_prefix, port_suffix,
		       node_prefix, node_suffix,
		       instance_prefix, instance_suffix);
  writer(s);
}

// @brief 内容を Verilog-HDL 形式で出力する．
void
BnNetwork::write_verilog(
  const string& filename,
  const string& port_prefix,
  const string& port_suffix,
  const string& node_prefix,
  const string& node_suffix,
  const string& instance_prefix,
  const string& instance_suffix
) const
{
  ofstream ofs(filename);
  if ( ofs ) {
    write_verilog(ofs);
  }
}


//////////////////////////////////////////////////////////////////////
// クラス VerilogWriter
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE

// 関数番号から UDP名を作る．
inline
string
udp_name(
  SizeType func_id
)
{
  ostringstream buf;
  buf << "__func" << func_id;
  return buf.str();
}

// UDP プリミティブの定義を出力する．
void
write_udp(
  ostream& s,
  const string& udp_name,
  const TvFunc& func
)
{
  SizeType ni = func.input_num();
  SizeType np = 1 << ni;

  s << "  primitive " << udp_name << "(";
  const char* comma = "";
  for ( SizeType i: Range(ni) ) {
    s << comma << "i" << i;
    comma = ", ";
  }
  s << ", o);" << endl;
  for ( SizeType i: Range(ni) ) {
    s << "    input i" << i << ";" << endl;
  }
  s << "    output o;";
  s << "    table" << endl;
  for ( SizeType p: Range(np) ) {
    s << "      ";
    for ( SizeType i: Range(ni) ) {
      if ( (p & (1 << i)) == 0 ) {
	s << "0";
      }
      else {
	s << "1";
      }
    }
    s << " : ";
    if ( func.value(p) ) {
      s << "1";
    }
    else {
      s << "0";
    }
    s << ";" << endl;
  }
  s << "    endtable" << endl;
  s << "  endprimitive" << endl;
}

void
write_op(
  ostream& s,
  const char* op_str,
  bool neg,
  const vector<string>& iname_array
)
{
  if ( neg ) {
    s << "~(";
  }
  const char* tmp_str = "";
  for ( const string& name: iname_array ) {
    s << tmp_str;
    tmp_str = op_str;
    s << name;
  }
  if ( neg ) {
    s << ")";
  }
}

void
write_primitive(
  ostream& s,
  PrimType type,
  const vector<string>& iname_array
)
{
  switch ( type ) {
  case PrimType::C0:
    s << "1'b0";
    break;
  case PrimType::C1:
    s << "1'b1";
    break;
  case PrimType::Buff:
    s << iname_array[0];
    break;
  case PrimType::Not:
    s << "~" << iname_array[0];
    break;
  case PrimType::And:
    write_op(s, " & ", false, iname_array);
    break;
  case PrimType::Nand:
    write_op(s, " & ", true, iname_array);
    break;
  case PrimType::Or:
    write_op(s, " | ", false, iname_array);
    break;
  case PrimType::Nor:
    write_op(s, " | ", true, iname_array);
    break;
  case PrimType::Xor:
    write_op(s, " ^ ", false, iname_array);
    break;
  case PrimType::Xnor:
    write_op(s, " ^ ", true, iname_array);
    break;
  case PrimType::None:
    ASSERT_NOT_REACHED;
    break;
  }
}

void
write_expr(
  ostream& s,
  const Expr& expr,
  const vector<string>& iname_array
)
{
  if ( expr.is_zero() ) {
    s << "1'b0";
  }
  else if ( expr.is_one() ) {
    s << "1'b1";
  }
  else if ( expr.is_posi_literal() ) {
    auto varid = expr.varid();
    s << iname_array[varid];
  }
  else if ( expr.is_nega_literal() ) {
    auto varid = expr.varid();
    s << "~" << iname_array[varid];
  }
  else {
    const char* op_str = "";
    if ( expr.is_and() ) {
      op_str = " & ";
    }
    else if ( expr.is_or() ) {
      op_str = " | ";
    }
    else if ( expr.is_xor() ) {
      op_str = " ^ ";
    }
    else {
      ASSERT_NOT_REACHED;
    }
    const char* tmp_str = "";
    for ( auto& opr: expr.operand_list() ) {
      s << tmp_str;
      tmp_str = op_str;
      s << "(";
      write_expr(s, opr, iname_array);
      s << ")";
    }
  }
}

END_NONAMESPACE


// @brief コンストラクタ
VerilogWriter::VerilogWriter(
  const BnNetwork& network,
  const string& port_prefix,
  const string& port_suffix,
  const string& node_prefix,
  const string& node_suffix,
  const string& instance_prefix,
  const string& instance_suffix
) : mNetwork{network},
    mPortNameArray(network.port_num()),
    mNodeNameArray(network.node_num()),
    mNodeInstanceNameArray(network.node_num()),
    mDffInstanceNameArray(network.dff_num())
{
  if ( port_prefix == string{} ) {
    mPortPrefix = "__port";
  }
  else {
    mPortPrefix = port_prefix;
  }
  if ( port_suffix == string{} ) {
    mPortSuffix = "";
  }
  else {
    mPortSuffix = port_suffix;
  }
  if ( node_prefix == string{} ) {
    mNodePrefix = "__wire";
  }
  else {
    mNodePrefix = node_prefix;
  }
  if ( node_suffix == string{} ) {
    mNodeSuffix = "";
  }
  else {
    mNodeSuffix = node_suffix;
  }
  if ( instance_prefix == string{} ) {
    mInstancePrefix = "__U";
  }
  else {
    mInstancePrefix = instance_prefix;
  }
  if ( instance_suffix == string{} ) {
    mInstanceSuffix = "";
  }
  else {
    mInstanceSuffix = instance_suffix;
  }
}

// @brief blif 形式で出力する．
void
VerilogWriter::operator()(
  ostream& s
)
{
  // TODO
  // * ラッチ記述のチェック
  // * セル，UDP のチェック

  init_name_array();

  s << "module " << mNetwork.name() << "(";
  const char* comma = "";
  for ( auto port: mNetwork.port_list() ) {
    const string& port_name = mPortNameArray[port.id()];
    SizeType nb = port.bit_width();
    s << comma;
    comma = ", ";
    if ( nb == 1 ) {
      auto id = port.bit(0).id();
      if ( port_name == mNodeNameArray[id] ) {
	s << port_name;
      }
      else {
	s << "." << port_name << "(" << mNodeNameArray[id] << ")";
      }
    }
    else {
      s << "." << port_name << "(";
      const char* bit_comma = "";
      for ( SizeType i: Range(nb) ) {
	auto id = port.bit(i).id();
	s << bit_comma << mNodeNameArray[id];
	bit_comma = ", ";
      }
      s << ")";
    }
  }
  s << ");" << endl;

  // 外部入力
  for ( auto node: mNetwork.primary_input_list() ) {
    auto id = node.id();
    s << "  " << "input  " << mNodeNameArray[id] << ";" << endl;
  }
  // 外部出力
  for ( auto node: mNetwork.primary_output_list() ) {
    auto onode = node.output_src();
    s << "  " << "output " << mNodeNameArray[node.id()] << ";" << endl;
  }
  s << endl;

  // このネットワークで使用されている TvFunc を UDP として定義しておく．
  for ( SizeType i: Range(mNetwork.func_num()) ) {
    auto& func = mNetwork.func(i);
    write_udp(s, udp_name(i), func);
  }

  // DFFの宣言
  for ( auto dff: mNetwork.dff_list() ) {
    if ( dff.is_dff() || dff.is_latch() ) {
      s << "  reg    "
	<< mNodeNameArray[dff.data_out().id()] << ";" << endl;
    }
    else if ( dff.is_cell() ) {
      SizeType no = dff.cell_output_num();
      for ( SizeType i: Range(no) ) {
	s << "  wire   "
	  << mNodeNameArray[dff.cell_output(i).id()] << ";" << endl;
      }
    }
    else {
      ASSERT_NOT_REACHED;
    }
  }

  // 論理ノードの宣言
  for ( auto node: mNetwork.logic_list() ) {
    s << "  " << "wire   " << mNodeNameArray[node.id()] << ";" << endl;
  }

  s << endl;

  // DFFの記述
  for ( auto dff: mNetwork.dff_list() ) {
    if ( dff.is_dff() ) {
      const string& dff_output = mNodeNameArray[dff.data_out().id()];
      const string& dff_input = mNodeNameArray[dff.data_in().id()];
      s << "  " << "always @ ( posedge " << mNodeNameArray[dff.clock().id()];
      auto clear = dff.clear();
      if ( clear.is_valid() ) {
	s << " or posedge " << mNodeNameArray[clear.id()];
      }
      auto preset = dff.preset();
      if ( preset.is_valid() ) {
	s << " or posedge " << mNodeNameArray[preset.id()];
      }
      s << " )" << endl;
      const char* if_str = "if";
      if ( clear.is_valid() ) {
	s << "    " << if_str << " ( " << mNodeNameArray[clear.id()] << ")" << endl
	  << "      " << dff_output << " <= 1'b0;" << endl;
	if_str = "else if";
      }
      if ( preset.is_valid() ) {
	s << "    " << if_str << " ( " << mNodeNameArray[preset.id()] << ")" << endl
	  << "      " << dff_output << " <= 1'b1;" << endl;
	if_str = "else if";
      }
      if ( clear.is_valid() || preset.is_valid() ) {
	s << "    else" << endl
	  << "      " << dff_output << " <= " << dff_input << ";" << endl;
      }
      else {
	s << "    " << dff_output << " <= " << dff_input << ";" << endl;
      }
    }
    else if ( dff.is_latch() ) {
      const string& latch_output = mNodeNameArray[dff.data_out().id()];
      const string& latch_input = mNodeNameArray[dff.data_in().id()];
      s << "  " << "always @ ( " << mNodeNameArray[dff.clock().id()];
      auto clear = dff.clear();
      if ( clear.is_valid() ) {
	s << " or " << mNodeNameArray[clear.id()];
      }
      auto preset = dff.preset();
      if ( preset.is_valid() ) {
	s << " or " << mNodeNameArray[preset.id()];
      }
      s << " )" << endl;
      const char* if_str = "if";
      if ( clear.is_valid() ) {
	s << "    " << if_str << " ( " << mNodeNameArray[clear.id()] << ")" << endl
	  << "      " << latch_output << " = 1'b0;" << endl;
	if_str = "else if";
      }
      if ( preset.is_valid() ) {
	s << "    " << if_str << " ( " << mNodeNameArray[preset.id()] << ")" << endl
	  << "      " << latch_output << " = 1'b1;" << endl;
	if_str = "else if";
      }
      if ( clear.is_valid() || preset.is_valid() ) {
	s << "    else" << endl
	  << "      " << latch_output << " = " << latch_input << ";" << endl;
      }
      else {
	s << "    " << latch_output << " = " << latch_input << ";" << endl;
      }
    }
    else if ( dff.is_cell() ) {
      // セルインスタンス記述
      auto cell = dff.cell();
      s << ";" << endl;
      s << "  " << cell.name()
	<< " " << mDffInstanceNameArray[dff.id()] << "(";
      SizeType ni = cell.input_num();
      for ( SizeType i: Range(ni) ) {
	s << ", ." << cell.input(i).name() << "("
	  << mNodeNameArray[dff.cell_input(i).id()] << ")";
      }
      SizeType no = cell.output_num();
      for ( SizeType i: Range(no) ) {
	s << "." << cell.output(i).name() << "("
	  << mNodeNameArray[dff.cell_output(i).id()] << ")";
      }
      s << ");" << endl;
    }
  }

  // 論理ノードの記述
  for ( auto node: mNetwork.logic_list() ) {
    auto id = node.id();
    SizeType ni = node.fanin_num();
    vector<string> iname_array(ni);
    for ( SizeType i: Range(ni) ) {
      iname_array[i] = mNodeNameArray[node.fanin_id(i)];
    }
    if ( node.type() == BnNodeType::TvFunc ) {
      // 予め mNetwork に登録されている TvFunc に対応する UDP を定義しておいて
      // ここはそのインスタンス化だけを行う．
      s << "  " << udp_name(node.func_id()) << "(";
      for ( int i: Range(ni) ) {
	s << ".i" << i << "(" << iname_array[i] << "), ";
      }
      s << ".o(" << mNodeNameArray[id] << ")";
      s << ");" << endl;
    }
    else if ( node.type() == BnNodeType::Cell ) {
      // セルインスタンス記述
      auto cell = node.cell();
      s << ";" << endl;
      s << "  " << cell.name()
	<< " " << mNodeInstanceNameArray[id] << "(";
      s << "." << cell.output(0).name() << "(" << mNodeNameArray[id] << ")";
      for ( int i: Range(ni) ) {
	s << ", ." << cell.input(i).name() << "(" << iname_array[i] << ")";
      }
      s << ");" << endl;
    }
    else {
      // assign 文で内容を記述する．
      s << "  " << "assign " << mNodeNameArray[id] << " = ";
      switch ( node.type() ) {
      case BnNodeType::Prim:
	write_primitive(s, node.primitive_type(), iname_array);
	break;
      case BnNodeType::Expr:
	write_expr(s, mNetwork.expr(node.expr_id()), iname_array);
	break;
      default:
	ASSERT_NOT_REACHED;
      }
      s << ";" << endl;
    }
  }

  s << "endmodule" << endl;
}

// @brief 名前の配列を初期化する．
void
VerilogWriter::init_name_array()
{
  // ポート名を管理するクラス
  NameMgr port_name_mgr(mPortPrefix, mPortSuffix);

  // ポート名のハッシュ
  unordered_set<string> port_name_hash;

  // もともとポート名があればそれを使う．
  // ただし，重複していたら後ろのポート名を無効化する．
  // ポート名を持たない(もしくは無効化された)場合は自動生成名を用いる．
  for ( auto port: mNetwork.port_list() ) {
    reg_port_name(port, port_name_hash, port_name_mgr);
  }

  // 名無しのポートに名前を与える．
  for ( auto port: mNetwork.port_list() ) {
    auto id = port.id();
    if ( mPortNameArray[id] == string() ) {
      string name = port_name_mgr.new_name(true);
      mPortNameArray[id] = name;
    }
  }

  // ノード名を管理するクラス
  NameMgr node_name_mgr(mNodePrefix, mNodeSuffix);

  // インスタンス名のハッシュ
  unordered_set<string> name_hash;

  // 外部入力ノードの名前を登録する．
  for ( auto node: mNetwork.primary_input_list() ) {
    reg_node_name(node, name_hash, node_name_mgr);
  }

  // FFの出力を登録する．
  for ( auto dff: mNetwork.dff_list() ) {
    if ( dff.is_dff() || dff.is_latch() ) {
      reg_node_name(dff.data_out(), name_hash, node_name_mgr);
    }
  }

  // 論理ノードを登録する．
  for ( auto node: mNetwork.logic_list() ) {
    reg_node_name(node, name_hash, node_name_mgr);
  }

  // 名無しのノードに名前を与える．
  for ( auto id: Range(mNetwork.node_num()) ) {
    if ( mNodeNameArray[id] == string() ) {
      string name = node_name_mgr.new_name(true);
      mNodeNameArray[id] = name;
    }
  }

  // インスタンス名を管理するクラス
  NameMgr instance_name_mgr(mInstancePrefix, mInstanceSuffix);

  // ノード名を instance_name_mgr に登録しておく．
  for ( auto id: Range(mNetwork.node_num()) ) {
    string name = mNodeNameArray[id];
    instance_name_mgr.add(name);
  }

  // ノード用のインスタンス名を登録する．
  for ( auto node: mNetwork.logic_list() ) {
    if ( node.type() == BnNodeType::Cell ) {
      string name = instance_name_mgr.new_name(true);
      mNodeInstanceNameArray[node.id()] = name;
    }
  }

  // DFF用のインスタンス名を登録する．
  for ( auto dff: mNetwork.dff_list() ) {
    if ( dff.is_cell() ) {
      string name = instance_name_mgr.new_name(true);
      mDffInstanceNameArray[dff.id()] = name;
    }
  }

  // 外部出力ノードの名前をそのファンインの名前に付け替える．
  for ( auto node: mNetwork.primary_output_list() ) {
    replace_node_name(node);
  }

  // FFの入力ノードの名前をそのファンインの名前に付け替える．
  for ( auto dff: mNetwork.dff_list() ) {
    if ( dff.is_dff() || dff.is_latch() ) {
      replace_node_name(dff.data_in());
      replace_node_name(dff.clock());
      replace_node_name(dff.clear());
      replace_node_name(dff.preset());
    }
  }
}

// @brief ノード名をそのファンインのノード名に付け替える．
void
VerilogWriter::replace_node_name(
  BnNode node
)
{
  if ( node.is_invalid() ) {
    return;
  }
  ASSERT_COND( node.is_output() );
  auto src_node = node.output_src();
  mNodeNameArray[node.id()] = mNodeNameArray[src_node.id()];
}

BEGIN_NONAMESPACE

// 名前を補正する．
// 具体的には [a-zA-Z0-9_] 以外の文字を含んでいる場合にエスケープ識別子にする．
string
coerce_name(
  const string& name
)
{
  bool need_escape = false;
  for ( char c: name ) {
    if ( !isalnum(c) && c != '_' ) {
      need_escape = true;
      break;
    }
  }
  if ( need_escape ) {
    return "\\" + name + " ";
  }
  else {
    return name;
  }
}

END_NONAMESPACE

// @brief ポート名の登録を行う．
void
VerilogWriter::reg_port_name(
  BnPort port,
  unordered_set<string>& name_hash,
  NameMgr& name_mgr
)
{
  string name = port.name();
  if ( name == string() ) {
    // ポート名がなかった．
    return;
  }
  name = coerce_name(name);
  if ( name_hash.count(name) > 0 ) {
    // 名前が重複していた．
    return;
  }

  // 名前を登録する．
  name_mgr.add(name);
  name_hash.insert(name);
  mPortNameArray[port.id()] = name;
}

// @brief ノード名の登録を行う．
void
VerilogWriter::reg_node_name(
  BnNode node,
  unordered_set<string>& name_hash,
  NameMgr& name_mgr
)
{
  string name = node.name();
  if ( name == string() ) {
    // 名前がなかった．
    return;
  }
  name = coerce_name(name);
  if ( name_hash.count(name) > 0 ) {
    // 名前が重複していた．
    return;
  }

  // 名前を登録する．
  name_mgr.add(name);
  name_hash.insert(name);
  mNodeNameArray[node.id()] = name;
}

END_NAMESPACE_YM_BNET
