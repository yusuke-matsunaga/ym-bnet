#ifndef BNMODIFIER_H
#define BNMODIFIER_H

/// @file BnModifier.h
/// @brief BnModifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnModifier BnModifier.h "BnModifier.h"
/// @brief BnNetwork を変更するクラスの基底クラス
//////////////////////////////////////////////////////////////////////
class BnModifier :
  public BnNetwork
{
public:

  /// @brief コンストラクタ
  ///
  /// 内部に空のネットワークを持つ．
  BnModifier();

  /// @brief BnNetwork からのムーブコンストラクタ
  BnModifier(
    BnNetwork&& src
  );

  /// @brief BnNetwork からのムーブ代入演算
  BnModifier&
  operator=(
    BnNetwork&& src
  );

  /// @brief デストラクタ
  ~BnModifier();


public:
  //////////////////////////////////////////////////////////////////////
  // コピーは禁止
  //////////////////////////////////////////////////////////////////////

  BnModifier(
    const BnModifier& src
  ) = delete;

  BnModifier(
    const BnNetwork& src
  ) = delete;

  BnModifier&
  operator=(
    const BnModifier& src
  ) = delete;

  BnModifier&
  operator=(
    const BnNetwork& src
  ) = delete;


public:
  //////////////////////////////////////////////////////////////////////
  /// @name ネットワークの内容を変更する関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  ///
  /// コンストラクタ直後と同じ状態になる．
  void
  clear();

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
  /// @return 生成したポートを返す．
  BnPort
  new_input_port(
    const string& port_name ///< [in] ポート名
  );

  /// @brief 多ビットの入力ポートを作る．
  /// @return 生成したポートを返す．
  BnPort
  new_input_port(
    const string& port_name, ///< [in] ポート名
    SizeType bit_width       ///< [in] ビット幅
  );

  /// @brief 1ビットの出力ポートを作る．
  /// @return 生成したポートを返す．
  BnPort
  new_output_port(
    const string& port_name ///< [in] ポート名
  );

  /// @brief 多ビットの出力ポートを作る．
  /// @return 生成したポートを返す．
  BnPort
  new_output_port(
    const string& port_name, ///< [in] ポート名
    SizeType bit_width       ///< [in] ビット幅
  );

  /// @brief 入出力混合のポートを作る．
  /// @return 生成したポートを返す．
  ///
  /// - dir_vect[i] == 0 の時，入力を表す．
  BnPort
  new_port(
    const string& port_name,      ///< [in] ポート名
    const vector<BnDir>& dir_vect ///< [in] 向きを表すベクタ
  );

  /// @brief DFFを追加する．
  /// @return 生成したDFFを返す．
  ///
  /// 生成されたDFFは以下の入出力端子を持つ．
  /// - データ入力
  /// - クロック入力(positive edge)
  /// - 非同期クリア(positive edge): has_clear = true の時
  /// - 非同期プリセット(positive edge): has_preset = true の時
  /// - データ出力
  ///
  /// 名前の重複に関しては感知しない．
  BnDff
  new_dff(
    const string& name,      ///< [in] DFF名
    bool has_clear = false,  ///< [in] クリア端子を持つ時 true にする．
    bool has_preset = false, ///< [in] プリセット端子を持つ時 true にする．
    BnCPV cpv = BnCPV::L     ///< [in] クリアとプリセットが衝突したときの挙動
  );

  /// @brief ラッチを追加する．
  /// @return 生成したDFFを返す．
  ///
  /// 生成されたラッチは以下の入出力端子を持つ．
  /// - データ入力
  /// - イネーブル入力(positive edge)
  /// - 非同期クリア(positive edge): has_clear = true の時
  /// - 非同期プリセット(positive edge): has_preset = true の時
  /// - データ出力
  ///
  /// - 名前の重複に関しては感知しない．
  BnDff
  new_latch(
    const string& name,      ///< [in] ラッチ名
    bool has_clear = false,  ///< [in] クリア端子を持つ時 true にする．
    bool has_preset = false, ///< [in] プリセット端子を持つ時 true にする．
    BnCPV cpv = BnCPV::L     ///< [in] クリアとプリセットが衝突したときの挙動
  );

  /// @brief セルの情報を持ったDFFを追加する．
  /// @return 生成したDFFを返す．
  ///
  /// 名前の重複に関しては感知しない．
  /// cell_id が FFセルでない場合はエラーとなる．
  BnDff
  new_dff_cell(
    const string& name, ///< [in] DFF名
    SizeType cell_id    ///< [in] 対応するセル番号
  );

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
  BnNode
  new_logic_primitive(
    const string& node_name,            ///< [in] ノード名
    BnNodeType logic_type,              ///< [in] 論理型
    const vector<BnNode>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 論理式型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は expr.input_num() を用いる．
  BnNode
  new_logic_expr(
    const string& node_name,            ///< [in] ノード名
    const Expr& expr,                   ///< [in] 論理式
    const vector<BnNode>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 真理値表型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は tv.input_num() を用いる．
  BnNode
  new_logic_tv(
    const string& node_name,            ///< [in] ノード名
    const TvFunc& tv,                   ///< [in] 真理値表
    const vector<BnNode>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief BDD型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_logic_bdd(
    const string& node_name,            ///< [in] ノード名
    const Bdd& bdd,                     ///< [in] BDD
    const vector<BnNode>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 論理セルを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_logic_cell(
    const string& node_name,            ///< [in] ノード名
    SizeType cell_id,                   ///< [in] セル番号
    const vector<BnNode>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief C0型(定数０)の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_c0(
    const string& node_name ///< [in] ノード名
  );

  /// @brief C1型(定数1)の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_c1(
    const string& node_name ///< [in] ノード名
  );

  /// @brief BUFF型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_buff(
    const string& node_name, ///< [in] ノード名
    BnNode fanin             ///< [in] ファンインのノード
  );

  /// @brief NOT型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_not(
    const string& node_name, ///< [in] ノード名
    BnNode fanin             ///< [in] ファンインのノード番号
  );

  /// @brief AND型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_and(
    const string& node_name,         ///< [in] ノード名
    const vector<BnNode>& fanin_list ///< [in] ファンインのノードのリスト
  );

  /// @brief NAND型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_nand(
    const string& node_name,         ///< [in] ノード名
    const vector<BnNode>& fanin_list ///< [in] ファンインのノードのリスト
  );

  /// @brief OR型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_or(
    const string& node_name,         ///< [in] ノード名
    const vector<BnNode>& fanin_list ///< [in] ファンインのノードのリスト
  );

  /// @brief NOR型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_nor(
    const string& node_name,         ///< [in] ノード名
    const vector<BnNode>& fanin_list ///< [in] ファンインのノードのリスト
  );

  /// @brief XOR型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_xor(
    const string& node_name,         ///< [in] ノード名
    const vector<BnNode>& fanin_list ///< [in] ファンインのノードのリスト
  );

  /// @brief XNOR型の論理ノードを追加する．
  /// @return 生成した論理ノードを返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  BnNode
  new_xnor(
    const string& node_name,         ///< [in] ノード名
    const vector<BnNode>& fanin_list ///< [in] ファンインのノードのリスト
  );

  /// @brief プリミティブ型の論理ノードに変更する．
  ///
  /// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
  void
  change_primitive(
    BnNode node,                     ///< [in] ノード
    BnNodeType logic_type,           ///< [in] 論理型
    const vector<BnNode>& fanin_list ///< [in] ファンインのノードのリスト
  );

  /// @brief 論理式型の論理ノードに変更する．
  void
  change_expr(
    BnNode node,                        ///< [in] ノード
    const Expr& expr,                   ///< [in] 論理式
    const vector<BnNode>& fanin_id_list ///< [in] ファンインのノードのリスト
  );

  /// @brief 真理値表型の論理ノードに変更する．
  void
  change_tv(
    BnNode node,                     ///< [in] ノード
    const TvFunc& tv,                ///< [in] 真理値表
    const vector<BnNode>& fanin_list ///< [in] ファンインのノードのリスト
  );

  /// @brief 論理セルに変更する．
  void
  change_cell(
    BnNode node,                        ///< [in] ノード
    SizeType cell_id,                   ///< [in] セル番号
    const vector<BnNode>& fanin_id_list ///< [in] ファンインのノードのリスト
  );

  /// @brief ポートの情報のみコピーする．
  BnNodeMap
  make_skelton_copy(
    const BnNetwork& src_network ///< [in] コピー元の回路
  );

  /// @brief DFFをコピーする．
  /// @return 生成したDFFを返す．
  BnDff
  copy_dff(
    BnDff src_dff,      ///< [in] コピー元のDFF
    BnNodeMap& node_map ///< [inout] ID番号の対応表
  );

  /// @brief 論理ノードをコピーする．
  /// @brief 生成した論理ノードを返す．
  BnNode
  copy_logic(
    BnNode src_node,    ///< [in] コピー元のノード番号
    BnNodeMap& node_map ///< [inout] ID番号の対応表
  );

  /// @brief 出力ノードを複製する．
  ///
  /// 出力ノードそのものはすでに生成済みでファンインの情報の
  /// 設定のみを行う．
  void
  copy_output(
    BnNode src_node,    ///< [in] 出力ノード
    BnNodeMap& node_map ///< [inout] ID番号の対応表
  );

  /// @brief 部分回路を追加する．
  /// @return インポートした部分回路の出力ノードのリストを返す．
  ///
  /// * src_network のポートの情報は失われる．
  /// * 矛盾しない限りセルライブラリの情報も引継がれる．
  vector<BnNode>
  import_subnetwork(
    const BnNetwork& src_network,    ///< [in] 部分回路
    const vector<BnNode>& input_list ///< [in] インポートする部分回路の入力に接続するノードのリスト
  );

  /// @brief 出力ノードのファンインを設定する．
  void
  set_output_src(
    BnNode output,  ///< [in] 出力ノード
    BnNode src_node ///< [in] ファンインノード
  );

  /// @brief ファンアウトをつなぎ替える．
  void
  substitute_fanout(
    BnNode old_node, ///< [in] もとのノード
    BnNode new_node  ///< [in] つなぎ替える新しいノード
  );

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード番号のリストを作る．
  vector<SizeType>
  make_id_list(
    const vector<BnNode>& node_list
  );

  /// @brief ノードのリストを作る．
  vector<BnNode>
  make_node_list(
    const vector<SizeType>& id_list
  );

};

END_NAMESPACE_YM_BNET

#endif // BNMODIFIER_H
