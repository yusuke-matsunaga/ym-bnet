#ifndef YM_ISCAS89_NSDEF_H
#define YM_ISCAS89_NSDEF_H

/// @file ym/iscas89_nsdef.h
/// @brief ym-iscas89 パッケージの基本ヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym_config.h"


/// @brief ym-iscas89 用の名前空間の開始
#define BEGIN_NAMESPACE_YM_ISCAS89 \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsIscas89)

/// @brief ym-iscas89 用の名前空間の終了
#define END_NAMESPACE_YM_ISCAS89 \
END_NAMESPACE(nsIscas89) \
END_NAMESPACE_YM


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
// クラスの先行宣言
//////////////////////////////////////////////////////////////////////

class Iscas89Parser;
class Iscas89ExParser;
class Iscas89Handler;
class Iscas89Model;

END_NAMESPACE_YM_ISCAS89


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @brief ノードの種類
//////////////////////////////////////////////////////////////////////

enum class Iscas89Type {
  None,   ///< 不正値
  Input,  ///< 入力
  Dff,    ///< DFF
  Gate,   ///< 論理ゲート
  Complex ///< 複合ゲート
};

using nsIscas89::Iscas89Parser;
using nsIscas89::Iscas89ExParser;
using nsIscas89::Iscas89Handler;
using nsIscas89::Iscas89Model;

END_NAMESPACE_YM

#endif // YM_ISCAS89_NSDEF_H
