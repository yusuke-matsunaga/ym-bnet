﻿#ifndef YM_ISCAS89HANDLER_H
#define YM_ISCAS89HANDLER_H

/// @file ym/Iscas89Handler.h
/// @brief Iscas89Handler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"
#include "ym/BnNode.h"
#include "ym/FileRegion.h"


BEGIN_NAMESPACE_YM_BNET

class Iscas89ParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Handler Iscas89Handler.h "ym/Iscas89Handler.h"
/// @ingroup Iscas89Group
/// @brief iscas89 パーサーのイベントハンドラの基底クラス
/// @sa Iscas89Parser
//////////////////////////////////////////////////////////////////////
class Iscas89Handler
{
protected:

  /// @brief コンストラクタ
  /// @param[in] parser ISCAS89 パーサー
  Iscas89Handler(Iscas89Parser& parser);

  /// @brief デストラクタ
  virtual
  ~Iscas89Handler();


public:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスは必要に応じて下記の仮想関数を上書きすること
  // デフォルトの実装はなにもしない．
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  init();

  /// @brief INPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 入力ピン名の ID 番号
  /// @param[in] name 入力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_input(const FileRegion& loc,
	     int name_id,
	     const char* name);

  /// @brief OUTPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 出力ピン名の ID 番号
  /// @param[in] name 出力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_output(const FileRegion& loc,
	      int name_id,
	      const char* name);

  /// @brief ゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] logic_type ゲートの型
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] oname 出力名
  /// @param[in] iname_list 入力名のリスト
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_gate(const FileRegion& loc,
	    BnNodeType logic_type,
	    int oname_id,
	    const char* oname,
	    const vector<int>& iname_list);

  /// @brief D-FF用のゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] oname 出力名
  /// @param[in] iname_id 入力名の ID 番号
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_dff(const FileRegion& loc,
	   int oname_id,
	   const char* oname,
	   int iname_id);

  /// @brief 終了操作
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  end();

  /// @brief 通常終了時の処理
  virtual
  void
  normal_exit();

  /// @brief エラー終了時の処理
  virtual
  void
  error_exit();


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが利用することのできる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID 番号から文字列を得る．
  const char*
  id2str(int id) const;

  /// @brief ID 番号から位置情報を得る．
  FileRegion
  id2loc(int id) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パーサー本体
  Iscas89ParserImpl& mParser;

};

END_NAMESPACE_YM_BNET

#endif // YM_ISCAS89HANDLER_H
