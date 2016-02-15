
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
  return new BnFuncTypePrim(type, input_num);
}

// @brief セルタイプを得る．
// @param[in] cell セル
const BnFuncType*
BnFuncTypeMgr::cell_type(const Cell* cell)
{
  return new BnFuncTypeCell(cell);
}

// @brief 論理式タイプを得る．
// @param[in] expr 論理式
// @param[in] input_num 入力数
const BnFuncType*
BnFuncTypeMgr::expr_type(Expr expr,
			 ymuint input_num)
{
  return new BnFuncTypeExpr(expr, input_num);
}

// @brief 真理値表タイプを得る．
const BnFuncType*
BnFuncTypeMgr::tv_type(const TvFunc& tv)
{
  return new BnFuncTypeTv(tv);
}

END_NAMESPACE_YM_BNET
