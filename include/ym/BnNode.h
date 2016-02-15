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

  /// @brief 外部入力の時 true を返す．
  virtual
  bool
  is_input() const = 0;

  /// @brief 論理ノードの時 true を返す．
  virtual
  bool
  is_logic() const = 0;

  /// @brief D-FF ノードの時 true を返す．
  virtual
  bool
  is_dff() const = 0;

  /// @brief ファンアウト数を得る．
  virtual
  ymuint
  fanout_num() const = 0;

  /// @brief ファンアウトのノードIDを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  virtual
  ymuint
  fanout_id(ymuint pos) const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理ノードの外部インターフェイス
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

  /// @brief 関数のタイプを返す．
  const BnFuncType*
  func_type() const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // D-FFノードの外部インターフェイス
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
