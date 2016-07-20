
/// @file FuncAnalyzer.cc
/// @brief FuncAnalyzer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "FuncAnalyzer.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

// @brief 真理値表から論理型を得る．
//
// プリミティブ型でなかった場合は kBnLt_NONE を返す．
BnLogicType
tv2logic_type(const TvFunc& tv)
{
  if ( tv == TvFunc::const_zero(0) ) {
    return kBnLt_C0;
  }
  else if ( tv == TvFunc::const_one(0) ) {
    return kBnLt_C1;
  }
  else if ( tv == TvFunc::posi_literal(1, VarId(0)) ) {
    return kBnLt_BUFF;
  }
  else if ( tv == TvFunc::nega_literal(1, VarId(0)) ) {
    return kBnLt_NOT;
  }
  else {
    ymuint input_num = tv.input_num();
    ymuint np = 1UL << input_num;
    int val_0;
    int val_1;
    bool has_0 = false;
    bool has_1 = false;
    bool xor_match = true;
    bool xnor_match = true;
    for (ymuint p = 0; p < np; ++ p) {
      int val = tv.value(p);
      if ( p == 0UL ) {
	val_0 = val;
      }
      else if ( p == (np - 1) ) {
	val_1 = val;
      }
      else {
	if ( val == 0 ) {
	  has_0 = true;
	}
	else {
	  has_1 = true;
	}
      }
      bool parity = false;
      for (ymuint i = 0; i < input_num; ++ i) {
	if ( (1UL << i) & p ) {
	  parity = !parity;
	}
      }
      if ( parity ) {
	if ( val ) {
	  xnor_match = false;
	}
      }
      else {
	if ( val ) {
	  xor_match = false;
	}
      }
    }
    if ( val_0 == 0 && val_1 == 1 ) {
      if ( !has_0 ) {
	// 00...00 だけ 0 で残りが 1
	return kBnLt_OR;
      }
      else if ( !has_1 ) {
	// 11...11 だけ 1 で残りが 0
	return kBnLt_AND;
      }
    }
    else if ( val_0 == 1 && val_1 == 0 ) {
      if ( !has_0 ) {
	// 11...11 だけ 0 で残りが 1
	return kBnLt_NAND;
      }
      else if ( !has_1 ) {
	// 00...00 だけ 1 で残りが 0
	return kBnLt_NOR;
      }
    }
    else if ( xor_match ) {
      return kBnLt_XOR;
    }
    else if ( xnor_match ) {
      return kBnLt_XNOR;
    }
  }

  return kBnLt_NONE;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス FuncAnalyzer
//////////////////////////////////////////////////////////////////////

// @brief 与えられた論理式が組み込み型かどうか判定する．
//
// 組み込み型でない場合には kBnLt_EXPR が返される．
BnLogicType
FuncAnalyzer::analyze(const Expr& expr)
{
  ymuint input_num = expr.input_size();
  if ( input_num <= 10 ) {
    // 10入力以下の場合は一旦 TvFunc に変換する．
    TvFunc tv = expr.make_tv(input_num);
    BnLogicType logic_type = tv2logic_type(tv);
    if ( logic_type != kBnLt_NONE ) {
      return logic_type;
    }
  }
  return kBnLt_EXPR;
}

// @brief 与えられた真理値表が組み込み型かどうか判定する．
//
// 組み込み型でない場合には kBnLt_TV が返される．
BnLogicType
FuncAnalyzer::analyze(const TvFunc& func)
{
  BnLogicType logic_type = tv2logic_type(func);
  if ( logic_type != kBnLt_NONE ) {
    return logic_type;
  }
  return kBnLt_TV;
}

END_NAMESPACE_YM_BNET
