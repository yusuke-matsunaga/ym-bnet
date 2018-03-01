#ifndef BNNETWORK_H
#define BNNETWORK_H

/// @file BnNetwork.h
/// @brief BnNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"
#include "ym/logic.h"
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
  /// @return 生成したポートを返す．
  BnPort*
  new_input_port(const string& port_name);

  /// @brief 多ビットの入力ポートを作る．
  /// @param[in] port_name ポート名
  /// @param[in] bit_width ビット幅
  /// @return 生成したポートを返す．
  BnPort*
  new_input_port(const string& port_name,
		 int bit_width);

  /// @brief 1ビットの出力ポートを作る．
  /// @param[in] port_name ポート名
  /// @return 生成したポートを返す．
  BnPort*
  new_output_port(const string& port_name);

  /// @brief 多ビットの出力ポートを作る．
  /// @param[in] port_name ポート名
  /// @param[in] bit_width ビット幅
  /// @return 生成したポートを返す．
  BnPort*
  new_output_port(const string& port_name,
		  int bit_width);

  /// @brief 入出力混合のポートを作る．
  /// @param[in] port_name ポート名
  /// @param[in] dir_vect 向きを表すベクタ
  /// @return 生成したポートを返す．
  ///
  /// - dir_vect[i] == 0 の時，入力を表す．
  BnPort*
  new_port(const string& port_name,
	   const vector<int>& dir_vect);

  /// @brief DFFを追加する．
  /// @param[in] name DFF名
  /// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
  /// @param[in] has_clear クリア端子を持つ時 true にする．
  /// @param[in] has_preset プリセット端子を持つ時 true にする．
  /// @return 生成したDFFを返す．
  ///
  /// - 名前の重複に関しては感知しない．
  BnDff*
  new_dff(const string& name,
	  bool has_xoutput = false,
	  bool has_clear = false,
	  bool has_preset = false);

  /// @brief セルの情報を持ったDFFを追加する．
  /// @param[in] name DFF名
  /// @param[in] cell_name 対応するセル名
  /// @return 生成したDFFを返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - セル名に合致するセルがない場合とFFセルでない場合には nullptr を返す．
  BnDff*
  new_dff_cell(const string& name,
	       const string& cell_name);

  /// @brief ラッチを追加する．
  /// @param[in] name ラッチ名
  /// @param[in] has_clear クリア端子を持つ時 true にする．
  /// @param[in] has_preset プリセット端子を持つ時 true にする．
  /// @return 生成したラッチを返す．
  ///
  /// - 名前の重複に関しては感知しない．
  BnLatch*
  new_latch(const string& name,
	    bool has_clear = false,
	    bool has_preset = false);

  /// @brief セルの情報を持ったラッチを追加する．
  /// @param[in] name ラッチ名
  /// @param[in] cell_name 対応するセル名．
  /// @return 生成したラッチを返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - セル名に合致するセルがない場合とラッチセルでない場合には nullptr を返す．
  BnLatch*
  new_latch_cell(const string& name,
		 const string& cell_name);

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @param[in] logic_type 論理型
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
  int
  new_primitive(const string& node_name,
		int ni,
		BnNodeType logic_type);

  /// @brief 論理式型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @param[in] expr 論理式
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_expr(const string& node_name,
	   int ni,
	   const Expr& expr);

  /// @brief 真理値表型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni 入力数
  /// @param[in] tv 真理値表
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  int
  new_tv(const string& node_name,
	 int ni,
	 const TvFunc& tv);

  /// @brief 論理セルを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] cell_name セル名
  /// @return 生成した論理ノードの番号を返す．
  ///
  /// - ノード名の重複に関しては感知しない．
  /// - セル名に合致するセルがない場合と論理セルでない場合には kBnNullId を返す．
  int
  new_logic_cell(const string& node_name,
		 const string& cell_name);

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
  const BnPort*
  port(int pos) const;

  /// @brief ポートのリストを得る．
  const vector<const BnPort*>&
  port_list() const;

  /// @brief DFF数を得る．
  int
  dff_num() const;

  /// @brief DFFを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
  const BnDff*
  dff(int pos) const;

  /// @brief DFFのリストを得る．
  const vector<const BnDff*>&
  dff_list() const;

  /// @brief ラッチ数を得る．
  int
  latch_num() const;

  /// @brief ラッチを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < latch_num() )
  const BnLatch*
  latch(int pos) const;

  /// @brief ラッチのリストを得る．
  const vector<const BnLatch*>&
  latch_list() const;

  /// @brief ノード数を得る．
  int
  node_num() const;

  /// @brief ノードを得る．
  /// @param[in] id ノード番号 ( 0 <= id < node_num() )
  ///
  /// BnNode* node = BnNetwork::node(id);
  /// node->id() == id が成り立つ．
  const BnNode*
  node(int id) const;

  /// @brief ノードのリストを得る．
  const vector<const BnNode*>&
  node_list() const;

  /// @brief 入力数を得る．
  int
  input_num() const;

  /// @brief 入力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
  const BnNode*
  input(int pos) const;

  /// @brief 入力ノードのリストを得る．
  const vector<const BnNode*>&
  input_list() const;

  /// @brief 出力数を得る．
  int
  output_num() const;

  /// @brief 出力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  const BnNode*
  output(int pos) const;

  /// @brief 出力ノードのリストを得る．
  const vector<const BnNode*>&
  output_list() const;

  /// @brief 論理ノード数を得る．
  int
  logic_num() const;

  /// @brief 論理ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
  const BnNode*
  logic(int pos) const;

  /// @brief 論理ノードのリストを得る．
  const vector<const BnNode*>&
  logic_list() const;

  /// @brief 関数の数を得る．
  int
  func_num() const;

  /// @brief 関数番号から関数を得る．
  /// @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
  const TvFunc&
  func(int func_id) const;

  /// @brief 関数のリストを得る．
  const vector<TvFunc>&
  func_list() const;

  /// @brief 論理式の数を得る．
  int
  expr_num() const;

  /// @brief 論理式番号から論理式を得る．
  /// @param[in] expr_id 論理式番号 ( 0 <= expr_id < expr_num() )
  Expr
  expr(int expr_id) const;

  /// @brief 論理式のリストを得る．
  const vector<Expr>&
  expr_list() const;

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

  /// @brief DFFを追加する共通の処理を行う関数
  /// @param[in] name DFF名
  /// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
  /// @param[in] has_clear クリア端子を持つ時 true にする．
  /// @param[in] has_preset プリセット端子を持つ時 true にする．
  /// @param[in] cell 対応するセル．
  /// @return 生成したDFFを返す．
  ///
  /// - 名前の重複に関しては感知しない．
  BnDff*
  _new_dff(const string& name,
	   bool has_xoutput,
	   bool has_clear,
	   bool has_preset,
	   const ClibCell* cell);

  /// @brief ラッチを追加する共通の処理を行う関数
  /// @param[in] name ラッチ名
  /// @param[in] has_clear クリア端子を持つ時 true にする．
  /// @param[in] has_preset プリセット端子を持つ時 true にする．
  /// @param[in] cell 対応するセル．
  /// @return 生成したラッチを返す．
  ///
  /// - 名前の重複に関しては感知しない．
  /// - cell はラッチのセルでなければならない．
  BnLatch*
  _new_latch(const string& name,
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

  // 入力ノードのリスト
  vector<const BnNode*> mInputList;

  // 出力ノードのリスト
  vector<const BnNode*> mOutputList;

  // 論理ノードのリスト
  vector<const BnNode*> mLogicList;

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
write_blif(ostream& s,
	   const BnNetwork& network);


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

// @brief ポートのリストを得る．
inline
const vector<const BnPort*>&
BnNetwork::port_list() const
{
  return mPortList;
}

// @brief DFFのリストを得る．
inline
const vector<const BnDff*>&
BnNetwork::dff_list() const
{
  return mDffList;
}

// @brief ラッチのリストを得る．
inline
const vector<const BnLatch*>&
BnNetwork::latch_list() const
{
  return mLatchList;
}

// @brief 入力ノードのリストを得る．
inline
const vector<const BnNode*>&
BnNetwork::input_list() const
{
  return mInputList;
}

// @brief 出力ノードのリストを得る．
inline
const vector<const BnNode*>&
BnNetwork::output_list() const
{
  return mOutputList;
}

// @brief 論理ノードのリストを得る．
inline
const vector<const BnNode*>&
BnNetwork::logic_list() const
{
  return mLogicList;
}

// @brief 関数のリストを得る．
inline
const vector<TvFunc>&
BnNetwork::func_list() const
{
  return mFuncList;
}

// @brief 論理式のリストを得る．
inline
const vector<Expr>&
BnNetwork::expr_list() const
{
  return mExprList;
}

END_NAMESPACE_YM_BNET

#endif // BNNETWORK_H
