#ifndef ISCAS89WRITER_H
#define ISCAS89WRITER_H

/// @file Iscas89Writer.h
/// @brief Iscas89Writer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "WriterBase.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Writer Iscas89Writer.h "Iscas89Writer.h"
/// @brief BnNetwork を iscas89(.bench) 形式で出力するクラス
//////////////////////////////////////////////////////////////////////
class Iscas89Writer :
  public WriterBase
{
public:

  /// @brief コンストラクタ
  Iscas89Writer(
    const BnNetwork& network, ///< [in] 対象のネットワーク
    const string& prefix,     ///< [in] 自動生成名の接頭語
    const string& suffix      ///< [in] 自動生成名の接尾語
  );

  /// @brief デストラクタ
  ~Iscas89Writer() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief iscas89(.bench) 形式で出力する．
  void
  operator()(
    ostream& s ///< [in] 出力先のストリーム
  );

};

END_NAMESPACE_YM_BNET

#endif // ISCAS89WRITER_H
