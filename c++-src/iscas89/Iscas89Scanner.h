#ifndef ISCAS89SCANNER_H
#define ISCAS89SCANNER_H

/// @file Iscas89Scanner.h
/// @brief Iscas89Scanner のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
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
  /// @param[in] s 入力ストリーム
  /// @param[in] file_info ファイル情報
  Iscas89Scanner(istream& s,
		 const FileInfo& file_info);

  /// @brief デストラクタ
  ~Iscas89Scanner();


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
  cur_string();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief read_token() の下請け関数
  /// @return トークンを返す．
  Iscas89Token
  scan();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 文字列バッファ
  StrBuff mCurString;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 最後の read_token() で読み出した字句の文字列を返す．
inline
const char*
Iscas89Scanner::cur_string()
{
  return mCurString.c_str();
}

END_NAMESPACE_YM_BNET

#endif // ISCAS89SCANNER_H
