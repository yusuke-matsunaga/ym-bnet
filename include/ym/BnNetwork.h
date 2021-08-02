#ifndef BNNETWORK_H
#define BNNETWORK_H

/// @file BnNetwork.h
/// @brief BnNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/clib.h"


BEGIN_NAMESPACE_YM_BNET

class BnNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class BnNetwork BnNetwork.h "ym/BnNetwork.h"
/// @brief ブーリアンネットワークを表すクラス
/// @sa BnPort
/// @sa BnDff
/// @sa BnLatch
/// @sa BnNode
///
/// ただし，このネットワークはDFFノードを含むので正確には
/// syncronous boolean network かもしれない．
///
/// 組み合わせ回路部分は BnNode のネットワークで表す．
/// 全体の構造を表すためにそれ以外のデータ構造として以下の要素を持つ．
/// - ポート(BnPort)
///   名前とビット幅，各ビットに対応するノード番号を持つ．
/// - D-FFノード(BnDff)
///   入力と出力およびクロックのノード番号を持つ．
///   反転出力を持つ場合もある．
///   オプションとしてセット，リセット端子のノード番号を持つ．
/// - ラッチノード(BnLatch)
///   入力と出力およびイネーブル端子のノード番号を持つ．
///   オプションとしてセット，リセット端子のノード番号を持つ．
///
/// BnNetwork の構成要素は名前を持つ．
/// ただし，このクラスでは名前の重複チェックは行わない．
/// 名前の一貫性を管理するのは名前をつける側の責任となる．
/// 内容をファイル出力する際には重複した名前は自動生成名で置き換えられる．
///
/// ポートの名前空間はノードとは別に設ける．
/// このような細かな指定が可能なのは Verilog-HDL のような本格的な HDL のみ．
/// 通常の blif ファイルや .bench(iscas89) ファイルを読んだ場合，
/// ポートは1つのノードに対応する．
/// この場合，ポート名は外部入力ノード，外部出力ノードの名前と同じになる．
/// ただし，blifやiscas89で記述可能な外部入力と外部出力に同名の信号がある場合には
/// 同名の外部入力ポートと外部出力ポートが出来てしまうため，どちらかが自動生成名
/// に置き換えられることになる．
///
/// 回路構造を作るには new_XXXX() でノードを作り，connect() で接続を作る．
/// 最後に wrap_up() で最終処理を行う．
///
/// ノード番号は変化しない．また，ノードを削除することもできない．
/// ただし，内容をファイルに書き出す際に出力ノードへ到達しないノードは削除される
/// ので，そのファイルを読み込んだネットワークではノード番号が異なる可能性がある．
//////////////////////////////////////////////////////////////////////
class BnNetwork
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief 空のコンストラクタ
  ///
  /// 空の状態で初期化される．
  BnNetwork();

  /// @brief コピーコンストラクタ
  BnNetwork(
    const BnNetwork& src ///< [in] コピー元のオブジェクト
  );

  /// @brief ムーブコンストラクタ
  BnNetwork(
    BnNetwork&& src ///< [in] ムーブ元のオブジェクト
  );

  /// @brief コピー代入演算子
  /// @return 代入後の自身への参照を返す．
  BnNetwork&
  operator=(
    const BnNetwork& src ///< [in] コピー元のオブジェクト
  );

  /// @brief ムーブ代入演算子
  /// @return 代入後の自身への参照を返す．
  BnNetwork&
  operator=(
    BnNetwork&& src ///< [in] ムーブ元のオブジェクト
  );

  /// @brief デストラクタ
  ~BnNetwork();


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
    const BnNetwork& src ///< [in] コピー元のオブジェクト
  );

  /// @brief 内容をムーブする．
  ///
  /// src は破壊される．
  void
  move(
    BnNetwork&& src ///< [in] ムーブ元のオブジェクト
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

  /// @brief 1ビットの入力ポートを作る．
  /// @return 生成したポート番号を返す．
  SizeType
  new_input_port(
    const string& port_name ///< [in] ポート名
  );

  /// @brief 多ビットの入力ポートを作る．
  /// @return 生成したポート番号を返す．
  SizeType
  new_input_port(
    const string& port_name, ///< [in] ポート名
    SizeType bit_width       ///< [in] ビット幅
  );

  /// @brief 1ビットの出力ポートを作る．
  /// @return 生成したポート番号を返す．
  SizeType
  new_output_port(
    const string& port_name ///< [in] ポート名
  );

  /// @brief 多ビットの出力ポートを作る．
  /// @return 生成したポート番号を返す．
  SizeType
  new_output_port(
    const string& port_name, ///< [in] ポート名
    SizeType bit_width       ///< [in] ビット幅
  );

  /// @brief 入出力混合のポートを作る．
  /// @return 生成したポート番号を返す．
  ///
  /// - dir_vect[i] == 0 の時，入力を表す．
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
    bool has_xoutput = false, ///< [in] 反転出力端子を持つ時 true にする．
    bool has_clear = false,   ///< [in] クリア端子を持つ時 true にする．
    bool has_preset = false   ///< [in] プリセット端子を持つ時 true にする．
  );

  /// @brief セルの情報を持ったDFFを追加する．
  /// @return 生成したDFF番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - FFセルでない場合には -1 を返す．
  SizeType
  new_dff(
    const string& name, ///< [in] DFF名
    int cell_id         ///< [in] 対応するセル番号
  );

  /// @brief ラッチを追加する．
  /// @return 生成したラッチ番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  SizeType
  new_latch(
    const string& name,       ///< [in] ラッチ名
    bool has_xoutput = false, ///< [in] 反転出力端子を持つ時 true にする．
    bool has_clear = false,   ///< [in] クリア端子を持つ時 true にする．
    bool has_preset = false   ///< [in] プリセット端子を持つ時 true にする．
  );

  /// @brief セルの情報を持ったラッチを追加する．
  /// @return 生成したラッチ番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - ラッチセルでない場合には -1 を返す．
  SizeType
  new_latch(
    const string& name, ///< [in] ラッチ名
    int cell_id         ///< [in] 対応するセル番号
  );

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
  SizeType
  new_logic(
    const string& node_name, ///< [in] ノード名
    BnNodeType logic_type,   ///< [in] 論理型
    SizeType ni              ///< [in] 入力数
  );

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
  SizeType
  new_logic(
    const string& node_name,              ///< [in] ノード名
    BnNodeType logic_type,                ///< [in] 論理型
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 論理式型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は expr.input_num() を用いる．
  SizeType
  new_logic(
    const string& node_name,               ///< [in] ノード名
    const Expr& expr,                      ///< [in] 論理式
    const vector<SizeType>& fanin_id_list  ///< [in] ファンインのノード番号のリスト
    = vector<SizeType>{}
  );

  /// @brief 真理値表型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は tv.input_num() を用いる．
  SizeType
  new_logic(
    const string& node_name,              ///< [in] ノード名
    const TvFunc& tv,                     ///< [in] 真理値表
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
    = vector<SizeType>{}
  );

  /// @brief 論理セルを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 論理セルでない場合には BNET_NULLID を返す．
  SizeType
  new_logic(
    const string& node_name,              ///< [in] ノード名
    int cell_id,                          ///< [in] セル番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
    = vector<SizeType>{}
  );

  /// @brief 与えられたノードと同型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  SizeType
  dup_logic(
    const string& node_name,              ///< [in] ノード名
    SizeType node_id,                     ///< [in] コピー元のノード番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
    = vector<SizeType>{}
  );

  /// @brief C0型(定数０)の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_c0(
    const string& node_name ///< [in] ノード名
  );

  /// @brief C1型(定数1)の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_c1(
    const string& node_name ///< [in] ノード名
  );

  /// @brief BUFF型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_buff(
    const string& node_name,      ///< [in] ノード名
    SizeType fanin_id = BNET_NULLID ///< [in] ファンインのノード番号
  );

  /// @brief NOT型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_not(
    const string& node_name,      ///< [in] ノード名
    SizeType fanin_id = BNET_NULLID ///< [in] ファンインのノード番号
  );

  /// @brief AND型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_and(
    const string& node_name, ///< [in] ノード名
    SizeType ni              ///< [in] 入力数
  );

  /// @brief AND型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_and(
    const string& node_name,              ///< [in] ノード名
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief NAND型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_nand(
    const string& node_name, ///< [in] ノード名
    SizeType ni              ///< [in] 入力数
  );

  /// @brief NAND型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_nand(
    const string& node_name,              ///< [in] ノード名
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief OR型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_or(
    const string& node_name, ///< [in] ノード名
    SizeType ni              ///< [in] 入力数
  );

  /// @brief OR型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_or(
    const string& node_name,              ///< [in] ノード名
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief NOR型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_nor(
    const string& node_name, ///< [in] ノード名
    SizeType ni              ///< [in] 入力数
  );

  /// @brief NOR型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_nor(
    const string& node_name,              ///< [in] ノード名
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief XOR型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_xor(
    const string& node_name, ///< [in] ノード名
    SizeType ni              ///< [in] 入力数
  );

  /// @brief XOR型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_xor(
    const string& node_name,              ///< [in] ノード名
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief XNOR型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_xnor(
    const string& node_name, ///< [in] ノード名
    SizeType ni              ///< [in] 入力数
  );

  /// @brief XNOR型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_xnor(
    const string& node_name,              ///< [in] ノード名
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief プリミティブ型の論理ノードに変更する．
  ///
  /// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
  void
  change_primitive(
    SizeType id,           ///< [in] ノード番号
    BnNodeType logic_type, ///< [in] 論理型
    SizeType ni            ///< [in] 入力数
  );

  /// @brief プリミティブ型の論理ノードに変更する．
  ///
  /// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
  void
  change_primitive(
    SizeType id,                          ///< [in] ノード番号
    BnNodeType logic_type,                ///< [in] 論理型
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 論理式型の論理ノードに変更する．
  ///
  /// 入力数は expr.input_size() から得る．
  void
  change_expr(
    SizeType id,     ///< [in] ノード番号
    const Expr& expr ///< [in] 論理式
  );

  /// @brief 論理式型の論理ノードに変更する．
  void
  change_expr(
    SizeType id,                          ///< [in] ノード番号
    const Expr& expr,                     ///< [in] 論理式
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 真理値表型の論理ノードに変更する．
  ///
  /// 入力数は tv.input_num() から得る．
  void
  change_tv(
    SizeType id,     ///< [in] ノード番号
    const TvFunc& tv ///< [in] 真理値表
  );

  /// @brief 真理値表型の論理ノードに変更する．
  void
  change_tv(
    SizeType id,                          ///< [in] ノード番号
    const TvFunc& tv,                     ///< [in] 真理値表
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 論理セルに変更する．
  void
  change_cell(
    SizeType id,                          ///< [in] ノード番号
    int cell_id,                          ///< [in] セル番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 論理セルに変更する．
  ///
  /// - 入力数はセルから取得する．
  /// - 論理セルでない場合にはなにもしない．
  void
  change_cell(
    SizeType id,     ///< [in] ノード番号
    int cell_id ///< [in] セル番号
  );

  /// @brief 部分回路を追加する．
  ///
  /// * src_network は wrap_up() されている必要がある．
  /// * src_network のポートの情報は失われる．
  /// * 矛盾しない限りセルライブラリの情報も引継がれる．
  void
  import_subnetwork(
    const BnNetwork& src_network,       ///< [in] 部分回路
    const vector<SizeType>& input_list, ///< [in] インポートした部分回路の入力に接続するノード番号のリスト
    vector<SizeType>& output_list       ///< [in] インポートした部分回路の出力ノード番号のリスト
  );

  /// @brief 単純なノードに分解する．
  ///
  /// 単純なノードとは以下のノード型
  /// * BnNodeType::C0
  /// * BnNodeType::C1
  /// * BnNodeType::Buff
  /// * BnNodeType::Not
  /// * BnNodeType::And
  /// * BnNodeType::Nand
  /// * BnNodeType::Or
  /// * BnNodeType::Nor
  /// * BnNodeType::Xor
  /// * BnNodeType::Xnor
  void
  simple_decomp();

  /// @brief ノード間を接続する．
  void
  connect(
    SizeType src_node, ///< [in] ファンアウト元のノード番号
    SizeType dst_node, ///< [in] ファンイン先のノード番号
    SizeType ipos ///< [in] ファンインの位置
  );

  /// @brief ファンアウトをつなぎ替える．
  void
  substitute_fanout(
    SizeType old_id, ///< [in] もとのノード番号
    SizeType new_id  ///< [in] つなぎ替える新しいノード番号
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
  name() const;

  /// @brief 関連するセルライブラリを得る．
  ///
  /// 場合によっては空の場合もある．
  const ClibCellLibrary&
  library() const;

  /// @brief ポート数を得る．
  SizeType
  port_num() const;

  /// @brief ポートの情報を得る．
  const BnPort&
  port(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < port_num() )
  ) const;

  /// @brief DFF数を得る．
  SizeType
  dff_num() const;

  /// @brief DFFを得る．
  const BnDff&
  dff(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < dff_num() )
  ) const;

  /// @brief ラッチ数を得る．
  SizeType
  latch_num() const;

  /// @brief ラッチを得る．
  const BnLatch&
  latch(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < latch_num() )
  ) const;

  /// @brief ノード数を得る．
  SizeType
  node_num() const;

  /// @brief ノードを得る．
  ///
  /// const BnNode& node = BnNetwork::node(id);
  /// node.id() == id が成り立つ．
  const BnNode&
  node(
    SizeType id ///< [in] ノード番号 ( 0 <= id < node_num() )
  ) const;

  /// @brief 入力数を得る．
  SizeType
  input_num() const;

  /// @brief 入力ノードのノード番号を得る．
  SizeType
  input_id(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < input_num() )
  ) const;

  /// @brief 入力ノードのノード番号のリストを得る．
  const vector<SizeType>&
  input_id_list() const;

  /// @brief 外部入力ノードのノード番号のリストを得る．
  const vector<SizeType>&
  primary_input_id_list() const;

  /// @brief 出力数を得る．
  SizeType
  output_num() const;

  /// @brief 出力ノードのノード番号を得る．
  SizeType
  output_id(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const;

  /// @brief 出力ノードのノード番号のリストを得る．
  const vector<SizeType>&
  output_id_list() const;

  /// @brief 出力ノードのソースノード番号を得る．
  ///
  /// ソースノードとは出力ノードのファンインのノード
  SizeType
  output_src_id(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const;

  /// @brief 出力ノードのソースノード番号のリストを得る．
  ///
  /// ソースノードとは出力ノードのファンインのノード
  const vector<SizeType>&
  output_src_id_list() const;

  /// @brief 外部出力ノードのノード番号のリストを得る．
  const vector<SizeType>&
  primary_output_id_list() const;

  /// @brief 外部出力ノードのソースノード番号のリストを得る．
  const vector<SizeType>&
  primary_output_src_id_list() const;

  /// @brief 論理ノード数を得る．
  SizeType
  logic_num() const;

  /// @brief 論理ノードのノード番号を得る．
  SizeType
  logic_id(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < logic_num() )
  ) const;

  /// @brief 論理ノードのノード番号のリストを得る．
  const vector<SizeType>&
  logic_id_list() const;

  /// @brief 関数の数を得る．
  SizeType
  func_num() const;

  /// @brief 関数番号から関数を得る．
  const TvFunc&
  func(
    SizeType func_id ///< [in] 関数番号 ( 0 <= func_id < func_num() )
  ) const;

  /// @brief 論理式の数を得る．
  SizeType
  expr_num() const;

  /// @brief 論理式番号から論理式を得る．
  Expr
  expr(
    SizeType expr_id ///< [in] 論理式番号 ( 0 <= expr_id < expr_num() )
  ) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name ファイル入出力関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief blif ファイルを読み込む．
  /// @return ネットワークを返す．
  static
  BnNetwork
  read_blif(
    const string& filename,             ///< [in] ファイル名
    const string& clock_name = "clock", ///< [in] クロック端子名
    const string& reset_name = "reset"  ///< [in] リセット端子名
  );

  /// @brief blif ファイルを読み込む(セルライブラリ付き)．
  /// @return ネットワークを返す．
  static
  BnNetwork
  read_blif(
    const string& filename,              ///< [in] ファイル名
    const ClibCellLibrary& cell_library, ///< [in] セルライブラリ
    const string& clock_name = "clock",  ///< [in] クロック端子名
    const string& reset_name = "reset"   ///< [in] リセット端子名
  );

  /// @brief iscas89 ファイルを読み込む．
  /// @return ネットワークを返す．
  static
  BnNetwork
  read_iscas89(
    const string& filename,            ///< [in] ファイル名
    const string& clock_name = "clock" ///< [in] クロック端子名
  );

  /// @brief 内容を blif 形式で出力する．
  ///
  /// ポートの情報は無視される．
  void
  write_blif(
    const string& filename,          ///< [in] 出力先のファイル名
    const string& prefix = "__node", ///< [in] 自動生成名の接頭語
    const string& suffix = ""        ///< [in] 自動生成名の接尾語
  ) const;

  /// @brief 内容を ISCAS89(.bench) 形式で出力する．
  ///
  /// ポートの情報は無視される．
  void
  write_iscas89(
    const string& filename,           ///< [in] 出力先のファイル名
    const string& prefix = "__node",  ///< [in] 自動生成名の接頭語
    const string& suffix = "") const; ///< [in] 自動生成名の接尾語

  /// @brief 内容を Verilog-HDL 形式で出力する．
  void
  write_verilog(
    const string& filename,                 ///< [in] 出力先のファイル名
    const string& port_prefix = "__port",   ///< [in] ポート自動生成名の接頭語
    const string& port_suffix = "",         ///< [in] ポート自動生成名の接尾語
    const string& node_prefix = "__node",   ///< [in] ノード自動生成名の接頭語
    const string& node_suffix = "",         ///< [in] ノード自動生成名の接尾語
    const string& instance_prefix = "__U",  ///< [in] インスタンス自動生成名の接頭語
    const string& instance_suffix = ""      ///< [in] インスタンス自動生成名の接尾語
  ) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name ストリーム出力関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を blif 形式で出力する．
  ///
  /// ポートの情報は無視される．
  void
  write_blif(
    ostream& s,                      ///< [in] 出力先のストリーム
    const string& prefix = "__node", ///< [in] 自動生成名の接頭語
    const string& suffix = ""        ///< [in] 自動生成名の接尾語
  ) const;

  /// @brief 内容を ISCAS89(.bench) 形式で出力する．
  ///
  /// ポートの情報は無視される．
  void
  write_iscas89(
    ostream& s,                      ///< [in] 出力先のストリーム
    const string& prefix = "__node", ///< [in] 自動生成名の接頭語
    const string& suffix = ""        ///< [in] 自動生成名の接尾語
  ) const;

  /// @brief 内容を Verilog-HDL 形式で出力する．
  void
  write_verilog(
    ostream& s,                            ///< [in] 出力先のストリーム
    const string& port_prefix = "__port",  ///< [in] ポート自動生成名の接頭語
    const string& port_suffix = "",        ///< [in] ポート自動生成名の接尾語
    const string& node_prefix = "__node",  ///< [in] ノード自動生成名の接頭語
    const string& node_suffix = "",        ///< [in] ノード自動生成名の接尾語
    const string& instance_prefix = "__U", ///< [in] インスタンス自動生成名の接頭語
    const string& instance_suffix = ""     ///< [in] インスタンス自動生成名の接尾語
  ) const;

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

  /// @brief ファンインの接続を行う．
  void
  connect_fanins(
    SizeType id,                          ///< [in] 対象のノード番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装クラスへのポインタ
  unique_ptr<BnNetworkImpl> mImpl;

};

END_NAMESPACE_YM_BNET

#endif // BNNETWORK_H
