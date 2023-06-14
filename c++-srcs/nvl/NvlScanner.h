#ifndef NVLSCANNER_H
#define NVLSCANNER_H

/// @file NvlScanner.h
/// @brief NvlScanner のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2019, 2021, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/nvl_nsdef.h"
#include "ym/Scanner.h"
#include "ym/StrBuff.h"
#include "NvlToken.h"


BEGIN_NAMESPACE_YM_NVL

//////////////////////////////////////////////////////////////////////
/// @class NvlScanner NvlScanner.h "NvlScanner.h"
/// @brief nvl 用の字句解析器
//////////////////////////////////////////////////////////////////////
class NvlScanner :
  public Scanner
{
public:

  /// @brief コンストラクタ
  NvlScanner(
    istream& s,               ///< [in] 入力ストリーム
    const FileInfo& file_info ///< [in] ファイル情報
  );

  /// @brief デストラクタ
  ~NvlScanner() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief トークンを一つ読み出す．
  NvlToken
  read_token();

  /// @brief 最後の read_token() で読み出した字句の文字列を返す．
  string
  cur_string() { return string{mCurString.c_str()}; }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief read_token() の下請け関数
  /// @return トークンを返す．
  NvlToken::Type
  scan();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 文字列バッファ
  StrBuff mCurString;

  // 予約語の辞書
  unordered_map<string, NvlToken::Type> mRsvDict;

};

END_NAMESPACE_YM_NVL

#endif // NVLSCANNER_H
