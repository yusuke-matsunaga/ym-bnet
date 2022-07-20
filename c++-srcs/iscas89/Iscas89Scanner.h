#ifndef ISCAS89SCANNER_H
#define ISCAS89SCANNER_H

/// @file Iscas89Scanner.h
/// @brief Iscas89Scanner のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2019, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/Scanner.h"
#include "ym/StrBuff.h"
#include "Iscas89Token.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Scanner Iscas89Scanner.h "Iscas89Scanner.h"
/// @brief iscas89 用の字句解析器
//////////////////////////////////////////////////////////////////////
class Iscas89Scanner :
  public Scanner
{
public:

  /// @brief コンストラクタ
  Iscas89Scanner(
    istream& s,               ///< [in] 入力ストリーム
    const FileInfo& file_info ///< [in] ファイル情報
  );

  /// @brief デストラクタ
  ~Iscas89Scanner() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief トークンを一つ読み出す．
  /// @param[out] loc トークンの位置を格納する変数
  Iscas89Token
  read_token(FileRegion& loc);

  /// @brief 最後の read_token() で読み出した字句の文字列を返す．
  const char*
  cur_string() { return mCurString.c_str(); }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief read_token() の下請け関数
  /// @return トークンを返す．
  Iscas89Token
  scan();

  /// @brief 予約語の検査を行う．
  /// @return トークンを返す．
  ///
  /// 予約語でなければ NAME を返す．
  Iscas89Token
  check_word();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 文字列バッファ
  StrBuff mCurString;

  // 文字列の先頭の位置
  FileLoc mFirstLoc;

};

END_NAMESPACE_YM_BNET

#endif // ISCAS89SCANNER_H
