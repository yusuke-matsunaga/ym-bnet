#ifndef BNISCAS89HANDLER_H
#define BNISCAS89HANDLER_H

/// @file BnIscas89Handler.h
/// @brief BnIscas89Handler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "Iscas89Handler.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnIscas89Handler BnIscas89Handler.h
/// @brief BnNetwork 用の Iscas89Handler
//////////////////////////////////////////////////////////////////////
class BnIscas89Handler :
  public Iscas89Handler
{
public:

  /// @brief コンストラクタ
  BnIscas89Handler(
    Iscas89Parser& parser,             ///< [in] パーサー
    BnNetwork& network,                ///< [in] 設定対象のネットワーク
    const string& clock_name = "clock" ///< [in] クロック端子名
  ) : Iscas89Handler{parser},
      mNetwork{network},
      mClockName{clock_name}
  {
  }

  /// @brief デストラクタ
  ~BnIscas89Handler() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // Iscas89Handler の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  init() override;

  /// @brief INPUT 文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_input(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType name_id,      ///< [in] 入力ピン名の ID 番号
    const string& name     ///< [in] 入力ピン名
  ) override;

  /// @brief OUTPUT 文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_output(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType name_id,      ///< [in] 出力ピン名の ID 番号
    const string& name     ///< [in] 出力ピン名
  ) override;

  /// @brief ゲート文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_gate(
    const FileRegion& loc,        ///< [in] ファイル位置
    BnNodeType logic_type,        ///< [in] ゲートの型
    SizeType oname_id,            ///< [in] 出力名の ID 番号
    const string& name,           ///< [in] 出力名
    const vector<SizeType>& iname_list ///< [in] 入力名のリスト
  ) override;

  /// @brief ゲート文(MUX)を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  ///
  /// 入力数のチェックは済んでいるものとする．
  bool
  read_mux(
    const FileRegion& loc,             ///< [in] ファイル位置
    SizeType oname_id,                 ///< [in] 出力名の ID 番号
    const string& oname,               ///< [in] 出力名
    const vector<SizeType>& iname_list ///< [in] 入力名のリスト
  ) override;

  /// @brief D-FF用のゲート文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_dff(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType oname_id,     ///< [in] 出力名の ID 番号
    const string& oname,   ///< [in] 出力名
    SizeType iname_id      ///< [in] 入力名の ID 番号
  ) override;

  /// @brief 終了操作
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  end() override;

  /// @brief 通常終了時の処理
  void
  normal_exit() override;

  /// @brief エラー終了時の処理
  void
  error_exit() override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの情報
  struct NodeInfo
  {
    string oname;         ///< 出力名
    BnNodeType gate_type; ///< ゲートタイプ
    bool is_mux;          ///< MUXタイプの時 true
    vector<SizeType> iname_id_list; ///< 入力の名前IDのリスト
  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief name_id のノードを生成する．
  SizeType
  make_node(
    SizeType name_id ///< [in] 名前ID
  );

  SizeType
  make_mux(
    const string& oname,                  ///< [in] 出力名
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノードIDのリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  BnNetwork& mNetwork;

  // クロック端子名
  string mClockName;

  // 名前IDをキーにしてノード番号を格納するハッシュ表
  unordered_map<SizeType, SizeType> mIdMap;

  // 名前IDをキーにしてノード情報を格納するハッシュ表
  unordered_map<SizeType, NodeInfo> mNodeMap;

  // 出力のノードIDをキーにしてファンインの名前IDを格納するハッシュ表
  unordered_map<SizeType, SizeType> mOutputMap;

  // クロック端子のノード番号
  SizeType mClockId;

};

END_NAMESPACE_YM_BNET

#endif // BNISCAS89HANDLER_H
