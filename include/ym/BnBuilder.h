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

    // 名前
    string mName;

    // ビットの内容
    vector<ymuint> mBits;
  };

  // DFFの情報
  struct DffInfo {

    // コンストラクタ
    DffInfo(const string& name) :
      mName(name),
      mClear(-1),
      mPreset(-1)
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
    ymint mClear;

    // プリセット端子のノード番号
    ymint mPreset;

  };

  // ラッチの情報
  struct LatchInfo {

    // コンストラクタ
    LatchInfo(const string& name) :
      mName(name),
      mClear(-1),
      mPreset(-1)
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
    ymint mClear;

    // プリセット端子のノード番号
    ymint mPreset;

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
      mInodeList(1, input)
    {
    }

    // プリミティブ型の論理ノード用のコンストラクタ
    NodeInfo(const string& name,
	     ymuint ni,
	     BnLogicType logic_type) :
      mType(BnNode::kLogic),
      mName(name),
      mLogicType(logic_type),
      mInodeList(ni),
      mCell(nullptr)
    {
    }

    // 論理式型の論理ノード用のコンストラクタ
    NodeInfo(const string& name,
	     ymuint ni,
	     const Expr& expr) :
      mType(BnNode::kLogic),
      mName(name),
      mInodeList(ni),
      mLogicType(kBnLt_EXPR),
      mExpr(expr),
      mCell(nullptr)
    {
    }

    // セル型の論理ノード用のコンストラクタ
    NodeInfo(const string& name,
	     ymuint ni,
	     const Cell* cell) :
      mType(BnNode::kLogic),
      mName(name),
      mInodeList(ni),
      mLogicType(kBnLt_NONE),
      mCell(cell)
    {
    }

    // 真理値表型の論理ノード用のコンストラクタ
    NodeInfo(const string& name,
	     const TvFunc& tv) :
      mType(BnNode::kLogic),
      mName(name),
      mInodeList(tv.input_num()),
      mLogicType(kBnLt_TV),
      mTv(tv),
      mCell(nullptr)
    {
    }

    // タイプ
    BnNode::Type mType;

    // 名前
    string mName;

    // ファンインのノード番号の配列
    vector<ymuint> mInodeList;

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

  /// @brief コンストラクタ
  BnBuilder();

  /// @brief デストラクタ
  ~BnBuilder();


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
  // ファイル出力
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を書き出す．
  /// @param[in] s 出力先のストリーム
  void
  write(ostream& s) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定する外部インターフェイス
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
  add_port(const string& port_name,
	   const vector<ymuint>& bits);

  /// @brief ポートを追加する(1ビット版)．
  /// @param[in] port_name ポート名
  /// @param[in] bit 内容のノード番号
  void
  add_port(const string& port_name,
	   ymuint bit);

  /// @brief DFFを追加する．
  /// @param[in] name DFF名
  ymuint
  add_dff(const string& name);

  /// @brief DFFのデータ入力を設定する．
  /// @param[in] dff_id DFF番号
  /// @param[in] input 入力のノード番号
  void
  set_dff_input(ymuint dff_id,
		ymuint input);

  /// @brief DFFのクロック端子を設定する．
  /// @param[in] dff_id DFF番号
  /// @param[in] clock クロック端子のノード番号
  void
  set_dff_clock(ymuint dff_id,
		ymuint clock);

  /// @brief DFFのクリア端子を設定する．
  /// @param[in] dff_id DFF番号
  /// @param[in] clear クリア端子のノード番号
  void
  set_dff_clear(ymuint dff_id,
		ymuint clear);

  /// @brief DFFのプリセット端子を設定する．
  /// @param[in] dff_id DFF番号
  /// @param[in] preset プリセット端子のノード番号
  void
  set_dff_preset(ymuint dff_id,
		 ymuint preset);

  /// @brief DFFの出力端子を設定する．
  /// @param[in] dff_id DFF番号
  /// @param[in] output 出力のノード番号
  void
  set_dff_output(ymuint dff_id,
		 ymuint output);

  /// @brief ラッチを追加する．
  /// @param[in] name ラッチ名
  /// @return ラッチ番号を返す．
  ymuint
  add_latch(const string& name);

  /// @brief ラッチのデータ入力を設定する．
  /// @param[in] latch_id ラッチ番号
  /// @param[in] input 入力端子のノード番号
  void
  set_latch_input(ymuint latch_id,
		  ymuint input);

  /// @brief ラッチのイネーブル端子を設定する．
  /// @param[in] latch_id ラッチ番号
  /// @param[in] enable イネーブル端子のノード番号
  void
  set_latch_enable(ymuint latch_id,
		   ymuint enable);

  /// @brief ラッチのクリア端子を設定する．
  /// @param[in] latch_id ラッチ番号
  /// @param[in] clear クリア端子のノード番号
  void
  set_latch_clear(ymuint latch_id,
		  ymuint clear);

  /// @brief ラッチのプリセット端子を設定する．
  /// @param[in] latch_id ラッチ番号
  /// @param[in] preset プリセット端子のノード番号
  void
  set_latch_preset(ymuint latch_id,
		   ymuint preset);

  /// @brief ラッチの出力を設定する．
  /// @param[in] latch_id ラッチ番号
  /// @param[in] output 出力端子のノード番号
  void
  set_latch_output(ymuint latch_id,
		   ymuint output);

  /// @brief 外部入力ノードを追加する．
  /// @param[in] node_name ノード名
  /// @return ノード番号を返す．
  ///
  /// ノード名の重複に関しては感知しない．
  ymuint
  add_input(const string& node_name);

  /// @brief 外部出力ノードを追加する．
  /// @param[in] node_name ノード名
  /// @return ノード番号を返す．
  ///
  /// ノード名の重複に関しては感知しない．
  ymuint
  add_output(const string& node_name);

  /// @brief 外部出力ノードの入力を設定する．
  /// @param[in] node_id ノード番号
  /// @param[in] input 入力のノード番号
  void
  set_output_input(ymuint node_id,
		   ymuint input);

  /// @brief プリミティブ型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni ファンイン数
  /// @param[in] prim_type プリミティブの型
  /// @return ノード番号を返す．
  ///
  /// ノード名の重複に関しては感知しない．
  ymuint
  add_primitive(const string& node_name,
		ymuint ni,
		BnLogicType prim_type);

  /// @brief セル型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] cell セル
  /// @return ノード番号を返す．
  ///
  /// ノード名の重複に関しては感知しない．
  ymuint
  add_cell(const string& node_name,
	   const Cell* cell);

  /// @brief 論理式型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] ni ファンイン数
  /// @param[in] expr 論理式
  /// @return ノード番号を返す．
  ///
  /// ノード名の重複に関しては感知しない．
  ymuint
  add_expr(const string& node_name,
	   ymuint ni,
	   const Expr& expr);

  /// @brief 真理値表型の論理ノードを追加する．
  /// @param[in] node_name ノード名
  /// @param[in] tv_func 真理値表
  /// @return ノード番号を返す．
  ///
  /// ノード名の重複に関しては感知しない．
  ymuint
  add_tv(const string& node_name,
	 const TvFunc& tv);

  /// @brief 論理ノードのファンインを設定する．
  /// @param[in] node_id ノード番号
  /// @param[in] pos ファンイン番号
  /// @param[in] input 入力ノードの番号
  void
  set_fanin(ymuint node_id,
	    ymuint pos,
	    ymuint input);


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を得る．
  string
  name() const;

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
  /// @param[in] id ノード番号 ( 0 <= id < node_num() )
  const NodeInfo&
  node(ymuint id) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


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

};

END_NAMESPACE_YM_BNET

#endif // BNBUILDER_H
