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

class Iscas89Model;

END_NAMESPACE_YM_ISCAS89


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @brief ノードの種類
//////////////////////////////////////////////////////////////////////

enum class Iscas89Type {
  None,  ///< 不正値
  Input, ///< 入力
  Dff,   ///< DFF
  Gate   ///< 論理ゲート
};


//////////////////////////////////////////////////////////////////////
/// @brief 論理ゲートの型
//////////////////////////////////////////////////////////////////////

enum class Iscas89Gate {
  None, ///< 不正値
  C0,   ///< 定数0
  C1,   ///< 定数1
  Buff, ///< バッファ
  Not,  ///< not
  And,  ///< and
  Nand, ///< nand
  Or,   ///< or
  Nor,  ///< nor
  Xor,  ///< xor
  Xnor, ///< xnor
  Mux,  ///< multiplexor
};

inline
ostream&
operator<<(
  ostream& s,
  Iscas89Gate gtype
)
{
  switch ( gtype ) {
  case Iscas89Gate::None: s << "None"; break;
  case Iscas89Gate::C0:   s << "C0"; break;
  case Iscas89Gate::C1:   s << "C1"; break;
  case Iscas89Gate::Buff: s << "Buff"; break;
  case Iscas89Gate::Not:  s << "Not"; break;
  case Iscas89Gate::And:  s << "And"; break;
  case Iscas89Gate::Nand: s << "Nand"; break;
  case Iscas89Gate::Or:   s << "Or"; break;
  case Iscas89Gate::Nor:  s << "Nor"; break;
  case Iscas89Gate::Xor:  s << "Xor"; break;
  case Iscas89Gate::Xnor: s << "Xnor"; break;
  case Iscas89Gate::Mux:  s << "Mux"; break;
  default:                    s << "---"; break;
  }
  return s;
}

using nsIscas89::Iscas89Model;

END_NAMESPACE_YM

#endif // YM_ISCAS89_NSDEF_H
