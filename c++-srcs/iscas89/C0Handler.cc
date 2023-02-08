
/// @file C0Handler.cc
/// @brief C0Handler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "C0Handler.h"


BEGIN_NAMESPACE_YM_ISCAS89

// @brief ゲートの読み込みを行う．
bool
C0Handler::read(
  const FileRegion& loc,
  SizeType oname_id
)
{
  set_gate(oname_id, loc, PrimType::C0, {});

  return true;
}

END_NAMESPACE_YM_ISCAS89
