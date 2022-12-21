#ifndef BNNETWORK_H
#define BNNETWORK_H

/// @file BnNetwork.h
/// @brief BnNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2021, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/clib.h"
#include "ym/Bdd.h"
#include "ym/BinEnc.h"
#include "ym/BinDec.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/BnPort.h"
#include "ym/BnPortList.h"
#include "ym/BnDff.h"
#include "ym/BnDffList.h"
#include "ym/BnNode.h"
#include "ym/BnNodeList.h"


BEGIN_NAMESPACE_YM_BNET

class BnNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class BnNetwork BnNetwork.h "ym/BnNetwork.h"
/// @brief ブーリアンネットワークを表すクラス
/// @sa BnPort
/// @sa BnDff
/// @sa BnNode
///
/// ただし，このネットワークはDFFノードを含むので正確には
/// syncronous boolean network かもしれない．
///
/// 組み合わせ回路部分は BnNode の DAG で表す．
/// 全体の構造を表すためにそれ以外のデータ構造として以下の要素を持つ．
/// - ポート(BnPort)
///   名前とビット幅，各ビットに対応するノード番号を持つ．
/// - D-FF/ラッチノード(BnDff)
///   入力と出力およびクロックのノード番号を持つ．
///   反転出力を持つ場合もある．
///   オプションとしてセット，リセット端子のノード番号を持つ．
///   場合によってはセルの割当情報を持つ．
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
/// ノード番号は変化しない．また，ノードを削除することもできない．
/// ただし，内容をファイルに書き出す際に出力ノードへ到達しないノードは削除される
/// ので，そのファイルを読み込んだネットワークではノード番号が異なる可能性がある．
//////////////////////////////////////////////////////////////////////
class BnNetwork
{
  friend class BnModifier;

public:
  //////////////////////////////////////////////////////////////////////
  /// @name コンストラクタ/デストラクタ
  /// @{
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

  /// @brief ポート名からポート番号を得る．
  ///
  /// 見つからない場合は -1 を返す．
  SizeType
  find_port(
    const string& name ///< [in] ポート名
  ) const;

  /// @brief ポートのリストを得る．
  BnPortList
  port_list() const;

  /// @brief DFF数を得る．
  SizeType
  dff_num() const;

