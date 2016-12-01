﻿#ifndef YM_BNNODE_H
#define YM_BNNODE_H

/// @file ym/BnNode.h
/// @brief BnNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
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
/// - 入力ノード
///   外部入力もしくはDFF，ラッチの出力を表す．
///
/// - 出力ノード
///   外部出力もしくはDFF，ラッチの入力を表す．
///   ファンインのノード番号を持つ．
///
/// - 論理ノード
///   ファンインと論理関数を持つ．
///   論理関数は以下の3種類の方法で表す．
///   - プリミティブ(AND, NOT等)
///   - 論理式
///   - 真理値表
///   その他にセルのポインタを持つ場合もある．
///   論理式と真理値表は BnNetwork 単位でユニークなID番号を持つ．
///   ただし，論理式に関しては手抜きで 11 入力以上の式はすべて別個の式
///   として扱う．
///
/// ノードは名前を持つが．同じ名前のノードがあってもかまわない．
/// ということは名前をキーにしてノードを検索することはできない．
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
  string
  name() const = 0;

  /// @brief ノードのタイプを返す．
  virtual
  BnNodeType
  type() const = 0;

  /// @brief 入力タイプの時 true を返す．
  ///
  /// type() == kBnInput と等価
  virtual
  bool
  is_input() const = 0;

  /// @brief 出力タイプの時 true を返す．
  ///
  /// type() == kBnOutput と等価
  virtual
  bool
  is_output() const = 0;

  /// @brief 論理ノードの時 true を返す．
  ///
  /// type() == kBnLogic と等価
  virtual
  bool
  is_logic() const = 0;

  /// @brief ファンアウト数を得る．
  virtual
  ymuint
  fanout_num() const = 0;

  /// @brief ファンアウトのノード番号を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  virtual
  ymuint
  fanout(ymuint pos) const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力番号を返す．
  ///
  /// is_input() == false の時の動作は不定<br>
  /// node = BnNetwork::input(id) の時 node->input_id() = id となる．
  virtual
  ymuint
  input_id() const = 0;

  /// @brief 外部入力端子の時 true を返す．
  virtual
  bool
  is_port_input() const = 0;

  /// @brief DFFの出力端子の時 true を返す．
  virtual
  bool
  is_dff_output() const = 0;

  /// @brief ラッチの出力端子の時 true を返す．
  virtual
  bool
  is_latch_output() const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号を返す．
  ///
  /// is_output() == false の時の動作は不定<br>
  /// node = BnNetwork::output(id) の時，node->output_id() = id となる．
  virtual
  ymuint
  output_id() const = 0;

  /// @brief 外部出力端子の時に true を返す．
  virtual
  bool
  is_port_output() const = 0;

  /// @brie DFFの入力端子の時に true を返す．
  virtual
  bool
  is_dff_input() const = 0;

  /// @brief DFFのクロック端子の時に true を返す．
  virtual
  bool
  is_dff_clock() const = 0;

  /// @brief DFFのクリア端子の時に true を返す．
  virtual
  bool
  is_dff_clear() const = 0;

  /// @brief DFFのプリセット端子の時に true を返す．
  virtual
  bool
  is_dff_preset() const = 0;

  /// @brief ラッチの入力端子の時に true を返す．
  virtual
  bool
  is_latch_input() const = 0;

  /// @brief ラッチのイネーブル端子の時に true を返す．
  virtual
  bool
  is_latch_enable() const = 0;

  /// @brief ラッチのクリア端子の時に true を返す．
  virtual
  bool
  is_latch_clear() const = 0;

  /// @brief ラッチのプリセット端子の時に true を返す．
  virtual
  bool
  is_latch_preset() const = 0;

  /// @brief ファンインのノード番号を返す．
  ///
  /// is_output() == false の時の動作は不定
  virtual
  ymuint
  fanin() const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているポート番号を返す．
  ///
  /// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
  virtual
  ymuint
  port_id() const = 0;

  /// @brief 接続しているポート中のビット番号を返す．
  ///
  /// is_port_input() || is_port_output() の時のみ意味を持つ．
  virtual
  ymuint
  port_bit() const = 0;

  /// @brief 接続しているDFFの番号を返す．
  ///
  /// is_dff_input() || is_dff_output() || is_dff_clock() || is_dff_clear() || is_dff_preset()
  /// の時のみ意味を持つ．
  virtual
  ymuint
  dff_id() const = 0;

  /// @brief 接続しているラッチの番号を返す．
  ///
  /// is_latch_input() || is_latch_output() || is_latch_enable() || is_latch_clear() || is_latch_preset()
  /// の時のみ意味を持つ．
  virtual
  ymuint
  latch_id() const = 0;


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

  /// @brief ファンインのノード番号を返す．
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

  /// @brief 論理式番号を返す．
  ///
  /// logic_type() == kBnLt_EXPR の時のみ意味を持つ．
  /// 論理式番号は同じ BnNetwork 内で唯一となるもの．
  virtual
  ymuint
  expr_id() const = 0;

  /// @brief 論理式を返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// logic_type() != kBnLt_EXPR の時の動作は不定
  ///
  /// 親のネットワークの expr(node->expr_id()) と同一
  virtual
  Expr
  expr() const = 0;

  /// @brief 関数番号を返す．
  ///
  /// logic_type() == kBnLt_TV の時のみ意味を持つ．
  /// 関数番号は同じ BnNetwork 内で唯一となるもの．
  virtual
  ymuint
  func_id() const = 0;

  /// @brief 真理値表を返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// logic_type() != kBnLt_TV の時の動作は不定
  /// 親のネットワークの func(node->func_id()) と同一
  virtual
  TvFunc
  func() const = 0;

  /// @brief セルを返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// 場合によっては nullptr を返す．
  virtual
  const Cell*
  cell() const = 0;

};

END_NAMESPACE_YM_BNET

#endif // YM_BNNODE_H
