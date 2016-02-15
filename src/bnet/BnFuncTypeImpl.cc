
/// @file BnFuncTypeImpl.cc
/// @brief BnFuncTypeImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnFuncTypeImpl.h"
#include "ym/Cell.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnFuncTypeImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
BnFuncTypeImpl::BnFuncTypeImpl(ymuint id) :
  mId(id)
{
}

// @brief デストラクタ
BnFuncTypeImpl::~BnFuncTypeImpl()
{
}

// @brief ID 番号を返す．
ymuint
BnFuncTypeImpl::id() const
{
  return mId;
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
// @param[in] id ID番号
// @param[in] type 型
// @param[in] ni 入力数
BnFuncTypePrim::BnFuncTypePrim(ymuint id,
			       BnFuncType::Type type,
			       ymuint ni) :
  BnFuncTypeImpl(id),
  mType(type),
  mInputNum(ni)
{
}

// @brief デストラクタ
BnFuncTypePrim::~BnFuncTypePrim()
{
}

// @brief 型を返す．
BnFuncType::Type
BnFuncTypePrim::type() const
{
  return mType;
}

// @brief 入力数を返す．
//
// 一部の型では固定となっている．
ymuint
BnFuncTypePrim::input_num() const
{
  return mInputNum;
}


//////////////////////////////////////////////////////////////////////
// クラス BnFuncTypeCell
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] cell セル
BnFuncTypeCell::BnFuncTypeCell(ymuint id,
			       const Cell* cell) :
  BnFuncTypeImpl(id),
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
// @param[in] id ID番号
// @param[in] expr 論理式
// @param[in] ni 入力数
BnFuncTypeExpr::BnFuncTypeExpr(ymuint id,
			       Expr expr,
			       ymuint ni) :
  BnFuncTypeImpl(id),
  mExpr(expr),
  mInputNum(ni)
{
}

// @brief デストラクタ
BnFuncTypeExpr::~BnFuncTypeExpr()
{
}

// @brief 型を返す．
BnFuncType::Type
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
// @param[in] id ID番号
// @param[in] tv 真理値表ベクタ
BnFuncTypeTv::BnFuncTypeTv(ymuint id,
			   const TvFunc& tv) :
  BnFuncTypeImpl(id),
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
