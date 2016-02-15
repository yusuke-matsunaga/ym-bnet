#ifndef BNFUNCTYPE_H
#define BNFUNCTYPE_H

/// @file BnGateType.h
/// @brief BnGateType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/ym_cell.h"
#include "ym/ym_logic.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnFuncType BnFuncType.h "BnFuncType.h"
/// @brief BnNode の論理関数を表すクラス
/// @sa BnNode
//////////////////////////////////////////////////////////////////////
class BnFuncType
{
public:
  //////////////////////////////////////////////////////////////////////
  // 型を表す列挙型
  //////////////////////////////////////////////////////////////////////
  enum Type {
    kFt_C0,
    kFt_C1,
    kFt_BUFF,
    kFt_NOT,
    kFt_AND,
    kFt_NAND,
    kFt_OR,
    kFt_NOR,
    kFt_XOR,
    kFt_XNOR,
    kFt_CELL,
    kFt_EXPR,
    kFt_TV
  };


public:

  /// @brief デストラクタ
  virtual
  ~BnFuncType() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を返す．
  virtual
  Type
  type() const = 0;

  /// @brief 入力数を返す．
  ///
  /// 一部の型では固定となっている．
  virtual
  ymuint
  input_num() const = 0;

  /// @brief セルを返す．
  virtual
  const Cell*
  cell() const = 0;

  /// @brief 論理式を返す．
  virtual
  Expr
  expr() const = 0;

  /// @brief 真理値表を返す．
  virtual
  TvFunc
  truth_vector() const = 0;

};

/// @brief BnFuncType のストリーム出力演算子
ostream&
operator<<(ostream& s,
	   const BnFuncType* func_type);

END_NAMESPACE_YM_BNET

#endif // BNFUNCTYPE_H
