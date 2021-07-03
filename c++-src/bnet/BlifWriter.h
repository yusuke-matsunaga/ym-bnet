#ifndef BLIFWRITER_H
#define BLIFWRITER_H

/// @file BlifWriter.h
/// @brief BlifWriter のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "WriterBase.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BlifWriter BlifWriter.h "BlifWriter.h"
/// @brief BnNetwork を blif 形式で出力するクラス
//////////////////////////////////////////////////////////////////////
class BlifWriter :
  public WriterBase
{
public:

  /// @brief コンストラクタ
  BlifWriter(
    const BnNetwork& network, ///< [in] 対象のネットワーク
    const string& prefix,     ///< [in] 自動生成名の接頭語
    const string& suffix      ///< [in] 自動生成名の接尾語
  );

  /// @brief デストラクタ
  ~BlifWriter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief blif 形式で出力する．
  void
  operator()(
    ostream& s ///< [in] 出力先のストリーム
  );

};

END_NAMESPACE_YM_BNET

#endif // BLIFWRITER_H
