#ifndef BLIFBNNETWORKHANDLER_H
#define BLIFBNNETWORKHANDLER_H

/// @file BlifBnNetworkHandler.h
/// @brief BlifBnNetworkHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BlifHandler.h"
#include "ym/ym_cell.h"


BEGIN_NAMESPACE_YM_BNET

class BnNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class BlifBnNetworkHandler BlifBnNetworkHandler.h
/// @brief BnNetwork 用の BlifHandler
//////////////////////////////////////////////////////////////////////
class BlifBnNetworkHandler :
  public BlifHandler
{
public:

  /// @brief コンストラクタ
  BlifBnNetworkHandler(BnNetworkImpl* network);

  /// @brief デストラクタ
  virtual
  ~BlifBnNetworkHandler();


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
  /// @param[in] onode_id ノード名のID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id_array ファンイン各のID番号の配列
  /// @param[in] cover_id カバーID
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  names(ymuint onode_id,
	const char* oname,
	const vector<ymuint>& inode_id_array,
	ymuint cover_id);

  /// @brief .gate 文の処理
  /// @param[in] onode_id 出力ノードのID番号
  /// @param[in] oname 出力名
  /// @param[in] cell セル
  /// @param[in] inode_id_array 入力ノードのID番号の配列
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  gate(ymuint onode_id,
       const char* oname,
       const vector<ymuint>& inode_id_array,
       const Cell* cell);

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
  latch(ymuint onode_id,
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

  // 対象のネットワーク
  BnNetworkImpl* mNetwork;

  // カバーIDをキーにしてFuncType持つ配列
  vector<const BnFuncType*> mFuncTypeArray;

};

END_NAMESPACE_YM_BNET

#endif // BLIFBNNETWORKHANDLER_H