  /// @brief DFFを得る．
  const BnDff&
  dff(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < dff_num() )
  ) const;

  /// @brief DFFのリストを得る．
  BnDffList
  dff_list() const;

  /// @brief ノード数を得る．
  SizeType
  node_num() const;

  /// @brief ノードを得る．
  ///
  /// const BnNode& node = BnNetwork::node(id);
  /// node.id() == id が成り立つ．
  const BnNode&
  node(
    SizeType id ///< [in] ノード番号 ( 1 <= id <= node_num() )
  ) const;

  /// @brief 全てのノードのリストを得る．
  BnAllNodeList
  all_node_list() const;

  /// @brief 入力数を得る．
  SizeType
  input_num() const;

  /// @brief 入力ノードのノード番号を得る．
  SizeType
  input_id(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < input_num() )
  ) const;

  /// @brief 入力ノードのノード番号を得る．
  const BnNode&
  input_node(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < input_num() )
  ) const
  {
    return node(input_id(pos));
  }

  /// @brief 入力ノードのノードのリストを得る．
  BnNodeList
  input_list() const;

  /// @brief 外部入力ノードのノードのリストを得る．
  BnNodeList
  primary_input_list() const;

  /// @brief 出力数を得る．
  SizeType
  output_num() const;

  /// @brief 出力ノードのノード番号を得る．
  SizeType
  output_id(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const;

  /// @brief 出力ノードを得る．
  const BnNode&
  output_node(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const
  {
    return node(output_id(pos));
  }

  /// @brief 出力ノードのノードのリストを得る．
  BnNodeList
  output_list() const;

  /// @brief 外部出力ノードのノードのリストを得る．
  BnNodeList
  primary_output_list() const;

  /// @brief 論理ノード数を得る．
  SizeType
  logic_num() const;

  /// @brief 論理ノードのノード番号を得る．
  SizeType
  logic_id(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < logic_num() )
  ) const;

  /// @brief 論理ノードのリストを得る．
  BnNodeList
  logic_list() const;

  /// @brief 実装可能な構造を持っている時 true を返す．
  bool
  is_concrete() const;

  /// @brief 全てのノードがセル割当情報を持つ時 true を返す．
  bool
  is_mapped() const;

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
  /// @name 変形操作を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 1つの出力に関係したノードのみからなるネットワークを返す．
  BnNetwork
  output_split(
    SizeType output_id ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const;

  /// @brief primitive ノードに分解したネットワークを返す．
  BnNetwork
  simple_decomp() const;

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
    const string& filename,              ///< [in] ファイル名
    const string& clock_name = string{}, ///< [in] クロック端子名
    const string& reset_name = string{}  ///< [in] リセット端子名
  );

  /// @brief blif ファイルを読み込む(セルライブラリ付き)．
  /// @return ネットワークを返す．
  static
  BnNetwork
  read_blif(
    const string& filename,              ///< [in] ファイル名
    const ClibCellLibrary& cell_library, ///< [in] セルライブラリ
    const string& clock_name = string{}, ///< [in] クロック端子名
    const string& reset_name = string{}  ///< [in] リセット端子名
  );

  /// @brief iscas89 ファイルを読み込む．
  /// @return ネットワークを返す．
  static
  BnNetwork
  read_iscas89(
    const string& filename,             ///< [in] ファイル名
    const string& clock_name = string{} ///< [in] クロック端子名
  );

  /// @brief .truth 形式のファイルを読み込む．
  /// @return ネットワークを返す．
  ///
  /// 論理ノードは BDD タイプとなる．
  static
  BnNetwork
  read_truth(
    const string& filename ///< [in] ファイル名
  );

  /// @brief .aag 形式のファイルを読み込む．
  /// @return ネットワークを返す．
  static
  BnNetwork
  read_aag(
    const string& filename,              ///< [in] ファイル名
    const string& clock_name = string{}, ///< [in] クロック端子名
    const string& reset_name = string{}  ///< [in] リセット端子名
  );

  /// @brief .aig 形式のファイルを読み込む．
  /// @return ネットワークを返す．
  static
  BnNetwork
  read_aig(
    const string& filename,              ///< [in] ファイル名
    const string& clock_name = string{}, ///< [in] クロック端子名
    const string& reset_name = string{}  ///< [in] リセット端子名
  );

  /// @brief 内容を blif 形式で出力する．
  ///
  /// ポートの情報は無視される．
  void
  write_blif(
    const string& filename,          ///< [in] 出力先のファイル名
    const string& prefix = string{}, ///< [in] 自動生成名の接頭語
    const string& suffix = string{}  ///< [in] 自動生成名の接尾語
  ) const;

  /// @brief 内容を ISCAS89(.bench) 形式で出力する．
  ///
  /// ポートの情報は無視される．
  void
  write_iscas89(
    const string& filename,          ///< [in] 出力先のファイル名
    const string& prefix = string{}, ///< [in] 自動生成名の接頭語
    const string& suffix = string{}  ///< [in] 自動生成名の接尾語
  ) const;

  /// @brief 内容を Verilog-HDL 形式で出力する．
  void
  write_verilog(
    const string& filename,                   ///< [in] 出力先のファイル名
    const string& port_prefix = string{},     ///< [in] ポート自動生成名の接頭語
    const string& port_suffix = string{},     ///< [in] ポート自動生成名の接尾語
    const string& node_prefix = string{},     ///< [in] ノード自動生成名の接頭語
    const string& node_suffix = string{},     ///< [in] ノード自動生成名の接尾語
    const string& instance_prefix = string{}, ///< [in] インスタンス自動生成名の接頭語
    const string& instance_suffix = string{}  ///< [in] インスタンス自動生成名の接尾語
  ) const;

  /// @brief 内容を aig 形式で出力する．
  void
  write_aig(
    const string& filename,          ///< [in] ファイル名
    const string& comment = string{} ///< [in] コメント文字列
  ) const;

  /// @brief 内容を aag (ascii aig) 形式で出力する．
  void
  write_aag(
    const string& filename,          ///< [in] ファイル名
    const string& comment = string{} ///< [in] コメント文字列
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
    const string& prefix = string{}, ///< [in] 自動生成名の接頭語
    const string& suffix = string{}  ///< [in] 自動生成名の接尾語
  ) const;

  /// @brief 内容を ISCAS89(.bench) 形式で出力する．
  ///
  /// ポートの情報は無視される．
  void
  write_iscas89(
    ostream& s,                      ///< [in] 出力先のストリーム
    const string& prefix = string{}, ///< [in] 自動生成名の接頭語
    const string& suffix = string{}  ///< [in] 自動生成名の接尾語
  ) const;

  /// @brief 内容を Verilog-HDL 形式で出力する．
  void
  write_verilog(
    ostream& s,                               ///< [in] 出力先のストリーム
    const string& port_prefix = string{},     ///< [in] ポート自動生成名の接頭語
    const string& port_suffix = string{},     ///< [in] ポート自動生成名の接尾語
    const string& node_prefix = string{},     ///< [in] ノード自動生成名の接頭語
    const string& node_suffix = string{},     ///< [in] ノード自動生成名の接尾語
    const string& instance_prefix = string{}, ///< [in] インスタンス自動生成名の接頭語
    const string& instance_suffix = string{}  ///< [in] インスタンス自動生成名の接尾語
  ) const;

  /// @brief 内容を aig 形式で出力する．
  void
  write_aig(
    ostream& s,                      ///< [in] 出力先のストリーム
    const string& comment = string{} ///< [in] コメント文字列
  ) const;

  /// @brief 内容を aag (ascii aig) 形式で出力する．
  void
  write_aag(
    ostream& s,                      ///< [in] 出力先のストリーム
    const string& comment = string{} ///< [in] コメント文字列
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


public:
  //////////////////////////////////////////////////////////////////////
  /// @name dump/restore関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を独自形式でバイナリダンプする．
  void
  dump(
    BinEnc& s ///< [in] 出力ストリーム
  ) const;

  /// @brief バイナリダンプされた内容を復元する．
  static
  BnNetwork
  restore(
    BinDec& s ///< [in] 入力ストリーム
  );

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装クラスへのポインタ
  unique_ptr<BnNetworkImpl> mImpl;

};

END_NAMESPACE_YM_BNET

#endif // BNNETWORK_H
