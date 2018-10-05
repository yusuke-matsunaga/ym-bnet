
/// @file BnBlifReader.cc
/// @brief BnBlifReader の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BlifParser.h"
#include "BnBlifHandler.h"



BEGIN_NAMESPACE_YM_BNET

// @brief blif ファイルを読み込む．
// @param[in] network 設定対象のネットワーク
// @param[in] filename ファイル名
// @param[in] clock_name クロック端子名
// @param[in] reset_name リセット端子名
// @return 読み込みが成功したら true を返す．
bool
read_blif(BnNetwork& network,
	  const string& filename,
	  const string& clock_name,
	  const string& reset_name)
{
  BlifParser parser;
  BnBlifHandler handler(parser, &network, clock_name, reset_name);

  bool stat = parser.read(filename);

  return stat;
}

// @brief blif ファイルを読み込む．
// @param[in] network 設定対象のネットワーク
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @param[in] clock_name クロック端子名
// @param[in] reset_name リセット端子名
// @return 読み込みが成功したら true を返す．
bool
read_blif(BnNetwork& network,
	  const string& filename,
	  const ClibCellLibrary& cell_library,
	  const string& clock_name,
	  const string& reset_name)
{
  BlifParser parser;
  BnBlifHandler handler(parser, &network, clock_name, reset_name);

  bool stat = parser.read(filename, cell_library);

  return stat;
}

END_NAMESPACE_YM_BNET