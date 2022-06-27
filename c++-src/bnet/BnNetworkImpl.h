#ifndef BNNETWORKIMPL_H
#define BNNETWORKIMPL_H

/// @file BnNetworkImpl.h
/// @brief BnNetworkImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/ClibFFInfo.h"
#include "ym/ClibLatchInfo.h"
#include "BnNodeImpl.h"
#include "BnInputNode.h"
#include "BnOutputNode.h"


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
  ///
  /// * src_network は wrap_up() されている必要がある．
  /// * src_network のポートの情報は失われる．
  /// * 矛盾しない限りセルライブラリの情報も引く継がれる．
  void
  import_subnetwork(
    const BnNetworkImpl& src_network,   ///< [in] 追加する部分回路
    const vector<SizeType>& input_list, ///< [in] インポートした部分回路の入力に接続するノード番号のリスト
    vector<SizeType>& output_list       ///< [in] インポートした部分回路の出力ノード番号のリスト
  );

  /// @brief 各ノードがプリミティブ型になるように分解する．
  void
  simple_decomp();

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
    const string& name,       ///< [in] DFF名
    bool has_clear = false,   ///< [in] クリア端子を持つ時 true にする．
    bool has_preset = false,  ///< [in] プリセット端子を持つ時 true にする．
    bool has_xoutput = false  ///< [in] 反転出力端子を持つ時 true にする．
  )
  {
    return _new_dff(name, has_clear, has_preset, -1);
  }

  /// @brief セルの情報を持ったDFFを追加する．
  /// @return 生成したDFF番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - FFセルでない場合には -1 を返す．
  SizeType
  new_dff(
    const string& name, ///< [in] DFF名
    int cell_id         ///< [in] 対応するセル番号
  )
  {
    const ClibCell& cell = mCellLibrary.cell(cell_id);
    if ( !cell.is_ff() ) {
      return -1;
    }

    bool has_clear = cell.has_clear();
    bool has_preset = cell.has_preset();
    return _new_dff(name, has_clear, has_preset, cell_id);
  }

  /// @brief ラッチを追加する．
  /// @return 生成したラッチ番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  SizeType
  new_latch(
    const string& name,       ///< [in] ラッチ名
    bool has_clear = false,   ///< [in] クリア端子を持つ時 true にする．
    bool has_preset = false   ///< [in] プリセット端子を持つ時 true にする．
  )
  {
    return _new_latch(name, has_clear, has_preset, -1);
  }

  /// @brief セルの情報を持ったラッチを追加する．
  /// @return 生成したラッチ番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - ラッチセルでない場合には -1 を返す．
  SizeType
  new_latch(
    const string& name, ///< [in] ラッチ名
    int cell_id         ///< [in] 対応するセル番号
  )
  {
    const ClibCell& cell = mCellLibrary.cell(cell_id);
    if ( !cell.is_latch() ) {
      return -1;
    }

    bool has_clear = cell.has_clear();
    bool has_preset = cell.has_preset();
    return _new_latch(name, has_clear, has_preset, cell_id);
  }

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  SizeType
  new_primitive(
    const string& node_name, ///< [in] ノード名
    SizeType ni,             ///< [in] 入力数
    BnNodeType logic_type    ///< [in] 論理型
  )
  {
    return _reg_logic(node_name, ni, logic_type, -1, -1, -1);
  }

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  SizeType
  new_primitive(
    const string& node_name,              ///< [in] ノード名
    BnNodeType logic_type,                ///< [in] 論理型
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  )
  {
    auto ni = fanin_id_list.size();
    auto id = new_primitive(node_name, ni, logic_type);
    connect_fanins(id, fanin_id_list);
    return id;
  }

  /// @brief 論理式型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は expr.input_num() を用いる．
  SizeType
  new_expr(
    const string& node_name, ///< [in] ノード名
    const Expr& expr,        ///< [in] 論理式
    int cell_id = -1         ///< [in] セル番号
  )
  {
    SizeType ni;
    BnNodeType logic_type;
    SizeType expr_id;
    tie(ni, logic_type, expr_id) = _analyze_expr(expr);
    return _reg_logic(node_name, ni, logic_type, expr_id, -1, cell_id);
  }

  /// @brief 論理式型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は expr.input_num() を用いる．
  SizeType
  new_expr(
    const string& node_name,              ///< [in] ノード名
    const Expr& expr,                     ///< [in] 論理式
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  )
  {
    auto id = new_expr(node_name, expr);
    connect_fanins(id, fanin_id_list);
    return id;
  }

  /// @brief 真理値表型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は tv.input_num() を用いる．
  SizeType
  new_tv(
    const string& node_name, ///< [in] ノード名
    const TvFunc& tv,        ///< [in] 真理値表
    int cell_id = -1         ///< [in] セル番号
  )
  {
    auto ni = tv.input_num();
    auto func_id = _reg_tv(tv);
    return _reg_logic(node_name, ni, BnNodeType::TvFunc, -1, func_id, cell_id);
  }

  /// @brief 真理値表型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は tv.input_num() を用いる．
  SizeType
  new_tv(
    const string& node_name,              ///< [in] ノード名
    const TvFunc& tv,                     ///< [in] 真理値表
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  )
  {
    auto id = new_tv(node_name, tv);
    connect_fanins(id, fanin_id_list);
    return id;
  }

  /// @brief 論理セルを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 論理セルでない場合には BNET_NULLID を返す．
  SizeType
  new_cell(
    const string& node_name, ///< [in] ノード名
    int cell_id              ///< [in] セル番号
  )
  {
    auto& cell = mCellLibrary.cell(cell_id);
    if ( cell.type() != ClibCellType::Logic || cell.output_num() != 1 ) {
      return BNET_NULLID;
    }

    Expr expr = cell.logic_expr(0);
    return new_expr(node_name, expr, cell_id);
  }

  /// @brief 論理セルを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 論理セルでない場合には BNET_NULLID を返す．
  SizeType
  new_cell(
    const string& node_name,              ///< [in] ノード名
    int cell_id,                          ///< [in] セル番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  )
  {
    auto id = new_cell(node_name, cell_id);
    connect_fanins(id, fanin_id_list);
    return id;
  }

  /// @brief プリミティブ型の論理ノードに変更する．
  void
  change_primitive(
    SizeType id,           ///< [in] ノード番号
    BnNodeType logic_type, ///< [in] 入力数
    SizeType ni            ///< [in] 論理型
  )
  {
    _change_logic(id, ni, logic_type, -1, -1, -1);
  }

  /// @brief プリミティブ型の論理ノードに変更する．
  void
  change_primitive(
    SizeType id,                          ///< [in] ノード番号
    BnNodeType logic_type,                ///< [in] 論理型
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  )
  {
    auto ni = fanin_id_list.size();
    change_primitive(id, logic_type, ni);
    connect_fanins(id, fanin_id_list);
  }

  /// @brief 論理式型の論理ノードに変更する．
  ///
  /// 入力数は expr.input_size() を用いる．
  void
  change_expr(
    SizeType id,           ///< [in] ノード番号
    const Expr& expr, ///< [in] 論理式
    int cell_id = -1  ///< [in] セル番号
  )
  {
    SizeType ni;
    BnNodeType logic_type;
    SizeType expr_id;
    tie(ni, logic_type, expr_id) = _analyze_expr(expr);
    _change_logic(id, ni, logic_type, expr_id, -1, cell_id);
  }

  /// @brief 論理式型の論理ノードに変更する．
  ///
  /// 入力数は expr.input_size() を用いる．
  void
  change_expr(
    SizeType id,                          ///< [in] ノード番号
    const Expr& expr,                     ///< [in] 論理式
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  )
  {
    ASSERT_COND( expr.input_size() == fanin_id_list.size() );

    change_expr(id, expr);
    connect_fanins(id, fanin_id_list);
  }

  /// @brief 真理値表型の論理ノードに変更する．
  ///
  /// - 入力数は tv.input_num() を用いる．
  void
  change_tv(
    SizeType id,      ///< [in] ノード番号
    const TvFunc& tv, ///< [in] 真理値表
    int cell_id = -1  ///< [in] セル番号
  )
  {
    auto ni = tv.input_num();
    auto func_id = _reg_tv(tv);
    _change_logic(id, ni, BnNodeType::TvFunc, -1, func_id, cell_id);
  }

  /// @brief 真理値表型の論理ノードに変更する．
  ///
  /// - 入力数は tv.input_num() を用いる．
  void
  change_tv(
    SizeType id,                          ///< [in] ノード番号
    const TvFunc& tv,                     ///< [in] 真理値表
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  )
  {
    ASSERT_COND( tv.input_num() == fanin_id_list.size() );

    change_tv(id, tv);
    connect_fanins(id, fanin_id_list);
  }

  /// @brief セル型の論理ノードに変更する．
  ///
  /// - 入力数はセルから取得する．
  /// - 論理セルでない場合にはなにもしない．
  void
  change_cell(
    SizeType id,     ///< [in] ノード番号
    SizeType cell_id ///< [in] セル番号
  )
  {
    auto& cell = mCellLibrary.cell(cell_id);
    if ( !cell.is_logic() || cell.output_num() != 1 ) {
      return;
    }

    Expr expr = cell.logic_expr(0);
    change_expr(id, expr, cell_id);
  }

  /// @brief セル型の論理ノードに変更する．
  ///
  /// - 入力数はセルから取得する．
  /// - 論理セルでない場合にはなにもしない．
  void
  change_cell(
    SizeType id,                          ///< [in] ノード番号
    int cell_id,                          ///< [in] セル番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  )
  {
    change_cell(id, cell_id);
    connect_fanins(id, fanin_id_list);
  }

  /// @brief ノードを複製する．
  SizeType
  dup_logic(
    const string& node_name, ///< [in] ノード名
    SizeType src_id          ///< [in] もとのノード番号
  );

  /// @brief ノードを複製する．
  SizeType
  dup_logic(
    const string& node_name,              ///< [in] ノード名
    SizeType src_id,                      ///< [in] もとのノード番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  )
  {
    auto id = dup_logic(node_name, src_id);
    connect_fanins(id, fanin_id_list);
    return id;
  }

  /// @brief ファンアウトをつなぎ替える．
  void
  substitute_fanout(
    SizeType old_id, ///< [in] もとのノード番号
    SizeType new_id  ///< [in] つなぎ替える新しいノード番号
  );

  /// @brief ファンインをつなぎ替える．
  void
  connect_fanins(
    SizeType id,                          ///< [in] ノード番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief ノード間を接続する．
  void
  connect(
    SizeType src_node, ///< [in] ファンアウト元のノード番号
    SizeType dst_node, ///< [in] ファンイン先のノード番号
    SizeType ipos      ///< [in] ファンインの位置 ( 0 <= ipos < dst_node->fanin_num() )
  );

  /// @brief 整合性のチェックを行う．
  /// @return チェック結果を返す．
  ///
  /// チェック項目は以下の通り
  /// - name() が設定されているか？
  ///   設定されていない場合にはデフォルト値を設定する．
  ///   エラーとはならない．
  /// - 各ポートの各ビットのノード番号が適切か？
  /// - 各DFFの入力，出力およびクロックが設定されているか？
  /// - 各ラッチの入力，出力およびイネーブルが設定されているか？
  /// - 各ノードのファンインが設定されているか？
  ///
  /// この関数を呼んだあとは論理ノードがトポロジカルソートされる．
  /// というかこの関数を呼ばないと logic_num(), logic() は正しくない．
  bool
  wrap_up();

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 内容を取得する関数
  /// @{
  //////////////////////////////////////////////////////////////////////

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

  /// @brief ラッチ数を得る．
  SizeType
  latch_num() const { return mLatchList.size(); }

  /// @brief ラッチを得る．
  const BnLatch&
  latch(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < latch_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < latch_num() );
    return *mLatchList[pos];
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
    ASSERT_COND( id >= 0 && id < node_num() );

    return *mNodeList[id];
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

  /// @brief 外部入力ノードのノード番号のリストを得る．
  const vector<SizeType>&
  primary_input_id_list() const { return mPrimaryInputList; }

  /// @brief 出力数を得る．
  SizeType
  output_num() const { return mOutputList.size(); }

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

  /// @brief 出力ノードのソースノード番号を得る．
  ///
  /// ソースノードとは出力ノードのファンインのノード
  SizeType
  output_src_id(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < output_num() );

    return mOutputSrcList[pos];
  }

  /// @brief 出力ノードのソースノード番号のリストを得る．
  ///
  /// ソースノードとは出力ノードのファンインのノード
  const vector<SizeType>&
  output_src_id_list() const
  {
    return mOutputSrcList;
  }

  /// @brief 外部出力ノードのノード番号のリストを得る．
  const vector<SizeType>&
  primary_output_id_list() const
  {
    return mPrimaryOutputList;
  }

  /// @brief 外部出力ノードのソースノード番号のリストを得る．
  const vector<SizeType>&
  primary_output_src_id_list() const
  {
    return mPrimaryOutputSrcList;
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

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief DFFを複製する．
  /// @return 生成した DFF 番号を返す．
  SizeType
  _copy_dff(
    const BnDff& src_dff,    ///< [in] 元のDFF
    vector<SizeType>& id_map ///< [out] 生成したノードの対応関係を記録するハッシュ表
  );

  /// @brief ラッチを複製する．
  /// @return 生成したラッチ番号を返す．
  SizeType
  _copy_latch(
    const BnLatch& src_latch, ///< [in] 元のラッチ
    vector<SizeType>& id_map  ///< [out] 生成したノードの対応関係を記録するハッシュ表
  );

  /// @brief ノードを登録する．
  void
  _reg_node(
    BnNodeImpl* node
  )
  {
    node->mId = mNodeList.size();
    mNodeList.push_back(node);
  }

  /// @brief 入力ノードを登録する．
  void
  _reg_input(
    BnInputNode* node
  )
  {
    _reg_node(node);
    node->mInputPos = mInputList.size();
    mInputList.push_back(node->id());
  }

  /// @brief 外部入力ノードを登録する．
  void
  _reg_primary_input(
    BnInputNode* node
  )
  {
    _reg_input(node);
    mPrimaryInputList.push_back(node->id());
  }

  /// @brief 出力ノードを登録する．
  void
  _reg_output(
    BnOutputNode* node
  )
  {
    _reg_node(node);
    node->mOutputPos = mOutputList.size();
    mOutputList.push_back(node->id());
  }

  /// @brief 外部出力ノードを登録する．
  void
  _reg_primary_output(
    BnOutputNode* node
  )
  {
    _reg_output(node);
    mPrimaryOutputList.push_back(node->id());
  }

  /// @brief 論理ノードを複製する．
  /// @return 生成したノード番号を返す．
  ///
  /// id_map の内容の基づいてファンイン間の接続を行う．
  SizeType
  _copy_logic(
    const BnNode& src_node,           ///< [in] 元のノード
    const BnNetworkImpl& src_network, ///< [in] 元のネットワーク
    vector<SizeType>& id_map          ///< [out] 生成したノードの対応関係を記録するハッシュ表
  );

  /// @brief 既存の論理ノードを変更する最も低レベルの関数
  ///
  /// expr_id, func_id, cell_id は場合によっては無効な値が入る．
  void
  _change_logic(
    SizeType node_id,      ///< [in] ノード番号
    SizeType fanin_num,    ///< [in] ファンイン数
    BnNodeType logic_type, ///< [in] 論理ノードの型
    SizeType expr_id,      ///< [in] 論理式番号
    SizeType func_id,      ///< [in] 論理関数番号
    int cell_id            ///< [in] セル番号
  );

  /// @brief 新しい論理ノードを作って登録する．
  /// @return 生成したノード番号を返す．
  ///
  /// expr_id, func_id, cell_id は場合によっては無効な値が入る．
  SizeType
  _reg_logic(
    const string& name,    ///< [in] ノード名
    SizeType fanin_num,    ///< [in] ファンイン数
    BnNodeType logic_type, ///< [in] 論理ノードの型
    SizeType expr_id,      ///< [in] 論理式番号
    SizeType func_id,      ///< [in] 論理関数番号
    int cell_id            ///< [in] セル番号
  );

  /// @brief 論理ノードを作る最も低レベルの関数
  /// @return 生成したノード番号を返す．
  ///
  /// expr_id, func_id, cell_id は場合によっては無効な値が入る．
  BnNodeImpl*
  _new_logic(
    const string& name,    ///< [in] ノード名
    SizeType fanin_num,    ///< [in] ファンイン数
    BnNodeType logic_type, ///< [in] 論理ノードの型
    SizeType expr_id,      ///< [in] 論理式番号
    SizeType func_id,      ///< [in] 論理関数番号
    int cell_id            ///< [in] セル番号
  );

  /// @brief 論理式型のノードを分解する．
  /// @return ノード番号を返す．
  ///
  /// * id が BNET_NULLID 以外ならそのノードを置き換える．
  /// * そうでなければ新規のノードを作る．
  /// * term_list は入力数の2倍のサイズ(i * 2 + 0 で肯定のリテラルを表す)
  SizeType
  _decomp_expr(
    SizeType id,                ///< [in] ノード番号
    const Expr& expr,           ///< [in] 論理式
    vector<SizeType>& term_list ///< [in] リテラルに対するノード番号のリスト
  );

  /// @brief 真理値表型のノードを分解する．
  /// @return ノード番号を返す．
  ///
  /// * id が BNET_NULLID 以外ならそのノードを置き換える．
  /// * そうでなければ新規のノードを作る．
  SizeType
  _decomp_tvfunc(
    SizeType id,                          ///< [in] ノード番号
    const TvFunc& func,                   ///< [in] 関数
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief DFFを追加する共通の処理を行う関数
  /// @return 生成したDFF番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  SizeType
  _new_dff(
    const string& name, ///< [in] DFF名
    bool has_clear,     ///< [in] クリア端子を持つ時 true にする．
    bool has_preset,    ///< [in] プリセット端子を持つ時 true にする．
    int cell_id         ///< [in] 対応するセル番号．
  );

  /// @brief ラッチを追加する共通の処理を行う関数
  /// @return 生成したラッチ番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - cell はラッチのセルでなければならない．
  SizeType
  _new_latch(
    const string& name, ///< [in] ラッチ名
    bool has_clear,     ///< [in] クリア端子を持つ時 true にする．
    bool has_preset,    ///< [in] プリセット端子を持つ時 true にする．
    int cell_id         ///< [in] 対応するセル番号．
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

  // 関連するセルライブラリ
  ClibCellLibrary mCellLibrary;

  // ポートのリスト
  vector<const BnPort*> mPortList;

  // DFFのリスト
  vector<const BnDff*> mDffList;

  // ラッチのリスト
  vector<const BnLatch*> mLatchList;

  // ノード番号をキーにしてノードを納めた配列
  vector<BnNodeImpl*> mNodeList;

  // 入力ノード番号のリスト
  vector<SizeType> mInputList;

  // 外部入力ノード番号のリスト
  vector<SizeType> mPrimaryInputList;

  // 出力ノード番号のリスト
  vector<SizeType> mOutputList;

  // 出力ソースノード番号のリスト
  vector<SizeType> mOutputSrcList;

  // 外部出力ノード番号のリスト
  vector<SizeType> mPrimaryOutputList;

  // 外部出力ソースノード番号のリスト
  vector<SizeType> mPrimaryOutputSrcList;

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
