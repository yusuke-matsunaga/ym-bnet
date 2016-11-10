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
  /// @param[in] input 入力端子のノード番号
  /// @param[in] output 出力端子のノード番号
  /// @param[in] clock クロック端子のノード番号
  /// @param[in] clear クリア端子のノード番号
  /// @param[in] preset プリセット端子のノード番号
  BnDffImpl(ymuint id,
	    const string& name,
	    ymuint input,
	    ymuint output,
	    ymuint clock,
	    ymuint clear,
	    ymuint preset);

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

  /// @brief データ出力のノード番号を返す．
  virtual
  ymuint
  output() const;

  /// @brief データ入力のノード番号を返す．
  virtual
  ymuint
  input() const;

  /// @brief クロックのノード番号を返す．
  virtual
  ymuint
  clock() const;

  /// @brief クリア信号のノード番号を返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  ymuint
  clear() const;

  /// @brief プリセット信号のノードを返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  ymuint
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
  ymuint mOutput;

  // データ入力ノード
  ymuint mInput;

  // クロックノード
  ymuint mClock;

  // クリア信号ノード
  ymuint mClear;

  // プリセット信号ノード
  ymuint mPreset;

};

END_NAMESPACE_YM_BNET

#endif // BNDFFIMPL_H
