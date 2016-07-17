#ifndef BNLATCHIMPL_H
#define BNLATCHIMPL_H

/// @file BnLatchImpl.h
/// @brief BnLatchImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnLatch.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnLatchImpl BnLatchImpl.h "BnLatchImpl.h"
/// @brief BnLatch の実装クラス
//////////////////////////////////////////////////////////////////////
class BnLatchImpl :
  public BnLatch
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] name 名前
  /// @param[in] input 入力端子のノード
  /// @param[in] output 出力端子のノード
  /// @param[in] enable イネーブル端子のノード
  /// @param[in] clear クリア端子のノード
  /// @param[in] preset プリセット端子のノード
  BnLatchImpl(ymuint id,
	      const string& name,
	      BnNode* input,
	      BnNode* output,
	      BnNode* enable,
	      BnNode* clear,
	      BnNode* preset);

  /// @brief デストラクタ
  virtual
  ~BnLatchImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID 番号の取得
  /// @return ID 番号を返す．
  virtual
  ymuint
  id() const;

  /// @brief 名前を返す．
  virtual
  string
  name() const;

  /// @brief データ出力ノードを返す．
  virtual
  const BnNode*
  output() const;

  /// @brief データ入力ノードを返す．
  virtual
  const BnNode*
  input() const;

  /// @brief イネーブルのノードを返す．
  virtual
  const BnNode*
  enable() const;

  /// @brief クリア信号のノードを返す．
  /// @note nullptr の場合もある．
  virtual
  const BnNode*
  clear() const;

  /// @brief プリセット信号のノードを返す．
  /// @note nullptr の場合もある．
  virtual
  const BnNode*
  preset() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  ymuint32 mId;

  // 名前
  string mName;

  // データ出力ノード
  BnNode* mOutput;

  // データ入力ノード
  BnNode* mInput;

  // イネーブルノード
  BnNode* mEnable;

  // クリア信号ノード
  BnNode* mClear;

  // プリセット信号ノード
  BnNode* mPreset;

};

END_NAMESPACE_YM_BNET

#endif // BNLATCHIMPL_H
