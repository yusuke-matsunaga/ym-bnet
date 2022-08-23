﻿#ifndef BLIFHANDLER_H
#define BLIFHANDLER_H

/// @file BlifHandler.h
/// @brief BlifHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2017, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/clib.h"
#include "ym/FileRegion.h"


BEGIN_NAMESPACE_YM_BNET

class BlifParser;
class BlifCover;

//////////////////////////////////////////////////////////////////////
/// @class BlifHandler BlifHandler.h "BlifHandler.h"
/// @ingroup BlifGroup
/// @brief blif パーサーのイベントハンドラの基底クラス
///
/// 純粋仮想関数を持っているので，このクラスを継承し，仮想関数を上書き
/// する必要がある．
/// @sa BlifParser
//////////////////////////////////////////////////////////////////////
class BlifHandler
{
  friend class BlifParser;

protected:

  /// @brief コンストラクタ
  ///
  /// 生成されたハンドラは自動的に parser に登録される．
  BlifHandler(
    BlifParser& parser ///< [in] blif パーサー
  );

  /// @brief デストラクタ
  virtual
  ~BlifHandler() = default;


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
  ///
  /// この関数が呼ばれないこともある．
  virtual
  void
  set_cell_library(
    const ClibCellLibrary& library ///< [in] セルライブラリ
  ) = 0;

  /// @brief .model 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  model(
    const FileRegion& loc1, ///< [in] .model の位置情報
    const FileRegion& loc2, ///< [in] 文字列の位置情報
    const string& name      ///< [in] 文字列
  ) = 0;

  /// @brief .inputs 文中の文字列の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  inputs_elem(
    SizeType name_id,  ///< [in] 文字列のID番号
    const string& name ///< [in] 入力ピン名
  ) = 0;

  /// @brief .outputs 文中の文字列の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  outputs_elem(
    SizeType name_id,  ///< [in] 文字列のID番号
    const string& name ///< [in] 出力ピン名
  ) = 0;

  /// @brief .names 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  ///
  /// cover_pat は ni 個ごとに1行のパタンを表す．
  /// 各要素のとりうる値は '0', '1', '-' を表す．
  ///
  /// opat は '0' か '1' のどちらか
  virtual
  bool
  names(
    SizeType onode_id,                      ///< [in] ノード名のID番号
    const string& oname,                    ///< [in] 出力名
    const vector<SizeType>& inode_id_array, ///< [in] ファンイン各のID番号の配列
    SizeType cover_id                       ///< [in] カバーのID番号
  ) = 0;

  /// @brief .gate 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  gate(
    SizeType onode_id,                       ///< [in] 出力ノードのID番号
    const string& oname,                     ///< [in] 出力名
    const vector<SizeType>& inode_id_array,  ///< [in] 入力ノードのID番号の配列
    SizeType cell_id                         ///< [in] セル番号
  ) = 0;

  /// @brief .latch 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  latch(
    SizeType onode_id,       ///< [in] 出力ノードのID番号
    const string& oname,     ///< [in] 出力名
    SizeType inode_id,       ///< [in] 入力ノードのID番号
    const FileRegion& loc4,  ///< [in] リセット値の位置情報
    char rval                ///< [in] リセット時の値('0'/'1') 未定義なら ' '
  ) = 0;

  /// @brief .end 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  end(
    const FileRegion& loc ///< [in] 位置情報
  ) = 0;

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
  id2str(
    SizeType id ///< [in] ID番号
  );

  /// @brief ID番号からそれに関連した位置情報を得る．
  const FileRegion&
  id2loc(
    SizeType id ///< [in] ID番号
  );

  /// @brief カバーの数を得る．
  SizeType
  cover_num();

  /// @brief カバーIDからカバーを得る．
  const BlifCover&
  id2cover(
    SizeType id ///< [in] ID番号
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パーサー本体
  BlifParser& mParser;

};

END_NAMESPACE_YM_BNET

#endif // BLIFHANDLER_H