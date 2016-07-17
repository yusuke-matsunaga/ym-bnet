#ifndef BNDFFIMPL_H
#define BNDFFIMPL_H

/// @file BnDffImpl.h
/// @brief BnDffImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnDff.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnDffImpl BnDffImpl.h "BnDffImpl.h"
/// @brief BnDff の実装クラス
//////////////////////////////////////////////////////////////////////
class BnDffImpl :
  public BnDff
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] name 名前
  /// @param[in] input 入力端子のノード
  /// @param[in] output 出力端子のノード
  /// @param[in] clock クロック端子のノード
  /// @param[in] clear クリア端子のノード
  /// @param[in] preset プリセット端子のノード
  BnDffImpl(ymuint id,
	    const string& name,
	    BnNode* input,
	    BnNode* output,
	    BnNode* clock,
	    BnNode* clear,
	    BnNode* preset);

  /// @brief デストラクタ
  virtual
  ~BnDffImpl();


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

  /// @brief クロックのノードを返す．
  virtual
  const BnNode*
  clock() const;

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

  // クロックノード
  BnNode* mClock;

  // クリア信号ノード
  BnNode* mClear;

  // プリセット信号ノード
  BnNode* mPreset;

};

END_NAMESPACE_YM_BNET

#endif // BNDFFIMPL_H
