#ifndef BLIFINPUT_H
#define BLIFINPUT_H

/// @file BlifInput.h
/// @brief BlifInput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BlifNode.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
/// @class BlifInput BlifInput.h "BlifInput.h"
/// @brief .inputs の要素を表す BlifAst
//////////////////////////////////////////////////////////////////////
class BlifInput :
  public BlifNode
{
public:

  /// @brief コンストラクタ
  BlifInput(
    SizeType id,       ///< [in] ID番号
    const string& name ///< [in] 名前
  );

  /// @brief デストラクタ
  ~BlifInput();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  Type
  type() const override;

};

END_NAMESPACE_YM_BLIF

#endif // BLIFINPUT_H
