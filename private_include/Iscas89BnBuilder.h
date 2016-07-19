#ifndef ISCAS89BNBUILDER_H
#define ISCAS89BNBUILDER_H

/// @file Iscas89BnBuilder.h
/// @brief Iscas89BnBuilder のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnBuilder.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class Iscas89BnBuilder BnBuilder.h "BnBuilder.h"
/// @brief blif ファイル読み込み用の BnBuilder
//////////////////////////////////////////////////////////////////////
class Iscas89BnBuilder :
  public BnBuilder
{
public:

  /// @brief コンストラクタ
  /// @param[in] clock_name クロック端子名
  Iscas89BnBuilder(const string& clock_name = "clock");

  /// @brief デストラクタ
  virtual
  ~Iscas89BnBuilder();


public:
  //////////////////////////////////////////////////////////////////////
  // ファイル入力
  //////////////////////////////////////////////////////////////////////

  /// @brief iscas89(.bench) 形式のファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @retval true 正常に読み込めた
  /// @retval false 読み込み中にエラーが起こった．
  bool
  read_iscas89(const string& filename);


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定する外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  ///
  /// コンストラクタ直後と同じ状態になる．
  void
  clear();

  /// @brief DFFを追加する．
  /// @param[in] oname_id DFF名のID番号
  /// @param[in] oname DFF名
  /// @param[in] iname_id 入力名のID番号
  /// @param[in] iname 入力名
  /// @param[in] rval リセット値
  void
  add_dff(ymuint oname_id,
	  const string& oname,
	  ymuint iname_id,
	  const string& iname);

  /// @brief 外部入力ノードを追加する．
  /// @param[in] name_id 名前のID番号
  /// @param[in] name 名前
  void
  add_input(ymuint name_id,
	    const string& name);

  /// @brief 外部出力ノードを追加する．
  /// @param[in] name_id 名前のID番号
  /// @param[in] name 名前
  void
  add_output(ymuint name_id,
	     const string& name);

  /// @brief 論理式型の論理ノードを追加する．
  /// @param[in] oname_id 出力名のID番号
  /// @param[in] oname 出力名
  /// @param[in] inode_id_array ファンインのノード番号のリスト
  /// @param[in] logic_type 論理型
  void
  add_primitive(ymuint oname_id,
		const string& oname,
		const vector<ymuint>& inode_id_array,
		BnLogicType logic_type);

  /// @brief 最終処理を行う．
  /// @retval true 正しく設定されている．
  /// @retval false エラーが起こった．
  bool
  wrap_up();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // クロック端子名
  string mClockName;

  // 名前IDをキーにしてノード番号を格納するハッシュ表
  HashMap<ymuint, ymuint> mIdMap;

  // ノードIDをキーにしてファンイン情報を格納するハッシュ表
  HashMap<ymuint, vector<ymuint> > mFaninInfoMap;

  // クロック端子のノード番号
  ymuint mClockId;

  // 内容が正常かどうかを示すフラグ
  bool mSane;

};

END_NAMESPACE_YM_BNET

#endif // ISCAS89BNBUILDER_H
