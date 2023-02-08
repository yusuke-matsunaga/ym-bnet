#ifndef C0HANDLER_H
#define C0HANDLER_H

/// @file C0Handler.h
/// @brief C0Handler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Handler.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
/// @class C0Handler C0Handler.h "C0Handler.h"
/// @brief C0 用の拡張ハンドラ
//////////////////////////////////////////////////////////////////////
class C0Handler :
  public Iscas89Handler
{
public:

  /// @brief コンストラクタ
  C0Handler() = default;

  /// @brief デストラクタ
  ~C0Handler() = default;


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

#endif // C0HANDLER_H
