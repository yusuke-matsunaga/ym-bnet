﻿#ifndef BNLOGICNODE_H
#define BNLOGICNODE_H

/// @file BnLogicNode.h
/// @brief BnLogicNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnNodeImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnLogicNode BnNodeImpl.h "BnNodeImpl.h"
/// @brief 論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class BnLogicNode :
  public BnNodeImpl
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] name ノード名
  /// @param[in] ni 入力数
  /// @param[in] cell セル (nullptr の場合もあり)
  BnLogicNode(int id,
	      const string& name,
	      int ni,
	      const ClibCell* cell);

  /// @brief デストラクタ
  ~BnLogicNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力の時 true を返す．
  bool
  is_logic() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理タイプ/ゲートタイプに共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  int
  fanin_num() const override;

  /// @brief ファンインのノード番号を返す．
  /// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
  int
  fanin_id(int pos) const override;

#if 0
  /// @brief ファンインのノード番号のリストを返す．
  Array<int>
  fanin_id_list() const override;
#else
  const vector<int>&
  fanin_id_list() const override;
#endif

  /// @brief セルを返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// 場合によっては nullptr を返す．
  const ClibCell*
  cell() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用のインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインを設定する．
  /// @param[in] ipos 入力位置
  /// @param[in] fanin_id ファンインのノード番号
  void
  set_fanin(int ipos,
	    int fanin_id) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

#if 1
  // ファンイン数
  int mFaninNum;

  // ファンインのノード番号の配列
  // サイズは mFaninNum
  int* mFanins;
#else
  vector<int> mFanins;
#endif

  // セル
  const ClibCell* mCell;

  // セルのピン番号の対応表
  // mCell == nullptr なら nullptr
  // mCellPinMap[i] に i 番目のファンインに対応するピン番号が入る．
  int* mCellPinMap;

};


//////////////////////////////////////////////////////////////////////
/// @class BnPrimNode BnNodeImpl.h "BnNodeImpl.h"
/// @brief プリミティブ型の論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class BnPrimNode :
  public BnLogicNode
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] name ノード名
  /// @param[in] ni 入力数
  /// @param[in] logic_type 論理タイプ
  /// @param[in] cell セル (nullptr の場合もあり)
  BnPrimNode(int id,
	     const string& name,
	     int ni,
	     BnNodeType logic_type,
	     const ClibCell* cell = nullptr);

  /// @brief デストラクタ
  ~BnPrimNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnNodeType
  type() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理タイプ/ゲートタイプに共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  // ここのメモリ領域はすべて BnNetworkImpl::mAlloc が管理する．
  //////////////////////////////////////////////////////////////////////

  // 論理タイプ
  BnNodeType mLogicType;

};


//////////////////////////////////////////////////////////////////////
/// @class BnExprNode BnNodeImpl.h "BnNodeImpl.h"
/// @brief 論理式型の論理ノードのクラス
//////////////////////////////////////////////////////////////////////
class BnExprNode :
  public BnLogicNode
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] name ノード名
  /// @param[in] ni 入力数
  /// @param[in] expr_id 関数番号
  /// @param[in] cell セル (nullptr の場合もあり)
  BnExprNode(int id,
	     const string& name,
	     int ni,
	     int expr_id,
	     const ClibCell* cell = nullptr);

  /// @brief デストラクタ
  ~BnExprNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnNodeType
  type() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 論理式番号を返す．
  ///
  /// logic_type() == BnNodeType::Expr の時のみ意味を持つ．
  /// 論理式番号は同じ BnNetwork 内で唯一となるもの．
  int
  expr_id() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 論理式番号
  int mExprId;

};


//////////////////////////////////////////////////////////////////////
/// @class BnTvNode BnNodeImpl.h "BnNodeImpl.h"
/// @brief 真理値表型の論理ノードのクラス
//////////////////////////////////////////////////////////////////////
class BnTvNode :
  public BnLogicNode
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] name ノード名
  /// @param[in] ni 入力数
  /// @param[in] func_id 関数番号
  /// @param[in] cell セル (nullptr の場合もあり)
  BnTvNode(int id,
	   const string& name,
	   int ni,
	   int func_id,
	   const ClibCell* cell = nullptr);

  /// @brief デストラクタ
  ~BnTvNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnNodeType
  type() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 関数番号を返す．
  ///
  /// logic_type() == BnNodeType::TvFunc の時のみ意味を持つ．
  /// 関数番号は同じ BnNetwork 内で唯一となるもの．
  int
  func_id() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 関数番号
  int mFuncId;

};

END_NAMESPACE_YM_BNET

#endif // BNLOGICNODE_H
