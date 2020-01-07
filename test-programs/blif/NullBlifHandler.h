#ifndef NULLBLIFHANDLER_H
#define NULLBLIFHANDLER_H

/// @file NullblifHandler.h
/// @brief NullBlifHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BlifHandler.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class NullBlifHandler NullBlifHandler.h
/// @brief なにもしない BlifHandler
//////////////////////////////////////////////////////////////////////
class NullBlifHandler :
  public BlifHandler
{
public:

  /// @brief コンストラクタ
  /// @param[in] parser パーサー
  NullBlifHandler(BlifParser& parser);

  /// @brief デストラクタ
  ~NullBlifHandler();


public:

  /// @brief 初期化
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  init() override;

  /// @brief セルライブラリの設定
  /// @param[in] library セルライブラリ
  ///
  /// この関数が呼ばれないこともある．
  void
  set_cell_library(const ClibCellLibrary& library) override;

  /// @brief .model 文の処理
  /// @param[in] loc1 .model の位置情報
  /// @param[in] loc2 文字列の位置情報
  /// @param[in] name 文字列
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  model(const FileRegion& loc1,
	const FileRegion& loc2,
	const string& name) override;

  /// @brief .inputs 文中の文字列の処理
  /// @param[in] name_id 文字列のID番号
  /// @param[in] name 入力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  inputs_elem(int name_id,
	      const string& name) override;

  /// @brief .outputs 文中の文字列の処理
  /// @param[in] name_id 文字列のID番号
  /// @param[in] name 出力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  outputs_elem(int name_id,
	       const string& name) override;

  /// @brief .names 文の処理
  /// @param[in] onode_id 出力ノードのID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id_array 各識別子のID番号の配列
  /// @param[in] cover カバー
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  /// @note cover_pat は ni 個ごとに1行のパタンを表す．
  /// 各要素のとりうる値は '0', '1', '-' を表す．
  /// @note opat は '0' か '1' のどちらか
  bool
  names(int onode_id,
	const string& oname,
	const vector<int>& inode_id_array,
	int cover_id) override;

  /// @brief .gate 文の処理
  /// @param[in] onode_id 出力ノードのID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id_array 入力ノードのID番号の配列
  /// @param[in] cell_id セル番号
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  gate(int onode_id,
       const string& oname,
       const vector<int>& inode_id_array,
       int cell_id) override;

  /// @brief .latch 文の処理
  /// @param[in] onode_id 出力ノードのID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id 入力ノードのID番号
  /// @param[in] loc4 リセット値の位置情報
  /// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  latch(int onode_id,
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

};

END_NAMESPACE_YM_BNET

#endif // NULLBLIFHANDLER_H
