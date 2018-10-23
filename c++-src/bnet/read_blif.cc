
/// @file BnBlifReader.cc
/// @brief BnBlifReader の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BlifParser.h"
#include "ym/BnNetwork.h"
#include "ym/ClibCellLibrary.h"
#include "BnBlifHandler.h"



BEGIN_NAMESPACE_YM_BNET

// @brief blif ファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] clock_name クロック端子名
// @param[in] reset_name リセット端子名
// @return ネットワークを返す．
BnNetwork
BnNetwork::read_blif(const string& filename,
		     const string& clock_name,
		     const string& reset_name)
{
  return read_blif(filename, ClibCellLibrary(), clock_name, reset_name);
}

// @brief blif ファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @param[in] clock_name クロック端子名
// @param[in] reset_name リセット端子名
// @return ネットワークを返す．
BnNetwork
BnNetwork::read_blif(const string& filename,
		     const ClibCellLibrary& cell_library,
		     const string& clock_name,
		     const string& reset_name)
{
  BnNetwork network;

  BlifParser parser;
  BnBlifHandler handler(parser, &network, clock_name, reset_name);

  bool stat = parser.read(filename, cell_library);
  if ( !stat ) {
    network.clear();
  }

  return network;
}

END_NAMESPACE_YM_BNET
