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
/// @ingroup BlifGroup
/// @brief Boolean Network (bn)のノードを表すクラス
/// @sa BnNetwork
///
/// このクラスは実体を持たない純粋仮想基底クラスである．
//////////////////////////////////////////////////////////////////////
class BnNode
{
public:

  /// @brief ノードの種類
  enum Type {
    /// @brief 外部入力
    kInput,
    /// @brief プリミティブゲート
    kPrimGate,
    /// @brief セル
    kCell,
    /// @brief カバー
    kCover,
    /// @brief 論理式
    kExpr,
    /// @brief 真理値表
    kTv,
    /// @brief ラッチ
    kLatch
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
  const char*
  name() const = 0;

  /// @brief 型を返す．
  virtual
  Type
  type() const = 0;

  /// @brief ファンアウト数を得る．
  virtual
  ymuint
  fanout_num() const = 0;

  /// @brief ファンアウトのノードIDを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  virtual
  ymuint
  fanout_id(ymuint pos) const = 0;

  /// @brief 内容を blif 形式で出力する．
  /// @param[in] s 出力先のストリーム
  virtual
  void
  write_blif(ostream& s) const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理タイプ/ゲートタイプに共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const = 0;

  /// @brief ファンインのノードIDを返す．
  /// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
  virtual
  ymuint
  fanin_id(ymuint pos) const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // プリミティブゲートタイプの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートの型を返す．
  virtual
  GateType
  gate_type() const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // セルタイプの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief セルを返す．
  virtual
  const Cell*
  cell() const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // カバータイプの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief カバーを得る．
  virtual
  const BlifCover*
  cover() const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理式タイプの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 論理式を得る．
  virtual
  Expr
  expr() const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 真理値表タイプの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 真理値表を得る．
  virtual
  TvFunc
  truth_vector() const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // ラッチタイプの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力ノードのID番号を返す．
  virtual
  ymuint
  inode_id() const = 0;

  /// @brief リセット値を返す．
  virtual
  char
  reset_val() const = 0;

};

END_NAMESPACE_YM_BNET

#endif // YM_BNNODE_H
