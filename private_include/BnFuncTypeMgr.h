#ifndef BNFUNCTYPEMGR_H
#define BNFUNCTYPEMGR_H

/// @file BnFuncTypeMgr.h
/// @brief BnFuncTypeMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/BnFuncType.h"
#include "ym/Alloc.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnFuncTypeMgr BnFuncTypeMgr.h "BnFuncTypeMgr.h"
/// @brief BnFuncType を管理するクラス
///
/// @todo FuncType の検索を線形探索から効率化する．
//////////////////////////////////////////////////////////////////////
class BnFuncTypeMgr
{
public:

  /// @brief コンストラクタ
  /// @param[in] alloc メモリアロケータ
  BnFuncTypeMgr(Alloc& alloc);

  /// @brief デストラクタ
  ~BnFuncTypeMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief プリミティブタイプを得る．
  /// @param[in] type プリミティブタイプ
  /// @param[in] input_num 入力数
  ///
  /// type がプリミティブタイプでない場合と
  /// input_num が type に合わない場合はエラー
  /// となり nullptr を返す．
  const BnFuncType*
  primitive_type(BnFuncType::Type type,
		 ymuint input_num);

  /// @brief セルタイプを得る．
  /// @param[in] cell セル
  const BnFuncType*
  cell_type(const Cell* cell);

  /// @brief 論理式タイプを得る．
  /// @param[in] expr 論理式
  /// @param[in] input_num 入力数
  const BnFuncType*
  expr_type(Expr expr,
	    ymuint input_num);

  /// @brief 真理値表タイプを得る．
  const BnFuncType*
  tv_type(const TvFunc& tv);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // メモリアロケータ
  Alloc& mAlloc;

  // FuncType のリスト
  vector<BnFuncType*> mFuncTypeList;

};

END_NAMESPACE_YM_BNET

#endif // BNFUNCTYPEMGR_H
