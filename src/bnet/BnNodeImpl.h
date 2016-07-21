#ifndef BNNODEIMPL_H
#define BNNODEIMPL_H

/// @file BnNodeImpl.h
/// @brief BnNodeImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNode.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnNodeImpl BnNodeImpl.h "BnNodeImpl.h"
/// @brief BnNode の実装クラスの基底クラス
//////////////////////////////////////////////////////////////////////
class BnNodeImpl :
  public BnNode
{
  friend class BnNetworkImpl;

public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID 番号
  /// @param[in] name ノード名
  BnNodeImpl(ymuint id,
	     const string& name);

  /// @brief デストラクタ
  virtual
  ~BnNodeImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードID を返す．
  virtual
  ymuint
  id() const;

  /// @brief 名前を返す．
  virtual
  string
  name() const;

  /// @brief 外部入力の時 true を返す．
  virtual
  bool
  is_input() const;

  /// @brief 外部出力の時 true を返す．
  virtual
  bool
  is_output() const;

  /// @brief 論理ノードの時 true を返す．
  virtual
  bool
  is_logic() const;

  /// @brief ファンアウトを追加する．
  /// @param[in] node ノード番号
  virtual
  void
  add_fanout(BnNode* node);

  /// @brief ファンアウト数を得る．
  virtual
  ymuint
  fanout_num() const;

  /// @brief ファンアウトのノード番号を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  virtual
  const BnNode*
  fanout(ymuint pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const;

  /// @brief ファンインのノード番号を返す．
  /// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
  virtual
  const BnNode*
  fanin(ymuint pos) const;

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
  ///
  /// 親のネットワークの expr(node->expr_id()) と同一
  virtual
  Expr
  expr() const;

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
  /// 親のネットワークの func(node->func_id()) と同一
  virtual
  TvFunc
  func() const;

  /// @brief セルを返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// 場合によっては nullptr の場合もある．
  virtual
  const Cell*
  cell() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力のノードを返す．
  virtual
  const BnNode*
  input() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  ymuint mId;

  // 名前
  string mName;

  // ファンアウトのノードのリスト
  vector<BnNode*> mFanoutList;

};


//////////////////////////////////////////////////////////////////////
/// @class BnInputNode BnNodeImpl.h "BnNodeImpl.h"
/// @brief 入力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class BnInputNode :
  public BnNodeImpl
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID 番号
  /// @param[in] name ノード名
  BnInputNode(ymuint id,
	      const string& name);

  /// @brief デストラクタ
  virtual
  ~BnInputNode();


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
  is_input() const;

};


//////////////////////////////////////////////////////////////////////
/// @class BnOutputNode BnNodeImpl.h "BnNodeImpl.h"
/// @brief 外部出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class BnOutputNode :
  public BnNodeImpl
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID 番号
  /// @param[in] name ノード名
  /// @param[in] input 入力ノード
  BnOutputNode(ymuint id,
	       const string& name,
	       BnNode* input);

  /// @brief デストラクタ
  virtual
  ~BnOutputNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  virtual
  BnNodeType
  type() const;

  /// @brief 外部出力ノードの時 true を返す．
  virtual
  bool
  is_output() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力のノードを返す．
  ///
  /// is_output() == false の時の動作は不定
  virtual
  const BnNode*
  input() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力のノード
  BnNode* mInput;

};


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
  /// @param[in] fanins ファンインのノードの配列
  /// @param[in] cell セル (nullptr の場合もあり)
  BnLogicNode(ymuint id,
	      const string& name,
	      const vector<BnNode*>& fanins,
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

  /// @brief ファンインのノードを返す．
  /// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
  virtual
  const BnNode*
  fanin(ymuint pos) const;

  /// @brief セルを返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// 場合によっては nullptr を返す．
  virtual
  const Cell*
  cell() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンインのノードの配列
  vector<BnNode*> mFanins;

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
  /// @param[in] fanins ファンインのノードの配列
  /// @param[in] logic_type 論理タイプ
  /// @param[in] cell セル (nullptr の場合もあり)
  BnPrimNode(ymuint id,
	     const string& name,
	     const vector<BnNode*>& fanins,
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
  /// @param[in] fanins ファンインのノードの配列
  /// @param[in] expr 論理式
  /// @param[in] expr_id 関数番号
  /// @param[in] cell セル (nullptr の場合もあり)
  BnExprNode(ymuint id,
	     const string& name,
	     const vector<BnNode*>& fanins,
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
  /// @param[in] fanins ファンインのノードの配列
  /// @param[in] func 真理値表
  /// @param[in] func_id 関数番号
  /// @param[in] cell セル (nullptr の場合もあり)
  BnTvNode(ymuint id,
	   const string& name,
	   const vector<BnNode*>& fanins,
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

#endif // BNNODEIMPL_H
