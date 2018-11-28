#ifndef BNNETWORKIMPL_H
#define BNNETWORKIMPL_H

/// @file BnNetworkImpl.h
/// @brief BnNetworkImpl のヘッダファイル
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
  BnNetworkImpl();

  /// @brief デストラクタ
  ~BnNetworkImpl();


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
  copy(const BnNetworkImpl& src);

  /// @brief セルライブラリをセットする．
  /// @param[in] library ライブラリ
  void
  set_library(const ClibCellLibrary& library);

  /// @brief ネットワーク名を設定する．
  /// @param[in] name ネットワーク名
  void
  set_name(const string& name);

  /// @brief 部分回路を追加する．
  /// @param[in] src_network 部分回路
  /// @param[in] input_list インポートした部分回路の入力に接続するノード番号のリスト
  /// @param[out] output_list インポートした部分回路の出力ノード番号のリスト
  ///
  /// * src_network は wrap_up() されている必要がある．
  /// * src_network のポートの情報は失われる．
  /// * 矛盾しない限りセルライブラリの情報も引く継がれる．
  void
  import_subnetwork(const BnNetworkImpl& src_network,
		    const vector<int>& input_list,
		    vector<int>& output_list);

  /// @brief 各ノードがプリミティブ型になるように分解する．
  void
  simple_decomp();

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
  /// @param[in] cell_id 対応するセル番号
  /// @return 生成したDFF番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - FFセルでない場合には -1 を返す．
  int
  new_dff(const string& name,
	  int cell_id);

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
  /// @param[in] cell_id 対応するセル番号
  /// @return 生成したラッチ番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - ラッチセルでない場合には -1 を返す．
  int
  new_latch(const string& name,
	    int cell_id);

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @param[in] logic_type 論理型
  /// @return 生成した論理ノードの番号を返す．
  int
  new_primitive(const string& node_name,
		int ni,
		BnNodeType logic_type);

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] logic_type 論理型
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  int
  new_primitive(const string& node_name,
		BnNodeType logic_type,
		const vector<int>& fanin_id_list);

  /// @brief 論理式型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] expr 論理式
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は expr.input_num() を用いる．
  int
  new_expr(const string& node_name,
	   const Expr& expr);

  /// @brief 論理式型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] expr 論理式
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は expr.input_num() を用いる．
  int
  new_expr(const string& node_name,
	   const Expr& expr,
	   const vector<int>& fanin_id_list);

  /// @brief 真理値表型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] tv 真理値表
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は tv.input_num() を用いる．
  int
  new_tv(const string& node_name,
	 const TvFunc& tv);

  /// @brief 真理値表型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] tv 真理値表
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 入力数は tv.input_num() を用いる．
  int
  new_tv(const string& node_name,
	 const TvFunc& tv,
	 const vector<int>& fanin_id_list);

  /// @brief 論理セルを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] cell_id セル番号
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 論理セルでない場合には kBnNullId を返す．
  int
  new_cell(const string& node_name,
	   int cell_id);

  /// @brief 論理セルを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] cell_id セル番号
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - 論理セルでない場合には kBnNullId を返す．
  int
  new_cell(const string& node_name,
	   int cell_id,
	   const vector<int>& fanin_id_list);

  /// @brief プリミティブ型の論理ノードに変更する．
  /// @param[in] id ノード番号
  /// @param[in] ni 入力数
  /// @param[in] logic_type 論理型
  void
  change_primitive(int id,
		   int ni,
		   BnNodeType logic_type);

  /// @brief プリミティブ型の論理ノードに変更する．
  /// @param[in] id ノード番号
  /// @param[in] logic_type 論理型
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  void
  change_primitive(int id,
		   BnNodeType logic_type,
		   const vector<int>& fanin_id_list);

  /// @brief 論理式型の論理ノードに変更する．
  /// @param[in] id ノード番号
  /// @param[in] expr 論理式
  ///
  /// 入力数は expr.input_size() を用いる．
  void
  change_expr(int id,
	      const Expr& expr);

  /// @brief 論理式型の論理ノードに変更する．
  /// @param[in] id ノード番号
  /// @param[in] expr 論理式
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  ///
  /// 入力数は expr.input_size() を用いる．
  void
  change_expr(int id,
	      const Expr& expr,
	      const vector<int>& fanin_id_list);

  /// @brief 真理値表型の論理ノードに変更する．
  /// @param[in] id ノード番号
  /// @param[in] tv 真理値表
  ///
  /// - 入力数は tv.input_num() を用いる．
  void
  change_tv(int id,
	    const TvFunc& tv);

  /// @brief 真理値表型の論理ノードに変更する．
  /// @param[in] id ノード番号
  /// @param[in] tv 真理値表
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  ///
  /// - 入力数は tv.input_num() を用いる．
  void
  change_tv(int id,
	    const TvFunc& tv,
	    const vector<int>& fanin_id_list);

  /// @brief セル型の論理ノードに変更する．
  /// @param[in] id ノード番号
  /// @param[in] cell_id セル番号
  ///
  /// - 入力数はセルから取得する．
  /// - 論理セルでない場合にはなにもしない．
  void
  change_cell(int id,
	      int cell_id);

  /// @brief セル型の論理ノードに変更する．
  /// @param[in] id ノード番号
  /// @param[in] cell_id セル番号
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  ///
  /// - 入力数はセルから取得する．
  /// - 論理セルでない場合にはなにもしない．
  void
  change_cell(int id,
	      int cell_id,
	      const vector<int>& fanin_id_list);

  /// @brief ファンアウトをつなぎ替える．
  /// @param[in] old_id もとのノード番号
  /// @param[in] new_id つなぎ替える新しいノード番号
  void
  substitute_fanout(int old_id,
		    int new_id);

  /// @brief ファンインをつなぎ替える．
  /// @param[in] id ノード番号
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  void
  connect_fanins(int id,
		 const vector<int>& fanin_id_list);

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
  /// BnNode* node = BnNetworkImpl::node(id);
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
  _dup_dff(const BnDff& src_dff,
	   vector<int>& id_map);

  /// @brief ラッチを複製する．
  /// @param[in] src_latch 元のラッチ
  /// @param[out] id_map 生成したノードの対応関係を記録するハッシュ表
  /// @return 生成したラッチ番号を返す．
  int
  _dup_latch(const BnLatch& src_latch,
	     vector<int>& id_map);

  /// @brief 論理ノードを複製する．
  /// @param[in] src_node 元のノード
  /// @param[in] src_network 元のネットワーク
  /// @param[out] id_map 生成したノードの対応関係を記録するハッシュ表
  /// @return 生成したノード番号を返す．
  ///
  /// id_map の内容の基づいてファンイン間の接続を行う．
  int
  _dup_logic(const BnNode& src_node,
	     const BnNetworkImpl& src_network,
	     vector<int>& id_map);

  /// @brief 論理式型のノードを分解する．
  /// @param[in] id ノード番号
  /// @param[in] expr 論理式
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return ノード番号を返す．
  ///
  /// * id が kBnNullId 以外ならそのノードを置き換える．
  /// * そうでなければ新規のノードを作る．
  int
  _decomp_expr(int id,
	       const Expr& expr,
	       const vector<int>& fanin_id_list);

  /// @brief 真理値表型のノードを分解する．
  /// @param[in] id ノード番号
  /// @param[in] func 関数
  /// @param[in] fanin_id_list ファンインのノード番号のリスト
  /// @return ノード番号を返す．
  ///
  /// * id が kBnNullId 以外ならそのノードを置き換える．
  /// * そうでなければ新規のノードを作る．
  int
  _decomp_tvfunc(int id,
		 const TvFunc& func,
		 const vector<int>& fanin_id_list);

  /// @brief DFFを追加する共通の処理を行う関数
  /// @param[in] name DFF名
  /// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
  /// @param[in] has_clear クリア端子を持つ時 true にする．
  /// @param[in] has_preset プリセット端子を持つ時 true にする．
  /// @param[in] cell_id 対応するセル番号．
  /// @return 生成したDFF番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  int
  _new_dff(const string& name,
	   bool has_xoutput,
	   bool has_clear,
	   bool has_preset,
	   int cell_id);

  /// @brief ラッチを追加する共通の処理を行う関数
  /// @param[in] name ラッチ名
  /// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
  /// @param[in] has_clear クリア端子を持つ時 true にする．
  /// @param[in] has_preset プリセット端子を持つ時 true にする．
  /// @param[in] cell_id 対応するセル番号．
  /// @return 生成したラッチ番号を返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - cell はラッチのセルでなければならない．
  int
  _new_latch(const string& name,
	     bool has_xoutput,
	     bool has_clear,
	     bool has_preset,
	     int cell_id);

  /// @brief プリミティブ型の論理ノードを生成する．
  /// @param[in] id ノード番号
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @param[in] logic_type 論理型
  /// @param[in] cell_id 対応するセル番号．
  /// @return 生成した論理ノードを返す．
  BnNodeImpl*
  _new_primitive(int id,
		 const string& node_name,
		 int ni,
		 BnNodeType logic_type,
		 int cell_id);

  /// @brief 論理式型の論理ノードを生成する．
  /// @param[in] id ノード番号
  /// @param[in] node_name ノード名
  /// @param[in] expr 論理式
  /// @param[in] cell_id 対応するセル番号．
  /// @return 生成した論理ノードを返す．
  BnNodeImpl*
  _new_expr(int id,
	    const string& node_name,
	    const Expr& expr,
	    int cell_id);

  /// @brief 真理値表型の論理ノードを生成する．
  /// @param[in] id ノード番号
  /// @param[in] node_name ノード名
  /// @param[in] tv 真理値表
  /// @param[in] cell_id 対応するセル番号．
  /// @return 生成した論理ノードを返す．
  BnNodeImpl*
  _new_tv(int id,
	  const string& node_name,
	  const TvFunc& tv,
	  int cell_id);

  /// @brief セル型の論理ノードを生成する．
  /// @param[in] id ノード番号
  /// @param[in] node_name ノード名
  /// @param[in] cell_id セル番号
  /// @return 生成した論理ノードを返す．
  ///
  /// - セル名に合致するセルがない場合と論理セルでない場合には nullptr を返す．
  BnNodeImpl*
  _new_cell(int id,
	    const string& node_name,
	    int cell_id);

  /// @brief 論理式を登録する．
  /// @param[in] expr 論理式
  /// @return 関数番号を返す．
  int
  _reg_expr(const Expr& expr);

  /// @brief 真理値表を登録する．
  /// @param[in] tv 真理値表
  /// @return 関数番号を返す．
  int
  _reg_tv(const TvFunc& tv);


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
  vector<int> mInputList;

  // 出力ノード番号のリスト
  vector<int> mOutputList;

  // 出力ソースノード番号のリスト
  vector<int> mOutputSrcList;

  // 論理ノード番号のリスト
  vector<int> mLogicList;

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


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ネットワーク名を得る．
inline
string
BnNetworkImpl::name() const
{
  return mName;
}

