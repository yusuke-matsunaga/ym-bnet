﻿#ifndef BNLOGICNODE_H
#define BNLOGICNODE_H

/// @file BnLogicNode.h
/// @brief BnLogicNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "BnNodeImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnLogicNode BnLogicNode.h "BnLogicNode.h"
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
  BnLogicNode(
    const string& name, ///< [in] ノード名
    SizeType ni         ///< [in] 入力数
  ) : BnNodeImpl{name},
      mFanins(ni, BNET_NULLID)
  {
  }

  /// @brief デストラクタ
  ~BnLogicNode() = default;


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
  SizeType
  fanin_num() const override;

  /// @brief ファンインのノード番号を返す．
  SizeType
  fanin_id(
    SizeType pos ///< [in] 入力位置 ( 0 <= pos < fanin_num() )
  ) const override;

  /// @brief ファンインのノード番号のリストを返す．
  vector<SizeType>
  fanin_id_list() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用のインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインを設定する．
  /// @param[in] ipos
  /// @param[in] fanin_id
  void
  set_fanin(
    SizeType ipos,    ///< [in] 入力位置
    SizeType fanin_id ///< [in] ファンインのノード番号
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  vector<SizeType> mFanins;

};


//////////////////////////////////////////////////////////////////////
/// @class BnPrimNode BnLogicNode.h "BnLogicNode.h"
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
  BnPrimNode(
    const string& name,   ///< [in] ノード名
    SizeType ni,          ///< [in] 入力数
    BnNodeType logic_type ///< [in] 論理タイプ
  ) : BnLogicNode{name, ni},
      mLogicType{logic_type}
  {
  }

  /// @brief デストラクタ
  ~BnPrimNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnNodeType
  type() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 論理タイプ
  BnNodeType mLogicType;

};


//////////////////////////////////////////////////////////////////////
/// @class BnExprNode BnLogicNode.h "BnLogicNode.h"
/// @brief 論理式型の論理ノードのクラス
//////////////////////////////////////////////////////////////////////
class BnExprNode :
  public BnLogicNode
{
public:

  /// @brief コンストラクタ
  BnExprNode(
    const string& name, ///< [in] ノード名
    SizeType ni,        ///< [in] 入力数
    SizeType expr_id    ///< [in] 関数番号
  ) : BnLogicNode{name, ni},
      mExprId{expr_id}
  {
  }

  /// @brief デストラクタ
  ~BnExprNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnNodeType
  type() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 論理式番号を返す．
  ///
  /// logic_type() == BnNodeType::Expr の時のみ意味を持つ．
  /// 論理式番号は同じ BnNetwork 内で唯一となるもの．
  SizeType
  expr_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 論理式番号
  SizeType mExprId;

};


//////////////////////////////////////////////////////////////////////
/// @class BnTvNode BnLogicNode.h "BnLogicNode.h"
/// @brief 真理値表型の論理ノードのクラス
//////////////////////////////////////////////////////////////////////
class BnTvNode :
  public BnLogicNode
{
public:

  /// @brief コンストラクタ
  BnTvNode(
    const string& name, ///< [in] ノード名
    SizeType ni,        ///< [in] 入力数
    SizeType func_id    ///< [in] 関数番号
  ) : BnLogicNode{name, ni},
      mFuncId{func_id}
  {
  }

  /// @brief デストラクタ
  ~BnTvNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnNodeType
  type() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 関数番号を返す．
  ///
  /// logic_type() == BnNodeType::TvFunc の時のみ意味を持つ．
  /// 関数番号は同じ BnNetwork 内で唯一となるもの．
  SizeType
  func_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 関数番号
  SizeType mFuncId;

};


//////////////////////////////////////////////////////////////////////
/// @class BnBddNode BnLogicNode.h "BnLogicNode.h"
/// @brief Bdd型の論理ノードのクラス
//////////////////////////////////////////////////////////////////////
class BnBddNode :
  public BnLogicNode
{
public:

  /// @brief コンストラクタ
  BnBddNode(
    const string& name, ///< [in] ノード名
    SizeType ni,        ///< [in] 入力数
    const Bdd& bdd      ///< [in] BDD
  ) : BnLogicNode{name, ni},
      mBdd{bdd}
  {
  }

  /// @brief デストラクタ
  ~BnBddNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnNodeType
  type() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief Bdd を返す．
  ///
  /// - type() == Bdd の時のみ意味を持つ．
  Bdd
  bdd() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // BDD
  Bdd mBdd;

};

END_NAMESPACE_YM_BNET

#endif // BNLOGICNODE_H
