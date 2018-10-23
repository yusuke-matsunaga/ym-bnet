#ifndef BNNETWORK_H
#define BNNETWORK_H

/// @file BnNetwork.h
/// @brief BnNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/ClibCellLibrary.h"
#include "ym/HashMap.h"


BEGIN_NAMESPACE_YM_BNET

class BnNodeImpl;

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
/// ポートの名前空間はノードとは別に設ける．
/// 通常の blif ファイルや .bench(iscas89) ファイルを読んだ場合，ポートは1つのノードに対応する．
///
/// このクラスはファイル入出力用のモデルであり，このクラス上で
/// 回路変換などの処理を行うことは考えていない．
///
/// 回路構造を作るには new_XXXX() でノードを作り，connect() で接続を作る．
/// 最後に wrap_up() で最終処理を行う．
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
  /// @param[in] src コピー元のオブジェクト
  BnNetwork(const BnNetwork& src);

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
  /// @param[in] src コピー元のオブジェクト
  void
  copy(const BnNetwork& src);

  /// @brief セルライブラリをセットする．
  /// @param[in] library ライブラリ
  void
  set_library(const ClibCellLibrary& library);

  /// @brief ネットワーク名を設定する．
  /// @param[in] name ネットワーク名
  void
  set_name(const string& name);

  /// @brief 1ビットの入力ポートを作る．
  /// @param[in] port_name ポート名
  /// @return 生成したポート番号を返す．
  int
  new_input_port(const string& port_name);

  /// @brief 多ビットの入力ポートを作る．
  /// @param[in] port_name ポート名
  /// @param[in] bit_width ビット幅
  /// @return 生成したポート番号を返す．
  int
  new_input_port(const string& port_name,
		 int bit_width);

  /// @brief 1ビットの出力ポートを作る．
  /// @param[in] port_name ポート名
  /// @return 生成したポート番号を返す．
  int
  new_output_port(const string& port_name);

  /// @brief 多ビットの出力ポートを作る．
  /// @param[in] port_name ポート名
  /// @param[in] bit_width ビット幅
  /// @return 生成したポート番号を返す．
  int
  new_output_port(const string& port_name,
		  int bit_width);

  /// @brief 入出力混合のポートを作る．
  /// @param[in] port_name ポート名
  /// @param[in] dir_vect 向きを表すベクタ
  /// @return 生成したポート番号を返す．
  ///
  /// - dir_vect[i] == 0 の時，入力を表す．
  int
  new_port(const string& port_name,
	   const vector<int>& dir_vect);

  /// @brief DFFを追加する．
  /// @param[in] name DFF名
  /// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
  /// @param[in] has_clear クリア端子を持つ時 true にする．
  /// @param[in] has_preset プリセット端子を持つ時 true にする．
  /// @return 生成したDFF番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  int
  new_dff(const string& name,
	  bool has_xoutput = false,
	  bool has_clear = false,
	  bool has_preset = false);

  /// @brief セルの情報を持ったDFFを追加する．
  /// @param[in] name DFF名
  /// @param[in] cell_name 対応するセル名
  /// @return 生成したDFF番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - セル名に合致するセルがない場合とFFセルでない場合には nullptr を返す．
  int
  new_dff(const string& name,
	  const string& cell_name);

  /// @brief ラッチを追加する．
  /// @param[in] name ラッチ名
  /// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
  /// @param[in] has_clear クリア端子を持つ時 true にする．
  /// @param[in] has_preset プリセット端子を持つ時 true にする．
  /// @return 生成したラッチ番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  int
  new_latch(const string& name,
	    bool has_xoutput = false,
	    bool has_clear = false,
	    bool has_preset = false);

  /// @brief セルの情報を持ったラッチを追加する．
  /// @param[in] name ラッチ名
  /// @param[in] cell_name 対応するセル名．
  /// @return 生成したラッチ番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - セル名に合致するセルがない場合とラッチセルでない場合には nullptr を返す．
  int
  new_latch(const string& name,
	    const string& cell_name);

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] logic_type 論理型
  /// @param[in] ni 入力数
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
  int
  new_logic(const string& node_name,
	    BnNodeType logic_type,
	    int ni);

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] logic_type 論理型
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
  int
  new_logic(const string& node_name,
	    BnNodeType logic_type,
	    const vector<int>& fanin_id_list);

  /// @brief C0型(定数０)の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_c0(const string& node_name);

  /// @brief C1型(定数1)の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_c1(const string& node_name);

  /// @brief BUFF型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] fanin_id ファンインのノード番号
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_buff(const string& node_name,
	   int fanin_id = kBnNullId);

  /// @brief NOT型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] fanin_id ファンインのノード番号
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_not(const string& node_name,
	  int fanin_id = kBnNullId);

  /// @brief AND型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_and(const string& node_name,
	  int ni);

  /// @brief AND型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_and(const string& node_name,
	  const vector<int>& fanin_id_list);

  /// @brief NAND型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_nand(const string& node_name,
	   int ni);

  /// @brief NAND型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_nand(const string& node_name,
	   const vector<int>& fanin_id_list);

  /// @brief OR型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_or(const string& node_name,
	 int ni);

  /// @brief OR型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_or(const string& node_name,
	 const vector<int>& fanin_id_list);

  /// @brief NOR型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_nor(const string& node_name,
	  int ni);

  /// @brief NOR型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_nor(const string& node_name,
	  const vector<int>& fanin_id_list);

  /// @brief XOR型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_xor(const string& node_name,
	  int ni);

  /// @brief XOR型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_xor(const string& node_name,
	  const vector<int>& fanin_id_list);

  /// @brief XNOR型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_xnor(const string& node_name,
	   int ni);

  /// @brief XNOR型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_xnor(const string& node_name,
	   const vector<int>& fanin_id_list);

  /// @brief 論理式型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] expr 論理式
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は expr.input_num() を用いる．
  int
  new_logic(const string& node_name,
	    const Expr& expr,
	    const vector<int>& fanin_id_list = vector<int>(0));

  /// @brief 真理値表型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] tv 真理値表
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は tv.input_num() を用いる．
  int
  new_logic(const string& node_name,
	    const TvFunc& tv,
	    const vector<int>& fanin_id_list = vector<int>(0));

  /// @brief 論理セルを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] cell_name セル名
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - セル名に合致するセルがない場合と論理セルでない場合には kBnNullId を返す．
  int
  new_logic(const string& node_name,
	    const string& cell_name,
	    const vector<int>& fanin_id_list = vector<int>(0));

  /// @brief 部分回路を追加する．
  /// @param[in] src_network 部分回路
  /// @param[in] input_list インポートした部分回路の入力に接続するノード番号のリスト
  /// @param[out] output_list インポートした部分回路の出力ノード番号のリスト
  ///
  /// * src_network は wrap_up() されている必要がある．
  /// * src_network のポートの情報は失われる．
  /// * 矛盾しない限りセルライブラリの情報も引く継がれる．
  void
  import_subnetwork(const BnNetwork& src_network,
		    const vector<int>& input_list,
		    vector<int>& output_list);

  /// @brief ノード間を接続する．
  /// @param[in] src_node ファンアウト元のノード番号
  /// @param[in] dst_node ファンイン先のノード番号
  /// @param[in] ipos ファンインの位置
  void
  connect(int src_node,
	  int dst_node,
	  int ipos);

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
  int
  port_num() const;

  /// @brief ポートの情報を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
  const BnPort&
  port(int pos) const;

  /// @brief DFF数を得る．
  int
  dff_num() const;

  /// @brief DFFを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
  const BnDff&
  dff(int pos) const;

  /// @brief ラッチ数を得る．
  int
  latch_num() const;

  /// @brief ラッチを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < latch_num() )
  const BnLatch&
  latch(int pos) const;

  /// @brief ノード数を得る．
  int
  node_num() const;

  /// @brief ノードを得る．
  /// @param[in] id ノード番号 ( 0 <= id < node_num() )
  ///
  /// BnNode* node = BnNetwork::node(id);
  /// node->id() == id が成り立つ．
  const BnNode&
  node(int id) const;

  /// @brief 入力数を得る．
  int
  input_num() const;

  /// @brief 入力ノードのノード番号を得る．
  /// @param[in] pos 入力番号 ( 0 <= pos < input_num() )
  int
  input_id(int pos) const;

  /// @brief 入力ノードのノード番号のリストを得る．
  const vector<int>&
  input_id_list() const;

  /// @brief 出力数を得る．
  int
  output_num() const;

  /// @brief 出力ノードのノード番号を得る．
  /// @param[in] pos 出力番号 ( 0 <= pos < output_num() )
  int
  output_id(int pos) const;

  /// @brief 出力ノードのノード番号のリストを得る．
  const vector<int>&
  output_id_list() const;

  /// @brief 出力ノードのソースノード番号を得る．
  /// @param[in] pos 出力番号 ( 0 <= pos < output_num() )
  ///
  /// ソースノードとは出力ノードのファンインのノード
  int
  output_src_id(int pos) const;

  /// @brief 出力ノードのソースノード番号のリストを得る．
  ///
  /// ソースノードとは出力ノードのファンインのノード
  const vector<int>&
  output_src_id_list() const;

  /// @brief 論理ノード数を得る．
  int
  logic_num() const;

  /// @brief 論理ノードのノード番号を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
  int
  logic_id(int pos) const;

  /// @brief 論理ノードのノード番号のリストを得る．
  const vector<int>&
  logic_id_list() const;

  /// @brief 関数の数を得る．
  int
  func_num() const;

  /// @brief 関数番号から関数を得る．
  /// @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
  const TvFunc&
  func(int func_id) const;

  /// @brief 論理式の数を得る．
  int
  expr_num() const;

  /// @brief 論理式番号から論理式を得る．
  /// @param[in] expr_id 論理式番号 ( 0 <= expr_id < expr_num() )
  Expr
  expr(int expr_id) const;

  /// @brief 内容を出力する．
  /// @param[in] s 出力先のストリーム
  ///
  /// - 形式は独自フォーマット
  void
  write(ostream& s) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief DFFを複製する．
  /// @param[in] src_dff 元のDFF
  /// @param[out] id_map 生成したノードの対応関係を記録するハッシュ表
  /// @return 生成した DFF 番号を返す．
  int
  dup_dff(const BnDff& src_dff,
	  vector<int>& id_map);

  /// @brief ラッチを複製する．
  /// @param[in] src_latch 元のラッチ
  /// @param[out] id_map 生成したノードの対応関係を記録するハッシュ表
  /// @return 生成したラッチ番号を返す．
  int
  dup_latch(const BnLatch& src_latch,
	    vector<int>& id_map);

  /// @brief 論理ノードを複製する．
  /// @param[in] src_node 元のノード
  /// @param[in] src_network 元のネットワーク
  /// @param[out] id_map 生成したノードの対応関係を記録するハッシュ表
  /// @return 生成したノード番号を返す．
  ///
  /// id_map の内容の基づいてファンイン間の接続を行う．
  int
  dup_logic(const BnNode& src_node,
	    const BnNetwork& src_network,
	    vector<int>& id_map);

  /// @brief DFFを追加する共通の処理を行う関数
  /// @param[in] name DFF名
  /// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
  /// @param[in] has_clear クリア端子を持つ時 true にする．
  /// @param[in] has_preset プリセット端子を持つ時 true にする．
  /// @param[in] cell 対応するセル．
  /// @return 生成したDFF番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  int
  _new_dff(const string& name,
	   bool has_xoutput,
	   bool has_clear,
	   bool has_preset,
	   const ClibCell* cell);

  /// @brief ラッチを追加する共通の処理を行う関数
  /// @param[in] name ラッチ名
  /// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
  /// @param[in] has_clear クリア端子を持つ時 true にする．
  /// @param[in] has_preset プリセット端子を持つ時 true にする．
  /// @param[in] cell 対応するセル．
  /// @return 生成したラッチ番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - cell はラッチのセルでなければならない．
  int
  _new_latch(const string& name,
	     bool has_xoutput,
	     bool has_clear,
	     bool has_preset,
	     const ClibCell* cell);

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @param[in] logic_type 論理型
  /// @param[in] cell 対応するセル
  /// @return 生成した論理ノードの番号を返す．
  int
  _new_primitive(const string& node_name,
		 int ni,
		 BnNodeType logic_type,
		 const ClibCell* cell);

  /// @brief 論理式型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @param[in] expr 論理式
  /// @param[in] cell 対応するセル
  /// @return 生成した論理ノードの番号を返す．
  int
  _new_expr(const string& node_name,
	    int ni,
	    const Expr& expr,
	    const ClibCell* cell);

  /// @brief 真理値表型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @param[in] tv 真理値表
  /// @param[in] cell 対応するセル
  /// @return 生成した論理ノードの番号を返す．
  int
  _new_tv(const string& node_name,
	  int ni,
	  const TvFunc& tv,
	  const ClibCell* cell);

  /// @brief 論理式を登録する．
  /// @param[in] expr 論理式
  /// @return 関数番号を返す．
  int
  _add_expr(const Expr& expr);

  /// @brief 真理値表を登録する．
  /// @param[in] tv 真理値表
  /// @return 関数番号を返す．
  int
  _add_tv(const TvFunc& tv);


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

  // 入力ノード番号のリスト
  vector<int> mInputList;

  // 出力ノード番号のリスト
  vector<int> mOutputList;

  // 出力ソースノード番号のリスト
  vector<int> mOutputSrcList;

  // 論理ノード番号のリスト
  vector<int> mLogicList;

  // ノード番号をキーにしてノードを納めた配列
  vector<BnNodeImpl*> mNodeList;

  // 関数のリスト
  vector<TvFunc> mFuncList;

  // TvFunc をキーにして関数番号を入れるハッシュ表
  HashMap<TvFunc, int> mFuncMap;

  // 論理式のリスト
  vector<Expr> mExprList;

  // TvFunc をキーにして論理式番号を入れるハッシュ表
  HashMap<TvFunc, int> mExprMap;

  // wrap_up() が実行後の時に true となるフラグ
  bool mSane;

};