// @brief 関連するセルライブラリを得る．
//
// 場合によっては空の場合もある．
inline
const ClibCellLibrary&
BnNetworkImpl::library() const
{
  return mCellLibrary;
}

// @brief ポート数を得る．
inline
int
BnNetworkImpl::port_num() const
{
  return mPortList.size();
}

// @brief ポートの情報を得る．
// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
inline
const BnPort&
BnNetworkImpl::port(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < port_num() );
  return *mPortList[pos];
}

// @brief DFF数を得る．
inline
int
BnNetworkImpl::dff_num() const
{
  return mDffList.size();
}

// @brief DFFを返す．
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
inline
const BnDff&
BnNetworkImpl::dff(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < dff_num() );
  return *mDffList[pos];
}

// @brief ラッチ数を得る．
inline
int
BnNetworkImpl::latch_num() const
{
  return mLatchList.size();
}

// @brief ラッチを得る．
// @param[in] pos 位置番号 ( 0 <= pos < latch_num() )
inline
const BnLatch&
BnNetworkImpl::latch(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < latch_num() );
  return *mLatchList[pos];
}

// @brief ノード数を得る．
inline
int
BnNetworkImpl::node_num() const
{
  return mNodeList.size();
}

// @brief 入力数を得る．
inline
int
BnNetworkImpl::input_num() const
{
  return mInputList.size();
}

