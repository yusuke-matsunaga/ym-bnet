#ifndef YM_BNNODE_H
#define YM_BNNODE_H

/// @file ym/BnNode.h
/// @brief BnNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/ym_logic.h"
#include "ym/ym_cell.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnNode BnNode.h "ym/BnNode.h"
/// @ingroup BnetGroup
/// @brief Boolean Network (bn)のノードを表すクラス
/// @sa BnNetwork
///
/// ノードには以下の3種類がある．
/// - 外部入力ノード
/// - DFFノード
///   1つのファンインとリセット値を持つ．
/// - 論理ノード
///   ファンインと論理関数を持つ．
///   論理関数は以下の4種類の方法で表す．
///   - プリミティブ(AND, NOT等)
///   - セル
///   - 論理式
///   - 真理値表
///
/// このクラスは実体を持たない純粋仮想基底クラスである．
//////////////////////////////////////////////////////////////////////
class BnNode
{
public:
  //////////////////////////////////////////////////////////////////////
  // データ型の定義
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードタイプ
  enum Type {
    /// @brief 外部入力ノード
    kInput,

    /// @brief DFFノード
    kDFF,

    /// @brief 論理ノード
    kLogic
  };


public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief デストラクタ
  virtual
  ~BnNode() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードID を返す．
  virtual
  ymuint
  id() const = 0;

  /// @brief 名前を返す．
  virtual
  string
  name() const = 0;

  /// @brief ノードのタイプを返す．
  virtual
  Type
  type() const = 0;

  /// @brief 外部入力の時 true を返す．
  ///
  /// type() == kInput と等価
  virtual
  bool
  is_input() const = 0;

  /// @brief D-FF ノードの時 true を返す．
  ///
  /// type() == kDFF と等価
  virtual
  bool
  is_dff() const = 0;

  /// @brief 論理ノードの時 true を返す．
  ///
  /// type() == kLogic と等価
  virtual
  bool
  is_logic() const = 0;

  /// @brief ファンアウトを追加する．
  /// @param[in] node ノード
  virtual
  void
  add_fanout(const BnNode* node) = 0;

  /// @brief ファンアウト数を得る．
  virtual
  ymuint
  fanout_num() const = 0;

  /// @brief ファンアウトのノードを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  virtual
  const BnNode*
  fanout(ymuint pos) const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  ///
  /// is_logic() == false の時の動作は不定
  virtual
  ymuint
  fanin_num() const = 0;

  /// @brief ファンインのノードを返す．
  /// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
  ///
  /// is_logic() == false の時の動作は不定
  virtual
  ymuint
  fanin(ymuint pos) const = 0;

  /// @brief 論理タイプを返す．
  ///
  /// is_logic() == false の時の動作は不定
  virtual
  BnLogicType
  logic_type() const = 0;

  /// @brief 論理式を返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// logic_type() != kBnLt_EXPR の時の動作は不定
  virtual
  Expr
  expr() const = 0;

  /// @brief 関数番号を返す．
  ///
  /// logic_type() == kBnLt_EXPR|kBnLt_TV の時のみ意味を持つ．
  /// 論理式番号は同じ BnNetwork 内で唯一となるもの．
  virtual
  ymuint
  func_id() const = 0;

  /// @brief 真理値表を返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// logic_type() != kBnLt_TV の時の動作は不定
  virtual
  TvFunc
  tv() const = 0;

  /// @brief セルを返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// 場合によっては nullptr を返す．
  virtual
  const Cell*
  cell() const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // D-FFノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力のノード番号を返す．
  ///
  /// is_dff() == false の時の動作は不定
  virtual
  ymuint
  input() const = 0;

  /// @brief リセット値を返す．
  /// @retval '0': 0
  /// @retval '1': 1
  /// @retval '-': 未設定
  ///
  /// is_dff() == false の時の動作は不定
  virtual
  char
  reset_val() const = 0;

};

END_NAMESPACE_YM_BNET

#endif // YM_BNNODE_H
