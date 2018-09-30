#ifndef BLIFDIC_H
#define BLIFDIC_H

/// @file BlifDic.h
/// @brief BlibDic のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

/// @brief blif 形式のトークン
enum class BlifToken {
  _EOF,
  NL,
  STRING,
  EQ,
  MODEL,
  INPUTS,
  OUTPUTS,
  CLOCK,
  END,
  NAMES,
  EXDC,
  LATCH,
  GATE,
  MLATCH,
  SUBCKT,
  SEARCH,
  START_KISS,
  I,
  O,
  P,
  R,
  END_KISS,
  LATCH_ORDER,
  CODE,
  CYCLE,
  CLOCK_EVENT,
  AREA,
  DELAY,
  WIRE_LOAD_SLOPE,
  WIRE,
  INPUT_ARRIVAL,
  DEFAULT_INPUT_ARRIVAL,
  OUTPUT_REQUIRED,
  DEFAULT_OUTPUT_REQUIRED,
  INPUT_DRIVE,
  DEFAULT_INPUT_DRIVE,
  OUTPUT_LOAD,
  DEFAULT_OUTPUT_LOAD
};


//////////////////////////////////////////////////////////////////////
/// @class BlifDic BlifDic.h "BlifDic.h"
/// @brief blif 用の予約語辞書
//////////////////////////////////////////////////////////////////////
class BlifDic
{
public:

  /// @brief コンストラクタ
  BlifDic();

  /// @brief デストラクタ
  ~BlifDic();


public:

  /// @brief str に対応したトークンを返す．
  BlifToken
  get_token(const char* str);

  /// @brief トークンの内容を出力する．
  static
  void
  dump_token(ostream& s,
	     BlifToken token);


private:

  struct Cell {

    // 予約語の文字列
    const char* mStr;

    // 予約語のトークン
    BlifToken mToken;

    // ハッシュ表の次の要素を指すポインタ
    Cell* mLink;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ハッシュ表のサイズ
  int mTableSize;

  // ハッシュ表
  Cell** mHashTable;

  // 全セルの先頭
  Cell* mCellBlock;

};

/// @relates Token
/// @brief トークンを出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] token トークン
ostream&
operator<<(ostream& s,
	   BlifToken token);


END_NAMESPACE_YM_BNET

#endif // BLIFDIC_H
