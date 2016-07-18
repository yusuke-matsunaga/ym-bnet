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
    DffInfo(const string& name) :
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
    LatchInfo(const string& name) :
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
      mInodeList(ni, 0),
      mCell(nullptr)
    {
    }

    // 論理式型の論理ノード用のコンストラクタ
    NodeInfo(const string& name,
	     ymuint ni,
	     const Expr& expr) :
      mType(BnNode::kLogic),
      mName(name),
      mInodeList(ni, 0),
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
      mInodeList(ni, 0),
      mLogicType(kBnLt_NONE),
      mCell(cell)
    {
    }

    // 真理値表型の論理ノード用のコンストラクタ
    NodeInfo(const string& name,
	     const TvFunc& tv) :
      mType(BnNode::kLogic),
      mName(name),
      mInodeList(tv.input_num(), 0),
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

  /// @brief デストラクタ
  virtual
  ~BnBuilder() { }


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
  // 内容を取得する外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を得る．
  virtual
  string
  name() const = 0;

  /// @brief ポート数を得る．
  virtual
  ymuint
  port_num() const = 0;

  /// @brief ポート情報を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
  virtual
  const PortInfo&
  port(ymuint pos) const = 0;

  /// @brief DFF数を得る．
  virtual
  ymuint
  dff_num() const = 0;

  /// @brief DFF情報を得る．
  /// @param[in] id DFF番号 ( 0 <= id < dff_num() )
  virtual
  const DffInfo&
  dff(ymuint id) const = 0;

  /// @brief ラッチ数を得る．
  virtual
  ymuint
  latch_num() const = 0;

  /// @brief ラッチ情報を得る．
  /// @param[in] id ラッチ番号 ( 0 <= id < latch_num() )
  virtual
  const LatchInfo&
  latch(ymuint id) const = 0;

  /// @brief ノード数を得る．
  virtual
  ymuint
  node_num() const = 0;

  /// @brief ノード情報を得る．
  /// @param[in] id ノード番号 ( 0 < id <= node_num() )
  ///
  /// ノード番号 0 は不正な値として予約されている．
  virtual
  const NodeInfo&
  node(ymuint id) const = 0;

};

END_NAMESPACE_YM_BNET

#endif // BNBUILDER_H
