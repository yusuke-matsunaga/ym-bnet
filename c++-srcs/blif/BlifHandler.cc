
/// @file BlifHandler.cc
/// @brief BlibHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BlifHandler.h"
#include "BlifParser.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BlifHandler
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifHandler::BlifHandler(
  BlifParser& parser
) : mParser{parser}
{
  mParser.add_handler(this);
}

// @brief ID番号から文字列を得る．
const string&
BlifHandler::id2str(
  SizeType id
)
{
  return mParser.id2str(id);
}

// @brief ID番号からそれに関連した位置情報を得る．
const FileRegion&
BlifHandler::id2loc(
  SizeType id
)
{
  return mParser.id2loc(id);
}

// @brief カバーの数を得る．
SizeType
BlifHandler::cover_num()
{
  return mParser.cover_num();
}

// @brief カバーIDからカバーを得る．
const BlifCover&
BlifHandler::id2cover(
  SizeType id
)
{
  return mParser.id2cover(id);
}

END_NAMESPACE_YM_BNET
