#ifndef BNNETWORK_H
#define BNNETWORK_H

/// @file BnNetwork.h
/// @brief BnNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/ym_cell.h"
#include "ym/ym_logic.h"
#include "ym/HashMap.h"


BEGIN_NAMESPACE_YM_BNET

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

  /// @brief BnBuilder からのコンストラクタ
  /// @param[in] src コピー元のBnBuilder
  explicit
  BnNetwork(const BnBuilder& src);

  /// @brief デストラクタ
  ~BnNetwork();


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定する関数
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

  /// @brief 内容をコピーする．
  /// @param[in] src コピー元のBnBuilder
  void
  copy(const BnBuilder& src);

  /// @brief ネットワーク名を設定する．
  /// @param[in] name ネットワーク名
  void
  set_name(const string& name);

  /// @brief 外部入力ノードを追加する．
  /// @param[in] node_name ノード名
  /// @return 生成した入力ノードを返す．
  ///
  /// ノード名の重複に関しては感知しない．
  BnNode*
  new_input(const string& node_name);

  /// @brief 外部出力ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] inode_id 入力のノード番号
  /// @return 生成した出力ノードを返す．
  ///
  /// ノード名の重複に関しては感知しない．
  BnNode*
  new_output(const string& node_name,
	     ymuint inode_id);

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_list ファンインのノード番号のリスト
  /// @param[in] logic_type 論理型
  /// @param[in] cell セル
  /// @return 生成した論理ノードを返す．
  ///
  /// ノード名の重複に関しては感知しない．
  BnNode*
  new_primitive(const string& node_name,
		const vector<ymuint>& inode_id_list,
		BnLogicType logic_type,
		const Cell* cell = nullptr);

  /// @brief 論理式型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_list ファンインのノード番号のリスト
  /// @param[in] expr 論理式
  /// @param[in] cell セル
  /// @return 生成した論理ノードを返す．
  ///
  /// ノード名の重複に関しては感知しない．
  BnNode*
  new_expr(const string& node_name,
	   const vector<ymuint>& inode_id_list,
	   const Expr& expr,
	   const Cell* cell = nullptr);

  /// @brief 真理値表型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_list ファンインのノード番号のリスト
  /// @param[in] tv 真理値表
  /// @param[in] cell セル
  /// @return 生成した論理ノードを返す．
  ///
  /// ノード名の重複に関しては感知しない．
  BnNode*
  new_tv(const string& node_name,
	 const vector<ymuint>& inode_id_list,
	 const TvFunc& tv,
	 const Cell* cell = nullptr);

  /// @brief ポートを追加する．
  /// @param[in] port_name ポート名
  /// @param[in] bits 内容のノード番号のリスト
  void
  new_port(const string& port_name,
	   const vector<ymuint>& bits);

  /// @brief ポートを追加する(1ビット版)．
  /// @param[in] port_name ポート名
  /// @param[in] bit 内容のノード番号
  void
  new_port(const string& port_name,
	   ymuint bit);

  /// @brief DFFを追加する．
  /// @param[in] name DFF名
  /// @param[in] input 入力端子のノード番号
  /// @param[in] output 出力端子のノード番号
  /// @param[in] clock クロック端子のノード番号
  /// @param[in] clear クリア端子のノード番号
  /// @param[in] preset プリセット端子のノード番号
  /// @return 生成したDFFを返す．
  ///
  /// 名前の重複に関しては感知しない．
  BnDff*
  new_dff(const string& name,
	  ymuint input,
	  ymuint output,
	  ymuint clock,
	  ymuint clear = kBnNullId,
	  ymuint preset = kBnNullId);

  /// @brief ラッチを追加する．
  /// @param[in] name ラッチ名
  /// @param[in] input 入力端子のノード番号
  /// @param[in] output 出力端子のノード番号
  /// @param[in] enable イネーブル端子のノード番号
  /// @param[in] clear クリア端子のノード番号
  /// @param[in] preset プリセット端子のノード番号
  /// @return 生成したラッチを返す．
  ///
  /// 名前の重複に関しては感知しない．
  BnLatch*
  new_latch(const string& name,
	    ymuint input,
	    ymuint output,
	    ymuint enable,
	    ymuint clear = kBnNullId,
	    ymuint preset = kBnNullId);


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワーク名を得る．
  string
  name() const;

  /// @brief ポート数を得る．
  ymuint
  port_num() const;

  /// @brief ポートの情報を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
  const BnPort*
  port(ymuint pos) const;

  /// @brief DFF数を得る．
  ymuint
  dff_num() const;

  /// @brief DFFを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
  const BnDff*
  dff(ymuint pos) const;

  /// @brief ラッチ数を得る．
  ymuint
  latch_num() const;

  /// @brief ラッチを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < latch_num() )
  const BnLatch*
  latch(ymuint pos) const;

  /// @brief ノード数を得る．
  ymuint
  node_num() const;

  /// @brief ノードを得る．
  /// @param[in] id ノード番号 ( 0 <= id < node_num() )
  ///
  /// BnNode* node = BnNetwork::node(id);
  /// node->id() == id が成り立つ．
  const BnNode*
  node(ymuint id) const;

  /// @brief 入力数を得る．
  ymuint
  input_num() const;

  /// @brief 入力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
  const BnNode*
  input(ymuint pos) const;

  /// @brief 出力数を得る．
  ymuint
  output_num() const;

  /// @brief 出力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  const BnNode*
  output(ymuint pos) const;

  /// @brief 論理ノード数を得る．
  ymuint
  logic_num() const;

  /// @brief 論理ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
  const BnNode*
  logic(ymuint pos) const;

  /// @brief 関数の数を得る．
  ymuint
  func_num() const;

  /// @brief 関数番号から関数を得る．
  /// @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
  const TvFunc&
  func(ymuint func_id) const;

  /// @brief 論理式の数を得る．
  ymuint
  expr_num() const;

  /// @brief 論理式番号から論理式を得る．
  /// @param[in] expr_id 論理式番号 ( 0 <= expr_id < expr_num() )
  Expr
  expr(ymuint expr_id) const;

  /// @brief 内容を出力する．
  /// @param[in] s 出力先のストリーム
  ///
  /// 形式は独自フォーマット
  void
  write(ostream& s) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 論理式を登録する．
  /// @param[in] expr 論理式
  /// @return 関数番号を返す．
  ymuint
  _add_expr(const Expr& expr);

  /// @brief 真理値表を登録する．
  /// @param[in] tv 真理値表
  /// @return 関数番号を返す．
  ymuint
  _add_tv(const TvFunc& tv);

