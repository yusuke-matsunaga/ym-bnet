
/// @file BnFuncTypeMgr.cc
/// @brief BnFuncTypeMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnFuncTypeMgr.h"
#include "BnFuncTypeImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnFuncTypeMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BnFuncTypeMgr::BnFuncTypeMgr()
{
}

// @brief デストラクタ
BnFuncTypeMgr::~BnFuncTypeMgr()
{
}

// @brief プリミティブタイプを得る．
// @param[in] type プリミティブタイプ
// @param[in] input_num 入力数
//
// type がプリミティブタイプでない場合と
// input_num が type に合わない場合はエラー
// となり nullptr を返す．
const BnFuncType*
BnFuncTypeMgr::primitive_type(BnFuncType::Type type,
			      ymuint input_num)
{
  ymuint id;
  for (id = 0; id < mFuncTypeList.size(); ++ id) {
    const BnFuncType* func_type = mFuncTypeList[id];
    if ( func_type->type() == type && func_type->input_num() == input_num ) {
      return func_type;
    }
  }
  BnFuncType* func_type = new BnFuncTypePrim(id, type, input_num);
  mFuncTypeList.push_back(func_type);
  return func_type;
}

// @brief セルタイプを得る．
// @param[in] cell セル
const BnFuncType*
BnFuncTypeMgr::cell_type(const Cell* cell)
{
  ymuint id;
  for (id = 0; id < mFuncTypeList.size(); ++ id) {
    const BnFuncType* func_type = mFuncTypeList[id];
    if ( func_type->type() == BnFuncType::kFt_CELL && func_type->cell() == cell ) {
      return func_type;
    }
  }
  BnFuncType* func_type = new BnFuncTypeCell(id, cell);
  mFuncTypeList.push_back(func_type);
  return func_type;
}

// @brief 論理式タイプを得る．
// @param[in] expr 論理式
// @param[in] input_num 入力数
const BnFuncType*
BnFuncTypeMgr::expr_type(Expr expr,
			 ymuint input_num)
{
  // expr がプリミティブタイプなら primitive_type() を呼ぶ．
  {
    TvFunc tv = expr.make_tv(input_num);
    if ( tv == TvFunc::const_zero(0) ) {
      return primitive_type(BnFuncType::kFt_C0, 0);
    }
    else if ( tv == TvFunc::const_one(0) ) {
      return primitive_type(BnFuncType::kFt_C1, 0);
    }
    else if ( tv == TvFunc::posi_literal(1, VarId(0)) ) {
      return primitive_type(BnFuncType::kFt_BUFF, 1);
    }
    else if ( tv == TvFunc::nega_literal(1, VarId(0)) ) {
      return primitive_type(BnFuncType::kFt_NOT, 1);
    }
    else {
      ymuint ni = tv.input_num();
      ymuint np = 1UL << ni;
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
	for (ymuint i = 0; i < ni; ++ i) {
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
	  // 0000 だけ 0 で残りが 1
	  return primitive_type(BnFuncType::kFt_OR, input_num);
	}
	else if ( !has_1 ) {
	  // 1111 だけ 1 で残りが 0
	  return primitive_type(BnFuncType::kFt_AND, input_num);
	}
      }
      else if ( val_0 == 1 && val_1 == 0 ) {
	if ( !has_0 ) {
	  // 1111 だけ 0 で残りが 1
	  return primitive_type(BnFuncType::kFt_NAND, input_num);
	}
	else if ( !has_1 ) {
	  // 0000 だけ 1 で残りが 0
	  return primitive_type(BnFuncType::kFt_NOR, input_num);
	}
      }
      else if ( xor_match ) {
	return primitive_type(BnFuncType::kFt_XOR, input_num);
      }
      else if ( xnor_match ) {
	return primitive_type(BnFuncType::kFt_XNOR, input_num);
      }
    }
  }

  ymuint id;
  for (id = 0; id < mFuncTypeList.size(); ++ id) {
    const BnFuncType* func_type = mFuncTypeList[id];
    if ( func_type->type() == BnFuncType::kFt_EXPR &&
	 func_type->input_num() == input_num &&
	 check_equiv(func_type->expr(), expr) ) {
      return func_type;
    }
  }
  BnFuncType* func_type = new BnFuncTypeExpr(id, expr, input_num);
  mFuncTypeList.push_back(func_type);
  return func_type;
}

// @brief 真理値表タイプを得る．
const BnFuncType*
BnFuncTypeMgr::tv_type(const TvFunc& tv)
{
  ymuint id;
  for (id = 0; id < mFuncTypeList.size(); ++ id) {
    const BnFuncType* func_type = mFuncTypeList[id];
    if ( func_type->type() == BnFuncType::kFt_TV && func_type->truth_vector() == tv ) {
      return func_type;
    }
  }
  BnFuncType* func_type = new BnFuncTypeTv(id, tv);
  mFuncTypeList.push_back(func_type);
  return func_type;
}

END_NAMESPACE_YM_BNET
