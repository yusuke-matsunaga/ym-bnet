
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
