
/// @file BlifParser.cc
/// @brief BlifParser の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BlifParser.h"
#include "ParserImpl.h"


BEGIN_NAMESPACE_YM_BLIF

// @brief コンストラクタ
BlifParser::BlifParser()
{
  mImpl = new ParserImpl;
}

// @brief デストラクタ
BlifParser::~BlifParser()
{
  delete mImpl;
}

// @brief 読み込みを行う．
bool
BlifParser::read(
  const string& filename, ///< [in] ファイル名
  BlifModel& model        ///< [out] 結果を格納する変数
)
{
  return read(filename, ClibCellLibrary{}, model);
}

// @brief 読み込みを行う(セルライブラリ付き)．
bool
BlifParser::read(
  const string& filename,              ///< [in] ファイル名
  const ClibCellLibrary& cell_library, ///< [in] セルライブラリ
  BlifModel& model                     ///< [out] 結果を格納する変数
)
{
  return mImpl->read(filename, cell_library, model);
}

END_NAMESPACE_YM_BLIF
