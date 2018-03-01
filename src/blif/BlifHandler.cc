﻿
/// @file BlifHandler.cc
/// @brief BlibHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BlifHandler.h"
#include "BlifParserImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BlifHandler
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifHandler::BlifHandler()
{
}

// @brief デストラクタ
BlifHandler::~BlifHandler()
{
}

// @brief ID番号から文字列を得る．
const char*
BlifHandler::id2str(int id)
{
  return mParser->id2str(id);
}

// @brief ID番号からそれに関連した位置情報を得る．
const FileRegion&
BlifHandler::id2loc(int id)
{
  return mParser->id2loc(id);
}

// @brief ID番号からそれに関連した位置情報を得る．
const FileRegion&
BlifHandler::id2def_loc(int id)
{
  return mParser->id2def_loc(id);
}

// @brief カバーの数を得る．
int
BlifHandler::cover_num()
{
  return mParser->cover_num();
}

// @brief カバーIDからカバーを得る．
const BlifCover*
BlifHandler::id2cover(int id)
{
  return mParser->id2cover(id);
}

END_NAMESPACE_YM_BNET
