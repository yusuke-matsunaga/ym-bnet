#ifndef YM_BNNODE_H
#define YM_BNNODE_H

/// @file ym/BnNode.h
/// @brief BnNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016-2018, 2022-2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/clib.h"
#include "ym/logic.h"
#include "ym/bdd_nsdef.h"


BEGIN_NAMESPACE_YM_BNET

class BnNetworkImpl;
class BnNodeImpl;

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
/// このクラスは実はただのハンドルであり，実装は隠されている．
//////////////////////////////////////////////////////////////////////
class BnNode
{
  //friend class BnNetworkImpl;

public:
  //////////////////////////////////////////////////////////////////////
  /// @name コンストラクタ/デストラクタ
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 空のコンストラクタ
  ///
  /// 不正値となる．
  BnNode() = default;

  /// @brief コンストラクタ
  BnNode(
    const BnNetworkImpl* network, ///< [in] ネットワーク
    SizeType id                   ///< [in] ノード番号
  ) : mNetwork{network},
      mId{id}
  {
  }

  /// @brief デストラクタ
  ~BnNode() = default;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 全タイプ共通の外部インターフェイス
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 適正な値を持っている時 true を返す．
  bool
  is_valid() const
  {
    return mId != BNET_NULLID;
  }

  /// @brief 不正な値を持っている時 true を返す．
  bool
  is_invalid() const
  {
    return !is_valid();
  }

  /// @brief ノードID を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 名前を返す．
  string
  name() const;

  /// @brief ノードのタイプを返す．
  BnNodeType
  type() const;

  /// @brief 入力タイプの時 true を返す．
  ///
  /// type() == Input と等価
  bool
  is_input() const;

  /// @brief 出力タイプの時 true を返す．
  ///
  /// type() == Output と等価
  bool
  is_output() const;

  /// @brief 論理ノードの時 true を返す．
  ///
  /// type() != Input && type() != Output と等価
  bool
  is_logic() const;

  /// @brief プリミティブ型の論理ノードの時 true を返す．
  ///
  /// 具体的には以下の型
  /// C0, C1, Buff, Not, And, Nand, Or, Nor, Xor Xnor
  bool
  is_primitive_logic() const;

  /// @brief ファンアウト数を得る．
  SizeType
  fanout_num() const;

  /// @brief ファンアウトのノードを返す．
  BnNode
  fanout(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fanout_num() )
  ) const;

  /// @brief ファンアウトのノードのリストを返す．
  BnNodeList
  fanout_list() const;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 入力ノード(BnNodeType::Input)の外部インターフェイス
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力番号を返す．
  ///
  /// - is_input() == false の時の動作は不定
  /// - node_id = BnNetwork::input_id(pos) の時 node->input_pos() = pos となる．
  /// @sa BnNetwork::input_id()
  SizeType
  input_pos() const;

  /// @brief 外部入力端子の時 true を返す．
  bool
  is_port_input() const;

  /// @brief 外部入力番号を返す．
  ///
  /// is_port_input() == true の時のみ意味を持つ<br>
  /// node_id = BnNetwork::primary_input_id(pos) の時
  /// node->primary_input_pos() = pos となる．
  SizeType
  primary_input_pos() const;

  /// @brief DFF/ラッチのデータ出力端子の時 true を返す．
  bool
  is_data_out() const;

  /// @brief DFFセルの出力端子の時 true を返す．
  bool
  is_cell_output() const;

  /// @brief DFFセルの出力ピン番号を返す．
  SizeType
  cell_output_pos() const;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 出力ノード(BnNodeType::Output)の外部インターフェイス
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号を返す．
  ///
  /// - is_output() == false の時の動作は不定
  /// - node_id = BnNetwork::output_id(pos) の時，node->output_pos() = pos となる．
  /// @sa BnNetwork::output()
  SizeType
  output_pos() const;

  /// @brief ソースノードを返す．
  BnNode
  output_src() const;

  /// @brief 外部出力端子の時に true を返す．
  bool
  is_port_output() const;

  /// @brief 外部出力端子番号を返す．
  ///
  /// is_port_output() == true の時のみ意味を持つ．<br>
  /// node_id = BnNetwork::primary_output_id(pos) の時
  /// node->primary_output_pos() = pos となる．
  SizeType
  primary_output_pos() const;

  /// @brief DFF/ラッチののデータ入力端子の時に true を返す．
  bool
  is_data_in() const;

  /// @brief DFF/ラッチのクロック/イネーブル端子の時に true を返す．
  bool
  is_clock() const;

  /// @brief DFF/ラッチのクリア端子の時に true を返す．
  bool
  is_clear() const;

  /// @brief DFF/ラッチのプリセット端子の時に true を返す．
  bool
  is_preset() const;

  /// @brief DFF/ラッチセルの入力端子の時 true を返す．
  bool
  is_cell_input() const;

  /// @brief DFF/ラッチセルの入力ピン番号を返す．
  SizeType
  cell_input_pos() const;

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
  SizeType
  port_id() const;

  /// @brief 接続しているポート中のビット番号を返す．
  ///
  /// is_port_input() || is_port_output() の時のみ意味を持つ．
  SizeType
  port_bit() const;

  /// @brief 接続しているDFFの番号を返す．
  ///
  /// is_dff_input() || is_dff_output()
  /// の時のみ意味を持つ．
  SizeType
  dff_id() const;

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
  SizeType
  fanin_num() const;

  /// @brief ファンインのノード番号を返す．
  ///
  /// - is_logic() == false の時の動作は不定
  SizeType
  fanin_id(
    SizeType pos ///< [in] 入力位置 ( 0 <= pos < fanin_num() )
  ) const;

  /// @brief ファンインのノードを返す．
  BnNode
  fanin(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fanin_num() )
  ) const;

  /// @brief ファンインのノードのリストを返す．
  BnNodeList
  fanin_list() const;

  /// @brief 論理式番号を返す．
  ///
  /// - type() == Expr の時のみ意味を持つ．
  /// - 論理式番号は同じ BnNetwork 内で唯一となるもの．
  /// @sa BnNetwork::expr()
  SizeType
  expr_id() const;

  /// @brief 関数番号を返す．
  ///
  /// - type() == TvFunc の時のみ意味を持つ．
  /// - 関数番号は同じ BnNetwork 内で唯一となるもの．
  /// @sa BnNetwork::func()
  SizeType
  func_id() const;

  /// @brief Bdd を返す．
  ///
  /// - type() == Bdd の時のみ意味を持つ．
  Bdd
  bdd() const;

  /// @brief セル番号を返す．
  ///
  /// - type() == Cell の時のみ意味を持つ．
  SizeType
  cell_id() const;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 等価比較演算
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 等価比較演算
  bool
  operator==(
    const BnNode& right
  ) const
  {
    return mNetwork == right.mNetwork && mId == right.mId;
  }

  /// @brief 非等価比較演算
  bool
  operator!=(
    const BnNode& right
  ) const
  {
    return !operator==(right);
  }

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  // 内部の実装に関する走査
  //////////////////////////////////////////////////////////////////////

  /// @brief 実装クラスを取り出す．
  BnNodeImpl*
  _impl();

  /// @brief ネットワークを取り出す．
  const BnNetworkImpl*
  _network()
  {
    return mNetwork;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  const BnNetworkImpl* mNetwork{nullptr};

  // ノード番号
  SizeType mId{BNET_NULLID};

};

END_NAMESPACE_YM_BNET

#endif // YM_BNNODE_H
