#ifndef BNBUILDER_H
#define BNBUILDER_H

/// @file BnBuilder.h
/// @brief BnBuilder のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/ym_cell.h"
#include "ym/ym_logic.h"
#include "ym/BnNode.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnBuilder BnBuilder.h "BnBuilder.h"
/// @brief BnNetwork のビルダークラス
//////////////////////////////////////////////////////////////////////
class BnBuilder
{
public:
  //////////////////////////////////////////////////////////////////////
  // 各要素の情報を表すデータ構造
  //////////////////////////////////////////////////////////////////////

  // ポートの情報
  struct PortInfo {

    // コンストラクタ
    PortInfo(const string& name,
	     const vector<ymuint>& bits) :
      mName(name),
      mBits(bits)
    {
    }

    // 1ビット用のコンストラクタ
    PortInfo(const string& name,
	     ymuint bit) :
      mName(name),
      mBits(1, bit)
    {
    }

    // 名前
    string mName;

    // ビットの内容
    // ノード番号を持つ．
    vector<ymuint> mBits;
  };

  // DFFの情報
  struct DffInfo {

    // コンストラクタ
    DffInfo(const string& name = string()) :
      mName(name),
      mInput(0),
      mOutput(0),
      mClock(0),
      mClear(0),
      mPreset(0)
    {
    }

    // 名前
    string mName;

    // 入力のノード番号
    ymuint mInput;

    // 出力のノード番号
    ymuint mOutput;

    // クロック端子のノード番号
    ymuint mClock;

    // クリア端子のノード番号
    ymuint mClear;

    // プリセット端子のノード番号
    ymuint mPreset;

  };

  // ラッチの情報
  struct LatchInfo {

    // コンストラクタ
    LatchInfo(const string& name = string()) :
      mName(name),
      mInput(0),
      mOutput(0),
      mEnable(0),
      mClear(0),
      mPreset(0)
    {
    }

    // 名前
    string mName;

    // 入力のノード番号
    ymuint mInput;

    // 出力のノード番号
    ymuint mOutput;

    // イネーブル端子のノード番号
    ymuint mEnable;

    // クリア端子のノード番号
    ymuint mClear;

    // プリセット端子のノード番号
    ymuint mPreset;

  };

  // ノードの情報
  struct NodeInfo {

    // 空のコンストラクタ
    NodeInfo() :
      mType(BnNode::kInput)
    {
    }

    // 入力用のコンストラクタ
    NodeInfo(const string& name) :
      mType(BnNode::kInput),
      mName(name)
    {
    }

    // 出力用のコンストラクタ
    NodeInfo(const string& name,
	     ymuint input) :
      mType(BnNode::kOutput),
      mName(name),
      mFaninList(1, input)
    {
    }

    // プリミティブ型の論理ノード用のコンストラクタ
    NodeInfo(const string& name,
	     BnLogicType logic_type,
	     ymuint ni) :
      mType(BnNode::kLogic),
      mName(name),
      mLogicType(logic_type),
      mFaninList(ni, 0),
      mCell(nullptr)
    {
    }

    // 論理式型の論理ノード用のコンストラクタ
    NodeInfo(const string& name,
	     const Expr& expr,
	     ymuint ni) :
      mType(BnNode::kLogic),
      mName(name),
      mFaninList(ni, 0),
      mLogicType(kBnLt_EXPR),
      mExpr(expr),
      mCell(nullptr)
    {
    }

    // セル型の論理ノード用のコンストラクタ
    NodeInfo(const string& name,
	     const Cell* cell,
	     ymuint ni) :
      mType(BnNode::kLogic),
      mName(name),
      mFaninList(ni, 0),
      mLogicType(kBnLt_NONE),
      mCell(cell)
    {
    }

    // 真理値表型の論理ノード用のコンストラクタ
    NodeInfo(const string& name,
	     const TvFunc& tv) :
      mType(BnNode::kLogic),
      mName(name),
      mFaninList(tv.input_num(), 0),
      mLogicType(kBnLt_TV),
      mTv(tv),
      mCell(nullptr)
    {
    }

    // ID番号
    ymuint mId;

    // タイプ
    BnNode::Type mType;

    // 名前
    string mName;

    // ファンインのノード番号の配列
    vector<ymuint> mFaninList;

    // ファンアウトのノード番号の配列
    vector<ymuint> mFanoutList;

    // 論理型
    BnLogicType mLogicType;

    // 論理式
    Expr mExpr;

    // 真理値表
    TvFunc mTv;

    // セル
    const Cell* mCell;

  };


public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  BnBuilder();

  /// @brief デストラクタ
  virtual
  ~BnBuilder();


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を得る．
  string
  model_name() const;

  /// @brief ポート数を得る．
  ymuint
  port_num() const;

  /// @brief ポート情報を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
  const PortInfo&
  port(ymuint pos) const;

  /// @brief DFF数を得る．
  ymuint
  dff_num() const;

  /// @brief DFF情報を得る．
  /// @param[in] id DFF番号 ( 0 <= id < dff_num() )
  const DffInfo&
  dff(ymuint id) const;

  /// @brief ラッチ数を得る．
  ymuint
  latch_num() const;

  /// @brief ラッチ情報を得る．
  /// @param[in] id ラッチ番号 ( 0 <= id < latch_num() )
  const LatchInfo&
  latch(ymuint id) const;