/// @relates BnNetwork
/// @brief 内容を blif 形式で出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] network ネットワーク
///
/// ポートの情報は無視される．
void
write_blif(const BnNetwork& network,
	   ostream& s);

/// @relates BnNetwork
/// @brief 内容を blif 形式で出力する．
/// @param[in] filename 出力先のファイル名
/// @param[in] network ネットワーク
///
/// ポートの情報は無視される．
void
write_blif(const BnNetwork& network,
	   const string& filename);


// @brief blif ファイルを読み込む．
// @param[in] network 設定対象のネットワーク
// @param[in] filename ファイル名
// @param[in] clock_name クロック端子名
// @param[in] reset_name リセット端子名
// @return 読み込みが成功したら true を返す．
bool
read_blif(BnNetwork& network,
	  const string& filename,
	  const string& clock_name = "clock",
	  const string& reset_name = "reset");

// @brief blif ファイルを読み込む(セルライブラリ付き)．
// @param[in] network 設定対象のネットワーク
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @param[in] clock_name クロック端子名
// @param[in] reset_name リセット端子名
// @return 読み込みが成功したら true を返す．
bool
read_blif(BnNetwork& network,
	  const string& filename,
	  const ClibCellLibrary& cell_library,
	  const string& clock_name = "clock",
	  const string& reset_name = "reset");

