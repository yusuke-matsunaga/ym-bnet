#ifndef FUNCANALYZER_H
#define FUNCANALYZER_H

/// @file FuncAnalyzer.h
/// @brief FuncAnalyzer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "ym/ym_bnet.h"
#include "ym/ym_logic.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class FuncAnalyzer FuncAnalyzer.h "FuncAnalyzer.h"
/// @brief 組み込みタイプの論理型を見つけるクラス
///
/// 実はクラスにする必要はない．
//////////////////////////////////////////////////////////////////////
class FuncAnalyzer
{
public:


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 与えられた論理式が組み込み型かどうか判定する．
  ///
  /// 組み込み型でない場合には kBnLt_EXPR が返される．
  static
  BnLogicType
  analyze(const Expr& expr);

  /// @brief 与えられた真理値表が組み込み型かどうか判定する．
  ///
  /// 組み込み型でない場合には kBnLt_TV が返される．
  static
  BnLogicType
  analyze(const TvFunc& func);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////


};

END_NAMESPACE_YM_BNET

#endif // FUNCANALYZER_H
