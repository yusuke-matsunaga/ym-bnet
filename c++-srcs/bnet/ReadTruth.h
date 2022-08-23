#ifndef READTRUTH_H
#define READTRUTH_H

/// @file ReadTruth.h
/// @brief ReadTruth のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnModifier.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class ReadTruth ReadTruth.h "ReadTruth.h"
/// @brief 真理値表形式のファイルを読むためのクラス
//////////////////////////////////////////////////////////////////////
class ReadTruth :
  public BnModifier
{
public:

  /// @brief コンストラクタ
  ReadTruth() = default;

  /// @brief デストラクタ
  ~ReadTruth() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 真理値表形式のファイルを読み込む．
  void
  read(
    istream& s ///< [in] 入力ストリーム
  );

};

END_NAMESPACE_YM_BNET

#endif // READTRUTH_H
