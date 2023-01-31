#ifndef YM_BLIF_NSDEF_H
#define YM_BLIF_NSDEF_H

/// @file ym/blif_nsdef.h
/// @brief ym-blif パッケージの基本ヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym_config.h"


/// @brief ym-blif 用の名前空間の開始
#define BEGIN_NAMESPACE_YM_BLIF \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsBlif)

/// @brief ym-blif 用の名前空間の終了
#define END_NAMESPACE_YM_BLIF \
END_NAMESPACE(nsBlif) \
END_NAMESPACE_YM


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
// クラスの先行宣言
//////////////////////////////////////////////////////////////////////

class BlifModel;
class BlifCover;

END_NAMESPACE_YM_BLIF


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @brief ノードの種類を表す列挙型
//////////////////////////////////////////////////////////////////////
enum BlifType {
  None,  ///< 不正値
  Input, ///< 入力(.inputs)
  Dff,   ///< DFF(.latch)
  Cover, ///< カバー型論理ノード(.names)
  Cell   ///< セル型論理ノード(.gate)
};

using nsBlif::BlifModel;
using nsBlif::BlifCover;

END_NAMESPACE_YM

#endif // YM_BLIF_NSDEF_H
