﻿#ifndef BNNODEIMPL_H
#define BNNODEIMPL_H

/// @file BnNodeImpl.h
/// @brief BnNodeImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNode.h"


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
  BnNodeImpl(
    const string& name ///< [in] ノード名
  );

  /// @brief デストラクタ
  ~BnNodeImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードID を返す．
  SizeType
  id() const override;

  /// @brief 名前を返す．
  string
  name() const override;

  /// @brief 外部入力の時 true を返す．
  bool
  is_input() const override;

  /// @brief 外部出力の時 true を返す．
  bool
  is_output() const override;

  /// @brief 論理ノードの時 true を返す．
  bool
  is_logic() const override;

  /// @brief プリミティブ型の論理ノードの時 true を返す．
  ///
  /// 具体的には以下の型
  /// C0, C1, Buff, Not, And, Nand, Or, Nor, Xor Xnor
  bool
  is_primitive_logic() const override;

  /// @brief ファンアウト数を得る．
  SizeType
  fanout_num() const override;

  /// @brief ファンアウトのノード番号を返す．
  SizeType
  fanout_id(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fanout_num() )
  ) const override;

  /// @brief ファンアウトのノード番号のリストを返す．
  vector<SizeType>
  fanout_id_list() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力番号を返す．
  ///
  /// is_input() == false の時の動作は不定<br>
  /// node_id = BnNetwork::input_id(pos) の時 node->input_pos() = pos となる．
  SizeType
  input_pos() const override;

  /// @brief 外部入力端子の時 true を返す．
  bool
  is_port_input() const override;

  /// @brief 外部入力番号を返す．
  ///
  /// is_port_input() == true の時のみ意味を持つ<br>
  /// node_id = BnNetwork::primary_input_id(pos) の時
  /// node->primary_input_pos() = pos となる．
  SizeType
  primary_input_pos() const override;

  /// @brief DFFの出力端子の時 true を返す．
  bool
  is_data_out() const override;

  /// @brief DFFセルの出力端子の時 true を返す．
  virtual
  bool
  is_cell_output() const override;

  /// @brief DFFセルの出力ピン番号を返す．
  SizeType
  cell_output_pos() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号を返す．
  ///
  /// is_output() == false の時の動作は不定<br>
  /// node_id = BnNetwork::output_id(pos) の時，node->output_pos() = pos となる．
  SizeType
  output_pos() const override;

  /// @brief ソースノードのノード番号を返す．
  SizeType
  output_src() const override;

  /// @brief 外部出力端子の時に true を返す．
  bool
  is_port_output() const override;

  /// @brief 外部出力端子番号を返す．
  ///
  /// is_port_output() == true の時のみ意味を持つ．<br>
  /// node_id = BnNetwork::primary_output_id(pos) の時
  /// node->primary_output_pos() = pos となる．
  SizeType
  primary_output_pos() const override;

  /// @brie DFFの入力端子の時に true を返す．
  bool
  is_data_in() const override;

  /// @brief DFFのクロック端子の時に true を返す．
  bool
  is_clock() const override;

  /// @brief DFFのクリア端子の時に true を返す．
  bool
  is_clear() const override;

  /// @brief DFFのプリセット端子の時に true を返す．
  bool
  is_preset() const override;

  /// @brief DFFセルの入力端子の時 true を返す．
  bool
  is_cell_input() const override;

  /// @brief DFFセルの入力ピン番号を返す．
  SizeType
  cell_input_pos() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているポート番号を返す．
  ///
  /// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
  SizeType
  port_id() const override;

  /// @brief 接続しているポート中のビット番号を返す．
  ///
  /// is_port_input() || is_port_output() の時のみ意味を持つ．
  SizeType
  port_bit() const override;

  /// @brief 接続しているDFFの番号を返す．
  ///
  /// is_dff_input() || is_dff_output()
  /// is_clock() || is_clear() || is_preset()
  /// の時のみ意味を持つ．
  SizeType
  dff_id() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理ノードの外部インターフェイス
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

  /// @brief 論理式番号を返す．
  ///
  /// type() == BnNodeType::Expr の時のみ意味を持つ．
  /// 論理式番号は同じ BnNetwork 内で唯一となるもの．
  SizeType
  expr_id() const override;

  /// @brief 関数番号を返す．
  ///
  /// type() == BnNodeType::TvFunc の時のみ意味を持つ．
  /// 関数番号は同じ BnNetwork 内で唯一となるもの．
  SizeType
  func_id() const override;

  /// @brief Bdd を返す．
  ///
  /// - type() == Bdd の時のみ意味を持つ．
  Bdd
  bdd() const override;

  /// @brief セル番号を返す．
  ///
  /// - type() == Cell の時のみ意味を持つ．
  SizeType
  cell_id() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用のインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力位置を設定する．
  ///
  /// 入力ノード専用
  virtual
  void
  set_input_pos(
    SizeType ipos ///< [in] 入力位置
  );

  /// @brief 外部入力端子番号を設定する．
  ///
  /// 外部入力ノード専用
  virtual
  void
  set_primary_input_pos(
    SizeType pos ///< [in] 位置番号
  );

  /// @brief 出力位置を設定する．
  ///
  /// 出力ノード専用
  virtual
  void
  set_output_pos(
    SizeType opos ///< [in] 出力位置
  );

  /// @brief ソースノードを設定する．
  ///
  /// 出力ノード専用
  virtual
  void
  set_output_src(
    SizeType node_id ///< [in] ソースノードのノード番号
  );

  /// @brief 外部出力端子番号を設定する．
  ///
  /// 外部出力ノード専用
  virtual
  void
  set_primary_output_pos(
    SizeType pos ///< [in] 位置番号
  );

  /// @brief ファンインを設定する．
  virtual
  void
  set_fanin(
    SizeType ipos,    ///< [in] 入力位置
    SizeType fanin_id ///< [in] ファンインのノード番号
  );

  /// @brief ファンアウトリストをクリアする．
  void
  clear_fanout();

  /// @brief ファンアウトを追加する．
  void
  add_fanout(
    SizeType onode_id ///< [in] ファンアウトのノード番号
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 生成用のインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力端子のノードを作る．
  static
  BnNodeImpl*
  new_port_input(
    const string& name, ///< [in] ノード名
    SizeType port_id,   ///< [in] ポート番号
    SizeType port_bit   ///< [in] ポート中のビット位置
  );

  /// @brief DFFの出力端子のノードを作る．
  static
  BnNodeImpl*
  new_data_out(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  );

  /// @brief DFFセルの出力端子のノードを作る．
  static
  BnNodeImpl*
  new_cell_output(
    const string& name, ///< [in] ノード名
    SizeType dff_id,    ///< [in] DFF番号
    SizeType oid        ///< [in] セルの出力ピン番号
  );

  /// @brief 外部出力端子のノードを作る．
  static
  BnNodeImpl*
  new_port_output(
    const string& name, ///< [in] ノード名
    SizeType port_id,   ///< [in] ポート番号
    SizeType port_bit   ///< [in] ポート中のビット位置
  );

  /// @brief DFFの入力端子のノードを作る．
  static
  BnNodeImpl*
  new_data_in(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  );

  /// @brief DFFのクロック端子のノードを作る．
  static
  BnNodeImpl*
  new_clock(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  );

  /// @brief DFFのクリア端子のノードを作る．
  static
  BnNodeImpl*
  new_clear(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  );

  /// @brief DFFのプリセット端子のノードを作る．
  static
  BnNodeImpl*
  new_preset(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  );

  /// @brief DFFセルの入力端子のノードを作る．
  static
  BnNodeImpl*
  new_cell_input(
    const string& name, ///< [in] ノード名
    SizeType dff_id,    ///< [in] DFF番号
    SizeType iid        ///< [in] セルの入力ピン番号
  );

  /// @brief プリミティブ型のノードを作る．
  static
  BnNodeImpl*
  new_primitive(
    const string& name,                   ///< [in] 名前
    BnNodeType type,                      ///< [in] タイプ
    const vector<SizeType>& fanin_id_list ///< [in] ファンインの番号のリスト
  );

  /// @brief 論理式型のノードを作る．
  static
  BnNodeImpl*
  new_expr(
    const string& name,                   ///< [in] 名前
    SizeType expr_id,                     ///< [in] 論理式の番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインの番号のリスト
  );

  /// @brief 真理値表型のノードを作る．
  static
  BnNodeImpl*
  new_tv(
    const string& name,                   ///< [in] 名前
    SizeType func_id,                     ///< [in] 論理関数の番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインの番号のリスト
  );

  /// @brief BDD型のノードを作る．
  static
  BnNodeImpl*
  new_bdd(
    const string& name,                   ///< [in] 名前
    const Bdd& bdd,                       ///< [in] BDD
    const vector<SizeType>& fanin_id_list ///< [in] ファンインの番号のリスト
  );

  /// @brief 論理セル型のノードを作る．
  static
  BnNodeImpl*
  new_cell(
    const string& name,                   ///< [in] 名前
    SizeType cell_id,                     ///< [in] セルの番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインの番号のリスト
  );

  /// @brief 自分と同じタイプのノードを作る．
  virtual
  BnNodeImpl*
  duplicate(
    const string& name,                   ///< [in] 名前
    const vector<SizeType>& fanin_id_list ///< [in] ファンインの番号のリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId;

  // 名前
  string mName;

  // ファンアウトのノード番号のリスト
  vector<SizeType> mFanoutList;

};

END_NAMESPACE_YM_BNET

#endif // BNNODEIMPL_H