// @brief 入力ノードのノード番号を得る．
// @param[in] pos 入力番号 ( 0 <= pos < input_num() )
inline
int
BnNetworkImpl::input_id(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < input_num() );
  return mInputList[pos];
}

// @brief 入力ノードのノード番号のリストを得る．
inline
const vector<int>&
BnNetworkImpl::input_id_list() const
{
  return mInputList;
}

// @brief 出力数を得る．
inline
int
BnNetworkImpl::output_num() const
{
  return mOutputList.size();
}

// @brief 出力ノードのノード番号を得る．
// @param[in] pos 出力番号 ( 0 <= pos < output_num() )
inline
int
BnNetworkImpl::output_id(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < output_num() );
  return mOutputList[pos];
}

// @brief 出力ノードのノード番号のリストを得る．
inline
const vector<int>&
BnNetworkImpl::output_id_list() const
{
  return mOutputList;
}

// @brief 出力ノードのソースノード番号を得る．
// @param[in] pos 出力番号 ( 0 <= pos < output_num() )
inline
int
BnNetworkImpl::output_src_id(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < output_num() );
  return mOutputSrcList[pos];
}

// @brief 出力ノードのソースノード番号のリストを得る．
inline
const vector<int>&
BnNetworkImpl::output_src_id_list() const
{
  return mOutputSrcList;
}

// @brief 論理ノード数を得る．
inline
int
BnNetworkImpl::logic_num() const
{
  return mLogicList.size();
}

