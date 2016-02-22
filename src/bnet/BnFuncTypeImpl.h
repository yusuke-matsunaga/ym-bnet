#ifndef BNFUNCTYPEIMPL_H
#define BNFUNCTYPEIMPL_H

/// @file BnFuncTypeImpl.h
/// @brief BnFuncTypeImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnFuncType.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnFuncTypeImpl BnFuncTypeImpl.h "BnFuncTypeImpl.h"
/// @brief BnFuncType の実装クラス
///
/// といっても本当の役目は実際には使われないデフォルト実装を提供するだけ
//////////////////////////////////////////////////////////////////////
class BnFuncTypeImpl :
  public BnFuncType
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  BnFuncTypeImpl(ymuint id);

  /// @brief デストラクタ
  virtual
  ~BnFuncTypeImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID 番号を返す．
  virtual
  ymuint
  id() const;

  /// @brief セルを返す．
  virtual
  const Cell*
  cell() const;

  /// @brief 論理式を返す．
  virtual
  Expr
  expr() const;

  /// @brief 真理値表を返す．
  virtual
  TvFunc
  truth_vector() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  ymuint mId;

};


//////////////////////////////////////////////////////////////////////
/// @class BnFuncTypePrim BnFuncTypeImpl.h "BnFuncTypeImpl.h"
/// @brief プリミティブタイプ
//////////////////////////////////////////////////////////////////////
class BnFuncTypePrim :
  public BnFuncTypeImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] type プリミティタイプ
  /// @param[in] ni 入力数
  BnFuncTypePrim(ymuint id,
		 BnFuncType::Type type,
		 ymuint ni = 0);

  /// @brief デストラクタ
  ~BnFuncTypePrim();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を返す．
  virtual
  Type
  type() const;

  /// @brief 入力数を返す．
  ///
  /// 一部の型では固定となっている．
  virtual
  ymuint
  input_num() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 型
  Type mType;

  // 入力数
  ymuint mInputNum;

};


//////////////////////////////////////////////////////////////////////
/// @class BnFuncTypeCell BnFuncTypeCell.h "BnFuncTypeCell.h"
/// @brief セルタイプ
//////////////////////////////////////////////////////////////////////
class BnFuncTypeCell :
  public BnFuncTypeImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] cell セル
  BnFuncTypeCell(ymuint id,
		 const Cell* cell);

  /// @brief デストラクタ
  ~BnFuncTypeCell();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を返す．
  virtual
  Type
  type() const;

  /// @brief 入力数を返す．
  ///
  /// 一部の型では固定となっている．
  virtual
  ymuint
  input_num() const;

  /// @brief セルを返す．
  virtual
  const Cell*
  cell() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // セル
  const Cell* mCell;

};


//////////////////////////////////////////////////////////////////////
/// @class BnFuncTypeExpr BnFuncTypeExpr.h "BnFuncTypeExpr.h"
/// @brief 論理式タイプ
//////////////////////////////////////////////////////////////////////
class BnFuncTypeExpr :
  public BnFuncTypeImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] expr 論理式
  /// @param[in] ni 入力数
  BnFuncTypeExpr(ymuint id,
		 Expr expr,
		 ymuint ni);

  /// @brief デストラクタ
  ~BnFuncTypeExpr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を返す．
  virtual
  Type
  type() const;

  /// @brief 入力数を返す．
  ///
  /// 一部の型では固定となっている．
  virtual
  ymuint
  input_num() const;

  /// @brief 論理式を返す．
  virtual
  Expr
  expr() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 論理式
  Expr mExpr;

  // 入力数
  ymuint mInputNum;

};


//////////////////////////////////////////////////////////////////////
/// @class BnFuncTypeTv BnFuncTypeTv.h "BnFuncTypeTv.h"
/// @brief 真理値ベクタタイプ
//////////////////////////////////////////////////////////////////////
class BnFuncTypeTv :
  public BnFuncTypeImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] tv 真理値表ベクタ
  BnFuncTypeTv(ymuint id,
	       const TvFunc& tv);

  /// @brief デストラクタ
  ~BnFuncTypeTv();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を返す．
  virtual
  Type
  type() const;

  /// @brief 入力数を返す．
  ///
  /// 一部の型では固定となっている．
  virtual
  ymuint
  input_num() const;

  /// @brief 真理値表を返す．
  virtual
  TvFunc
  truth_vector() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 真理値表ベクタ
  TvFunc mFunc;

};

END_NAMESPACE_YM_BNET

#endif // BNFUNCTYPEIMPL_H