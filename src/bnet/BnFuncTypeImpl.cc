
/// @file BnFuncTypeImpl.cc
/// @brief BnFuncTypeImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnFuncTypeImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnFuncTypeImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BnFuncTypeImpl::BnFuncTypeImpl()
{
}

// @brief デストラクタ
BnFuncTypeImpl::~BnFuncTypeImpl()
{
}

// @brief セルを返す．
const Cell*
BnFuncTypeImpl::cell() const
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief 論理式を返す．
Expr
BnFuncTypeImpl::expr() const
{
  ASSERT_NOT_REACHED;
}

// @brief 真理値表を返す．
TvFunc
BnFuncTypeImpl::truth_vector() const
{
  ASSERT_NOT_REACHED;
}


//////////////////////////////////////////////////////////////////////
// クラス BnFuncTypePrim
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] type
BnFuncTypePrim::BnFuncTypePrim(BnFuncType::Type type,
			       ymuint ni) :
  mType(type),
  mInputNum(ni)
{
}

// @brief デストラクタ
BnFuncTypeImpl::~BnFuncTypePrim()
{
}

// @brief 型を返す．
BnFuncType::Type
BnFuncTypeImpl::type() const
{
  return mType;
}

// @brief 入力数を返す．
//
// 一部の型では固定となっている．
ymuint
BnFuncTypeImpl::input_num() const
{
  reutrn mInputNum;
}


//////////////////////////////////////////////////////////////////////
// クラス BnFuncTypeCell
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] cell セル
BnFuncTypeCell::BnFuncTypeCell(const Cell* cell) :
  mCell(cell)
{
}

// @brief デストラクタ
BnFuncTypeCell::~BnFuncTypeCell()
{
}

// @brief 型を返す．
BnFuncType::Type
BnFuncTypeCell::type() const
{
  return kFt_CELL;
}

// @brief 入力数を返す．
//
// 一部の型では固定となっている．
ymuint
BnFuncTypeCell::input_num() const
{
  return cell()->input_num();
}

// @brief セルを返す．
const Cell*
BnFuncTypeCell::cell() const
{
  return mCell;
}


//////////////////////////////////////////////////////////////////////
// クラス BnFuncTypeExpr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] expr 論理式
// @param[in] ni 入力数
BnFuncTypeExpr::BnFuncTypeExpr(Expr expr,
			       ymuint ni) :
  mExpr(expr),
  mInputNum(ni)
{
}

// @brief デストラクタ
BnFuncTypeExpr::~BnFuncTypeExpr()
{
}

// @brief 型を返す．
Type
BnFuncTypeExpr::type() const
{
  return kFt_EXPR;
}

// @brief 入力数を返す．
//
// 一部の型では固定となっている．
ymuint
BnFuncTypeExpr::input_num() const
{
  return mInputNum;
}

// @brief 論理式を返す．
Expr
BnFuncTypeExpr::expr() const
{
  return mExpr;
}


//////////////////////////////////////////////////////////////////////
// クラス BnFuncTypeTv
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] tv 真理値表ベクタ
BnFuncType::BnFuncTypeTv(const TvFunc& tv) :
  mFunc(tv)
{
}

// @brief デストラクタ
BnFuncTypeTv::~BnFuncTypeTv()
{
}

// @brief 型を返す．
BnFuncType::Type
BnFuncTypeTv::type() const
{
  return kFt_TV;
}

// @brief 入力数を返す．
//
// 一部の型では固定となっている．
ymuint
BnFuncTypeTv::input_num() const
{
  return mFunc.input_num();
}

// @brief 真理値表を返す．
TvFunc
BnFuncTypeTv::truth_vector() const
{
  return mFunc;
}

END_NAMESPACE_YM_BNET
