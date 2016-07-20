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
/// - D-FFノード(BnDff)
///   入力と出力およびクロックの BnNode を持つ．
///   オプションとしてセット，リセット端子のBnNodeを持つ．
/// - ラッチノード(BnLatch)
///   入力と出力およびイネーブル端子の BnNode を持つ．
///   オプションとしてセット，リセット端子のBnNodeを持つ．
///
/// 回路全体の入出力インターフェイスとしてポートを持つ．
/// ポートは複数ビットをひとまとめにしたもので名前を持つ．
/// - ポートの名前空間はノードとは別に設ける．
/// - ポートは入力/出力ノードのベクタを内容として持つ．
/// 通常の blif ファイルや .bench(iscas89) ファイルを読んだ場合，ポートは1つのノードに対応する．
///
/// このクラスはファイル入出力用のモデルであり，このクラス上で
/// 回路変換などの処理を行うことは考えていない．
//////////////////////////////////////////////////////////////////////
class BnNetwork
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 空の状態で初期化される．
  BnNetwork();

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のオブジェクト
  BnNetwork(const BnNetwork& src);

  /// @brief ビルダーを引数にとるコンストラクタ
  /// @param[in] builder ビルダーオブジェクト
  BnNetwork(const BnBuilder& builder);

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

  /// @brief ビルダーオブジェクトからの生成
  /// @param[in] builder ビルダーオブジェクト
  void
  copy(const BnBuilder& builder);


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワーク名を得る．
  string
  model_name() const;

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
  node(ymuint pos) const;

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

  /// @brief 関数番号から論理式を得る．
  /// @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
  Expr
  expr(ymuint func_id) const;

  /// @brief 内容を出力する．
  /// @param[in] s 出力先のストリーム
  ///
  /// 形式は独自フォーマット
  void
  write(ostream& s) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワーク名を設定する．
  /// @param[in] name ネットワーク名
  void
  set_model_name(const string& name);

  /// @brief 外部入力ノードを追加する．
  /// @param[in] node_name ノード名
  /// @return 生成した入力ノードを返す．
  ///
  /// ノード名の重複に関しては感知しない．
  BnNode*
  new_input(const string& node_name);

  /// @brief 外部出力ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] inode 入力のノード
  /// @return 生成した出力ノードを返す．
  ///
  /// ノード名の重複に関しては感知しない．
  BnNode*
  new_output(const string& node_name,
	     BnNode* inode);

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] inode_list ファンインのノードのリスト
  /// @param[in] prim_type プリミティブの型
  /// @return 生成した論理ノードを返す．
  ///
  /// ノード名の重複に関しては感知しない．
  BnNode*
  new_primitive(const string& node_name,
		const vector<BnNode*>& inode_list,
		BnLogicType prim_type);

  /// @brief セル型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] inode_list ファンインのノード番号のリスト
  /// @param[in] cell セル
  /// @return 生成した論理ノードを返す．
  ///
  /// ノード名の重複に関しては感知しない．
  BnNode*
  new_cell(const string& node_name,
	   const vector<BnNode*>& inode_list,
	   const Cell* cell);

  /// @brief 論理式型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] inode_list ファンインのノード番号のリスト
  /// @param[in] expr 論理式
  /// @return 生成した論理ノードを返す．
  ///
  /// ノード名の重複に関しては感知しない．
  BnNode*
  new_expr(const string& node_name,
	   const vector<BnNode*>& inode_list,
	   const Expr& expr);

  /// @brief 真理値表型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] inode_list ファンインのノード番号のリスト
  /// @param[in] tv_func 心理値表
  /// @return 生成した論理ノードを返す．
  ///
  /// ノード名の重複に関しては感知しない．
  BnNode*
  new_tv(const string& node_name,
	 const vector<BnNode*>& inode_list,
	 const TvFunc& tv);

  /// @brief ポートを追加する．
  /// @param[in] port_name ポート名
  /// @param[in] bits 内容のノードのリスト
  void
  new_port(const string& port_name,
	   const vector<BnNode*>& bits);

  /// @brief ポートを追加する(1ビット版)．
  /// @param[in] port_name ポート名
  /// @param[in] bit 内容のノード
  void
  new_port(const string& port_name,
	   BnNode* bit);

  /// @brief DFFを追加する．
  /// @param[in] name DFF名
  /// @param[in] input 入力端子のノード
  /// @param[in] output 出力端子のノード
  /// @param[in] clock クロック端子のノード
  /// @param[in] clear クリア端子のノード
  /// @param[in] preset プリセット端子のノード
  /// @return 生成したDFFを返す．
  ///
  /// 名前の重複に関しては感知しない．
  BnDff*
  new_dff(const string& name,
	  BnNode* input,
	  BnNode* output,
	  BnNode* clock,
	  BnNode* clear = nullptr,
	  BnNode* preset = nullptr);

  /// @brief ラッチを追加する．
  /// @param[in] name ラッチ名
  /// @param[in] input 入力端子のノード
  /// @param[in] output 出力端子のノード
  /// @param[in] enable イネーブル端子のノード
  /// @param[in] clear クリア端子のノード
  /// @param[in] preset プリセット端子のノード
  /// @return 生成したラッチを返す．
  ///
  /// 名前の重複に関しては感知しない．
  BnLatch*
  new_latch(const string& name,
	    BnNode* input,
	    BnNode* output,
	    BnNode* enable,
	    BnNode* clear = nullptr,
	    BnNode* preset = nullptr);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 論理式を解析する．
  /// @param[in] expr 対象の論理式
  /// @param[out] func_id 関数番号
  /// @return 論理タイプ
  BnLogicType
  analyze_expr(const Expr& expr,
	       ymuint& func_id);

  /// @brief 関数を解析する．
  /// @param[in] func 対象の関数
  /// @param[out] func_id 関数番号
  /// @return 論理タイプ
  ///
  /// func がプリミティブ型の場合，
  /// プリミティブ型の値を返す．
  /// それ以外の場合，既に同じ関数が登録されていないか調べ，
  /// 関数番号を func_id に設定する．
  /// この場合は kBnLt_TV を返す．
  BnLogicType
  analyze_func(const TvFunc& func,
	       ymuint& func_id);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク名
  string mNetworkName;

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

  // 真理値表をキーにして関数番号を記録するハッシュ表
  HashMap<TvFunc, ymuint> mFuncMap;

  // 関数のリスト
  // mFuncMap に対応する．
  vector<TvFunc> mFuncList;

  // 関数番号をキーにした論理式のハッシュ表
  // mFuncMap に対応する．
  // すべての関数が式を持つとは限らないのでハッシュを用いる．
  HashMap<ymuint, Expr> mExprMap;

};

END_NAMESPACE_YM_BNET

#endif // BNNETWORK_H
