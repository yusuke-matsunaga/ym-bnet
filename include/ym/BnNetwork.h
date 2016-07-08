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
/// @brief BnNetwork を作るためのビルダークラス
/// @brief ブーリアンネットワークを表すクラス
/// @sa BnNode
/// @sa BnPort
///
/// ただし，このネットワークはDFFノードを含むので正確には
/// syncronous boolean network かもしれない．
/// 以下の種類のノードを持つ．
/// - 外部入力ノード
/// - 論理ノード
///   論理関数表現とファンインのノードを持つ．
/// - D-FFノード
///   リセット状態(値)と1つのファンインのノードを持つ．
/// すべてのノードは任意で名前を持つ．
/// 外部出力は上記いずれかのノードの出力を指す．
/// 独立のノードとしては存在しない．
///
/// 回路全体の入出力インターフェイスとしてポートを持つ．
/// ポートは複数ビットをひとまとめにしたもので名前を持つ．
/// - ポートの名前空間はノードとは別に設ける．
/// - 一つのポートに入力ノードと出力ノード(※詳細は上記参照)が混在しても良い．
/// 通常の blif ファイルや .bench(iscas89) ファイルを読んだ場合，ポートは1つのノードに対応する．
///
/// このクラスはファイル入出力用のモデルであり，このクラス上で
/// 回路変換などの処理を行うことは考えていない．
//////////////////////////////////////////////////////////////////////
class BnNetwork
{
public:

  /// @brief コンストラクタ
  ///
  /// 空の状態で初期化される．
  BnNetwork();

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

  /// @brief ネットワーク名を設定する．
  /// @param[in] name ネットワーク名
  void
  set_model_name(const string& name);

  /// @brief ポートを追加する．
  /// @param[in] port_name ポート名
  /// @param[in] bits 内容のノード番号のリスト
  void
  new_port(const string& port_name,
	   const vector<ymuint>& bits);

  /// @brief 外部入力ノードを追加する．
  /// @param[in] node_id ノードID番号
  /// @param[in] node_name ノード名
  /// @return 追加が成功したら true を返す．
  ///
  /// すでに同じノード番号が存在したら失敗する．
  /// ノード名の重複に関しては感知しない．
  bool
  new_input(ymuint node_id,
	    const string& node_name);

  /// @brief DFFノードを追加する．
  /// @param[in] node_id ノードID番号
  /// @param[in] node_name ノード名
  /// @param[in] inode_id 入力のノード番号
  /// @param[in] reset_val リセット値 ('0', '1', '-' のいずれか)
  /// @return 追加が成功したら true を返す．
  ///
  /// すでに同じノード番号が存在したら失敗する．
  /// ノード名の重複に関しては感知しない．
  bool
  new_dff(ymuint node_id,
	  const string& node_name,
	  ymuint inode_id,
	  char reset_val);

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @param[in] node_id ノードID番号
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_list ファンインのノード番号のリスト
  /// @param[in] prim_type プリミティブの型
  /// @return 追加が成功したら true を返す．
  ///
  /// すでに同じノード番号が存在したら失敗する．
  /// ノード名の重複に関しては感知しない．
  bool
  new_primitive(ymuint node_id,
		const string& node_name,
		const vector<ymuint>& inode_id_list,
		BnLogicType prim_type);

  /// @brief セル型の論理ノードを追加する．
  /// @param[in] node_id ノードID番号
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_list ファンインのノード番号のリスト
  /// @param[in] cell セル
  /// @return 追加が成功したら true を返す．
  ///
  /// すでに同じノード番号が存在したら失敗する．
  /// ノード名の重複に関しては感知しない．
  bool
  new_cell(ymuint node_id,
	   const string& node_name,
	   const vector<ymuint>& inode_id_list,
	   const Cell* cell);

  /// @brief 論理式型の論理ノードを追加する．
  /// @param[in] node_id ノードID番号
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_list ファンインのノード番号のリスト
  /// @param[in] expr 論理式
  /// @return 追加が成功したら true を返す．
  ///
  /// すでに同じノード番号が存在したら失敗する．
  /// ノード名の重複に関しては感知しない．
  bool
  new_expr(ymuint node_id,
	   const string& node_name,
	   const vector<ymuint>& inode_id_list,
	   const Expr& expr);

  /// @brief 真理値表型の論理ノードを追加する．
  /// @param[in] node_id ノードID番号
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_list ファンインのノード番号のリスト
  /// @param[in] tv_func 心理値表
  /// @return 追加が成功したら true を返す．
  ///
  /// すでに同じノード番号が存在したら失敗する．
  /// ノード名の重複に関しては感知しない．
  bool
  new_tv(ymuint node_id,
	 const string& node_name,
	 const vector<ymuint>& inode_id_list,
	 const TvFunc& tv);

  /// @brief 最終処理と整合性のチェックを行う．
  /// @retval true 問題なし
  /// @retval false エラーがあった．
  ///
  /// 検査項目は以下の通り．
  /// - model_name が設定されているか
  ///   設定されていなければデフォルト値を使う．
  ///   -> warning
  /// - 各ポートで参照しているノード番号が存在するか
  ///   存在しなければ error
  /// - 各ノードのファンインのノード番号が存在するか
  ///   存在しなければ error
  ///
  /// BnNode のファンアウト情報はここで設定される．
  bool
  wrap_up();


public:
  //////////////////////////////////////////////////////////////////////
  // ファイル入力
  //////////////////////////////////////////////////////////////////////

  /// @brief blif 形式のファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @param[in] cell_library セルライブラリ
  /// @retval true 正常に読み込めた
  /// @retval false 読み込み中にエラーが起こった．
  bool
  read_blif(const string& filename,
	    const CellLibrary* cell_library = nullptr);

  /// @brief iscas89(.bench) 形式のファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @retval true 正常に読み込めた
  /// @retval false 読み込み中にエラーが起こった．
  bool
  read_iscas89(const string& filename);



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

  /// @brief 入力数を得る．
  ymuint
  input_num() const;

  /// @brief 入力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
  const BnNode*
  input(ymuint pos) const;

  /// @brief DFF数を得る．
  ymuint
  dff_num() const;

  /// @brief DFFノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
  const BnNode*
  dff(ymuint pos) const;

  /// @brief 論理ノード数を得る．
  ymuint
  logic_num() const;

  /// @brief 論理ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
  const BnNode*
  logic(ymuint pos) const;

  /// @brief ノードを得る．
  /// @param[in] node_id ノード番号
  ///
  /// 該当するノードがない場合には nullptr を返す．
  const BnNode*
  find_node(ymuint node_id) const;

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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク名
  string mNetworkName;

  // ポート情報のリスト
  vector<BnPort*> mPortList;

  // 入力ノードのノード番号の情報のリスト
  vector<BnNode*> mInputList;

  // DFFノードのノード番号のリスト
  vector<BnNode*> mDffList;

  // 論理ノードのノード番号のリスト
  vector<BnNode*> mLogicList;

  // ノード番号をキーにしてノード情報を記録するハッシュ表
  HashMap<ymuint, BnNode*> mNodeMap;

  // 正しい状態の時に true となるフラグ
  bool mSane;

};

END_NAMESPACE_YM_BNET

#endif // BNNETWORK_H
