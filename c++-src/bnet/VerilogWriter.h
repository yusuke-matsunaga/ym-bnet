#ifndef VERILOGWRITER_H
#define VERILOGWRITER_H

/// @file VerilogWriter.h
/// @brief VerilogWriter のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/NameMgr.h"


BEGIN_NAMESPACE_YM_BNET


//////////////////////////////////////////////////////////////////////
/// @class VerilogWriter VerilogWriter.h "VerilogWriter.h"
/// @brief BnNetwork を Verilog 形式で出力するクラス
///
/// BnNode の種類に応じて以下のように出力する．
/// - セル型 ( cell_id() != -1 )
///   セルインスタンス文を出力する．
/// - プリミティブ型 (Buff, Not, And, Nand, Or, Nor, Xor, Xnor) :
///   assign 文の右辺に対応する論理式を出力する．
/// - 論理式型 (Expr) :
///   assign 文の右辺に論理式を出力する．
/// - 真理値表型 (TvFunc) :
///   Verilog の UDP instance を用いる．
///
/// BnDff の種類に応じて以下のように出力する．
/// - セル型 ( cell_id() != -1 )
///   セルインスタンス文を出力する．
/// - それ以外
///   always 文を出力する．
///
/// 名前は以下のルールを適用する．
/// - 外部ポート名
///   * 重複しない名前を持つ場合はその名前を使う．
///   * 名前がない場合と他のポート名と重複する場合には自動生成名を使う．
///   * ポート名はポートの生成された順に優先順位を持つ．
///     同名のポートがあった場合には後ろのポート名が自動生成名に替わる．
///   * ポートの自動生成名は "%s%d%s", port_prefix(), port_id, port_suffix()
///     で作る．port_id は自動生成するポートに固有の番号
///
/// - ノード名(信号線名)
///   * 重複しない名前を持つ場合はその名前を使う．
///   * 名前がない場合と他のノード名と重複する場合には自動生成名を使う．
///   * ノード名は外部入力ノードが最も高い優先順位を持つ．
///     論理ノードは入力からのトポロジカル順に優先順位を持つ．
///   * ノード名の自動生成名は　"%s%d%s", node_prefix(), node_id, node_suffix()
///     で作る．node_id は自動生成するノードに固有の番号
///
/// - DFF名,ラッチ名，セルインスタンス名
///   * すべて自動生成名を用いる．
///   * 自動生成名は "%s%d%d", instance_prefix(), instance_id, instance_suffix()
///     で作る．instance_id は自動生成するインスタンスに固有の番号
//////////////////////////////////////////////////////////////////////
class VerilogWriter
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] port_prefix ポートの自動生成名の接頭語
  /// @param[in] port_suffix ポートの自動生成名の接尾語
  /// @param[in] node_prefix ノードの自動生成名の接頭語
  /// @param[in] node_suffix ノードの自動生成名の接尾語
  /// @param[in] instance_prefix インスタンスの自動生成名の接頭語
  /// @param[in] instance_suffix インスタンスの自動生成名の接尾語
  VerilogWriter(const BnNetwork& network,
		const string& port_prefix = "__port",
		const string& port_suffix = "",
		const string& node_prefix = "__wire",
		const string& node_suffix = "",
		const string& instance_prefix = "__U",
		const string& instance_suffix = "");

  /// @brief デストラクタ
  ~VerilogWriter();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief blif 形式で出力する．
  /// @param[in] s 出力先のストリーム
  void
  operator()(ostream& s);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前の配列を初期化する．
  void
  init_name_array();

  /// @brief ポート名の登録を行う．
  /// @param[in] port_id ポート番号
  /// @param[in] name_hash ポート名のハッシュ
  /// @param[in] name_mgr ポート名を管理するクラス
  void
  reg_port_name(int port_id,
		unordered_set<string>& name_hash,
		NameMgr& name_mgr);

  /// @brief ノード名の登録を行う．
  /// @param[in] node_id ノード番号
  /// @param[in] name_hash ノード名のハッシュ
  /// @param[in] name_mgr ノード名を管理するクラス
  /// @param[out] node_list ノード名の生成が必要なノード番号のリスト
  void
  reg_node_name(int node_id,
		unordered_set<string>& name_hash,
		NameMgr& name_mgr);

  /// @brief ノード名をそのファンインのノード名に付け替える．
  void
  replace_node_name(int node_id);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const BnNetwork& mNetwork;

  // ポートの自動生成名の接頭語
  string mPortPrefix;

  // ポートの自動生成名の接尾語
  string mPortSuffix;

  // ノードの自動生成名の接頭語
  string mNodePrefix;

  // ノードの自動生成名の接尾語
  string mNodeSuffix;

  // インスタンスの自動生成名の接頭語
  string mInstancePrefix;

  // インスタンスの自動生成名の接尾語
  string mInstanceSuffix;

  // ポート名を入れた配列
  vector<string> mPortNameArray;

  // ノード名を入れた配列
  vector<string> mNodeNameArray;

  // ノードのインスタンス名を入れた配列
  vector<string> mNodeInstanceNameArray;

  // DFFのインスタンス名を入れた配列
  vector<string> mDffInstanceNameArray;

  // ラッチのインスタンス名を入れた配列
  vector<string> mLatchInstanceNameArray;

};

END_NAMESPACE_YM_BNET

#endif // VERILOGWRITER_H
