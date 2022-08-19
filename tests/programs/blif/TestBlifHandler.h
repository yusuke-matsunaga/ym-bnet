#ifndef TESTBLIFHANDLER_H
#define TESTBLIFHANDLER_H

/// @file TestBlifHandler.h
/// @brief TestBlifHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BlifHandler.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class TestBlifHandler TestBlifHandler.h
/// @brief テスト用の BlifHandler
//////////////////////////////////////////////////////////////////////
class TestBlifHandler :
  public BlifHandler
{
public:

  /// @brief コンストラクタ
  TestBlifHandler(
    BlifParser& parser, ///< [in] パーサー
    ostream& stream     ///< [in] 出力ストリーム
  );

  /// @brief デストラクタ
  ~TestBlifHandler() = default;


public:

  /// @brief 初期化
  bool
  init() override;

  /// @brief セルライブラリの設定
  void
  set_cell_library(
    const ClibCellLibrary& library
  ) override;

  /// @brief .model 文の処理
  bool
  model(
    const FileRegion& loc1,
    const FileRegion& loc2,
    const string& name
  ) override;

  /// @brief .inputs 文中の文字列の処理
  bool
  inputs_elem(
    SizeType name_id,
    const string& name
  ) override;

  /// @brief .outputs 文中の文字列の処理
  bool
  outputs_elem(
    SizeType name_id,
    const string& name
  ) override;

  /// @brief .names 文の処理
  bool
  names(
    SizeType onode_id,
    const string& oname,
    const vector<SizeType>& inode_id_array,
    SizeType cover_id
  ) override;

  /// @brief .gate 文の処理
  bool
  gate(
    SizeType onode_id,
    const string& oname,
    const vector<SizeType>& inode_id_array,
    SizeType cell_id
  ) override;

  /// @brief .latch 文の処理
  bool
  latch(
    SizeType onode_id,
    const string& oname,
    SizeType inode_id,
    const FileRegion& loc4,
    char rval
  ) override;

  /// @brief .end 文の処理
  bool
  end(
    const FileRegion& loc
  ) override;

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

  ostream& mStream;

};

END_NAMESPACE_YM_BNET

#endif // TESTBLIFHANLDER_H
