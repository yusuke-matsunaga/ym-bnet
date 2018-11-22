
/// @file BnNodeEnc.cc
/// @brief BnNodeEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNodeEnc.h"
#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/Range.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM

// @brief コンストラクタ
// @param[in] solver SATソルバ
// @param[in] network 対象のネットワーク
// @param[in] varmap 変数番号のマップ
BnNodeEnc::BnNodeEnc(SatSolver& solver,
		     const BnNetwork& network,
		     const vector<SatVarId>& varmap) :
  mSolver(solver),
  mNetwork(network),
  mVarMap(varmap)
{
}

// @brief デストラクタ
BnNodeEnc::~BnNodeEnc()
{
}

// @brief ノードの入出力の関係を表すCNF式を作る．
// @param[in] node 対象のノード
// @param[in] var_map 変数マップ
void
BnNodeEnc::make_cnf(const BnNode& node)
{
  SatLiteral olit = lit(node.id());
  int ni = node.fanin_num();
  vector<SatLiteral> ilit_array(ni);
  for ( auto i: Range<>(ni) ) {
    ilit_array[i] = lit(node.fanin_id(i));
  }
  switch ( node.type() ) {
  case BnNodeType::Input:
    // なにもしない．
    break;

  case BnNodeType::C0:
    make_zero(olit);
    break;

  case BnNodeType::C1:
    make_one(olit);
    break;

  case BnNodeType::Buff:
    make_buff(olit, ilit_array[0]);
    break;

  case BnNodeType::Not:
    make_not(olit, ilit_array[0]);
    break;

  case BnNodeType::And:
    make_and(olit, ilit_array);
    break;

  case BnNodeType::Nand:
    make_and(~olit, ilit_array);
    break;

  case BnNodeType::Or:
    make_or(olit, ilit_array);
    break;

  case BnNodeType::Nor:
    make_or(~olit, ilit_array);
    break;

  case BnNodeType::Xor:
    make_xor(olit, ilit_array);
    break;

  case BnNodeType::Xnor:
    make_xor(~olit, ilit_array);
    break;

  case BnNodeType::Expr:
    {
      const Expr& expr = mNetwork.expr(node.expr_id());
      if ( expr.is_zero() ) {
	make_zero(olit);
      }
      else if ( expr.is_one() ) {
	make_one(olit);
      }
      else if ( expr.is_posi_literal() ) {
	VarId var = expr.varid();
	SatLiteral ilit = ilit_array[var.val()];
	make_buff(olit, ilit);
      }
      else if ( expr.is_nega_literal() ) {
	VarId var = expr.varid();
	SatLiteral ilit = ilit_array[var.val()];
	make_not(olit, ilit);
      }
      else {
	ASSERT_COND( expr.is_op() );
	int nc = expr.child_num();
	vector<SatLiteral> tmp_ilit_array(nc);
	for ( int i: Range<>(nc) ) {
	  tmp_ilit_array[i] = make_expr(expr.child(i), ilit_array);
	}

	if ( expr.is_and() ) {
	  make_and(olit, tmp_ilit_array);
	}
	else if ( expr.is_or() ) {
	  make_or(olit, tmp_ilit_array);
	}
	else if ( expr.is_xor() ) {
	  make_xor(olit, tmp_ilit_array);
	}
	else {
	  ASSERT_NOT_REACHED;
	}
      }
    }
    break;

  case BnNodeType::TvFunc:
    {
      const TvFunc& func = mNetwork.func(node.func_id());
      int np = 1 << ni;
      for ( int p: Range<>(np) ) {
	vector<SatLiteral> tmp_lits(ni + 1);
	for ( int i: Range<>(ni) ) {
	  SatLiteral ilit = ilit_array[i];
	  if ( p & (1 << i) ) {
	    tmp_lits[i] = ~ilit;
	  }
	  else {
	    tmp_lits[i] =  ilit;
	  }
	}
	if ( func.value(p) ) {
	  tmp_lits[ni] =  olit;
	}
	else {
	  tmp_lits[ni] = ~olit;
	}
	mSolver.add_clause(tmp_lits);
      }
    }
    // 未完
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief 定数０のCNFを作る．
void
BnNodeEnc::make_zero(SatLiteral olit)
{
  mSolver.add_clause(~olit);
}

// @brief 定数1のCNFを作る．
void
BnNodeEnc::make_one(SatLiteral olit)
{
  mSolver.add_clause( olit);
}

// @brief BUFFのCNFを作る．
void
BnNodeEnc::make_buff(SatLiteral olit,
		     SatLiteral ilit)
{
  mSolver.add_eq_rel(olit, ilit);
}

// @brief NOTのCNFを作る．
void
BnNodeEnc::make_not(SatLiteral olit,
		    SatLiteral ilit)
{
  mSolver.add_neq_rel(olit, ilit);
}

// @brief AND論理のCNFを作る．
void
BnNodeEnc::make_and(SatLiteral olit,
		    const vector<SatLiteral>& ilit_array)
{
  int ni = ilit_array.size();
  switch ( ni ) {
  case 2:
    {
      SatLiteral ilit0 = ilit_array[0];
      SatLiteral ilit1 = ilit_array[1];
      mSolver.add_andgate_rel( olit, ilit0, ilit1);
    }
    break;

  case 3:
    {
      SatLiteral ilit0 = ilit_array[0];
      SatLiteral ilit1 = ilit_array[1];
      SatLiteral ilit2 = ilit_array[2];
      mSolver.add_andgate_rel( olit, ilit0, ilit1, ilit2);
    }
    break;

  case 4:
    {
      SatLiteral ilit0 = ilit_array[0];
      SatLiteral ilit1 = ilit_array[1];
      SatLiteral ilit2 = ilit_array[2];
      SatLiteral ilit3 = ilit_array[3];
      mSolver.add_andgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
    }
    break;

  default:
    mSolver.add_andgate_rel( olit, ilit_array);
    break;
  }
}

// @brief OR論理のCNFを作る．
void
BnNodeEnc::make_or(SatLiteral olit,
		   const vector<SatLiteral>& ilit_array)
{
  int ni = ilit_array.size();
  switch ( ni ) {
  case 2:
    {
      SatLiteral ilit0 = ilit_array[0];
      SatLiteral ilit1 = ilit_array[1];
      mSolver.add_orgate_rel( olit, ilit0, ilit1);
    }
    break;

  case 3:
    {
      SatLiteral ilit0 = ilit_array[0];
      SatLiteral ilit1 = ilit_array[1];
      SatLiteral ilit2 = ilit_array[2];
      mSolver.add_orgate_rel( olit, ilit0, ilit1, ilit2);
    }
    break;

  case 4:
    {
      SatLiteral ilit0 = ilit_array[0];
      SatLiteral ilit1 = ilit_array[1];
      SatLiteral ilit2 = ilit_array[2];
      SatLiteral ilit3 = ilit_array[3];
      mSolver.add_orgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
    }
    break;

  default:
    mSolver.add_orgate_rel( olit, ilit_array);
    break;
  }
}

// @brief XOR論理のCNFを作る．
void
BnNodeEnc::make_xor(SatLiteral olit,
		    const vector<SatLiteral>& ilit_array)
{
  int ni = ilit_array.size();
  switch ( ni ) {
  case 2:
    {
      SatLiteral ilit0 = ilit_array[0];
      SatLiteral ilit1 = ilit_array[1];
      mSolver.add_xorgate_rel( olit, ilit0, ilit1);
    }
    break;

  case 3:
    {
      SatLiteral ilit0 = ilit_array[0];
      SatLiteral ilit1 = ilit_array[1];
      SatLiteral ilit2 = ilit_array[2];
      mSolver.add_xorgate_rel( olit, ilit0, ilit1, ilit2);
    }
    break;

  case 4:
    {
      SatLiteral ilit0 = ilit_array[0];
      SatLiteral ilit1 = ilit_array[1];
      SatLiteral ilit2 = ilit_array[2];
      SatLiteral ilit3 = ilit_array[3];
      mSolver.add_xorgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
    }
    break;

  default:
    mSolver.add_xorgate_rel( olit, ilit_array);
    break;
  }
}

// @brief 論理式のCNFを作る．
SatLiteral
BnNodeEnc::make_expr(const Expr& expr,
		     const vector<SatLiteral>& ilit_array)
{
  if ( expr.is_zero() ||
       expr.is_one() ) {
    ASSERT_NOT_REACHED;
    return kSatLiteralX;
  }

  if ( expr.is_posi_literal() ) {
    VarId var = expr.varid();
    return ilit_array[var.val()];
  }

  if ( expr.is_nega_literal() ) {
    VarId var = expr.varid();
    return ~ilit_array[var.val()];
  }

  ASSERT_COND( expr.is_op() );
  int nc = expr.child_num();
  vector<SatLiteral> tmp_ilit_array(nc);
  for ( int i: Range<>(nc) ) {
    tmp_ilit_array[i] = make_expr(expr.child(i), ilit_array);
  }

  SatVarId ovar = mSolver.new_variable();
  SatLiteral olit(ovar);
  if ( expr.is_and() ) {
    make_and(olit, tmp_ilit_array);
  }
  else if ( expr.is_or() ) {
    make_or(olit, tmp_ilit_array);
  }
  else if ( expr.is_xor() ) {
    make_xor(olit, tmp_ilit_array);
  }
  else {
    ASSERT_NOT_REACHED;
  }

  return olit;
}

// @brief ノードに対応するリテラルを返す．
SatLiteral
BnNodeEnc::lit(int node_id)
{
  return SatLiteral(mVarMap[node_id]);
}

END_NAMESPACE_YM
