#ifndef ISCAS89EXPARSER_H
#define ISCAS89EXPARSER_H

/// @file Iscas89ExParser.h
/// @brief Iscas89ExParser のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Parser.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
/// @class Iscas89ExParser Iscas89ExParser.h "Iscas89ExParser.h"
/// @brief 拡張 iscas89 パーサー
///
/// インターフェイスは Iscas89Parser の同一
//////////////////////////////////////////////////////////////////////
class Iscas89ExParser :
  public Iscas89Parser
{
public:

  /// @brief コンストラクタ
  Iscas89ExParser();

  /// @brief デストラクタ
  ~Iscas89ExParser() = default;

};

END_NAMESPACE_YM_ISCAS89

#endif // ISCAS89EXPARSER_H
