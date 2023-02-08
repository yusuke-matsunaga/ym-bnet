
/// @file Iscas89ExParser.cc
/// @brief Iscas89ExParser の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89ExParser.h"
#include "MuxHandler.h"
#include "C0Handler.h"
#include "C1Handler.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
// クラス Iscas89ExParser
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Iscas89ExParser::Iscas89ExParser()
{
  auto mux_handler = new MuxHandler;
  auto mux_id = reg_handler(mux_handler);
  bind_handler("MUX", mux_id);
  bind_handler("mux", mux_id);

  auto c0_handler = new C0Handler;
  auto c0_id = reg_handler(c0_handler);
  bind_handler("CONST0", c0_id);
  bind_handler("const0", c0_id);

  auto c1_handler = new C1Handler;
  auto c1_id = reg_handler(c1_handler);
  bind_handler("CONST1", c1_id);
  bind_handler("const1", c1_id);
}

END_NAMESPACE_YM_ISCAS89
