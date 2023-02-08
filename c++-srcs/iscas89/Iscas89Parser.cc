
/// @file Iscas89Parser.cc
/// @brief Iscas89Parser の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Parser.h"
#include "ym/Iscas89Model.h"
#include "ParserImpl.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
// クラス Iscas89Parser
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Iscas89Parser::Iscas89Parser()
{
  mImpl = new ParserImpl;
}

// @brief デストラクタ
Iscas89Parser::~Iscas89Parser()
{
  delete mImpl;
}

// @brief 読み込みを行う．
bool
Iscas89Parser::read(
  const string& filename,
  Iscas89Model& model
)
{
  return mImpl->read(filename, model.mImpl);
}

// @brief 拡張ハンドラを登録する．
SizeType
Iscas89Parser::reg_handler(
  Iscas89Handler* handler
)
{
  return mImpl->reg_handler(handler);
}

// @brief 拡張ハンドラを取り出す．
Iscas89Handler*
Iscas89Parser::get_handler(
  SizeType id
)
{
  return mImpl->get_handler(id);
}

// @brief ハンドラとキーワードを結びつける．
void
Iscas89Parser::bind_handler(
  const string& keyword,
  SizeType id
)
{
  mImpl->bind_handler(keyword, id);
}

END_NAMESPACE_YM_ISCAS89
