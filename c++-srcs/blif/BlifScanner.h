#ifndef BLIFSCANNER_H
#define BLIFSCANNER_H

/// @file BlifScanner.h
/// @brief BlibScanner のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2019, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/blif_nsdef.h"
#include "ym/Scanner.h"
#include "ym/StrBuff.h"
#include "BlifToken.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
/// @class BlifScanner BlifScanner.h "BlifScanner.h"
/// @brief blif 用の字句解析器
//////////////////////////////////////////////////////////////////////
class BlifScanner :
  public Scanner
{
public:

  /// @brief コンストラクタ
  BlifScanner(
    istream& s,               ///< [in] 入力ストリーム
    const FileInfo& file_info ///< [in] ファイル情報
  );

  /// @brief デストラクタ
  ~BlifScanner() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief トークンを一つ読み出す．
  BlifToken
  read_token(
    FileRegion& loc ///< [out] トークンの位置を格納する変数
  );

  /// @brief 最後の get_token() で読み出した字句の文字列を返す．
  string
  cur_string() { return mCurString; }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief read_token() の下請け関数
  /// @return トークンを返す．
  BlifToken
  scan();

  /// @brief 予約後の検査をする．
  /// @return トークンを返す．
  BlifToken
  check_word(
    bool start_with_dot ///< [in] '.' で始まっている時に true を渡す．
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 予約語テーブル
  unordered_map<string, BlifToken> mDic;

  // 文字列バッファ
  StrBuff mCurString;

};

END_NAMESPACE_YM_BLIF

#endif // BLIFSCANNER_H
