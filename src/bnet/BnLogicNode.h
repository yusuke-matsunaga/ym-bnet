#ifndef BNLOGICNODE_H
#define BNLOGICNODE_H

/// @file BnLogicNode.h
/// @brief BnLogicNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnNodeImpl.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"


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
  BnLogicNode(ymuint id,
	      const string& name,
	      ymuint ni,
	      const Cell* cell);

  /// @brief デストラクタ
  virtual
  ~BnLogicNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  virtual
  BnNodeType
  type() const;

  /// @brief 外部入力の時 true を返す．
  virtual
  bool
  is_logic() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理タイプ/ゲートタイプに共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const;

  /// @brief ファンインのノード番号を返す．
  /// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
  virtual
  ymuint
  fanin(ymuint pos) const;

  /// @brief セルを返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// 場合によっては nullptr を返す．
  virtual
  const Cell*
  cell() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用のインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインを設定する．
  /// @param[in] ipos 入力位置
  /// @param[in] fanin_id ファンインのノード番号
  virtual
  void
  set_fanin(ymuint ipos,
	    ymuint fanin_id);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンイン数
  ymuint mFaninNum;

  // ファンインのノード番号の配列
  // サイズは mFaninNum
  ymuint* mFanins;

  // セル
  const Cell* mCell;

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
  BnPrimNode(ymuint id,
	     const string& name,
	     ymuint ni,
	     BnLogicType logic_type,
	     const Cell* cell = nullptr);

  /// @brief デストラクタ
  virtual
  ~BnPrimNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 論理タイプ/ゲートタイプに共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 論理タイプを返す．
  ///
  /// is_logic() == false の時の動作は不定
  virtual
  BnLogicType
  logic_type() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  // ここのメモリ領域はすべて BnNetworkImpl::mAlloc が管理する．
  //////////////////////////////////////////////////////////////////////

  // 論理タイプ
  BnLogicType mLogicType;

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
  /// @param[in] expr 論理式
  /// @param[in] expr_id 関数番号
  /// @param[in] cell セル (nullptr の場合もあり)
  BnExprNode(ymuint id,
	     const string& name,
	     ymuint ni,
	     const Expr& expr,
	     ymuint expr_id,
	     const Cell* cell = nullptr);

  /// @brief デストラクタ
  ~BnExprNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 論理タイプを返す．
  ///
  /// is_logic() == false の時の動作は不定
  virtual
  BnLogicType
  logic_type() const;

  /// @brief 論理式番号を返す．
  ///
  /// logic_type() == kBnLt_EXPR の時のみ意味を持つ．
  /// 論理式番号は同じ BnNetwork 内で唯一となるもの．
  virtual
  ymuint
  expr_id() const;

  /// @brief 論理式を返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// logic_type() != kBnLt_EXPR の時の動作は不定
  virtual
  Expr
  expr() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 論理式
  Expr mExpr;

  // 論理式番号
  ymuint mExprId;

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
  /// @param[in] func 真理値表
  /// @param[in] func_id 関数番号
  /// @param[in] cell セル (nullptr の場合もあり)
  BnTvNode(ymuint id,
	   const string& name,
	   ymuint ni,
	   const TvFunc& func,
	   ymuint func_id,
	   const Cell* cell = nullptr);

  /// @brief デストラクタ
  ~BnTvNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 論理タイプを返す．
  ///
  /// is_logic() == false の時の動作は不定
  virtual
  BnLogicType
  logic_type() const;

  /// @brief 関数番号を返す．
  ///
  /// logic_type() == kBnLt_TV の時のみ意味を持つ．
  /// 関数番号は同じ BnNetwork 内で唯一となるもの．
  virtual
  ymuint
  func_id() const;

  /// @brief 真理値表を返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// logic_type() != kBnLt_TV の時の動作は不定
  TvFunc
  func() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 真理値表
  TvFunc mFunc;

  // 関数番号
  ymuint mFuncId;

};

END_NAMESPACE_YM_BNET

#endif // BNLOGICNODE_H
