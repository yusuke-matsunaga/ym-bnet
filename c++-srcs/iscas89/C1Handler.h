#ifndef C1HANDLER_H
#define C1HANDLER_H

/// @file C1Handler.h
/// @brief C1Handler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Handler.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
/// @class C1Handler C1Handler.h "C1Handler.h"
/// @brief C1 用の拡張ハンドラ
//////////////////////////////////////////////////////////////////////
class C1Handler :
  public Iscas89Handler
{
public:

  /// @brief コンストラクタ
  C1Handler() = default;

  /// @brief デストラクタ
  ~C1Handler() = default;


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

#endif // C1HANDLER_H
