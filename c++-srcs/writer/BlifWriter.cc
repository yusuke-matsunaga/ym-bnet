
/// @file BlifWriter.cc
/// @brief BlifWriter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BlifWriter.h"
#include "ym/BnNetwork.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief 内容を blif 形式で出力する．
void
BnNetwork::write_blif(
  const string& filename,
  const string& prefix,
  const string& suffix
) const
{
  ofstream ofs{filename};
  if ( ofs ) {
    write_blif(ofs, prefix, suffix);
  }
}

// @brief 内容を blif 形式で出力する．
void
BnNetwork::write_blif(
  ostream& s,
  const string& prefix,
  const string& suffix
) const
{
  // Latchタイプ，CellタイプのDFFノードを持つとき変換不能
  for ( auto& dff: dff_list() ) {
    if ( dff.type() != BnDffType::Dff ) {
      cerr << "Cannot convert to blif" << endl;
      return;
    }
  }

  // TvFuncタイプ，Bddタイプ，Cellタイプの論理ノードを持つ時変換不能
  for ( auto& node: logic_list() ) {
    if ( node.type() == BnNodeType::TvFunc ||
	 node.type() == BnNodeType::Bdd ||
	 node.type() == BnNodeType::Cell ) {
      cerr << "Cannot convert to blif" << endl;
      return;
    }
  }

  BlifWriter writer{*this, prefix, suffix};
  writer(s);
}


