﻿#ifndef YM_BLIF_NSDEF_H
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

class BlifParser;
class BlifModel;
class BlifNode;
class BlifCover;

END_NAMESPACE_YM_BLIF


BEGIN_NAMESPACE_YM

using nsBlif::BlifParser;
using nsBlif::BlifModel;
using nsBlif::BlifNode;
using nsBlif::BlifCover;

END_NAMESPACE_YM

#endif // YM_BLIF_NSDEF_H
