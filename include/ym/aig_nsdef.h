#ifndef YM_AIG_NSDEF_H
#define YM_AIG_NSDEF_H

/// @file ym/aig_nsdef.h
/// @brief ym-aig パッケージの基本ヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym_config.h"


/// @brief ym-aig 用の名前空間の開始
#define BEGIN_NAMESPACE_YM_AIG \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsAig)

/// @brief ym-aig 用の名前空間の終了
#define END_NAMESPACE_YM_AIG \
END_NAMESPACE(nsAig) \
END_NAMESPACE_YM


BEGIN_NAMESPACE_YM_AIG

//////////////////////////////////////////////////////////////////////
// クラスの先行宣言
//////////////////////////////////////////////////////////////////////

class AigModel;

END_NAMESPACE_YM_AIG


BEGIN_NAMESPACE_YM

using nsAig::AigModel;

END_NAMESPACE_YM

#endif // YM_AIG_NSDEF_H