  /// @brief ノード数を得る．
  ymuint
  node_num() const;

  /// @brief ノード情報を得る．
  /// @param[in] id ノード番号 ( 0 < id <= node_num() )
  ///
  /// ノード番号 0 は不正な値として予約されている．
  const NodeInfo&
  node(ymuint id) const;

  /// @brief 入力ノード数を得る．
  ymuint
  input_num() const;

  /// @brief 入力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
  const NodeInfo&
  input(ymuint pos) const;

  /// @brief 出力ノード数を得る．
  ymuint
  output_num() const;

  /// @brief 出力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  const NodeInfo&
  output(ymuint pos) const;

  /// @brief 論理ノード数を得る．
  ymuint
  logic_num() const;

  /// @brief 論理ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
  ///
  /// 入力からのトポロジカル順に整列している．
  const NodeInfo&
  logic(ymuint pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // ファイル出力
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を書き出す．
  /// @param[in] s 出力先のストリーム
  void
  write(ostream& s) const;


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

  /// @brief ポート情報を追加する．
  /// @param[in] name ポート名
  /// @param[in] bits ビットの内容(ノード番号)
  void
  add_port(const string& name,
	   const vector<ymuint>& bits);

  /// @brief ポート情報を追加する(1ビット版)．
  /// @param[in] name ポート名
  /// @param[in] bit ビットの内容(ノード番号)
  void
  add_port(const string& name,
	   ymuint bit);

  /// @brief DFF情報を追加する．
  /// @param[in] name DFF名
  /// @return DFF情報を返す．
  DffInfo&
  add_dff(const string& name);

  /// @brief ラッチ情報を追加する．
  /// @param[in] name ラッチ名
  /// @return ラッチ情報を返す．
  LatchInfo&
  add_latch(const string& name);

  /// @brief 入力用のノード情報を追加する．
  /// @param[in] name ノード名
  /// @return ノード番号を返す．
  ymuint
  add_input(const string& name);

  /// @brief 出力用のノード情報を追加する．
  /// @param[in] name ノード名
  /// @param[in] input 入力のノード番号
  /// @return ノード番号を返す．
  ymuint
  add_output(const string& name,
	     ymuint input = 0);

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @param[in] name ノード名
  /// @param[in] logic_type 論理型
  /// @praam[in] ni ファンイン数
  /// @return ノード番号を返す．
  ymuint
  add_primitive(const string& name,
		BnLogicType logic_type,
		ymuint ni);

  /// @brief 論理式型の論理ノードを追加する．
  /// @param[in] name ノード名
  /// @param[in] expr 論理式
  /// @param[in] ni ファンイン数
  /// @return ノード番号を返す．
  ymuint
  add_expr(const string& name,
	   const Expr& expr,
	   ymuint ni);

  /// @brief セル型の論理ノードを追加する．
  /// @param[in] name ノード名
  /// @param[in] cell セル
  /// @return ノード番号を返す．
  ymuint
  add_cell(const string& name,
	   const Cell* cell);

  /// @brief 真理値表型の論理ノードを追加する．
  /// @param[in] name ノード名
  /// @param[in] tv 真理値表
  /// @return ノード番号を返す．
  ymuint
  add_tv(const string& name,
	 const TvFunc& tv);

  /// @brief ノード間を接続する．
  /// @param[in] src_node ファンアウト元のノード番号
  /// @param[in] dst_node ファンイン先のノード番号
  /// @param[in] ipos ファンインの位置
  void
  connect(ymuint src_node,
	  ymuint dst_node,
	  ymuint ipos);

  /// @brief 整合性のチェックを行う．
  /// @return チェック結果を返す．
  ///
  /// チェック項目は以下の通り
  /// - model_name() が設定されているか？
  ///   設定されていない場合にはデフォルト値を設定する．
  ///   エラーとはならない．
  /// - 各ポートの各ビットが設定されているか？
  /// - 各DFFの入力，出力およびクロックが設定されているか？
  /// - 各ラッチの入力，出力およびイネーブルが設定されているか？
  /// - 各ノードのファンインが設定されているか？
  ///
  /// この関数を呼んだあとは論理ノードがトポロジカルソートされる．
  bool
  wrap_up();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード情報を追加する．
  /// @param[in] node_info ノード情報
  /// @return ノード番号を返す．
  ymuint
  _add_node(const NodeInfo& node_info);

  /// @brief ノード情報を得る．
  /// @param[in] id ノード番号 ( 0 < id <= node_num() )
  ///
  /// ノード番号 0 は不正な値として予約されている．
  NodeInfo&
  _node(ymuint id);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 名前
  string mName;

  // ポート情報のリスト
  vector<PortInfo> mPortInfoList;

  // DFF情報のリスト
  vector<DffInfo> mDffInfoList;

  // ラッチ情報のリスト
  vector<LatchInfo> mLatchInfoList;

  // ノード情報のリスト
  vector<NodeInfo> mNodeInfoList;

  // 入力ノード番号のリスト
  vector<ymuint> mInputList;

  // 出力ノード番号のリスト
  vector<ymuint> mOutputList;

  // 論理ノード番号のリスト
  vector<ymuint> mLogicList;

  // 内容が正常かどうかを示すフラグ
  bool mSane;

};

END_NAMESPACE_YM_BNET

#endif // BNBUILDER_H
