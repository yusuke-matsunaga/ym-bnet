
/// @file BnIscas89Reader.cc
/// @brief BnIscas89Reader の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/Iscas89Parser.h"
#include "ym/BnNetwork.h"
#include "BnIscas89Handler.h"


BEGIN_NAMESPACE_YM_BNET

// @brief blif ファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] clock_name クロック端子名
// @return ネットワークを返す．
BnNetwork
BnNetwork::read_iscas89(const string& filename,
			const string& clock_name)
{
  BnNetwork network;

  Iscas89Parser parser;
  BnIscas89Handler handler(parser, &network, clock_name);

  bool stat = parser.read(filename);
  if ( !stat ) {
    network.clear();
  }

  return network;
}

END_NAMESPACE_YM_BNET
