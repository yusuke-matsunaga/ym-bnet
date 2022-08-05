#ifndef BNETDUMPER_H
#define BNETDUMPER_H

/// @file BnetDumper.h
/// @brief BnetDumper のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/BinEnc.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnetDumper BnetDumper.h "BnetDumper.h"
/// @brief BnNetwork の内容を独自形式で書き出すクラス
//////////////////////////////////////////////////////////////////////
class BnetDumper
{
public:

  /// @brief コンストラクタ
  BnetDumper() = default;

  /// @brief デストラクタ
  ~BnetDumper() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief BnNetwork の内容をバイナリダンプする．
  void
  operator()(
    BinEnc& s,               ///< [in] 出力ストリーム
    const BnNetwork& network ///< [in] ネットワーク
  ) const;

};

END_NAMESPACE_YM_BNET

#endif // BNETDUMPER_H
