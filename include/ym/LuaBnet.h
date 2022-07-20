#ifndef LUABNET_H
#define LUABNET_H

/// @file LuaBnet.h
/// @brief LuaBnet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include <lua.hpp>


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class LuaBnet LuaBnet.h "LuaBnet.h"
/// @brief lua の BnNetwork 拡張
//////////////////////////////////////////////////////////////////////
class LuaBnet
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief BnNetwork 関係の初期化を行う．
  static
  void
  init(
    lua_State* L,                  ///< [in] lua インタープリタ
    vector<struct luaL_Reg>& mylib ///< [out] モジュールに登録する関数のリスト
  );

  /// @brief 対象が BnNetwork の時 true を返す．
  static
  bool
  is_bnet(
    lua_State* L, ///< [in] lua インタープリタ
    int idx       ///< [in] スタック上のインデックス
  );

  /// @brief 対象を BnNetwork として取り出す．
  ///
  /// BnNetwork でない時は nullptr を返す．
  static
  BnNetwork*
  to_bnet(
    lua_State* L, ///< [in] lua インタープリタ
    int idx       ///< [in] スタック上のインデックス
  );

};

END_NAMESPACE_YM

#endif // LUABNET_H
