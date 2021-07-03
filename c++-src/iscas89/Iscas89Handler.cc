
/// @file Iscas89Handler.cc
/// @brief BlibHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Handler.h"
#include "ym/Iscas89Parser.h"
#include "Iscas89ParserImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// Iscas89Handler
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Iscas89Handler::Iscas89Handler(
  Iscas89Parser& parser
) : mParser(*parser.mRep.get())
{
  mParser.add_handler(this);
}

// @brief デストラクタ
Iscas89Handler::~Iscas89Handler()
{
}

// @brief 初期化
bool
Iscas89Handler::init()
{
  return true;
}

// @brief INPUT 文を読み込む．
bool
Iscas89Handler::read_input(
  const FileRegion& loc,
  int name_id,
  const string& name
)
{
  return true;
}

// @brief OUTPUT 文を読み込む．
bool
Iscas89Handler::read_output(
  const FileRegion& loc,
  int name_id,
  const string& name
)
{
  return true;
}

// @brief ゲート文を読み込む．
bool
Iscas89Handler::read_gate(
  const FileRegion& loc,
  BnNodeType logic_type,
  int oname_id,
  const string& oname,
  const vector<int>& iname_list
)
{
  return true;
}

// @brief ゲート文(MUX)を読み込む．
bool
Iscas89Handler::read_mux(
  const FileRegion& loc,
  int oname_id,
  const string& oname,
  const vector<int>& iname_list
)
{
  return true;
}

// @brief D-FF用のゲート文を読み込む．
bool
Iscas89Handler::read_dff(
  const FileRegion& loc,
  int oname_id,
  const string& oname,
  int iname_id
)
{
  return true;
}

// @brief 終了操作
bool
Iscas89Handler::end()
{
  return true;
}

// @brief 通常終了時の処理
void
Iscas89Handler::normal_exit()
{
}

// @brief エラー終了時の処理
void
Iscas89Handler::error_exit()
{
}

// @brief ID 番号から文字列を得る．
const string&
Iscas89Handler::id2str(
  int id
) const
{
  return mParser.id2str(id);
}

// @brief ID 番号から位置情報を得る．
FileRegion
Iscas89Handler::id2loc(
  int id
) const
{
  return mParser.id2loc(id);
}

END_NAMESPACE_YM_BNET
