#ifndef BNBLIFHANDLER_H
#define BNBLIFHANDLER_H

/// @file BnBlifHandler.h
/// @brief BnBlifHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2019, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "BlifHandler.h"
#include "ym/BnModifier.h"
#include "ym/Expr.h"


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
  BnBlifHandler(
    BlifParser& parser,                 ///< [in] パーサー
    const string& clock_name = "clock", ///< [in] クロック端子名
    const string& reset_name = "reset"  ///< [in] リセット端子名
  ) : BlifHandler{parser},
      mClockName{clock_name},
      mResetName{reset_name}
  {
  }

  /// @brief デストラクタ
  ~BnBlifHandler() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 結果のネットワークを返す．
  BnNetwork
  get_network();


public:
  //////////////////////////////////////////////////////////////////////
  // BlifHandler の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化
  bool
  init() override;

  /// @brief セルライブラリの設定
  ///
  /// この関数が呼ばれないこともある．
  void
  set_cell_library(
    const ClibCellLibrary& library ///< [in] セルライブラリ
  ) override;

  /// @brief .model 文中の文字列の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  model(
    const FileRegion& loc1,
    const FileRegion& loc2,
    const string& name
  ) override;

  /// @brief .inputs 文中の文字列の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  inputs_elem(
    SizeType name_id,
    const string& name
  ) override;

  /// @brief .outputs 文中の文字列の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  outputs_elem(
    SizeType name_id,
    const string& name
  ) override;

  /// @brief .names 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  names(
    SizeType oname_id,
    const string& oname,
    const vector<SizeType>& inode_id_array,
    SizeType cover_id
  ) override;

  /// @brief .gate 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  gate(
    SizeType oname_id,
    const string& oname,
    const vector<SizeType>& inode_id_array,
    SizeType cell_id
  ) override;

  /// @brief .latch 文の処理
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  latch(
    SizeType oname_id,
    const string& oname,
    SizeType inode_id,
    const FileRegion& loc4,
    char rval
  ) override;

  /// @brief .end 文の処理
  /// @retval false エラーが起こった．
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
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief name_id のノードを生成する．
  SizeType
  make_node(
    SizeType name_id ///< [in] 名前ID
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  struct NodeInfo
  {
    string oname;     ///< [in] 出力名
    bool has_cell_id; ///< [in] セル番号を持つ時 true
    Expr expr;        ///< [in] 論理式
    SizeType cell_id; ///< [in] セル番号
    vector<SizeType> iname_id_array; ///< [in] 入力の名前IDのリスト
  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  BnModifier mNetwork;

  // クロック端子名
  string mClockName;

  // リセット端子名
  string mResetName;

  // 名前IDをキーにしてノード番号を格納するハッシュ表
  unordered_map<SizeType, SizeType> mIdMap;

  // 名前IDをキーにしてノード情報を格納するハッシュ表
  unordered_map<SizeType, NodeInfo> mNodeMap;

  // 出力ノードのノードIDをキーにしてファンインの名前IDを格納するハッシュ表
  unordered_map<SizeType, SizeType> mOutputMap;

  // クロック端子のノード番号
  SizeType mClockId;

  // リセット端子のノード番号
  SizeType mResetId;

};

END_NAMESPACE_YM_BNET

#endif // BNBLIFHANDLER_H
