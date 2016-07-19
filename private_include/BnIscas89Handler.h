#ifndef BNISCAS89HANDLER_H
#define BNISCAS89HANDLER_H

/// @file BnIscas89Handler.h
/// @brief BnIscas89Handler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/Iscas89Handler.h"
#include "ym/HashMap.h"


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
  /// @param[in] builder ビルダーオブジェクト
  /// @param[in] clock_name クロック端子名
  BnIscas89Handler(BnBuilder* builder,
		   const string& clock_name = "clock");

  /// @brief デストラクタ
  virtual
  ~BnIscas89Handler();


public:
  //////////////////////////////////////////////////////////////////////
  // Iscas89Handler の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  init();

  /// @brief INPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 入力ピン名の ID 番号
  /// @param[in] name 入力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_input(const FileRegion& loc,
	     ymuint name_id,
	     const char* name);

  /// @brief OUTPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 出力ピン名の ID 番号
  /// @param[in] name 出力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_output(const FileRegion& loc,
	      ymuint name_id,
	      const char* name);

  /// @brief ゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] logic_type ゲートの型
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] oname 出力名
  /// @param[in] iname_list 入力名のリスト
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_gate(const FileRegion& loc,
	    BnLogicType logic_type,
	    ymuint oname_id,
	    const char* name,
	    const vector<ymuint>& iname_list);

  /// @brief D-FF用のゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] oname 出力名
  /// @param[in] iname_id 入力名の ID 番号
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_dff(const FileRegion& loc,
	   ymuint oname_id,
	   const char* oname,
	   ymuint iname_id);

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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ビルダーオブジェクト
  BnBuilder* mBuilder;

  // クロック端子名
  string mClockName;

  // 名前IDをキーにしてノード番号を格納するハッシュ表
  HashMap<ymuint, ymuint> mIdMap;

  // ノードIDをキーにしてファンイン情報を格納するハッシュ表
  HashMap<ymuint, vector<ymuint> > mFaninInfoMap;

  // クロック端子のノード番号
  ymuint mClockId;

};

END_NAMESPACE_YM_BNET

#endif // BNISCAS89HANDLER_H
