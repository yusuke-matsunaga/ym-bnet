﻿#ifndef YM_BNNODE_H
#define YM_BNNODE_H

/// @file ym/BnNode.h
/// @brief BnNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016-2018, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/clib.h"
#include "ym/logic.h"
#include "ym/Bdd.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnNode BnNode.h "ym/BnNode.h"
/// @ingroup BnetGroup
/// @brief Boolean Network (bnet)のノードを表すクラス
/// @sa BnNetwork
///
/// ノードには以下の3種類がある．
/// - 入力ノード
///   外部入力もしくはDFF，ラッチの出力を表す．
///
/// - 出力ノード
///   外部出力もしくはDFF，ラッチの入力を表す．
///   ソースのノード番号を持つ．
///
/// - 論理ノード
///   ファンインと論理関数を持つ．
///   論理関数は以下の4種類の方法で表す．
///   - プリミティブ(AND, NOT等)
///   - 論理式
///   - 真理値表
///   - BDD
///   その他にセル番号を持つ場合もある．
///
///   論理式と真理値表は BnNetwork 単位でユニークなID番号を持つ．
///   BDD は本来の性質からユニークとなっている．
///   ただし，論理式に関しては手抜きで 11 入力以上の式はすべて別個の式
///   として扱う．
///
///   真理値表タイプとBDDタイプの場合，実装するための構造を持たないため
///   blif や aig/aag の形式で出力することはできない．
///
/// ノードは名前を持つが．同じ名前のノードがあってもかまわない．
/// そのため名前をキーにしてノードを検索することはできない．
///
/// このクラスは実体を持たない純粋仮想基底クラスである．
//////////////////////////////////////////////////////////////////////
class BnNode
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @name コンストラクタ/デストラクタ
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief デストラクタ
  virtual
  ~BnNode() = default;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 全タイプ共通の外部インターフェイス
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードID を返す．
  virtual
  SizeType
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
  /// type() == kBnLogic_XXX と等価
  virtual
  bool
  is_logic() const = 0;

  /// @brief プリミティブ型の論理ノードの時 true を返す．
  ///
  /// 具体的には以下の型
  /// C0, C1, Buff, Not, And, Nand, Or, Nor, Xor Xnor
  virtual
  bool
  is_primitive_logic() const = 0;

  /// @brief ファンアウト数を得る．
  virtual
  SizeType
  fanout_num() const = 0;

  /// @brief ファンアウトのノード番号を返す．
  virtual
  SizeType
  fanout_id(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fanout_num() )
  ) const = 0;

  /// @brief ファンアウトのノード番号のリストを返す．
  virtual
  vector<SizeType>
  fanout_id_list() const = 0;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 入力ノード(BnNodeType::INPUT)の外部インターフェイス
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力番号を返す．
  ///
  /// - is_input() == false の時の動作は不定
  /// - node_id = BnNetwork::input_id(pos) の時 node->input_pos() = pos となる．
  /// @sa BnNetwork::input_id()
  virtual
  SizeType
  input_pos() const = 0;

  /// @brief 外部入力端子の時 true を返す．
  virtual
  bool
  is_port_input() const = 0;

  /// @brief 外部入力番号を返す．
  ///
  /// is_port_input() == true の時のみ意味を持つ<br>
  /// node_id = BnNetwork::primary_input_id(pos) の時
  /// node->primary_input_pos() = pos となる．
  virtual
  SizeType
  primary_input_pos() const = 0;

  /// @brief DFF/ラッチのデータ出力端子の時 true を返す．
  virtual
  bool
  is_data_out() const = 0;

  /// @brief DFFセルの出力端子の時 true を返す．
  virtual
  bool
  is_cell_output() const = 0;

  /// @brief DFFセルの出力ピン番号を返す．
  virtual
  SizeType
  cell_output_pos() const = 0;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 出力ノード(BnNodeType::OUTPUT)の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号を返す．
  ///
  /// - is_output() == false の時の動作は不定
  /// - node_id = BnNetwork::output_id(pos) の時，node->output_pos() = pos となる．
  /// @sa BnNetwork::output()
  virtual
  SizeType
  output_pos() const = 0;

  /// @brief ソースノードのノード番号を返す．
  virtual
  SizeType
  output_src() const = 0;

  /// @brief 外部出力端子の時に true を返す．
  virtual
  bool
  is_port_output() const = 0;

  /// @brief 外部出力端子番号を返す．
  ///
  /// is_port_output() == true の時のみ意味を持つ．<br>
  /// node_id = BnNetwork::primary_output_id(pos) の時
  /// node->primary_output_pos() = pos となる．
  virtual
  SizeType
  primary_output_pos() const = 0;

  /// @brief DFF/ラッチののデータ入力端子の時に true を返す．
  virtual
  bool
  is_data_in() const = 0;

  /// @brief DFF/ラッチのクロック/イネーブル端子の時に true を返す．
  virtual
  bool
  is_clock() const = 0;

  /// @brief DFF/ラッチのクリア端子の時に true を返す．
  virtual
  bool
  is_clear() const = 0;

  /// @brief DFF/ラッチのプリセット端子の時に true を返す．
  virtual
  bool
  is_preset() const = 0;

  /// @brief DFF/ラッチセルの入力端子の時 true を返す．
  virtual
  bool
  is_cell_input() const = 0;

  /// @brief DFF/ラッチセルの入力ピン番号を返す．
  virtual
  SizeType
  cell_input_pos() const = 0;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 入力ノード/出力ノードに共通なインターフェイス
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているポート番号を返す．
  ///
  /// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
  virtual
  SizeType
  port_id() const = 0;

  /// @brief 接続しているポート中のビット番号を返す．
  ///
  /// is_port_input() || is_port_output() の時のみ意味を持つ．
  virtual
  SizeType
  port_bit() const = 0;

  /// @brief 接続しているDFFの番号を返す．
  ///
  /// is_dff_input() || is_dff_output()
  /// の時のみ意味を持つ．
  virtual
  SizeType
  dff_id() const = 0;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 論理ノードの外部インターフェイス
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  ///
  /// - is_logic() == false の時の動作は不定
  virtual
  SizeType
  fanin_num() const = 0;

  /// @brief ファンインのノード番号を返す．
  ///
  /// - is_logic() == false の時の動作は不定
  virtual
  SizeType
  fanin_id(
    SizeType pos ///< [in] 入力位置 ( 0 <= pos < fanin_num() )
  ) const = 0;

  /// @brief ファンインのノードを返す．
  const BnNode&
  fanin(
    SizeType pos,            ///< [in] 位置番号 ( 0 <= pos < fanin_num() )
    const BnNetwork& network ///< [in] 対象のネットワーク
  ) const;

  /// @brief ファンインのノード番号のリストを返す．
  virtual
  vector<SizeType>
  fanin_id_list() const = 0;

  /// @brief ファンインのノードのリストを返す．
  BnNodeList
  fanin_list(
    const BnNetwork& network ///< [in] 対象のネットワーク
  ) const;

  /// @brief 論理式番号を返す．
  ///
  /// - type() == Expr の時のみ意味を持つ．
  /// - 論理式番号は同じ BnNetwork 内で唯一となるもの．
  /// @sa BnNetwork::expr()
  virtual
  SizeType
  expr_id() const = 0;

  /// @brief 関数番号を返す．
  ///
  /// - type() == TvFunc の時のみ意味を持つ．
  /// - 関数番号は同じ BnNetwork 内で唯一となるもの．
  /// @sa BnNetwork::func()
  virtual
  SizeType
  func_id() const = 0;

  /// @brief Bdd を返す．
  ///
  /// - type() == Bdd の時のみ意味を持つ．
  virtual
  Bdd
  bdd() const = 0;

  /// @brief セル番号を返す．
  ///
  /// - type() == Cell の時のみ意味を持つ．
  virtual
  SizeType
  cell_id() const = 0;

  /// @}
  //////////////////////////////////////////////////////////////////////

};

END_NAMESPACE_YM_BNET

#endif // YM_BNNODE_H
