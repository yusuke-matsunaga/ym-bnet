#ifndef YM_BLIFHANDLER_H
#define YM_BLIFHANDLER_H

/// @file ym/BlifHandler.h
/// @brief BlifHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/ym_cell.h"
#include "ym/FileRegion.h"


BEGIN_NAMESPACE_YM_BNET

class BlifParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class BlifHandler BlifHandler.h "ym/BlifHandler.h"
/// @ingroup BlifGroup
/// @brief blif パーサーのイベントハンドラの基底クラス
/// 純粋仮想関数を持っているので，このクラスを継承し，仮想関数を上書き
/// する必要がある．
/// @sa BlifParser
//////////////////////////////////////////////////////////////////////
class BlifHandler
{
  friend class BlifParserImpl;

protected:

  /// @brief コンストラクタ
  BlifHandler();

  /// @brief デストラクタ
  virtual
  ~BlifHandler();


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
  init() = 0;

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
	const char* name) = 0;

  /// @brief .inputs 文中の文字列の処理
  /// @param[in] name_id 文字列のID番号
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  inputs_elem(ymuint name_id) = 0;

  /// @brief .outputs 文中の文字列の処理
  /// @param[in] name_id 文字列のID番号
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  outputs_elem(ymuint name_id) = 0;

  /// @brief .names 文の処理
  /// @param[in] onode_id ノード名のID番号
  /// @param[in] inode_id_array ファンイン各のID番号の配列
  /// @param[in] cover カバー
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  /// @note cover_pat は ni 個ごとに1行のパタンを表す．
  /// 各要素のとりうる値は '0', '1', '-' を表す．
  /// @note opat は '0' か '1' のどちらか
  virtual
  bool
  names(ymuint onode_id,
	const vector<ymuint>& inode_id_array,
	const BlifCover* cover) = 0;

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
       const Cell* cell) = 0;

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
	char rval) = 0;

  /// @brief .end 文の処理
  /// @param[in] loc 位置情報
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  end(const FileRegion& loc) = 0;

  /// @brief 通常終了時の処理
  virtual
  void
  normal_exit() = 0;

  /// @brief エラー終了時の処理
  virtual
  void
  error_exit() = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが利用することのできる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号から文字列を得る．
  const char*
  id2str(ymuint id);

  /// @brief ID番号からそれに関連した位置情報を得る．
  const FileRegion&
  id2loc(ymuint id);

  /// @brief ID番号からそれに関連した位置情報その2を得る．
  const FileRegion&
  id2def_loc(ymuint id);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パーサー本体
  BlifParserImpl* mParser;

};

END_NAMESPACE_YM_BNET

#endif // YM_BLIFHANDLER_H
