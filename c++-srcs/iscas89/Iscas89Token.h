#ifndef ISCAS89TOKEN_H
#define ISCAS89TOKEN_H

/// @file Token.h
/// @brief Token のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/iscas89_nsdef.h"
#include "ym/logic.h"
#include "ym/FileRegion.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Token Iscas89Token.h "Iscas89Token.h"
/// @brief トークンを表すクラス
//////////////////////////////////////////////////////////////////////
class Iscas89Token
{
public:

  /// @brief トークンの種類を表す列挙型
  enum Type {
    LPAR,
    RPAR,
    COMMA,
    EQ,
    INPUT,
    OUTPUT,
    GATE, // BUFF, NOT, AND, NAND, OR, NOR, XOR, XNOR
    EXGATE, // 拡張タイプ
    DFF,
    NAME,
    _EOF,
    ERROR
  };


public:

  /// @brief 空のコンストラクタ
  Iscas89Token() = default;

  /// @brief 内容を指定したコンストラクタ
  Iscas89Token(
    Type type,
    const FileRegion& loc,
    PrimType gate_type = PrimType::None,
    const string& name = {},
    SizeType ex_id = 0
  ) : mType{type},
      mLoc{loc},
      mGateType{gate_type},
      mName{name},
      mExId{ex_id}
  {
  }

  /// @brief デストラクタ
  ~Iscas89Token() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  Type
  type() const { return mType; }

  /// @brief ファイル上の位置を返す．
  const FileRegion&
  loc() const { return mLoc; }

  /// @brief ゲート型を返す．
  ///
  /// type() == GATE の時のみ意味を持つ．
  PrimType
  gate_type() const { return mGateType; }

  /// @brief 識別子を返す．
  ///
  /// type() == NAME の時のみ意味を持つ．
  const string&
  name() const { return mName; }

  /// @brief 拡張IDを返す．
  SizeType
  ex_id() const { return mExId; }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 種類
  Type mType{ERROR};

  // ファイル上の位置
  FileRegion mLoc;

  // GATE 型の場合のゲートタイプ
  PrimType mGateType{PrimType::None};

  // NAME 型の場合の識別子
  string mName;

  // 拡張ID
  SizeType mExId;

};

END_NAMESPACE_YM_ISCAS89

#endif // ISCAS89TOKEN_H
