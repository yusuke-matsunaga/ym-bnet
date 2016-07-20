
/// @file BnIscas89Reader.cc
/// @brief BnIscas89Reader の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnIscas89Reader.h"
#include "ym/BnBuilder.h"
#include "ym/Iscas89Parser.h"
#include "BnIscas89Handler.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnIscas89Reader
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] clock_name クロック端子名
BnIscas89Reader::BnIscas89Reader(const string& clock_name) :
  mClockName(clock_name)
{
}

// @brief デストラクタ
BnIscas89Reader::~BnIscas89Reader()
{
}

// @brief blif ファイルを読み込む．
// @param[in] builder 設定対象のビルダーオブジェクト
// @param[in] filename ファイル名
// @return 読み込みが成功したら true を返す．
bool
BnIscas89Reader::read(BnBuilder& builder,
		      const string& filename)
{
  BnIscas89Handler* handler = new BnIscas89Handler(&builder, mClockName);

  Iscas89Parser parser;
  parser.add_handler(handler);

  bool stat = parser.read(filename);

  return stat;
}

END_NAMESPACE_YM_BNET
