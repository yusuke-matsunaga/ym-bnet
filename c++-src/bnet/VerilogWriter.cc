
/// @file VerilogWriter.cc
/// @brief VerilogWriter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
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

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
VerilogWriter::VerilogWriter(const BnNetwork& network,
			     const string& port_prefix,
			     const string& port_suffix,
			     const string& node_prefix,
			     const string& node_suffix,
			     const string& instance_prefix,
			     const string& instance_suffix) :
  mNetwork(network),
  mPortPrefix(port_prefix),
  mPortSuffix(port_suffix),
  mNodePrefix(node_prefix),
  mNodeSuffix(node_suffix),
  mInstancePrefix(instance_prefix),
  mInstanceSuffix(instance_suffix),
  mPortNameArray(network.port_num()),
  mNodeNameArray(network.node_num()),
  mNodeInstanceNameArray(network.node_num()),
  mDffInstanceNameArray(network.dff_num()),
  mLatchInstanceNameArray(network.latch_num())
{
}

// @brief デストラクタ
VerilogWriter::~VerilogWriter()
{
}

BEGIN_NONAMESPACE

// 関数番号から UDP名を作る．
string
udp_name(int func_id)
{
  ostringstream buf;
  buf << "__func" << func_id;
  return buf.str();
}

// UDP プリミティブの定義を出力する．
void
write_udp(ostream& s,
	  const string& udp_name,
	  const TvFunc& func)
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
write_op(ostream& s,
	 const char* op_str,
	 const vector<string>& iname_array)
{
  const char* tmp_str = "";
  for ( const string& name: iname_array ) {
    s << tmp_str;
    tmp_str = op_str;
    s << name;
  }
}

void
write_expr(ostream& s,
	   const Expr& expr,
	   const vector<string>& iname_array)
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
// @param[in] s 出力先のストリーム
void
VerilogWriter::operator()(ostream& s)
{
  // TODO
  // * ポート記述
  // * DFF, latch 記述
  // * セル，UDP のチェック

  init_name_array();

  s << "module " << mNetwork.name() << "(";
  // ...
  s << ");" << endl;

  // 外部入力
  for ( int id: mNetwork.primary_input_id_list() ) {
    s << "  " << "input  " << mNodeNameArray[id] << ";" << endl;
  }
  // 外部出力
  for ( int id: mNetwork.primary_output_src_id_list() ) {
    s << "  " << "output " << mNodeNameArray[id] << ";" << endl;
  }

  // このネットワークで使用されている TvFunc を UDP として定義しておく．
  for ( int i: Range(mNetwork.func_num()) ) {
    auto& func = mNetwork.func(i);
    write_udp(s, udp_name(i), func);
  }

  for ( int id: mNetwork.logic_id_list() ) {
    auto& node = mNetwork.node(id);
    int ni = node.fanin_num();
    vector<string> iname_array(ni);
    for ( int i: Range(ni) ) {
      iname_array[i] = mNodeNameArray[node.fanin_id(i)];
    }
    s << "  " << "wire   " << mNodeNameArray[id];
    int cell_id = node.cell_id();
    if ( cell_id == -1 ) {
      switch ( node.type() ) {
      case BnNodeType::C0:
	s << " = 1'b0;" << endl;
	break;
      case BnNodeType::C1:
	s << " = 1'b1;" << endl;
	break;
      case BnNodeType::Buff:
	s << " = " << iname_array[0] << ";" << endl;
	break;
      case BnNodeType::Not:
	s << " = ~" << iname_array[0] << ";" << endl;
	break;
      case BnNodeType::And:
	s << " = ";
	write_op(s, " & ", iname_array);
	s << ";" << endl;
	break;
      case BnNodeType::Nand:
	s << " = ";
	s << "~(";
	write_op(s, " & ", iname_array);
	s << ")";
	s << ";" << endl;
	break;
      case BnNodeType::Or:
	s << " = ";
	write_op(s, " | ", iname_array);
	s << ";" << endl;
	break;
      case BnNodeType::Nor:
	s << " = ";
	s << "~(";
	write_op(s, " | ", iname_array);
	s << ")";
	s << ";" << endl;
	break;
      case BnNodeType::Xor:
	s << " = ";
	write_op(s, " ^ ", iname_array);
	s << ";" << endl;
	break;
      case BnNodeType::Xnor:
	s << " = ";
	s << "~(";
	write_op(s, " ^ ", iname_array);
	s << ")";
	s << ";" << endl;
	break;
      case BnNodeType::Expr:
	s << " = ";
	write_expr(s, mNetwork.expr(node.expr_id()), iname_array);
	s << ";" << endl;
	break;
      case BnNodeType::TvFunc:
	// 予め mNetwork に登録されている TvFunc に対応する UDP を定義しておいて
	// ここはそのインスタンス化だけを行う．
	s << ";" << endl;
	s << "  " << udp_name(node.func_id()) << "(";
	for ( int i: Range(ni) ) {
	  s << ".i" << i << "(" << iname_array[i] << "), ";
	}
	s << ".o(" << mNodeNameArray[id] << ")";
	s << ");" << endl;
	break;
      default:
	ASSERT_NOT_REACHED;
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
  HashSet<string> port_name_hash;

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
  HashSet<string> name_hash;

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
}

BEGIN_NONAMESPACE

// 名前を補正する．
// 具体的には [a-zA-Z0-9_] 以外の文字を含んでいる場合にエスケープ識別子にする．
string
coerce_name(const string& name)
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
// @param[in] port_id ポート番号
// @param[in] name_hash ポート名のハッシュ
// @param[in] name_mgr ポート名を管理するクラス
void
VerilogWriter::reg_port_name(int port_id,
			     HashSet<string>& name_hash,
			     NameMgr& name_mgr)
{
  auto& port = mNetwork.port(port_id);
  string name = port.name();
  if ( name == string() ) {
    // ポート名がなかった．
    return;
  }
  name = coerce_name(name);
  if ( name_hash.check(name) ) {
    // 名前が重複していた．
    return;
  }

  // 名前を登録する．
  name_mgr.add(name);
  name_hash.add(name);
  mPortNameArray[port_id] = name;
}

// @brief ノード名の登録を行う．
// @param[in] node_id ノード番号
// @param[in] name_hash ノード名のハッシュ
// @param[in] name_mgr ノード名を管理するクラス
void
VerilogWriter::reg_node_name(int node_id,
			     HashSet<string>& name_hash,
			     NameMgr& name_mgr)
{
  auto& node = mNetwork.node(node_id);
  string name = node.name();
  if ( name == string() ) {
    // 名前がなかった．
    return;
  }
  name = coerce_name(name);
  if ( name_hash.check(name) ) {
    // 名前が重複していた．
    return;
  }

  // 名前を登録する．
  name_mgr.add(name);
  name_hash.add(name);
  mNodeNameArray[node_id] = name;
}

END_NAMESPACE_YM_BNET
