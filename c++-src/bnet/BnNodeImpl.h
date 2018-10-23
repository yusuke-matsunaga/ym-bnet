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
  ~BnNodeImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードID を返す．
  int
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

  /// @brief ファンアウト数を得る．
  int
  fanout_num() const override;

  /// @brief ファンアウトのノード番号を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  int
  fanout_id(int pos) const override;

  /// @brief ファンアウトのノード番号のリストを返す．
  const vector<int>&
  fanout_id_list() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力番号を返す．
  ///
  /// is_input() == false の時の動作は不定<br>
  /// node_id = BnNetwork::input_id(pos) の時 node->input_pos() = pos となる．
  int
  input_pos() const override;

  /// @brief 外部入力端子の時 true を返す．
  bool
  is_port_input() const override;

  /// @brief DFFの出力端子の時 true を返す．
  bool
  is_dff_output() const override;

  /// @brief DFFの反転出力端子の時 true を返す．
  bool
  is_dff_xoutput() const override;

  /// @brief ラッチの出力端子の時 true を返す．
  bool
  is_latch_output() const override;

  /// @brief ラッチの出力端子の時 true を返す．
  bool
  is_latch_xoutput() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号を返す．
  ///
  /// is_output() == false の時の動作は不定<br>
  /// node_id = BnNetwork::output_id(pos) の時，node->output_pos() = pos となる．
  int
  output_pos() const override;

  /// @brief 外部出力端子の時に true を返す．
  bool
  is_port_output() const override;

  /// @brie DFFの入力端子の時に true を返す．
  bool
  is_dff_input() const override;

  /// @brief DFFのクロック端子の時に true を返す．
  bool
  is_dff_clock() const override;

  /// @brief DFFのクリア端子の時に true を返す．
  bool
  is_dff_clear() const override;

  /// @brief DFFのプリセット端子の時に true を返す．
  bool
  is_dff_preset() const override;

  /// @brief ラッチの入力端子の時に true を返す．
  bool
  is_latch_input() const override;

  /// @brief ラッチのイネーブル端子の時に true を返す．
  bool
  is_latch_enable() const override;

  /// @brief ラッチのクリア端子の時に true を返す．
  bool
  is_latch_clear() const override;

  /// @brief ラッチのプリセット端子の時に true を返す．
  bool
  is_latch_preset() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているポート番号を返す．
  ///
  /// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
  int
  port_id() const override;

  /// @brief 接続しているポート中のビット番号を返す．
  ///
  /// is_port_input() || is_port_output() の時のみ意味を持つ．
  int
  port_bit() const override;

  /// @brief 接続しているDFFの番号を返す．
  ///
  /// is_dff_input() || is_dff_output() || is_dff_xoutput() ||
  /// is_dff_clock() || is_dff_clear() || is_dff_preset()
  /// の時のみ意味を持つ．
  int
  dff_id() const override;

  /// @brief 接続しているラッチの番号を返す．
  ///
  /// is_latch_input() || is_latch_output() || is_latch_xoutput() ||
  /// is_latch_enable() || is_latch_clear() || is_latch_preset()
  /// の時のみ意味を持つ．
  int
  latch_id() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  int
  fanin_num() const override;

  /// @brief ファンインのノード番号を返す．
  /// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
  int
  fanin_id(int pos) const override;

#if 0
  /// @brief ファンインのノード番号のリストを返す．
  Array<int>
  fanin_id_list() const override;
#else
  const vector<int>&
  fanin_id_list() const override;
#endif

  /// @brief 論理式番号を返す．
  ///
  /// type() == BnNodeType::Expr の時のみ意味を持つ．
  /// 論理式番号は同じ BnNetwork 内で唯一となるもの．
  int
  expr_id() const override;

  /// @brief 関数番号を返す．
  ///
  /// type() == BnNodeType::TvFunc の時のみ意味を持つ．
  /// 関数番号は同じ BnNetwork 内で唯一となるもの．
  int
  func_id() const override;

  /// @brief セルを返す．
  ///
  /// is_logic() == false の時の動作は不定
  /// 場合によっては nullptr の場合もある．
  const ClibCell*
  cell() const override;


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

  /// @brief ファンアウトリストをクリアする．
  void
  clear_fanout();

  /// @brief ファンアウトを追加する．
  /// @param[in] onode_id ファンアウトのノード番号
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