// @brief iscas89 ファイルを読み込む．
// @param[in] network 設定対象のネットワーク
// @param[in] filename ファイル名
// @param[in] clock_name クロック端子名
// @return 読み込みが成功したら true を返す．
bool
read_iscas89(BnNetwork& network,
	     const string& filename,
	     const string& clock_name = "clock");


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ネットワーク名を得る．
inline
string
BnNetwork::name() const
{
  return mName;
}

// @brief 関連するセルライブラリを得る．
//
// 場合によっては空の場合もある．
inline
const ClibCellLibrary&
BnNetwork::library() const
{
  return mCellLibrary;
}

// @brief ポート数を得る．
inline
int
BnNetwork::port_num() const
{
  return mPortList.size();
}

// @brief ポートの情報を得る．
// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
inline
const BnPort&
BnNetwork::port(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < port_num() );
  return *mPortList[pos];
}

// @brief DFF数を得る．
inline
int
BnNetwork::dff_num() const
{
  return mDffList.size();
}

// @brief DFFを返す．
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
inline
const BnDff&
BnNetwork::dff(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < dff_num() );
  return *mDffList[pos];
}

// @brief ラッチ数を得る．
inline
int
BnNetwork::latch_num() const
{
  return mLatchList.size();
}

// @brief ラッチを得る．
// @param[in] pos 位置番号 ( 0 <= pos < latch_num() )
inline
const BnLatch&
BnNetwork::latch(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < latch_num() );
  return *mLatchList[pos];
}

