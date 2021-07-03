
/// @file VerilogWriter.cc
/// @brief VerilogWriter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "VerilogWriter.h"
#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/BnDff.h"
#include "ym/BnLatch.h"
#include "ym/BnPort.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/ClibCellPin.h"
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
string
udp_name(
  int func_id
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
  int ni = func.input_num();
  int np = 1 << ni;

  s << "  primitive " << udp_name << "(";
  const char* comma = "";
  for ( int i: Range(ni) ) {
    s << comma << "i" << i;
    comma = ", ";
  }
  s << ", o);" << endl;
  for ( int i: Range(ni) ) {
    s << "    input i" << i << ";" << endl;
  }
  s << "    output o;";
  s << "    table" << endl;
  for ( int p: Range(np) ) {
    s << "      ";
    for ( int i: Range(ni) ) {
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
    VarId varid = expr.varid();
    s << iname_array[varid.val()];
  }
  else if ( expr.is_nega_literal() ) {
    VarId varid = expr.varid();
    s << "~" << iname_array[varid.val()];
  }
  else {
    int nc = expr.child_num();
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
    for ( int i: Range(nc) ) {
      s << tmp_str;
      tmp_str = op_str;
      s << "(";
      write_expr(s, expr.child(i), iname_array);
      s << ")";
    }
  }
}

END_NONAMESPACE

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
  for ( int port_id: Range(mNetwork.port_num()) ) {
    auto& port = mNetwork.port(port_id);
    const string& port_name = mPortNameArray[port_id];
    int nb = port.bit_width();
    s << comma;
    comma = ", ";
    if ( nb == 1 ) {
      int id = port.bit(0);
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
      for ( int i: Range(nb) ) {
	int id = port.bit(i);
	s << bit_comma << mNodeNameArray[id];
	bit_comma = ", ";
      }
      s << ")";
    }
  }
  s << ");" << endl;

  // 外部入力
  for ( int id: mNetwork.primary_input_id_list() ) {
    s << "  " << "input  " << mNodeNameArray[id] << ";" << endl;
  }
  // 外部出力
  for ( int id: mNetwork.primary_output_src_id_list() ) {
    s << "  " << "output " << mNodeNameArray[id] << ";" << endl;
  }
  s << endl;

  // このネットワークで使用されている TvFunc を UDP として定義しておく．
  for ( int i: Range(mNetwork.func_num()) ) {
    auto& func = mNetwork.func(i);
    write_udp(s, udp_name(i), func);
  }

  // DFFの宣言
  for ( int i: Range(mNetwork.dff_num()) ) {
    auto& dff = mNetwork.dff(i);
    s << "  ";
    if ( dff.cell_id() == -1 ) {
      s << "reg    ";
    }
    else {
      s << "wire   ";
    }
    s << mNodeNameArray[dff.output()] << ";" << endl;
  }

  // ラッチの宣言
  for ( int i: Range(mNetwork.latch_num()) ) {
    auto& latch = mNetwork.latch(i);
    s << "  ";
    if ( latch.cell_id() == -1 ) {
      s << "reg   ";
    }
    else {
      s << "wire  ";
    }
    s << mNodeNameArray[latch.output()] << ";" << endl;
  }

  // 論理ノードの宣言
  for ( int id: mNetwork.logic_id_list() ) {
    s << "  " << "wire   " << mNodeNameArray[id] << ";" << endl;
  }

  s << endl;

  // DFFの記述
  for ( int i: Range(mNetwork.dff_num()) ) {
    auto& dff = mNetwork.dff(i);
    const string& dff_output = mNodeNameArray[dff.output()];
    const string& dff_input = mNodeNameArray[dff.input()];
    int cell_id = dff.cell_id();
    if ( cell_id == -1 ) {
      s << "  " << "always @ ( posedge " << mNodeNameArray[dff.clock()];
      int clear = dff.clear();
      if ( clear != -1 ) {
	s << " or posedge " << mNodeNameArray[clear];
      }
      int preset = dff.preset();
      if ( preset != -1 ) {
	s << " or posedge " << mNodeNameArray[preset];
      }
      s << " )" << endl;
      const char* if_str = "if";
      if ( clear != -1 ) {
	s << "    " << if_str << " ( " << mNodeNameArray[clear] << ")" << endl
	  << "      " << dff_output << " <= 1'b0;" << endl;
	if_str = "else if";
      }
      if ( preset != -1 ) {
	s << "    " << if_str << " ( " << mNodeNameArray[preset] << ")" << endl
	  << "      " << dff_output << " <= 1'b1;" << endl;
	if_str = "else if";
      }
      if ( clear != -1 || preset != -1 ) {
	s << "    else" << endl
	  << "      " << dff_output << " <= " << dff_input << ";" << endl;
      }
      else {
	s << "    " << dff_output << " <= " << dff_input << ";" << endl;
      }
    }
    else {
      // 未完
    }
  }

  // ラッチの記述
  for ( int i: Range(mNetwork.latch_num()) ) {
    auto& latch = mNetwork.latch(i);
    const string& latch_output = mNodeNameArray[latch.output()];
    const string& latch_input = mNodeNameArray[latch.input()];
    int cell_id = latch.cell_id();
    if ( cell_id == -1 ) {
      s << "  " << "always @ ( " << mNodeNameArray[latch.enable()];
      int clear = latch.clear();
      if ( clear != -1 ) {
	s << " or " << mNodeNameArray[clear];
      }
      int preset = latch.preset();
      if ( preset != -1 ) {
	s << " or " << mNodeNameArray[preset];
      }
      s << " )" << endl;
      const char* if_str = "if";
      if ( clear != -1 ) {
	s << "    " << if_str << " ( " << mNodeNameArray[clear] << ")" << endl
	  << "      " << latch_output << " = 1'b0;" << endl;
	if_str = "else if";
      }
      if ( preset != -1 ) {
	s << "    " << if_str << " ( " << mNodeNameArray[preset] << ")" << endl
	  << "      " << latch_output << " = 1'b1;" << endl;
	if_str = "else if";
      }
      if ( clear != -1 || preset != -1 ) {
	s << "    else" << endl
	  << "      " << latch_output << " = " << latch_input << ";" << endl;
      }
      else {
	s << "    " << latch_output << " = " << latch_input << ";" << endl;
      }
    }
    else {
      // 未完
    }
  }

  // 論理ノードの記述
  for ( int id: mNetwork.logic_id_list() ) {
    auto& node = mNetwork.node(id);
    int ni = node.fanin_num();
    vector<string> iname_array(ni);
    for ( int i: Range(ni) ) {
      iname_array[i] = mNodeNameArray[node.fanin_id(i)];
    }
    int cell_id = node.cell_id();
    if ( cell_id == -1 ) {
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
      else {
	// assign 文で内容を記述する．
	s << "  " << "assign " << mNodeNameArray[id] << " = ";
	switch ( node.type() ) {
	case BnNodeType::C0:
	  s << "1'b0";
	  break;
	case BnNodeType::C1:
	  s << "1'b1";
	  break;
	case BnNodeType::Buff:
	  s << iname_array[0];
	  break;
	case BnNodeType::Not:
	  s << "~" << iname_array[0];
	  break;
	case BnNodeType::And:
	  write_op(s, " & ", false, iname_array);
	  break;
	case BnNodeType::Nand:
	  write_op(s, " & ", true, iname_array);
	  break;
	case BnNodeType::Or:
	  write_op(s, " | ", false, iname_array);
	  break;
	case BnNodeType::Nor:
	  write_op(s, " | ", true, iname_array);
	  break;
	case BnNodeType::Xor:
	  write_op(s, " ^ ", false, iname_array);
	  break;
	case BnNodeType::Xnor:
	  write_op(s, " ^ ", true, iname_array);
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
    else {
      // セルインスタンス記述
      auto& cell = mNetwork.library().cell(cell_id);
      s << ";" << endl;
      s << "  " << cell.name()
	<< " " << mNodeInstanceNameArray[id] << "(";
      s << "." << cell.output(0).name() << "(" << mNodeNameArray[id] << ")";
      for ( int i: Range(ni) ) {
	s << ", ." << cell.input(i).name() << "(" << iname_array[i] << ")";
      }
      s << ");" << endl;
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
  for ( int id: Range(mNetwork.port_num()) ) {
    reg_port_name(id, port_name_hash, port_name_mgr);
  }

  // 名無しのポートに名前を与える．
  for ( int id: Range(mNetwork.port_num()) ) {
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
  for ( int id: mNetwork.primary_input_id_list() ) {
    reg_node_name(id, name_hash, node_name_mgr);
  }

  // FFの出力を登録する．
  for ( int id: Range(mNetwork.dff_num()) ) {
    auto& dff = mNetwork.dff(id);
    reg_node_name(dff.output(), name_hash, node_name_mgr);
  }

  // ラッチの出力を登録する．
  for ( int id: Range(mNetwork.latch_num()) ) {
    auto& latch = mNetwork.latch(id);
    reg_node_name(latch.output(), name_hash, node_name_mgr);
  }

  // 論理ノードを登録する．
  for ( int id: mNetwork.logic_id_list() ) {
    reg_node_name(id, name_hash, node_name_mgr);
  }

  // 名無しのノードに名前を与える．
  for ( int id: Range(mNetwork.node_num()) ) {
    if ( mNodeNameArray[id] == string() ) {
      string name = node_name_mgr.new_name(true);
      mNodeNameArray[id] = name;
    }
  }

  // インスタンス名を管理するクラス
  NameMgr instance_name_mgr(mInstancePrefix, mInstanceSuffix);

  // ノード名を instance_name_mgr に登録しておく．
  for ( int id: Range(mNetwork.node_num()) ) {
    string name = mNodeNameArray[id];
    instance_name_mgr.add(name);
  }

  // ノード用のインスタンス名を登録する．
  for ( int id: mNetwork.logic_id_list() ) {
    auto& node = mNetwork.node(id);
    if ( node.cell_id() != -1 ) {
      string name = instance_name_mgr.new_name(true);
      mNodeInstanceNameArray[id] = name;
    }
  }

  // DFF用のインスタンス名を登録する．
  for ( int id: Range(mNetwork.dff_num()) ) {
    auto& dff = mNetwork.dff(id);
    if ( dff.cell_id() != -1 ) {
      string name = instance_name_mgr.new_name(true);
      mDffInstanceNameArray[id] = name;
    }
  }

  // ラッチ用のインスタンス名を登録する．
  for ( int id: Range(mNetwork.latch_num()) ) {
    auto& latch = mNetwork.latch(id);
    if ( latch.cell_id() != -1 ) {
      string name = instance_name_mgr.new_name(true);
      mLatchInstanceNameArray[id] = name;
    }
  }

  // 外部出力ノードの名前をそのファンインの名前に付け替える．
  for ( int id: mNetwork.primary_output_id_list() ) {
    replace_node_name(id);
  }

  // FFの入力ノードの名前をそのファンインの名前に付け替える．
  for ( int i: Range(mNetwork.dff_num()) ) {
    auto& dff = mNetwork.dff(i);
    replace_node_name(dff.input());
    replace_node_name(dff.clock());
    replace_node_name(dff.clear());
    replace_node_name(dff.preset());
  }

  // ラッチの入力ノードの名前をそのファンインの名前に付け替える．
  for ( int i: Range(mNetwork.latch_num()) ) {
    auto& latch = mNetwork.latch(i);
    replace_node_name(latch.input());
    replace_node_name(latch.enable());
    replace_node_name(latch.clear());
    replace_node_name(latch.preset());
  }
}

// @brief ノード名をそのファンインのノード名に付け替える．
void
VerilogWriter::replace_node_name(
  int node_id
)
{
  if ( node_id == -1 ) {
    return;
  }
  auto& node = mNetwork.node(node_id);
  int src_id = node.fanin_id(0);
  mNodeNameArray[node_id] = mNodeNameArray[src_id];
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
  int port_id,
  unordered_set<string>& name_hash,
  NameMgr& name_mgr
)
{
  auto& port = mNetwork.port(port_id);
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
  mPortNameArray[port_id] = name;
}

// @brief ノード名の登録を行う．
void
VerilogWriter::reg_node_name(
  int node_id,
  unordered_set<string>& name_hash,
  NameMgr& name_mgr
)
{
  auto& node = mNetwork.node(node_id);
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
  mNodeNameArray[node_id] = name;
}

END_NAMESPACE_YM_BNET
