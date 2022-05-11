#ifndef NULLBLIFHANDLER_H
#define NULLBLIFHANDLER_H

/// @file NullblifHandler.h
/// @brief NullBlifHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2021 Yusuke Matsunaga
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
  NullBlifHandler(
    BlifParser& parser ///< [in] パーサー
  );

  /// @brief デストラクタ
  ~NullBlifHandler();


public:

  /// @brief 初期化
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  init() override;

  /// @brief セルライブラリの設定
  ///
  /// この関数が呼ばれないこともある．
  void
  set_cell_library(
    const ClibCellLibrary& library ///< [in] セルライブラリ
  ) override;

  /// @brief .model 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  model(
    const FileRegion& loc1, ///< [in] .model の位置情報
    const FileRegion& loc2, ///< [in] 文字列の位置情報
    const string& name      ///< [in] 文字列
  ) override;

  /// @brief .inputs 文中の文字列の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  inputs_elem(
    SizeType name_id,  ///< [in] 文字列のID番号
    const string& name ///< [in] 入力名
  ) override;

  /// @brief .outputs 文中の文字列の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  outputs_elem(
    SizeType name_id,  ///< [in] 文字列のID番号
    const string& name ///< [in] 出力名
  ) override;

  /// @brief .names 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  names(
    SizeType onode_id,                      ///< [in] 出力ノードのID番号
    const string& oname,                    ///< [in] 出力名
    const vector<SizeType>& inode_id_array, ///< [in] 各識別子のID番号の配列
    SizeType cover_id                       ///< [in] カバーID
  ) override;

  /// @brief .gate 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  gate(
    SizeType onode_id,                      ///< [in] 出力ノードのID番号
    const string& oname,                    ///< [in] 出力名
    const vector<SizeType>& inode_id_array, ///< [in] 入力ノードのID番号の配列
    int cell_id                             ///< [in] セル番号
  ) override;

  /// @brief .latch 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  latch(
    SizeType onode_id,      ///< [in] 出力ノードのID番号
    const string& oname,    ///< [in] 出力名
    SizeType inode_id,      ///< [in] 入力ノードのID番号
    const FileRegion& loc4, ///< [in] リセット値の位置情報
    char rval               ///< [in] リセット時の値('0'/'1') 未定義なら ' '
  ) override;

  /// @brief .end 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  end(
    const FileRegion& loc ///< [in] 位置情報
  ) override;

  /// @brief 通常終了時の処理
  void
  normal_exit() override;

  /// @brief エラー終了時の処理
  void
  error_exit() override;

};

END_NAMESPACE_YM_BNET

#endif // NULLBLIFHANDLER_H
