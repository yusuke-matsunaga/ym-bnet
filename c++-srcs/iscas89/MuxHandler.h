#ifndef MUXHANDLER_H
#define MUXHANDLER_H

/// @file MuxHandler.h
/// @brief MuxHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Handler.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
/// @class MuxHandler MuxHandler.h "MuxHandler.h"
/// @brief MUX 用の拡張ハンドラ
//////////////////////////////////////////////////////////////////////
class MuxHandler :
  public Iscas89Handler
{
public:

  /// @brief コンストラクタ
  MuxHandler() = default;

  /// @brief デストラクタ
  ~MuxHandler() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートの読み込みを行う．
  /// @return エラーが起きたら false を返す．
  bool
  read(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType oname_id      ///< [in] 出力名の ID 番号
  ) override;

};

END_NAMESPACE_YM_ISCAS89

#endif // MUXHANDLER_H
