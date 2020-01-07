#ifndef BNBLIFHANDLER_H
#define BNBLIFHANDLER_H

/// @file BnBlifHandler.h
/// @brief BnBlifHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BlifHandler.h"


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
  /// @param[in] parser パーサー
  /// @param[in] network ネットワーク
  /// @param[in] clock_name クロック端子名
  /// @param[in] reset_name リセット端子名
  BnBlifHandler(BlifParser& parser,
		BnNetwork* network,
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
  bool
  init() override;

  /// @brief セルライブラリの設定
  /// @param[in] library セルライブラリ
  ///
  /// この関数が呼ばれないこともある．
  void
  set_cell_library(const ClibCellLibrary& library) override;

  /// @brief .model 文中の文字列の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  model(const FileRegion& loc1,
	const FileRegion& loc2,
	const string& name) override;

  /// @brief .inputs 文中の文字列の処理
  /// @param[in] name_id 文字列の情報
  /// @param[in] name 入力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  inputs_elem(int name_id,
	      const string& name) override;

  /// @brief .outputs 文中の文字列の処理
  /// @param[in] name_id 文字列の情報
  /// @param[in] name 出力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  outputs_elem(int name_id,
	       const string& name) override;

  /// @brief .names 文の処理
  /// @param[in] oname_id ノード名のID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id_array ファンイン各のID番号の配列
  /// @param[in] cover_id カバーID
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  names(int oname_id,
	const string& oname,
	const vector<int>& inode_id_array,
	int cover_id) override;

  /// @brief .gate 文の処理
  /// @param[in] oname_id 出力ノードのID番号
  /// @param[in] oname 出力名
  /// @param[in] cell_id セル番号
  /// @param[in] inode_id_array 入力ノードのID番号の配列
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  gate(int oname_id,
       const string& oname,
       const vector<int>& inode_id_array,
       int cell_id) override;

  /// @brief .latch 文の処理
  /// @param[in] oname_id 出力ノードのID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id 入力ノードのID番号
  /// @param[in] loc4 リセット値の位置情報
  /// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  latch(int oname_id,
	const string& oname,
	int inode_id,
	const FileRegion& loc4,
	char rval) override;

  /// @brief .end 文の処理
  /// @param[in] loc 位置情報
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  end(const FileRegion& loc) override;

  /// @brief 通常終了時の処理
  void
  normal_exit() override;

  /// @brief エラー終了時の処理
  void
  error_exit() override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  BnNetwork* mNetwork;

  // クロック端子名
  string mClockName;

  // リセット端子名
  string mResetName;

  // 名前IDをキーにしてノード番号を格納するハッシュ表
  unordered_map<int, int> mIdMap;

  // ノードIDをキーにしてファンイン情報を格納するハッシュ表
  unordered_map<int, vector<int> > mFaninInfoMap;

  // クロック端子のノード番号
  int mClockId;

  // リセット端子のノード番号
  int mResetId;

};

END_NAMESPACE_YM_BNET

#endif // BNBLIFHANDLER_H