// @brief ノード数を得る．
inline
int
BnNetwork::node_num() const
{
  return mNodeList.size();
}

// @brief 入力数を得る．
inline
int
BnNetwork::input_num() const
{
  return mInputList.size();
}

// @brief 入力ノードのノード番号を得る．
// @param[in] pos 入力番号 ( 0 <= pos < input_num() )
inline
int
BnNetwork::input_id(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < input_num() );
  return mInputList[pos];
}

// @brief 入力ノードのノード番号のリストを得る．
inline
const vector<int>&
BnNetwork::input_id_list() const
{
  return mInputList;
}

// @brief 出力数を得る．
inline
int
BnNetwork::output_num() const
{
  return mOutputList.size();
}

// @brief 出力ノードのノード番号を得る．
// @param[in] pos 出力番号 ( 0 <= pos < output_num() )
inline
int
BnNetwork::output_id(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < output_num() );
  return mOutputList[pos];
}

// @brief 出力ノードのノード番号のリストを得る．
inline
const vector<int>&
BnNetwork::output_id_list() const
{
  return mOutputList;
}

// @brief 出力ノードのソースノード番号を得る．
// @param[in] pos 出力番号 ( 0 <= pos < output_num() )
inline
int
BnNetwork::output_src_id(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < output_num() );
  return mOutputSrcList[pos];
}

