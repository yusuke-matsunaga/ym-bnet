#ifndef ISCAS89BNNETWORKHANDLER_H
#define ISCAS89BNNETWORKHANDLER_H

/// @file BnNetworkHandler.h
/// @brief BnNetworkHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ym/Iscas89Handler.h"
#include "ym/HashMap.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class Iscas89BnNetworkHandler Iscas89BnNetworkHandler.h
/// @brief BnNetwork 用の Iscas89Handler
//////////////////////////////////////////////////////////////////////
class Iscas89BnNetworkHandler :
  public Iscas89Handler
{
public:

  /// @brief コンストラクタ
  /// @param[in] builder ビルダーオブジェクト
  /// @param[in] clock_name クロック端子名
  Iscas89BnNetworkHandler(BnBuilder* builder,
			  const string& clock_name = "clock");

  /// @brief デストラクタ
  virtual
  ~Iscas89BnNetworkHandler();


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
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  struct NodeInfo {

    // 空のコンストラクタ
    NodeInfo()
    {
    }

    // コンストラクタ
    NodeInfo(const vector<ymuint>& iname_id_array) :
      mInameIdArray(iname_id_array)
    {
    }

    // ファンインのノードIDの配列
    vector<ymuint> mInameIdArray;

  };

  struct LatchInfo {

    // コンストラクタ
    LatchInfo(ymuint iname_id) :
      mInameId(iname_id)
    {
    }

    // 入力のノード番号
    ymuint mInameId;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // クロック端子名
  string mClockName;

  // ビルダーオブジェクト
  BnBuilder* mBuilder;

  // 名前のID番号をキーにしてノード番号を納めたハッシュ表
  HashMap<ymuint, ymuint> mIdMap;

  // ノード番号をキーにしてノード情報を納めたハッシュ表
  HashMap<ymuint, NodeInfo> mNodeInfoMap;

  // DFF の情報のリスト
  vector<LatchInfo> mLatchInfoList;

  // クロック端子が必要の時 true にするフラグ
  bool mNeedClock;

};

END_NAMESPACE_YM_BNET

#endif // ISCAS89BNNETWORKHANDLER_H
