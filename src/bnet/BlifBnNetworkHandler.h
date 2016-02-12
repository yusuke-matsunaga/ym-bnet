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
  BlifBnNetworkHandler();

  /// @brief デストラクタ
  virtual
  ~BlifBnNetworkHandler();


public:

  /// @brief 読み込む対象のネットワークを設定する．
  void
  set(BnNetworkImpl* network);


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
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  inputs_elem(ymuint name_id);

  /// @brief .outputs 文中の文字列の処理
  /// @param[in] name_id 文字列の情報
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  outputs_elem(ymuint name_id);

  /// @brief .names 文の処理
  /// @param[in] onode_id ノード名のID番号
  /// @param[in] inode_id_array ファンイン各のID番号の配列
  /// @param[in] cover_id カバーID
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  names(ymuint onode_id,
	const vector<ymuint>& inode_id_array,
	ymuint cover_id);

  /// @brief .gate 文の処理
  /// @param[in] onode_id 出力ノードのID番号
  /// @param[in] cell セル
  /// @param[in] inode_id_array 入力ノードのID番号の配列
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  gate(ymuint onode_id,
       const vector<ymuint>& inode_id_array,
       const Cell* cell);

  /// @brief .latch 文の処理
  /// @param[in] onode_id 出力ノードのID番号
  /// @param[in] inode_id 入力ノードのID番号
  /// @param[in] loc4 リセット値の位置情報
  /// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  latch(ymuint onode_id,
	ymuint inode_id,
	const FileRegion& loc4,
	char rval);

  /// @brief カバー情報の登録
  /// @param[in] cover_id カバーのID番号
  /// @param[in] input_num 入力数
  /// @param[in] cube_num キューブ数
  /// @param[in] ipat_str 入力パタン文字列
  /// @param[in] opat 出力パタン
  virtual
  void
  cover(ymuint cover_id,
	ymuint input_num,
	ymuint cube_num,
	const string& ipat_str,
	BlifPat opat);

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

};

END_NAMESPACE_YM_BNET

#endif // BLIFBNNETWORKHANDLER_H
