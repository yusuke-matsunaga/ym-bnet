
/// @file BnBlifReader.cc
/// @brief BnBlifReader の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnBlifReader.h"

#include "ym/BlifParser.h"
#include "ym/BnBuilder.h"
#include "ym/BnNetwork.h"

#include "BnBlifHandler.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnBlifReader
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] clock_name クロック端子名
BnBlifReader::BnBlifReader(const string& clock_name,
			   const string& reset_name) :
  mClockName(clock_name),
  mResetName(reset_name)
{
}

// @brief デストラクタ
BnBlifReader::~BnBlifReader()
{
}

// @brief blif ファイルを読み込む．
// @param[in] network 設定対象のネットワーク
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
bool
BnBlifReader::read(BnNetwork& network,
		   const string& filename,
		   const CellLibrary* cell_library)
{
  BnBuilder builder;

  BnBlifHandler* handler = new BnBlifHandler(&builder, mClockName, mResetName);

  BlifParser parser;
  parser.add_handler(handler);

  bool stat = parser.read(filename, cell_library);
  if ( !stat ) {
    return false;
  }

  network.copy(builder);

  return true;
}

END_NAMESPACE_YM_BNET
