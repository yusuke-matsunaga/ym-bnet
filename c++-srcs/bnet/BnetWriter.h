#ifndef BNETWRITER_H
#define BNETWRITER_H

/// @file BnetWriter.h
/// @brief BnetWriter のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnetWriter BnetWriter.h "BnetWriter.h"
/// @brief BnNetwork の内容を独自形式で書き出すクラス
//////////////////////////////////////////////////////////////////////
class BnetWriter
{
public:

  /// @brief コンストラクタ
  BnetWriter() = default;

  /// @brief デストラクタ
  ~BnetWriter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief BnNetwork の内容を独自形式で書き出す．
  void
  operator()(
    ostream& s,              ///< [in] 出力ストリーム
    const BnNetwork& network ///< [in] ネットワーク
  ) const;

};

END_NAMESPACE_YM_BNET

#endif // BNETWRITER_H
