
/// @file Iscas89Handler.cc
/// @brief Iscas89Handler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Handler.h"
#include "ParserImpl.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
// クラス Isacas89Handler
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Iscas89Handler::Iscas89Handler()
{
}

// @brief デストラクタ
Iscas89Handler::~Iscas89Handler()
{
}

bool
Iscas89Handler::_read(
  ParserImpl* parser,
  const FileRegion& loc,
  SizeType oname_id
)
{
  mParser = parser;
  return read(loc, oname_id);
}

// @brief ゲートの設定を行う．
void
Iscas89Handler::set_gate(
  SizeType id,
  const FileRegion& loc,
  PrimType gate_type,
  const vector<SizeType>& fanin_list
)
{
  mParser->set_gate(id, loc, gate_type, fanin_list);
}

// @brief 複合ゲートの設定を行う．
void
Iscas89Handler::set_complex(
  SizeType id,
  const FileRegion& loc,
  const Expr& expr,
  const vector<SizeType>& fanin_list
)
{
  mParser->set_complex(id, loc, expr, fanin_list);
}

// @brief '(' ')' で囲まれた名前を読み込む．
bool
Iscas89Handler::parse_name(
  SizeType& name_id,
  FileRegion& last_loc
)
{
  return mParser->parse_name(name_id, last_loc);
}

// @brief '(' ')' で囲まれた名前のリストを読み込む．
bool
Iscas89Handler::parse_name_list(
  vector<SizeType>& name_id_list,
  FileRegion& last_loc
)
{
  return mParser->parse_name_list(name_id_list, last_loc);
}

// @brief ID 番号から文字列を得る．
const string&
Iscas89Handler::id2str(
  SizeType id
) const
{
  return mParser->id2str(id);
}

END_NAMESPACE_YM_ISCAS89
