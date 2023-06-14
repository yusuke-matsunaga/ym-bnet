#ifndef NVLTOKEN_H
#define NVLTOKEN_H

/// @file Token.h
/// @brief Token のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/nvl_nsdef.h"
#include "ym/logic.h"
#include "ym/FileRegion.h"


BEGIN_NAMESPACE_YM_NVL

//////////////////////////////////////////////////////////////////////
/// @class NvlToken NvlToken.h "NvlToken.h"
/// @brief トークンを表すクラス
//////////////////////////////////////////////////////////////////////
class NvlToken
{
public:

  /// @brief トークンの種類を表す列挙型
  enum Type {
    LP,         ///< '('
    RP,         ///< ')'
    COMMA,      ///< ','
    SEMI,       ///< ';'
    MODULE,     ///< 'module'
    ENDMODULE,  ///< 'endmodule'
    INPUT,      ///< 'input'
    OUTPUT,     ///< 'output'
    WIRE,       ///< 'wire'
    BUF,        ///< 'buf'
    NOT,        ///< 'not'
    AND,        ///< 'and'
    NAND,       ///< 'nand'
    OR,         ///< 'or'
    NOR,        ///< 'nor'
    XOR,        ///< 'xor'
    XNOR,       ///< 'xnor'
    C0,         ///< '1'b0'
    C1,         ///< '1'b1'
    STR,        ///< 文字列
    _EOF,       ///< end-of-file
    ERROR       ///< error
  };


public:

  /// @brief 空のコンストラクタ
  NvlToken() = default;

  /// @brief 内容を指定したコンストラクタ
  NvlToken(
    Type type,
    const FileRegion& loc,
    const string& str = {}
  ) : mType{type},
      mLoc{loc},
      mStr{str}
  {
  }

  /// @brief デストラクタ
  ~NvlToken() = default;


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

  /// @brief 識別子を返す．
  ///
  /// type() == STR の時のみ意味を持つ．
  const string&
  str() const { return mStr; }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 種類
  Type mType{ERROR};

  // ファイル上の位置
  FileRegion mLoc;

  // STR 型の場合の識別子
  string mStr;

};

END_NAMESPACE_YM_NVL

#endif // NVLTOKEN_H