// @brief 論理ノードのノード番号を得る．
// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
inline
int
BnNetworkImpl::logic_id(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < logic_num() );
  return mLogicList[pos];
}

// @brief 論理ノードのノード番号のリストを得る．
inline
const vector<int>&
BnNetworkImpl::logic_id_list() const
{
  return mLogicList;
}

// @brief 関数の数を得る．
inline
int
BnNetworkImpl::func_num() const
{
  return mFuncList.size();
}

// @brief 関数番号から関数を得る．
// @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
inline
const TvFunc&
BnNetworkImpl::func(int func_id) const
{
  ASSERT_COND( func_id >= 0 && func_id < func_num() );
  return mFuncList[func_id];
}

// @brief 論理式の数を得る．
inline
int
BnNetworkImpl::expr_num() const
{
  return mExprList.size();
}

// @brief 論理式番号から論理式を得る．
// @param[in] expr_id 論理式番号 ( 0 <= expr_id < expr_num() )
inline
Expr
BnNetworkImpl::expr(int expr_id) const
{
  ASSERT_COND( expr_id >= 0 && expr_id < expr_num() );
  return mExprList[expr_id];
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] logic_type 論理型
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
inline
int
BnNetworkImpl::new_primitive(const string& node_name,
			     BnNodeType logic_type,
			     const vector<int>& fanin_id_list)
{
  int ni = fanin_id_list.size();
  int id = new_primitive(node_name, ni, logic_type);
  connect_fanins(id, fanin_id_list);
  return id;
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] expr 論理式
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
// - 入力数は expr.input_num() を用いる．
inline
int
BnNetworkImpl::new_expr(const string& node_name,
			const Expr& expr,
			const vector<int>& fanin_id_list)
{
  int id = new_expr(node_name, expr);
  connect_fanins(id, fanin_id_list);
  return id;
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] tv 真理値表
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
// - 入力数は tv.input_num() を用いる．
inline
int
BnNetworkImpl::new_tv(const string& node_name,
		      const TvFunc& tv,
		      const vector<int>& fanin_id_list)
{
  int id = new_tv(node_name, tv);
  connect_fanins(id, fanin_id_list);
  return id;
}

// @brief 論理セルを追加する．
// @param[in] node_name ノード名
// @param[in] cell_id セル番号
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
// - 論理セルでない場合には kBnNullId を返す．
inline
int
BnNetworkImpl::new_cell(const string& node_name,
			int cell_id,
			const vector<int>& fanin_id_list)
{
  int id = new_cell(node_name, cell_id);
  connect_fanins(id, fanin_id_list);
  return id;
}

// @brief プリミティブ型の論理ノードに変更する．
// @param[in] id ノード番号
// @param[in] logic_type 論理型
// @param[in] fanin_id_list ファンインのノード番号のリスト
inline
void
BnNetworkImpl::change_primitive(int id,
				BnNodeType logic_type,
				const vector<int>& fanin_id_list)
{
  int ni = fanin_id_list.size();
  change_primitive(id, ni, logic_type);
  connect_fanins(id, fanin_id_list);
}

// @brief 論理式型の論理ノードに変更する．
// @param[in] id ノード番号
// @param[in] expr 論理式
// @param[in] fanin_id_list ファンインのノード番号のリスト
//
// 入力数は expr.input_size() を用いる．
inline
void
BnNetworkImpl::change_expr(int id,
			   const Expr& expr,
			   const vector<int>& fanin_id_list)
{
  ASSERT_COND( expr.input_size() == fanin_id_list.size() );

  change_expr(id, expr);
  connect_fanins(id, fanin_id_list);
}

// @brief 真理値表型の論理ノードに変更する．
// @param[in] id ノード番号
// @param[in] tv 真理値表
// @param[in] fanin_id_list ファンインのノード番号のリスト
//
// - 入力数は tv.input_num() を用いる．
inline
void
BnNetworkImpl::change_tv(int id,
			 const TvFunc& tv,
			 const vector<int>& fanin_id_list)
{
  ASSERT_COND( tv.input_num() == fanin_id_list.size() );

  change_tv(id, tv);
  connect_fanins(id, fanin_id_list);
}

// @brief セル型の論理ノードに変更する．
// @param[in] id ノード番号
// @param[in] cell_id セル番号
// @param[in] fanin_id_list ファンインのノード番号のリスト
//
// - 入力数はセルから取得する．
// - 論理セルでない場合にはなにもしない．
inline
void
BnNetworkImpl::change_cell(int id,
			   int cell_id,
			   const vector<int>& fanin_id_list)
{
  change_cell(id, cell_id);
  connect_fanins(id, fanin_id_list);
}

END_NAMESPACE_YM_BNET

#endif // BNNETWORKIMPL_H
