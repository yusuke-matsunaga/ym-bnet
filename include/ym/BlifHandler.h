#ifndef YM_BLIFHANDLER_H
#define YM_BLIFHANDLER_H

/// @file ym/BlifHandler.h
/// @brief BlifHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"
#include "ym/clib.h"
#include "ym/FileRegion.h"


BEGIN_NAMESPACE_YM_BNET

class BlifParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class BlifHandler BlifHandler.h "ym/BlifHandler.h"
/// @ingroup BlifGroup
/// @brief blif パーサーのイベントハンドラの基底クラス
///
/// 純粋仮想関数を持っているので，このクラスを継承し，仮想関数を上書き
/// する必要がある．
/// @sa BlifParser
//////////////////////////////////////////////////////////////////////
class BlifHandler
{
  friend class BlifParserImpl;

protected:

  /// @brief コンストラクタ
  /// @param[in] parser blif パーサー
  ///
  /// 生成されたハンドラは自動的に parser に登録される．
  BlifHandler(BlifParser& parser);

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

  /// @brief セルライブラリの設定
  /// @param[in] library セルライブラリ
  ///
  /// この関数が呼ばれないこともある．
  virtual
  void
  set_cell_library(const ClibCellLibrary& library) = 0;

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
	const string& name) = 0;

  /// @brief .inputs 文中の文字列の処理
  /// @param[in] name_id 文字列のID番号
  /// @param[in] name 入力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  inputs_elem(int name_id,
	      const string& name) = 0;

  /// @brief .outputs 文中の文字列の処理
  /// @param[in] name_id 文字列のID番号
  /// @param[in] name 出力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  outputs_elem(int name_id,
	       const string& name) = 0;

  /// @brief .names 文の処理
  /// @param[in] onode_id ノード名のID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id_array ファンイン各のID番号の配列
  /// @param[in] cover_id カバーのID番号
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  /// @note cover_pat は ni 個ごとに1行のパタンを表す．
  /// 各要素のとりうる値は '0', '1', '-' を表す．
  /// @note opat は '0' か '1' のどちらか
  virtual
  bool
  names(int onode_id,
	const string& oname,
	const vector<int>& inode_id_array,
	int cover_id) = 0;

  /// @brief .gate 文の処理
  /// @param[in] onode_id 出力ノードのID番号
  /// @param[in] oname 出力名
  /// @param[in] cell_id セル番号
  /// @param[in] inode_id_array 入力ノードのID番号の配列
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  gate(int onode_id,
       const string& oname,
       const vector<int>& inode_id_array,
       int cell_id)  = 0;

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
	const string& oname,
	int inode_id,
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
  const string&
  id2str(int id);

  /// @brief ID番号からそれに関連した位置情報を得る．
  const FileRegion&
  id2loc(int id);

  /// @brief カバーの数を得る．
  int
  cover_num();

  /// @brief カバーIDからカバーを得る．
  const BlifCover&
  id2cover(int id);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パーサー本体
  BlifParserImpl& mParser;

};

END_NAMESPACE_YM_BNET

#endif // YM_BLIFHANDLER_H
