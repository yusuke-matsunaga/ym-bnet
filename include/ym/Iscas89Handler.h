#ifndef YM_ISCAS89HANDLER_H
#define YM_ISCAS89HANDLER_H

/// @file ym/Iscas89Handler.h
/// @brief Iscas89Handler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/BnNode.h"
#include "ym/FileRegion.h"


BEGIN_NAMESPACE_YM_BNET

class Iscas89ParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Handler Iscas89Handler.h "ym/Iscas89Handler.h"
/// @ingroup Iscas89Group
/// @brief iscas89 パーサーのイベントハンドラの基底クラス
/// @sa Iscas89Parser
//////////////////////////////////////////////////////////////////////
class Iscas89Handler
{
protected:

  /// @brief コンストラクタ
  Iscas89Handler(
    Iscas89Parser& parser ///< [in] ISCAS89 パーサー
  );

  /// @brief デストラクタ
  virtual
  ~Iscas89Handler();


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
  init();

  /// @brief INPUT 文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_input(
    const FileRegion& loc, ///< [in] ファイル位置
    int name_id,           ///< [in] 入力ピン名の ID 番号
    const string& name     ///< [in] 入力ピン名
  );

  /// @brief OUTPUT 文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_output(
    const FileRegion& loc, ///< [in] ファイル位置
    int name_id,           ///< [in] 出力ピン名の ID 番号
    const string& name     ///< [in] 出力ピン名
  );

  /// @brief ゲート文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_gate(
    const FileRegion& loc,        ///< [in] ファイル位置
    BnNodeType logic_type,        ///< [in] ゲートの型
    int oname_id,                 ///< [in] 出力名の ID 番号
    const string& oname,          ///< [in] 出力名
    const vector<int>& iname_list ///< [in] 入力名のリスト
  );

  /// @brief ゲート文(MUX)を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  ///
  /// 入力数のチェックは済んでいるものとする．
  virtual
  bool
  read_mux(
    const FileRegion& loc,        ///< [in] ファイル位置
    int oname_id,                 ///< [in] 出力名の ID 番号
    const string& oname,          ///< [in] 出力名
    const vector<int>& iname_list ///< [in] 入力名のリスト
  );

  /// @brief D-FF用のゲート文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_dff(
    const FileRegion& loc, ///< [in] ファイル位置
    int oname_id,          ///< [in] 出力名の ID 番号
    const string& oname,   ///< [in] 出力名
    int iname_id           ///< [in] 入力名の ID 番号
  );

  /// @brief 終了操作
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  end();

  /// @brief 通常終了時の処理
  virtual
  void
  normal_exit();

  /// @brief エラー終了時の処理
  virtual
  void
  error_exit();


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが利用することのできる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID 番号から文字列を得る．
  const string&
  id2str(
    int id ///< [in] ID番号
  ) const;

  /// @brief ID 番号から位置情報を得る．
  FileRegion
  id2loc(
    int id ///< [in] ID番号
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パーサー本体
  Iscas89ParserImpl& mParser;

};

END_NAMESPACE_YM_BNET

#endif // YM_ISCAS89HANDLER_H
