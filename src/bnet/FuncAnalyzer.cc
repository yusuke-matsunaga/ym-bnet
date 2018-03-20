
/// @file FuncAnalyzer.cc
/// @brief FuncAnalyzer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "FuncAnalyzer.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/BnNodeType.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

// @brief 真理値表から論理型を得る．
//
// プリミティブ型でなかった場合は BnNodeType::Logic_NONE を返す．
BnNodeType
tv2logic_type(const TvFunc& tv)
{
  if ( tv == TvFunc::const_zero(0) ) {
    return BnNodeType::Logic_C0;
  }
  else if ( tv == TvFunc::const_one(0) ) {
    return BnNodeType::Logic_C1;
  }
  else if ( tv == TvFunc::posi_literal(1, VarId(0)) ) {
    return BnNodeType::Logic_BUFF;
  }
  else if ( tv == TvFunc::nega_literal(1, VarId(0)) ) {
    return BnNodeType::Logic_NOT;
  }
  else {
    int input_num = tv.input_num();
    int np = 1UL << input_num;
    int val_0;
    int val_1;
    bool has_0 = false;
    bool has_1 = false;
    bool xor_match = true;
    bool xnor_match = true;
    for ( int p = 0; p < np; ++ p ) {
      int val = tv.value(p);
      if ( p == 0UL ) {
	// 00...00 の時の値
	val_0 = val;
      }
      else if ( p == (np - 1) ) {
	// 11...11 の時の値
	val_1 = val;
      }
      else {
	if ( val == 0 ) {
	  // 少なくとも1つは0になった．
	  has_0 = true;
	}
	else {
	  // 少なくとも1つは1になった．
	  has_1 = true;
	}
      }

      // p のパリティを計算する．
      bool parity = false;
      for ( int i = 0; i < input_num; ++ i ) {
	if ( (1UL << i) & p ) {
	  parity = !parity;
	}
      }
      if ( parity ) {
	if ( val == 1 ) {
	  xnor_match = false;
	}
	else {
	  xor_match = false;
	}
      }
      else {
	if ( val == 1 ) {
	  xor_match = false;
	}
	else {
	  xnor_match = false;
	}
      }
    }
    if ( val_0 == 0 && val_1 == 1 ) {
      if ( !has_0 ) {
	// 00...00 だけ 0 で残りが 1
	return BnNodeType::Logic_OR;
      }
      else if ( !has_1 ) {
	// 11...11 だけ 1 で残りが 0
	return BnNodeType::Logic_AND;
      }
    }
    if ( val_0 == 1 && val_1 == 0 ) {
      if ( !has_0 ) {
	// 11...11 だけ 0 で残りが 1
	return BnNodeType::Logic_NAND;
      }
      else if ( !has_1 ) {
	// 00...00 だけ 1 で残りが 0
	return BnNodeType::Logic_NOR;
      }
    }
    if ( xor_match ) {
      return BnNodeType::Logic_XOR;
    }
    else if ( xnor_match ) {
      return BnNodeType::Logic_XNOR;
    }
  }

  return BnNodeType::Logic_NONE;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス FuncAnalyzer
//////////////////////////////////////////////////////////////////////

// @brief 与えられた論理式が組み込み型かどうか判定する．
//
// 組み込み型でない場合には BnNodeType::Logic_EXPR が返される．
BnNodeType
FuncAnalyzer::analyze(const Expr& expr)
{
  int input_num = expr.input_size();
  if ( input_num <= 10 ) {
    // 10入力以下の場合は一旦 TvFunc に変換する．
    TvFunc tv = expr.make_tv(input_num);
    BnNodeType logic_type = tv2logic_type(tv);
    if ( logic_type != BnNodeType::Logic_NONE ) {
      return logic_type;
    }
  }
  return BnNodeType::Logic_EXPR;
}

// @brief 与えられた真理値表が組み込み型かどうか判定する．
//
// 組み込み型でない場合には BnNodeType::Logic_TV が返される．
BnNodeType
FuncAnalyzer::analyze(const TvFunc& func)
{
  BnNodeType logic_type = tv2logic_type(func);
  if ( logic_type != BnNodeType::Logic_NONE ) {
    return logic_type;
  }
  return BnNodeType::Logic_TV;
}

END_NAMESPACE_YM_BNET
