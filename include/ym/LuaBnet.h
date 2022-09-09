#ifndef LUABNET_H
#define LUABNET_H

/// @file LuaBnet.h
/// @brief LuaBnet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/LuaClib.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class LuaBnet LuaBnet.h "LuaBnet.h"
/// @brief lua の BnNetwork 拡張
//////////////////////////////////////////////////////////////////////
class LuaBnet :
  public LuaClib
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief 空のコンストラクタ
  ///
  /// デフォルトの状態で初期化される．<br>
  /// このオブジェクトとともに Lua インタプリタも破壊される．
  LuaBnet() = default;

  /// @brief 詳細なパラメータを指定したコンストラクタ
  ///
  /// このオブジェクトとともに Lua インタプリタも破壊される．
  LuaBnet(
    lua_Alloc f, ///< [in] メモリアロケータ
    void* ud     ///< [in] ユーザーデータ
  ) : LuaClib{f, ud}
  {
  }

  /// @brief すでに生成済みのインタプリタを用いるコンストラクタ
  ///
  /// このオブジェクトが破壊されても Lua インタプリタは破壊されない．
  LuaBnet(
    lua_State* L ///< [in] lua インタプリタ
  ) : LuaClib{L}
  {
  }

  /// @brief デストラクタ
  ///
  /// 自前で Lua インタプリタを生成した場合には破壊される．
  ~LuaBnet() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief BnNetwork 関係の初期化を行う．
  void
  init(
    const char* parent, ///< [in] 親のモジュール名
    const char* name    ///< [in] 自身の名前
  );

  /// @brief 対象が BnNetwork の時 true を返す．
  bool
  is_bnet(
    int idx       ///< [in] スタック上のインデックス
  )
  {
    auto obj = to_bnet(idx);
    return obj != nullptr;
  }

  /// @brief 対象を BnNetwork として取り出す．
  ///
  /// BnNetwork でない時は nullptr を返す．
  BnNetwork*
  to_bnet(
    int idx       ///< [in] スタック上のインデックス
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief BnNetwork 関係の初期化を行う．
  static
  void
  init(
    lua_State* L,       ///< [in] lua インタープリタ
    const char* parent, ///< [in] 親のモジュール名
    const char* name    ///< [in] 自身の名前
  )
  {
    LuaBnet lua{L};
    lua.init(parent, name);
  }

  /// @brief 対象が BnNetwork の時 true を返す．
  static
  bool
  is_bnet(
    lua_State* L, ///< [in] lua インタープリタ
    int idx       ///< [in] スタック上のインデックス
  )
  {
    auto obj = to_bnet(L, idx);
    return obj != nullptr;
  }

  /// @brief 対象を BnNetwork として取り出す．
  ///
  /// BnNetwork でない時は nullptr を返す．
  static
  BnNetwork*
  to_bnet(
    lua_State* L, ///< [in] lua インタープリタ
    int idx       ///< [in] スタック上のインデックス
  )
  {
    LuaBnet lua{L};
    return lua.to_bnet(idx);
  }

};

/// @brief Lua用モジュールの初期化関数
extern
int
luaopen_bnet(
  lua_State* L
);

END_NAMESPACE_YM

#endif // LUABNET_H
