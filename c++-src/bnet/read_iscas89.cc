
/// @file BnIscas89Reader.cc
/// @brief BnIscas89Reader の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/Iscas89Parser.h"
#include "BnIscas89Handler.h"


BEGIN_NAMESPACE_YM_BNET

// @brief blif ファイルを読み込む．
// @param[in] network 設定対象のネットワーク
// @param[in] filename ファイル名
// @param[in] clock_name クロック端子名
// @return 読み込みが成功したら true を返す．
bool
read_iscas89(BnNetwork& network,
	     const string& filename,
	     const string& clock_name)
{
  Iscas89Parser parser;
  BnIscas89Handler handler(parser, &network, clock_name);

  bool stat = parser.read(filename);

  return stat;
}

END_NAMESPACE_YM_BNET
