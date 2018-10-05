﻿#ifndef BNISCAS89HANDLER_H
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
  /// @param[in] parser パーサー
  /// @param[in] network 設定対象のネットワーク
  /// @param[in] clock_name クロック端子名
  BnIscas89Handler(Iscas89Parser& parser,
		   BnNetwork* network,
		   const string& clock_name = "clock");

  /// @brief デストラクタ
  ~BnIscas89Handler();


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
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 入力ピン名の ID 番号
  /// @param[in] name 入力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_input(const FileRegion& loc,
	     int name_id,
	     const char* name) override;

  /// @brief OUTPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 出力ピン名の ID 番号
  /// @param[in] name 出力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_output(const FileRegion& loc,
	      int name_id,
	      const char* name) override;

  /// @brief ゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] logic_type ゲートの型
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] oname 出力名
  /// @param[in] iname_list 入力名のリスト
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_gate(const FileRegion& loc,
	    BnNodeType logic_type,
	    int oname_id,
	    const char* name,
	    const vector<int>& iname_list) override;

  /// @brief D-FF用のゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] oname 出力名
  /// @param[in] iname_id 入力名の ID 番号
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_dff(const FileRegion& loc,
	   int oname_id,
	   const char* oname,
	   int iname_id) override;

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

  /// @brief ファンイン情報
  class FaninInfo
  {
  public:

    /// @brief 空のコンストラクタ
    FaninInfo()
    {
      mNi = 0;
      mFaninArray = nullptr;
    }

    /// @brief コンストラクタ
    /// @param[in] fanin_list ファンイン番号のリスト
    FaninInfo(const vector<int>& fanin_list) :
      mNi(fanin_list.size())
    {
      mFaninArray = new int[mNi];
      for (int i = 0; i < mNi; ++ i) {
	mFaninArray[i] = fanin_list[i];
      }
    }

    /// @brief コピーコンストラクタ
    /// @param[in] src コピー元
    FaninInfo(const FaninInfo& src) :
      mNi(src.mNi)
    {
      mFaninArray = new int[mNi];
      for (int i = 0; i < mNi; ++ i) {
	mFaninArray[i] = src.mFaninArray[i];
      }
    }

    /// @brief 代入演算子
    const FaninInfo&
    operator=(const FaninInfo& src)
    {
      if ( &src != this ) {
	delete [] mFaninArray;
	mNi = src.mNi;
	mFaninArray = new int[mNi];
	for (int i = 0; i < mNi; ++ i) {
	  mFaninArray[i] = src.mFaninArray[i];
	}
      }
      return *this;
    }

    /// @brief デストラクタ
    ~FaninInfo()
    {
      delete [] mFaninArray;
    }

    /// @brief ファンイン数を返す．
    int
    fanin_num() const
    {
      return mNi;
    }

    /// @brief ファンインのノード番号を返す．
    /// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
    int
    fanin(int pos) const
    {
      ASSERT_COND( pos < fanin_num() );
      return mFaninArray[pos];
    }


  private:
    //////////////////////////////////////////////////////////////////////
    // データメンバ
    //////////////////////////////////////////////////////////////////////

    // ファンイン数
    int mNi;

    // ファンイン番号の配列
    int* mFaninArray;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン情報を追加する．
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンイン番号のリスト
  void
  add_fanin_info(int id,
		 const vector<int>& fanin_list);



private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  BnNetwork* mNetwork;

  // クロック端子名
  string mClockName;

  // 名前IDをキーにしてノード番号を格納するハッシュ表
  HashMap<int, int> mIdMap;

  // ノードIDをキーにしてファンイン情報を格納するハッシュ表
  HashMap<int, FaninInfo> mFaninInfoMap;

  // クロック端子のノード番号
  int mClockId;

};

END_NAMESPACE_YM_BNET

#endif // BNISCAS89HANDLER_H