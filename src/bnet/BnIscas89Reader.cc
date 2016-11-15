
/// @file BnIscas89Reader.cc
/// @brief BnIscas89Reader の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnIscas89Reader.h"
#include "ym/Iscas89Parser.h"
#include "BnIscas89Handler.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnIscas89Reader
//////////////////////////////////////////////////////////////////////

// @brief blif ファイルを読み込む．
// @param[in] network 設定対象のネットワーク
// @param[in] filename ファイル名
// @param[in] clock_name クロック端子名
// @return 読み込みが成功したら true を返す．
bool
BnIscas89Reader::read(BnNetwork& network,
		      const string& filename,
		      const string& clock_name)
{
  BnIscas89Handler* handler = new BnIscas89Handler(&network, clock_name);

  Iscas89Parser parser;
  parser.add_handler(handler);

  bool stat = parser.read(filename);

  return stat;
}

END_NAMESPACE_YM_BNET
