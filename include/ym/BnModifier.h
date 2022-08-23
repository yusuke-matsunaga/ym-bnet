#ifndef BNMODIFIER_H
#define BNMODIFIER_H

/// @file BnModifier.h
/// @brief BnModifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
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
  /// 生成されたDFFは以下の入出力端子を持つ．
  /// - データ入力
  /// - クロック入力(positive edge)
  /// - 非同期クリア(positive edge): has_clear = true の時
  /// - 非同期プリセット(positive edge): has_preset = true の時
  /// - データ出力
  ///
  /// 名前の重複に関しては感知しない．
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
  /// 生成されたラッチは以下の入出力端子を持つ．
  /// - データ入力
  /// - イネーブル入力(positive edge)
  /// - 非同期クリア(positive edge): has_clear = true の時
  /// - 非同期プリセット(positive edge): has_preset = true の時
  /// - データ出力
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
  /// 名前の重複に関しては感知しない．
  /// cell_id が FFセルでない場合はエラーとなる．
  SizeType
  new_dff_cell(
    const string& name, ///< [in] DFF名
    SizeType cell_id    ///< [in] 対応するセル番号
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
  new_logic(
    const string& node_name,              ///< [in] ノード名
    const TvFunc& tv,                     ///< [in] 真理値表
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief BDD型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_logic(
    const string& node_name,              ///< [in] ノード名
    const Bdd& bdd,                       ///< [in] BDD
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 論理セルを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_logic(
    const string& node_name,              ///< [in] ノード名
    SizeType cell_id,                     ///< [in] セル番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief 与えられたノードと同型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  SizeType
  dup_logic(
    const string& node_name,              ///< [in] ノード名
    SizeType node_id,                     ///< [in] コピー元のノード番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
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
    const string& node_name, ///< [in] ノード名
    SizeType fanin_id        ///< [in] ファンインのノード番号
  );

  /// @brief NOT型の論理ノードを追加する．
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  SizeType
  new_not(
    const string& node_name, ///< [in] ノード名
    SizeType fanin_id        ///< [in] ファンインのノード番号
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
    const string& node_name,              ///< [in] ノード名
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
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
    const string& node_name,              ///< [in] ノード名
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
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
    const string& node_name,              ///< [in] ノード名
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
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
  void
  change_expr(
    SizeType id,                          ///< [in] ノード番号
    const Expr& expr,                     ///< [in] 論理式
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
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
    SizeType cell_id,                     ///< [in] セル番号
    const vector<SizeType>& fanin_id_list ///< [in] ファンインのノード番号のリスト
  );

  /// @brief ポートの情報のみコピーする．
  unordered_map<SizeType, SizeType>
  make_skelton_copy(
    const BnNetwork& src_network ///< [in] コピー元の回路
  );

  /// @brief 部分回路を追加する．
  ///
  /// * src_network は wrap_up() されている必要がある．
  /// * src_network のポートの情報は失われる．
  /// * 矛盾しない限りセルライブラリの情報も引継がれる．
  vector<SizeType>
  import_subnetwork(
    const BnNetwork& src_network,      ///< [in] 部分回路
    const vector<SizeType>& input_list ///< [in] インポートした部分回路の入力に接続するノード番号のリスト
  );

  /// @brief 出力ノードのファンインを設定する．
  void
  set_output(
    SizeType output_id, ///< [in] 出力ノードのID番号
    SizeType src_id     ///< [in] ファンインノードのID番号
  );

  /// @brief ファンアウトをつなぎ替える．
  void
  substitute_fanout(
    SizeType old_id, ///< [in] もとのノード番号
    SizeType new_id  ///< [in] つなぎ替える新しいノード番号
  );

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

};

END_NAMESPACE_YM_BNET

#endif // BNMODIFIER_H
