﻿#ifndef BNLOGICNODE_H
#define BNLOGICNODE_H

/// @file BnLogicNode.h
/// @brief BnLogicNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
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
  BnLogicNode(
    int id,             ///< [in] ID番号
    const string& name, ///< [in] ノード名
    int ni,             ///< [in] 入力数
    int cell_id         ///< [in] セル番号 (-1 の場合もあり)
  ) : BnNodeImpl(id, name),
      mFanins(ni, kBnNullId),
      mCellId(cell_id)
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
  int
  fanin_num() const override;

  /// @brief ファンインのノード番号を返す．
  int
  fanin_id(
    int pos ///< [in] 入力位置 ( 0 <= pos < fanin_num() )
  ) const override;

  /// @brief ファンインのノード番号のリストを返す．
  vector<int>
  fanin_id_list() const override;

  /// @brief セル番号を返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// 場合によっては -1 を返す．
  int
  cell_id() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用のインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインを設定する．
  /// @param[in] ipos
  /// @param[in] fanin_id
  void
  set_fanin(
    int ipos,    ///< [in] 入力位置
    int fanin_id ///< [in] ファンインのノード番号
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  vector<int> mFanins;

  // セル番号
  int mCellId;

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
  BnPrimNode(
    int id,                ///< [in] ID番号
    const string& name,    ///< [in] ノード名
    int ni,                ///< [in] 入力数
    BnNodeType logic_type, ///< [in] 論理タイプ
    int cell_id            ///< [in] セル番号 (-1 の場合もあり)
  ) : BnLogicNode(id, name, ni, cell_id),
      mLogicType(logic_type)
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
  type() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
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
  BnExprNode(
    int id,             ///< [in] ID番号
    const string& name, ///< [in] ノード名
    int ni,             ///< [in] 入力数
    int expr_id,        ///< [in] 関数番号
    int cell_id         ///< [in] セル番号 (-1 の場合もあり)
  ) : BnLogicNode(id, name, ni, cell_id),
      mExprId(expr_id)
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
  BnTvNode(
    int id,             ///< [in] ID番号
    const string& name, ///< [in] ノード名
    int ni,             ///< [in] 入力数
    int func_id,        ///< [in] 関数番号
    int cell_id         ///< [in] セル 番号(-1 の場合もあり)
  ) : BnLogicNode(id, name, ni, cell_id),
      mFuncId(func_id)
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
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 関数番号
  int mFuncId;

};

END_NAMESPACE_YM_BNET

#endif // BNLOGICNODE_H