// @brief 出力ノードのソースノード番号のリストを得る．
inline
const vector<int>&
BnNetwork::output_src_id_list() const
{
  return mOutputSrcList;
}

// @brief 論理ノード数を得る．
inline
int
BnNetwork::logic_num() const
{
  return mLogicList.size();
}

// @brief 論理ノードのノード番号を得る．
// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
inline
int
BnNetwork::logic_id(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < logic_num() );
  return mLogicList[pos];
}

// @brief 論理ノードのノード番号のリストを得る．
inline
const vector<int>&
BnNetwork::logic_id_list() const
{
  return mLogicList;
}

// @brief 関数の数を得る．
inline
int
BnNetwork::func_num() const
{
  return mFuncList.size();
}

// @brief 関数番号から関数を得る．
// @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
inline
const TvFunc&
BnNetwork::func(int func_id) const
{
  ASSERT_COND( func_id >= 0 && func_id < func_num() );
  return mFuncList[func_id];
}

// @brief 論理式の数を得る．
inline
int
BnNetwork::expr_num() const
{
  return mExprList.size();
}

// @brief 論理式番号から論理式を得る．
// @param[in] expr_id 論理式番号 ( 0 <= expr_id < expr_num() )
inline
Expr
BnNetwork::expr(int expr_id) const
{
  ASSERT_COND( expr_id >= 0 && expr_id < expr_num() );
  return mExprList[expr_id];
}

END_NAMESPACE_YM_BNET

#endif // BNNETWORK_H
