#ifndef NULLBLIFHANDLER_H
#define NULLBLIFHANDLER_H

/// @file NullblifHandler.h
/// @brief NullBlifHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BlifHandler.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class NullBlifHandler NullBlifHandler.h
/// @brief なにもしない BlifHandler
//////////////////////////////////////////////////////////////////////
class NullBlifHandler :
  public BlifHandler
{
public:

  /// @brief コンストラクタ
  NullBlifHandler();

  /// @brief デストラクタ
  virtual
  ~NullBlifHandler();


public:

  /// @brief 初期化
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  init();

  /// @brief セルライブラリの設定
  /// @param[in] library セルライブラリ
  ///
  /// この関数が呼ばれないこともある．
  virtual
  void
  set_cell_library(const ClibCellLibrary& library);

  /// @brief .model 文の処理
  /// @param[in] loc1 .model の位置情報
  /// @param[in] loc2 文字列の位置情報
  /// @param[in] name 文字列
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  model(const FileRegion& loc1,
	const FileRegion& loc2,
	const char* name);

  /// @brief .inputs 文中の文字列の処理
  /// @param[in] name_id 文字列のID番号
  /// @param[in] name 入力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  inputs_elem(int name_id,
	      const char* name);

  /// @brief .outputs 文中の文字列の処理
  /// @param[in] name_id 文字列のID番号
  /// @param[in] name 出力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  outputs_elem(int name_id,
	       const char* name);

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
  virtual
  bool
  names(int onode_id,
	const char* oname,
	const vector<int>& inode_id_array,
	int cover_id);

  /// @brief .gate 文の処理
  /// @param[in] onode_id 出力ノードのID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id_array 入力ノードのID番号の配列
  /// @param[in] cell セル
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  gate(int onode_id,
       const char* oname,
       const vector<int>& inode_id_array,
       const ClibCell* cell);

  /// @brief .latch 文の処理
  /// @param[in] onode_id 出力ノードのID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id 入力ノードのID番号
  /// @param[in] loc4 リセット値の位置情報
  /// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  latch(int onode_id,
	const char* oname,
	int inode_id,
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

};

END_NAMESPACE_YM

#endif // NULLBLIFHANDLER_H
