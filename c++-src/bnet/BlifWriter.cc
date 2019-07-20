
/// @file BlifWriter.cc
/// @brief BlifWriter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "BlifWriter.h"
#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/BnDff.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
// @param[in] prefix 自動生成名の接頭語
// @param[in] suffix 自動生成名の接尾語
BlifWriter::BlifWriter(const BnNetwork& network,
		       const string& prefix,
		       const string& suffix) :
  WriterBase(network)
{
  string _prefix(prefix);
  if ( _prefix == string() ) {
    _prefix = "__node";
  }
  string _suffix(suffix);
  if ( _suffix == string() ) {
    _suffix = "";
  }
  init_name_array(_prefix, _suffix);
}

// @brief デストラクタ
BlifWriter::~BlifWriter()
{
}

// @brief blif 形式で出力する．
// @param[in] s 出力先のストリーム
void
BlifWriter::operator()(ostream& s)
{
  // .model 文の出力
  s << ".model " << network().name() << endl;

  // .inputs 文の出力
  int count = 0;
  for ( int id: network().primary_input_id_list() ) {
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
  for ( int id: network().primary_output_id_list() ) {
    if ( count == 0 ) {
      s << ".outputs";
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

  // .latch 文の出力
  for ( int id: Range(network().dff_num()) ) {
    auto& dff = network().dff(id);
    s << ".latch " << node_name(dff.input()) << " " << node_name(dff.output()) << endl;
  }

  // 出力用の追加の .names 文
  for ( int id: network().primary_output_id_list() ) {
    auto& node = network().node(id);
    int src_id = node.fanin_id(0);
    string src_name = node_name(src_id);
    string name = node_name(id);
    if ( name != src_name ) {
      s << ".names " << src_name << " " << name << endl
	<< "1 1" << endl;
    }
  }

  // .names 文の出力
  for ( int id: network().logic_id_list() ) {
    if ( !is_data(id) ) {
      continue;
    }

    s << ".names";
    auto& node = network().node(id);
    for ( int iid: node.fanin_id_list() ) {
      s << " " << node_name(iid);
    }
    s << " " << node_name(id) << endl;
    auto type = node.type();
    int ni = node.fanin_num();
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
      for ( int i: Range(ni) ) {
	for ( int j: Range(ni) ) {
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
      for ( int i: Range(ni) ) {
	for ( int j: Range(ni) ) {
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
      for ( int i: Range(ni) ) {
	s << "0";
      }
      s << " 1" << endl;
      break;
    case BnNodeType::Xor:
      for ( int p: Range(1 << ni) ) {
	int parity = 0;
	for ( int i: Range(ni) ) {
	  if ( p & (1 << i) ) {
	    parity ^= 1;
	  }
	}
	if ( parity ) {
	  for ( int i: Range(ni) ) {
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
      for ( int p: Range(1 << ni) ) {
	int parity = 0;
	for ( int i: Range(ni) ) {
	  if ( p & (1 << i) ) {
	    parity ^= 1;
	  }
	}
	if ( parity == 0 ) {
	  for ( int i: Range(ni) ) {
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
	  int nc = expr.child_num();
	  if ( expr.is_and() ) {
	    // 個々の子供はリテラルのはず．
	    vector<int> pol_array(ni, 0);
	    for ( int i: Range(expr.child_num()) ) {
	      Expr expr1 = expr.child(i);
	      if ( expr1.is_posi_literal() ) {
		VarId var = expr1.varid();
		int pos = var.val();
		pol_array[pos] = 1;
	      }
	      else if ( expr1.is_nega_literal() ) {
		VarId var = expr1.varid();
		int pos = var.val();
		pol_array[pos] = 2;
	      }
	      else {
		ASSERT_NOT_REACHED;
	      }
	    }
	    for ( int i: Range(ni) ) {
	      switch ( pol_array[i] ) {
	      case 0: s << "-"; break;
	      case 1: s << "1"; break;
	      case 2: s << "0"; break;
	      }
	    }
	    s << " 1" << endl;
	  }
	  else if ( expr.is_or() ) {
	    for ( int i: Range(expr.child_num()) ) {
	      Expr expr1 = expr.child(i);
	      if ( expr1.is_posi_literal() ) {
		VarId var = expr1.varid();
		int pos = var.val();
		for ( auto j: Range(ni) ) {
		  if ( j == pos ) {
		    s << "1";
		  }
		  else {
		    s << "-";
		  }
		}
		s << " 1" << endl;
	      }
	      else if ( expr1.is_nega_literal() ) {
		VarId var = expr1.varid();
		int pos = var.val();
		for ( int j: Range(ni) ) {
		  if ( j == pos ) {
		    s << "0";
		  }
		  else {
		    s << "-";
		  }
		}
		s << " 1" << endl;
	      }
	      else if ( expr1.is_and() ) {
		vector<int> lit_map(ni, 0);
		for ( int j: Range(expr1.child_num()) ) {
		  Expr expr2 = expr1.child(j);
		  ASSERT_COND( expr2.is_literal() );
		  VarId var = expr2.varid();
		  int pos = var.val();
		  if ( expr2.is_posi_literal() ) {
		    lit_map[pos] = 1;
		  }
		  else {
		    lit_map[pos] = 2;
		  }
		}
		for ( int j: Range(ni) ) {
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
	  for ( int p: Range(1 << ni) ) {
	    vector<Expr::BitVectType> vect_array(ni);
	    for ( int i: Range(ni) ) {
	      if ( p & (1 << i) ) {
		vect_array[i] = 1;
	      }
	      else {
		vect_array[i] = 0;
	      }
	    }
	    if ( expr.eval(vect_array, 1) == 1 ) {
	      for ( int i: Range(ni) ) {
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
	for ( int p: Range(1 << ni) ) {
	  if ( func.value(p) ) {
	    for ( int i: Range(ni) ) {
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