#if 0
  /// @brief 関数の数を設定する．
  /// @param[in] func_num 関数の数
  void
  set_func_num(ymuint func_num);

  /// @brief 関数を設定する．
  /// @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
  /// @param[in] tv 設定する関数
  void
  set_func(ymuint func_id,
	   const TvFunc& tv);

  /// @brief 論理式の数を設定する．
  /// @param[in] expr_num 論理式の数
  void
  set_expr_num(ymuint expr_num);

  /// @brief 論理式を設定する．
  /// @param[in] expr_id 論理式番号 ( 0 <= expr_id < expr_num() )
  /// @param[in] expr 設定する論理式
  void
  set_expr(ymuint expr_id,
	   const Expr& expr);
#endif

private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク名
  string mName;

  // ポート情報のリスト
  vector<BnPort*> mPortList;

  // DFFのリスト
  vector<BnDff*> mDffList;

  // ラッチのリスト
  vector<BnLatch*> mLatchList;

  // 入力ノードのリスト
  vector<BnNode*> mInputList;

  // 出力ノードのリスト
  vector<BnNode*> mOutputList;

  // 論理ノードのリスト
  vector<BnNode*> mLogicList;

  // ノード番号をキーにしてノードを納めた配列
  vector<BnNode*> mNodeList;

  // 関数のリスト
  vector<TvFunc> mFuncList;

  // TvFunc をキーにして関数番号を入れるハッシュ表
  HashMap<TvFunc, ymuint> mFuncMap;

  // 論理式のリスト
  vector<Expr> mExprList;

  // TvFunc をキーにして論理式番号を入れるハッシュ表
  HashMap<TvFunc, ymuint> mExprMap;

};

END_NAMESPACE_YM_BNET

#endif // BNNETWORK_H
