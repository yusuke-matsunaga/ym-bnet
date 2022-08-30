#ifndef BNNETWORKIMPL_H
#define BNNETWORKIMPL_H

/// @file BnNetworkImpl.h
/// @brief BnNetworkImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/BddMgr.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "BnNodeImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnNetworkImpl BnNetworkImpl.h "BnNetworkImpl.h"
/// @brief BnNetwork の実装クラス
//////////////////////////////////////////////////////////////////////
class BnNetworkImpl
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief 空のコンストラクタ
  ///
  /// 空の状態で初期化される．
  BnNetworkImpl() = default;

  /// @brief デストラクタ
  ~BnNetworkImpl()
  {
    clear();
  }


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 内容を設定する関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  ///
  /// コンストラクタ直後と同じ状態になる．
  void
  clear();

  /// @brief 内容をコピーする．
  void
  copy(
    const BnNetworkImpl& src ///< [in] コピー元のオブジェクト
  );

  /// @brief ポートの情報のみをコピーする．
  void
  make_skelton_copy(
    const BnNetworkImpl& src,                 ///< [in] コピー元のオブジェクト
    unordered_map<SizeType, SizeType>& id_map ///< [in] ノード番号の対応表
  );

  /// @brief セルライブラリをセットする．
  void
  set_library(
    const ClibCellLibrary& library ///< [in] ライブラリ
  );

  /// @brief ネットワーク名を設定する．
  void
  set_name(
    const string& name ///< [in] ネットワーク名
  );

  /// @brief 部分回路を追加する．
  /// @return src_network の出力に対応するノード番号のリストを返す．
  ///
  /// * src_network は wrap_up() されている必要がある．
  /// * src_network のポートの情報は失われる．
  /// * 矛盾しない限りセルライブラリの情報も引く継がれる．
  vector<SizeType>
  import_subnetwork(
    const BnNetworkImpl& src_network,  ///< [in] 追加する部分回路
    const vector<SizeType>& input_list ///< [in] インポートした部分回路の入力に接続するノード番号のリスト
  );

  /// @brief ポートの情報をコピーする．
  /// @return 生成したポートのポート番号を返す．
  SizeType
  copy_port(
    const BnPort& src_port,                   ///< [in] コピー元のオブジェクト
    const BnNetworkImpl& src_network,         ///< [in] 元のネットワーク
    unordered_map<SizeType, SizeType>& id_map ///< [in] ノード番号の対応関係を表すハッシュ表
  );

  /// @brief DFFの情報をコピーする．
  /// @return 生成したDFFのDFF番号を返す．
  SizeType
  copy_dff(
    const BnDff& src_dff,                     ///< [in] コピー元のオブジェクト
    unordered_map<SizeType, SizeType>& id_map ///< [in] ノード番号の対応関係を表すハッシュ表
  );

  /// @brief 論理ノードを複製する．
  /// @return 生成したノード番号を返す．
  ///
  /// id_map の内容の基づいてファンイン間の接続を行う．
  SizeType
  copy_logic(
    const BnNode& src_node,                   ///< [in] 元のノード
    const BnNetworkImpl& src_network,         ///< [in] 元のネットワーク
    unordered_map<SizeType, SizeType>& id_map ///< [in] ノード番号の対応関係を表すハッシュ表
  );

  /// @brief 出力ノードを複製する．
  ///
  /// 出力ノードそのものはすでに生成済みでファンインの情報の
  /// 設定のみを行う．
  void
  copy_output(
    const BnNode& src_node,                   ///< [in] 元のノード
    unordered_map<SizeType, SizeType>& id_map ///< [in] ノード番号の対応関係を表すハッシュ表
  );

  /// @brief 入出力混合のポートを作る．
  /// @return 生成したポート番号を返す．
  SizeType
  new_port(
    const string& port_name,      ///< [in] ポート名
    const vector<BnDir>& dir_vect ///< [in] 向きを表すベクタ
  );

  /// @brief DFFを追加する．
  /// @return 生成したDFF番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  SizeType
  new_dff(
    const string& name,      ///< [in] DFF名
    bool has_clear = false,  ///< [in] クリア端子を持つ時 true にする．
    bool has_preset = false, ///< [in] プリセット端子を持つ時 true にする．
    BnCPV cpv = BnCPV::L     ///< [in] クリアとプリセットが衝突したときの挙動
  );

  /// @brief ラッチを追加する．
  /// @return 生成したDFF番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  SizeType
  new_latch(
    const string& name,      ///< [in] ラッチ名
    bool has_clear = false,  ///< [in] クリア端子を持つ時 true にする．
    bool has_preset = false, ///< [in] プリセット端子を持つ時 true にする．
    BnCPV cpv = BnCPV::L     ///< [in] クリアとプリセットが衝突したときの挙動
  );

  /// @brief セルの情報を持ったDFFを追加する．
  /// @return 生成したDFF番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - FFセルでない場合にはエラーとなる．
  SizeType
  new_dff_cell(
    const string& name, ///< [in] DFF名
    SizeType cell_id    ///< [in] 対応するセル番号
  );

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  SizeType
  new_logic_primitive(
    const string& node_name,              ///< [in] ノード名
    BnNodeType logic_type,                ///< [in] 論理型
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 論理式型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は expr.input_num() を用いる．
  /// - expr 中の変数に抜けがある場合には詰められる．
  SizeType
  new_logic_expr(
    const string& node_name,              ///< [in] ノード名
    const Expr& expr,                     ///< [in] 論理式
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 真理値表型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は tv.input_num() を用いる．
  SizeType
  new_logic_tv(
    const string& node_name,              ///< [in] ノード名
    const TvFunc& tv,                     ///< [in] 真理値表
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief BDD型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_logic_bdd(
    const string& node_name,              ///< [in] ノード名
    const Bdd& bdd,                       ///< [in] BDD
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 論理セルを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 論理セルでない場合には BNET_NULLID を返す．
  SizeType
  new_logic_cell(
    const string& node_name,              ///< [in] ノード名
    SizeType cell_id,                     ///< [in] セル番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief プリミティブ型の論理ノードに変更する．
  void
  change_primitive(
    SizeType id,                          ///< [in] ノード番号
    BnNodeType logic_type,                ///< [in] 論理型
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 論理式型の論理ノードに変更する．
  ///
  /// 入力数は expr.input_size() を用いる．
  void
  change_expr(
    SizeType id,                          ///< [in] ノード番号
    const Expr& expr,                     ///< [in] 論理式
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 真理値表型の論理ノードに変更する．
  ///
  /// - 入力数は tv.input_num() を用いる．
  void
  change_tv(
    SizeType id,                          ///< [in] ノード番号
    const TvFunc& tv,                     ///< [in] 真理値表
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief BDD型の論理ノードに変更する．
  void
  change_bdd(
    SizeType id,                          ///< [in] ノード番号
    const Bdd& bdd,                       ///< [in] 真理値表
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief セル型の論理ノードに変更する．
  void
  change_cell(
    SizeType id,                          ///< [in] ノード番号
    int cell_id,                          ///< [in] セル番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief ノードを複製する．
  SizeType
  dup_logic(
    const string& node_name,              ///< [in] ノード名
    SizeType src_id,                      ///< [in] もとのノード番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief ファンアウトをつなぎ替える．
  void
  substitute_fanout(
    SizeType old_id, ///< [in] もとのノード番号
    SizeType new_id  ///< [in] つなぎ替える新しいノード番号
  );

  /// @brief 出力ノードのファンインを設定する．
  void
  set_output_src(
    SizeType output_id, ///< [in] 出力ノードのID番号
    SizeType src_id     ///< [in] ファンインノードのID番号
  );

  /// @brief 整合性のチェックを行う．
  ///
  /// チェック項目は以下の通り
  /// - 各ポートの各ビットのノード番号が適切か？
  /// - 各DFFの入力，出力およびクロックが設定されているか？
  /// - 各ラッチの入力，出力およびイネーブルが設定されているか？
  /// - 各ノードのファンインが設定されているか？
  void
  wrap_up();

  /// @brief BDDの情報を復元する．
  ///
  /// BnNetwork::restore() で用いられる．
  vector<Bdd>
  restore_bdds(
    BinDec& s ///< [in] 入力ストリーム
  )
  {
    return mBddMgr.restore(s);
  }

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 内容を取得する関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 実装可能な構造を持っている時 true を返す．
  bool
  is_concrete() const;

  /// @brief ネットワーク名を得る．
  string
  name() const { return mName; }

  /// @brief 関連するセルライブラリを得る．
  ///
  /// 場合によっては空の場合もある．
  const ClibCellLibrary&
  library() const { return mCellLibrary; }

  /// @brief ポート数を得る．
  SizeType
  port_num() const { return mPortList.size(); }

  /// @brief ポートの情報を得る．
  const BnPort&
  port(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < port_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < port_num() );
    return *mPortList[pos];
  }

  /// @brief DFF数を得る．
  SizeType
  dff_num() const { return mDffList.size(); }

  /// @brief DFFを得る．
  const BnDff&
  dff(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < dff_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < dff_num() );
    return *mDffList[pos];
  }

  /// @brief ノード数を得る．
  SizeType
  node_num() const { return mNodeList.size(); }

  /// @brief ノードを得る．
  ///
  /// BnNode* node = BnNetworkImpl::node(id);
  /// node->id() == id が成り立つ．
  const BnNode&
  node(
    SizeType id ///< [in] ノード番号 ( 0 <= id < node_num() )
  ) const
  {
    return *_node_p(id);
  }

  /// @brief 入力数を得る．
  SizeType
  input_num() const
  {
    return mInputList.size();
  }

  /// @brief 入力ノードのノード番号を得る．
  SizeType
  input_id(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < input_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < input_num() );
    return mInputList[pos];
  }

  /// @brief 入力ノードのノード番号のリストを得る．
  const vector<SizeType>&
  input_id_list() const { return mInputList; }

  /// @brief 外部入力数を得る．
  SizeType
  primary_input_num() const
  {
    return mPrimaryInputList.size();
  }

  /// @brief 外部入力ノードのノード番号を得る．
  SizeType
  primary_input_id(
    SizeType pos ///< [in] 外部入力番号 ( 0 <= pos < primary_input_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < primary_input_num() );
    return mPrimaryInputList[pos];
  }

  /// @brief 外部入力ノードのノード番号のリストを得る．
  const vector<SizeType>&
  primary_input_id_list() const
  {
    return mPrimaryInputList;
  }

  /// @brief 出力数を得る．
  SizeType
  output_num() const
  {
    return mOutputList.size();
  }

  /// @brief 出力ノードのノード番号を得る．
  SizeType
  output_id(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < output_num() );

    return mOutputList[pos];
  }

  /// @brief 出力ノードのノード番号のリストを得る．
  const vector<SizeType>&
  output_id_list() const
  {
    return mOutputList;
  }

  /// @brief 外部出力数を得る．
  SizeType
  primary_output_num() const
  {
    return mPrimaryOutputList.size();
  }

  /// @brief 外部出力ノードのノード番号を得る．
  SizeType
  primary_output_id(
    SizeType pos ///< [in] 外部出力番号 ( 0 <= pos < primary_output_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < primary_output_num() );
    return mPrimaryOutputList[pos];
  }

  /// @brief 外部出力ノードのノード番号のリストを得る．
  const vector<SizeType>&
  primary_output_id_list() const
  {
    return mPrimaryOutputList;
  }

  /// @brief 論理ノード数を得る．
  SizeType
  logic_num() const
  {
    return mLogicList.size();
  }

  /// @brief 論理ノードのノード番号を得る．
  SizeType
  logic_id(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < logic_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < logic_num() );

    return mLogicList[pos];
  }

  /// @brief 論理ノードのノード番号のリストを得る．
  const vector<SizeType>&
  logic_id_list() const
  {
    return mLogicList;
  }

  /// @brief セル割当情報を持つ時 true を返す．
  bool
  is_mapped() const;

  /// @brief 関数の数を得る．
  SizeType
  func_num() const
  {
    return mFuncList.size();
  }

  /// @brief 関数番号から関数を得る．
  const TvFunc&
  func(
    SizeType func_id ///< [in] 関数番号 ( 0 <= func_id < func_num() )
  ) const
  {
    ASSERT_COND( func_id >= 0 && func_id < func_num() );
    return mFuncList[func_id];
  }

  /// @brief 論理式の数を得る．
  SizeType
  expr_num() const
  {
    return mExprList.size();
  }

  /// @brief 論理式番号から論理式を得る．
  Expr
  expr(
    SizeType expr_id ///< [in] 論理式番号 ( 0 <= expr_id < expr_num() )
  ) const
  {
    ASSERT_COND( expr_id >= 0 && expr_id < expr_num() );
    return mExprList[expr_id];
  }

  /// @brief 内容を出力する．
  ///
  /// - 形式は独自フォーマット
  void
  write(
    ostream& s ///< [in] 出力先のストリーム
  ) const;

  /// @brief 内容をバイナリダンプする．
  void
  dump(
    BinEnc& s ///< [in] 出力ストリーム
  ) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief データ入力ノードを作る．
  /// @return 生成したノード番号を返す．
  SizeType
  _new_data_in(
    const string& dff_name, ///< [in] DFF名
    SizeType dff_id         ///< [in] DFF番号
  );

  /// @brief データ出力ノードを作る．
  /// @return 生成したノード番号を返す．
  SizeType
  _new_data_out(
    const string& dff_name, ///< [in] DFF名
    SizeType dff_id         ///< [in] DFF番号
  );

  /// @brief クロック端子ノードを作る．
  /// @return 生成したノード番号を返す．
  SizeType
  _new_clock(
    const string& dff_name, ///< [in] DFF名
    SizeType dff_id         ///< [in] DFF番号
  );

  /// @brief クリア端子ノードを作る．
  /// @return 生成したノード番号を返す．
  ///
  /// has_clear が false の時は BNET_NULLID を返す．
  SizeType
  _new_clear(
    const string& dff_name, ///< [in] DFF名
    SizeType dff_id,        ///< [in] DFF番号
    bool has_clear          ///< [in] クリア端子を持つ時 true
  );

  /// @brief プリセット端子ノードを作る．
  /// @return 生成したノード番号を返す．
  ///
  /// has_clear が false の時は BNET_NULLID を返す．
  SizeType
  _new_preset(
    const string& dff_name, ///< [in] DFF名
    SizeType dff_id,        ///< [in] DFF番号
    bool has_clear          ///< [in] クリア端子を持つ時 true
  );

  /// @brief DFFセルの入力端子を作る．
  /// @return 生成したノード番号を返す．
  SizeType
  _new_cell_input(
    const string& dff_name, ///< [in] DFF名
    SizeType dff_id,        ///< [in] DFF番号
    SizeType pos            ///< [in] ピン番号
  );

  /// @brief DFFセルの出力端子を作る．
  /// @return 生成したノード番号を返す．
  SizeType
  _new_cell_output(
    const string& dff_name, ///< [in] DFF名
    SizeType dff_id,        ///< [in] DFF番号
    SizeType pos            ///< [in] ピン番号
  );

  /// @brief プリミティブ型の論理ノードを作る．
  BnNodeImpl*
  _new_logic_primitive(
    const string& node_name,              ///< [in] ノード名
    BnNodeType type,                      ///< [in] ノードの型(プリミティブ型のみ)
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 論理式型の論理ノードを作る．
  BnNodeImpl*
  _new_logic_expr(
    const string& node_name,              ///< [in] ノード名
    const Expr& expr,			  ///< [in] 論理式
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 真理値表型の論理ノードを作る．
  BnNodeImpl*
  _new_logic_tv(
    const string& node_name,              ///< [in] ノード名
    const TvFunc& tv,                     ///< [in] 真理値表
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief BDD型の論理ノードを作る．
  BnNodeImpl*
  _new_logic_bdd(
    const string& node_name,              ///< [in] ノード名
    const Bdd& bdd,                       ///< [in] BDD
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief セル型の論理ノードを作る．
  BnNodeImpl*
  _new_logic_cell(
    const string& node_name,              ///< [in] ノード名
    SizeType cell_id,                     ///< [in] セル番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief ノード番号が正しいかチェックする．
  bool
  _check_node_id(
    SizeType id
  ) const
  {
    return ( 0 < id && id <= mNodeList.size() );
  }

  /// @brief ノードを取り出す．
  BnNodeImpl*
  _node_p(
    SizeType id
  ) const
  {
    ASSERT_COND( _check_node_id(id) );
    return mNodeList[id - 1];
  }

  /// @brief 入力ノードを登録する．
  SizeType
  _reg_input(
    BnNodeImpl* node
  );

  /// @brief 外部入力ノードを登録する．
  SizeType
  _reg_primary_input(
    BnNodeImpl* node
  );

  /// @brief 出力ノードを登録する．
  SizeType
  _reg_output(
    BnNodeImpl* node
  );

  /// @brief 外部出力ノードを登録する．
  SizeType
  _reg_primary_output(
    BnNodeImpl* node
  );

  /// @brief 論理ノードを登録する．
  SizeType
  _reg_logic(
    BnNodeImpl* node
  );

  /// @brief ノードを登録する．
  /// @return 登録したノードの番号を返す．
  SizeType
  _reg_node(
    BnNodeImpl* node
  );

  /// @brief ノードを取り替える．
  void
  _chg_node(
    BnNodeImpl* old_node,
    BnNodeImpl* new_node
  );

  /// @brief 論理式を解析する．
  /// @return 入力数，ノードタイプ, 論理式番号のタプルを返す．
  ///
  /// 場合によってはプリミティブ型となる．
  tuple<SizeType, BnNodeType, SizeType>
  _analyze_expr(
    const Expr& expr ///< [in] 論理式
  );

  /// @brief 論理式を登録する．
  /// @return 論理式番号を返す．
  SizeType
  _reg_expr(
    const Expr& expr ///< [in] 論理式
  );

  /// @brief 真理値表を登録する．
  /// @return 関数番号を返す．
  SizeType
  _reg_tv(
    const TvFunc& tv ///< [in] 真理値表
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク名
  string mName;

  // Bddマネージャ
  BddMgr mBddMgr;

  // 関連するセルライブラリ
  ClibCellLibrary mCellLibrary;

  // ポートのリスト
  vector<const BnPort*> mPortList;

  // 名前をキーにしてポートを持つ辞書
  unordered_map<string, BnPort*> mPortDict;

  // DFFのリスト
  vector<const BnDff*> mDffList;

  // ノード番号をキーにしてノードを納めた配列
  vector<BnNodeImpl*> mNodeList;

  // 入力ノード番号のリスト
  vector<SizeType> mInputList;

  // 外部入力ノード番号のリスト
  vector<SizeType> mPrimaryInputList;

  // 出力ノード番号のリスト
  vector<SizeType> mOutputList;

  // 外部出力ノード番号のリスト
  vector<SizeType> mPrimaryOutputList;

  // 論理ノード番号のリスト
  vector<SizeType> mLogicList;

  // 関数のリスト
  vector<TvFunc> mFuncList;

  // TvFunc をキーにして関数番号を入れるハッシュ表
  unordered_map<TvFunc, SizeType> mFuncMap;

  // 論理式のリスト
  vector<Expr> mExprList;

  // TvFunc をキーにして論理式番号を入れるハッシュ表
  unordered_map<TvFunc, SizeType> mExprMap;

  // wrap_up() が実行後の時に true となるフラグ
  bool mSane{false};

};

END_NAMESPACE_YM_BNET

#endif // BNNETWORKIMPL_H