//////////////////////////////////////////////////////////////////////
// クラス BlifWriter
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifWriter::BlifWriter(
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
BlifWriter::operator()(
  ostream& s
)
{
  // .model 文の出力
  s << ".model " << network().name() << endl;

  // .inputs 文の出力
  int count = 0;
  for ( auto& node: network().primary_input_list() ) {
    auto id = node.id();
    if ( !is_data(id) ) {
      continue;
    }
    if ( count == 0 ) {
      s << ".inputs";
    }
    s << " " << node_name(id);
    ++ count;
    if ( count >= 10 ) {
      s << endl;
      count = 0;
    }
  }
  if ( count > 0 ) {
    s << endl;
  }

  // .outputs 文の出力
  count = 0;
  for ( auto& node: network().primary_output_list() ) {
    if ( count == 0 ) {
      s << ".outputs";
    }

    auto src_id = node.output_src();
    s << " " << node_name(src_id);
    ++ count;
    if ( count >= 10 ) {
      s << endl;
      count = 0;
    }
  }
  if ( count > 0 ) {
    s << endl;
  }

  // .latch 文の出力
  for ( auto& dff: network().dff_list() ) {
    s << ".latch " << node_name(dff.data_in())
      << " " << node_name(dff.data_out()) << endl;
  }

  // 出力用の追加の .names 文
  for ( auto& node: network().primary_output_list() ) {
    auto id = node.id();
    string name = node_name(id);
    auto src_id = node.output_src();
    string src_name = node_name(src_id);
    if ( name != src_name ) {
      s << ".names " << src_name << " " << name << endl
	<< "1 1" << endl;
    }
  }

  // .names 文の出力
  for ( auto& node: network().logic_list() ) {
    auto id = node.id();
    if ( !is_data(id) ) {
      continue;
    }

    s << ".names";
    for ( auto iid: node.fanin_id_list() ) {
      s << " " << node_name(iid);
    }
    s << " " << node_name(id) << endl;
    auto type = node.type();
    auto ni = node.fanin_num();
    switch ( type ) {
    case BnNodeType::C0:
      s << "0" << endl;
      break;
    case BnNodeType::C1:
      s << "1" << endl;
      break;
    case BnNodeType::Buff:
      s << "1 1" << endl;
      break;
    case BnNodeType::Not:
      s << "0 1" << endl;
      break;
    case BnNodeType::And:
      for ( auto i: Range(ni) ) {
	s << "1";
      }
      s << " 1" << endl;
      break;
    case BnNodeType::Nand:
      for ( auto i: Range(ni) ) {
	for ( auto j: Range(ni) ) {
	  if ( i == j ) {
	    s << "0";
	  }
	  else {
	    s << "-";
	  }
	}
	s << " 1" << endl;
      }
      break;
    case BnNodeType::Or:
      for ( auto i: Range(ni) ) {
	for ( auto j: Range(ni) ) {
	  if ( i == j ) {
	    s << "1";
	  }
	  else {
	    s << "-";
	  }
	}
	s << " 1" << endl;
      }
      break;
    case BnNodeType::Nor:
      for ( auto i: Range(ni) ) {
	s << "0";
      }
      s << " 1" << endl;
      break;
    case BnNodeType::Xor:
      for ( auto p: Range(1 << ni) ) {
	int parity = 0;
	for ( auto i: Range(ni) ) {
	  if ( p & (1 << i) ) {
	    parity ^= 1;
	  }
	}
	if ( parity ) {
	  for ( auto i: Range(ni) ) {
	    if ( p & (1 << i) ) {
	      s << "1";
	    }
	    else {
	      s << "0";
	    }
	  }
	  s << " 1" << endl;
	}
      }
      break;
    case BnNodeType::Xnor:
      for ( auto p: Range(1 << ni) ) {
	int parity = 0;
	for ( auto i: Range(ni) ) {
	  if ( p & (1 << i) ) {
	    parity ^= 1;
	  }
	}
	if ( parity == 0 ) {
	  for ( auto i: Range(ni) ) {
	    if ( p & (1 << i) ) {
	      s << "1";
	    }
	    else {
	      s << "0";
	    }
	  }
	  s << " 1" << endl;
	}
      }
      break;
    case BnNodeType::Expr:
      {
	const Expr& expr = network().expr(node.expr_id());
	if ( expr.is_sop() ) {
	  if ( expr.is_and() ) {
	    // 個々の子供はリテラルのはず．
	    vector<int> pol_array(ni, 0);
	    for ( auto& expr1: expr.operand_list() ) {
	      if ( expr1.is_posi_literal() ) {
		auto var = expr1.varid();
		pol_array[var] = 1;
	      }
	      else if ( expr1.is_nega_literal() ) {
		auto var = expr1.varid();
		pol_array[var] = 2;
	      }
	      else {
		ASSERT_NOT_REACHED;
	      }
	    }
	    for ( auto i: Range(ni) ) {
	      switch ( pol_array[i] ) {
	      case 0: s << "-"; break;
	      case 1: s << "1"; break;
	      case 2: s << "0"; break;
	      }
	    }
	    s << " 1" << endl;
	  }
	  else if ( expr.is_or() ) {
	    for ( auto& expr1: expr.operand_list() ) {
	      if ( expr1.is_posi_literal() ) {
		auto var = expr1.varid();
		for ( auto j: Range(ni) ) {
		  if ( j == var ) {
		    s << "1";
		  }
		  else {
		    s << "-";
		  }
		}
		s << " 1" << endl;
	      }
	      else if ( expr1.is_nega_literal() ) {
		auto var = expr1.varid();
		for ( auto j: Range(ni) ) {
		  if ( j == var ) {
		    s << "0";
		  }
		  else {
		    s << "-";
		  }
		}
		s << " 1" << endl;
	      }
	      else if ( expr1.is_and() ) {
		vector<SizeType> lit_map(ni, 0);
		for ( auto& expr2: expr1.operand_list() ) {
		  ASSERT_COND( expr2.is_literal() );
		  auto var = expr2.varid();
		  if ( expr2.is_posi_literal() ) {
		    lit_map[var] = 1;
		  }
		  else {
		    lit_map[var] = 2;
		  }
		}
		for ( auto j: Range(ni) ) {
		  if ( lit_map[j] == 1 ) {
		    s << "1";
		  }
		  else if ( lit_map[j] == 2 ) {
		    s << "0";
		  }
		  else {
		    s << "-";
		  }
		}
		s << " 1" << endl;
	      }
	    }
	  }
	  else {
	    ASSERT_NOT_REACHED;
	  }
	}
	else {
	  // めんどくさいので最小項ごとに記述する．
	  for ( auto p: Range(1 << ni) ) {
	    vector<Expr::BitVectType> vect_array(ni);
	    for ( auto i: Range(ni) ) {
	      if ( p & (1 << i) ) {
		vect_array[i] = 1;
	      }
	      else {
		vect_array[i] = 0;
	      }
	    }
	    if ( expr.eval(vect_array, 1) == 1 ) {
	      for ( auto i: Range(ni) ) {
		if ( p & (1 << i) ) {
		  s << "1";
		}
		else {
		  s << "0";
		}
	      }
	      s << " 1" << endl;
	    }
	  }
	}
      }
      break;
    case BnNodeType::TvFunc:
      {
	const TvFunc& func = network().func(node.func_id());
	for ( auto p: Range(1 << ni) ) {
	  if ( func.value(p) ) {
	    for ( auto i: Range(ni) ) {
	      if ( p & (1 << i) ) {
		s << "1";
	      }
	      else {
		s << "0";
	      }
	    }
	    s << " 1" << endl;
	  }
	}
      }
      break;
    default:
      ASSERT_NOT_REACHED;
      break;
    }
  }
  s << ".end" << endl;
}

END_NAMESPACE_YM_BNET
