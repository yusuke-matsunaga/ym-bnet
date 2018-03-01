#ifndef BNNODEIMPL_H
#define BNNODEIMPL_H

/// @file BnNodeImpl.h
/// @brief BnNodeImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
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
  /// @param[in] id ID 番号
  /// @param[in] name ノード名
  BnNodeImpl(int id,
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
  int
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

  /// @brief ファンアウト数を得る．
  virtual
  int
  fanout_num() const;

  /// @brief ファンアウトのノード番号を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  virtual
  int
  fanout(int pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力番号を返す．
  ///
  /// is_input() == false の時の動作は不定<br>
  /// node = BnNetwork::input(id) の時 node->input_id() = id となる．
  virtual
  int
  input_id() const;

  /// @brief 外部入力端子の時 true を返す．
  virtual
  bool
  is_port_input() const;

  /// @brief DFFの出力端子の時 true を返す．
  virtual
  bool
  is_dff_output() const;

  /// @brief DFFの反転出力端子の時 true を返す．
  virtual
  bool
  is_dff_xoutput() const;

  /// @brief ラッチの出力端子の時 true を返す．
  virtual
  bool
  is_latch_output() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号を返す．
  ///
  /// is_output() == false の時の動作は不定<br>
  /// node = BnNetwork::output(id) の時，node->output_id() = id となる．
  virtual
  int
  output_id() const;

  /// @brief 外部出力端子の時に true を返す．
  virtual
  bool
  is_port_output() const;

  /// @brie DFFの入力端子の時に true を返す．
  virtual
  bool
  is_dff_input() const;

  /// @brief DFFのクロック端子の時に true を返す．
  virtual
  bool
  is_dff_clock() const;

  /// @brief DFFのクリア端子の時に true を返す．
  virtual
  bool
  is_dff_clear() const;

  /// @brief DFFのプリセット端子の時に true を返す．
  virtual
  bool
  is_dff_preset() const;

  /// @brief ラッチの入力端子の時に true を返す．
  virtual
  bool
  is_latch_input() const;

  /// @brief ラッチのイネーブル端子の時に true を返す．
  virtual
  bool
  is_latch_enable() const;

  /// @brief ラッチのクリア端子の時に true を返す．
  virtual
  bool
  is_latch_clear() const;

  /// @brief ラッチのプリセット端子の時に true を返す．
  virtual
  bool
  is_latch_preset() const;

  /// @brief ファンインのノード番号を返す．
  ///
  /// is_output() == false の時の動作は不定
  virtual
  int
  fanin() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているポート番号を返す．
  ///
  /// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
  virtual
  int
  port_id() const;

  /// @brief 接続しているポート中のビット番号を返す．
  ///
  /// is_port_input() || is_port_output() の時のみ意味を持つ．
  virtual
  int
  port_bit() const;

  /// @brief 接続しているDFFの番号を返す．
  ///
  /// is_dff_input() || is_dff_output() || is_dff_clock() || is_dff_clear() || is_dff_preset()
  /// の時のみ意味を持つ．
  virtual
  int
  dff_id() const;

  /// @brief 接続しているラッチの番号を返す．
  ///
  /// is_latch_input() || is_latch_output() || is_latch_enable() || is_latch_clear() || is_latch_preset()
  /// の時のみ意味を持つ．
  virtual
  int
  latch_id() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  virtual
  int
  fanin_num() const;

  /// @brief ファンインのノード番号を返す．
  /// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
  virtual
  int
  fanin(int pos) const;

  /// @brief 論理式番号を返す．
  ///
  /// type() == kBnLogic_EXPR の時のみ意味を持つ．
  /// 論理式番号は同じ BnNetwork 内で唯一となるもの．
  virtual
  int
  expr_id() const;

  /// @brief 論理式を返す．
  ///
  /// type() != kBnLogic_EXPR の時の動作は不定
  ///
  /// 親のネットワークの expr(node->expr_id()) と同一
  virtual
  Expr
  expr() const;

  /// @brief 関数番号を返す．
  ///
  /// type() == kBnLogic_TV の時のみ意味を持つ．
  /// 関数番号は同じ BnNetwork 内で唯一となるもの．
  virtual
  int
  func_id() const;

  /// @brief 真理値表を返す．
  ///
  /// type() != kBnLogic_TV の時の動作は不定
  /// 親のネットワークの func(node->func_id()) と同一
  virtual
  TvFunc
  func() const;

  /// @brief セルを返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// 場合によっては nullptr の場合もある．
  virtual
  const ClibCell*
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
  set_fanin(int ipos,
	    int fanin_id);

  /// @brief ファンアウトを追加する．
  /// @param[in] onode_id ファンアウトのノード番号
  virtual
  void
  add_fanout(int onode_id);



private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  int mId;

  // 名前
  string mName;

  // ファンアウトのノード番号のリスト
  vector<int> mFanoutList;

};

END_NAMESPACE_YM_BNET

#endif // BNNODEIMPL_H
