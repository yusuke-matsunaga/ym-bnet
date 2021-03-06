#ifndef FUNCANALYZER_H
#define FUNCANALYZER_H

/// @file FuncAnalyzer.h
/// @brief FuncAnalyzer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/logic.h"


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
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 与えられた論理式が組み込み型かどうか判定する．
  ///
  /// 組み込み型でない場合には kBnExpr が返される．
  static
  BnNodeType
  analyze(
    const Expr& expr ///< [in] 対象の式
  );

  /// @brief 与えられた真理値表が組み込み型かどうか判定する．
  ///
  /// 組み込み型でない場合には kBnTvFunc が返される．
  static
  BnNodeType
  analyze(
    const TvFunc& func ///< [in] 真理値表
  );

};

END_NAMESPACE_YM_BNET

#endif // FUNCANALYZER_H
