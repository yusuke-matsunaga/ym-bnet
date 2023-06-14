
/// @file NvlParser.cc
/// @brief NvlParser の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/NvlParser.h"
#include "ym/NvlModel.h"
#include "ParserImpl.h"


BEGIN_NAMESPACE_YM_NVL

//////////////////////////////////////////////////////////////////////
// クラス NvlParser
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NvlParser::NvlParser()
{
  mImpl = new ParserImpl;
}

// @brief デストラクタ
NvlParser::~NvlParser()
{
  delete mImpl;
}

// @brief 読み込みを行う．
bool
NvlParser::read(
  const string& filename,
  NvlModel& model
)
{
  return mImpl->read(filename, model.mImpl);
}

END_NAMESPACE_YM_NVL
