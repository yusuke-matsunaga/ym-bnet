#ifndef YM_BLIFSCANNER_H
#define YM_BLIFSCANNER_H

/// @file ym/BlifScanner.h
/// @brief BlibScanner のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2019, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/InputFileObj.h"
#include "ym/StrBuff.h"
#include "BlifToken.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BlifScanner BlifScanner.h "BlifScanner.h"
/// @brief blif 用の字句解析器
//////////////////////////////////////////////////////////////////////
class BlifScanner
{
public:

  /// @brief コンストラクタ
  BlifScanner(
    InputFileObj& in ///< [in] 入力ファイルオブジェクト
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

  // 入力ファイルオブジェクト
  InputFileObj& mIn;

  // 予約語テーブル
  unordered_map<string, BlifToken> mDic;

  // 文字列バッファ
  string mCurString;

  // 現在のトークンの始まりの位置
  FileLoc mFirstLoc;

};

END_NAMESPACE_YM_BNET

#endif // YM_BLIFSCANNER_H
