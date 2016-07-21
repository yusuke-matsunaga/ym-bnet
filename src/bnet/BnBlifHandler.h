﻿#ifndef BNBLIFHANDLER_H
#define BNBLIFHANDLER_H

/// @file BnBlifHandler.h
/// @brief BnBlifHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BlifHandler.h"
#include "ym/ym_cell.h"
#include "ym/HashMap.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnBlifHandler BnBlifHandler.h
/// @brief BnNetwork 用の BlifHandler
//////////////////////////////////////////////////////////////////////
class BnBlifHandler :
  public BlifHandler
{
public:

  /// @brief コンストラクタ
  /// @param[in] builder ビルダーオブジェクト
  /// @param[in] clock_name クロック端子名
  /// @param[in] reset_name リセット端子名
  BnBlifHandler(BnBuilder* builder,
		const string& clock_name = "clock",
		const string& reset_name = "reset");

  /// @brief デストラクタ
  virtual
  ~BnBlifHandler();


public:
  //////////////////////////////////////////////////////////////////////
  // BlifHandler の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化
  virtual
  bool
  init();

  /// @brief .model 文中の文字列の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  model(const FileRegion& loc1,
	const FileRegion& loc2,
	const char* name);

  /// @brief .inputs 文中の文字列の処理
  /// @param[in] name_id 文字列の情報
  /// @param[in] name 入力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  inputs_elem(ymuint name_id,
	      const char* name);

  /// @brief .outputs 文中の文字列の処理
  /// @param[in] name_id 文字列の情報
  /// @param[in] name 出力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  outputs_elem(ymuint name_id,
	       const char* name);

  /// @brief .names 文の処理
  /// @param[in] oname_id ノード名のID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id_array ファンイン各のID番号の配列
  /// @param[in] cover_id カバーID
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  names(ymuint oname_id,
	const char* oname,
	const vector<ymuint>& inode_id_array,
	ymuint cover_id);

  /// @brief .gate 文の処理
  /// @param[in] oname_id 出力ノードのID番号
  /// @param[in] oname 出力名
  /// @param[in] cell セル
  /// @param[in] inode_id_array 入力ノードのID番号の配列
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  gate(ymuint oname_id,
       const char* oname,
       const vector<ymuint>& inode_id_array,
       const Cell* cell);

  /// @brief .latch 文の処理
  /// @param[in] oname_id 出力ノードのID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id 入力ノードのID番号
  /// @param[in] loc4 リセット値の位置情報
  /// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  latch(ymuint oname_id,
	const char* oname,
	ymuint inode_id,
	const FileRegion& loc4,
	char rval);

  /// @brief .end 文の処理
  /// @param[in] loc 位置情報
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  end(const FileRegion& loc);

  /// @brief 通常終了時の処理
  virtual
  void
  normal_exit();

  /// @brief エラー終了時の処理
  virtual
  void
  error_exit();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ビルダーオブジェクト
  BnBuilder* mBuilder;

  // クロック端子名
  string mClockName;

  // リセット端子名
  string mResetName;

  // 名前IDをキーにしてノード番号を格納するハッシュ表
  HashMap<ymuint, ymuint> mIdMap;

  // ノードIDをキーにしてファンイン情報を格納するハッシュ表
  HashMap<ymuint, vector<ymuint> > mFaninInfoMap;

  // クロック端子のノード番号
  ymuint mClockId;

  // リセット端子のノード番号
  ymuint mResetId;

};

END_NAMESPACE_YM_BNET

#endif // BNBLIFHANDLER_H