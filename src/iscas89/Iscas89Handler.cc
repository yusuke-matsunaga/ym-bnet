﻿
/// @file Iscas89Handler.cc
/// @brief BlibHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ym/Iscas89Handler.h"
#include "Iscas89ParserImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// Iscas89Handler
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Iscas89Handler::Iscas89Handler()
{
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
// @param[in] loc ファイル位置
// @param[in] name_id 入力ピン名の ID 番号
// @param[in] name 入力ピン名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
Iscas89Handler::read_input(const FileRegion& loc,
			   ymuint name_id,
			   const char* name)
{
  return true;
}

// @brief OUTPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 出力ピン名の ID 番号
// @param[in] name 出力ピン名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
Iscas89Handler::read_output(const FileRegion& loc,
			    ymuint name_id,
			    const char* name)
{
  return true;
}

// @brief ゲート文を読み込む．
// @param[in] loc ファイル位置
// @param[in] type ゲートの型
// @param[in] oname_id 出力名の ID 番号
// @param[in] oname 出力名
// @param[in] iname_list 入力名のリスト
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
Iscas89Handler::read_gate(const FileRegion& loc,
			  BnFuncType::Type type,
			  ymuint oname_id,
			  const char* oname,
			  const vector<ymuint>& iname_list)
{
  return true;
}

// @brief D-FF用のゲート文を読み込む．
// @param[in] loc ファイル位置
// @param[in] oname_id 出力名の ID 番号
// @param[in] oname 出力名
// @param[in] iname_id 入力名の ID 番号
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
Iscas89Handler::read_dff(const FileRegion& loc,
			 ymuint oname_id,
			 const char* oname,
			 ymuint iname_id)
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
const char*
Iscas89Handler::id2str(ymuint id) const
{
  return mParser->id2str(id);
}

// @brief ID 番号から位置情報を得る．
FileRegion
Iscas89Handler::id2loc(ymuint id) const
{
  return mParser->id2loc(id);
}

END_NAMESPACE_YM_BNET
