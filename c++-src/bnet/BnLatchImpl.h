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
  /// @param[in] input 入力端子のノード番号
  /// @param[in] output 出力端子のノード番号
  /// @param[in] xoutput 反転出力端子のノード番号
  /// @param[in] enable イネーブル端子のノード番号
  /// @param[in] clear クリア端子のノード番号
  /// @param[in] preset プリセット端子のノード番号
  /// @param[in] cell_id セル番号
  BnLatchImpl(int id,
	      const string& name,
	      int input,
	      int output,
	      int xoutput,
	      int enable,
	      int clear,
	      int preset,
	      int cell_id);

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
  int
  id() const;

  /// @brief 名前を返す．
  virtual
  string
  name() const;

  /// @brief データ出力のノード番号を返す．
  virtual
  int
  output() const;

  /// @brief データ反転出力のノード番号を返す．
  virtual
  int
  xoutput() const;

  /// @brief データ入力のノード番号を返す．
  virtual
  int
  input() const;

  /// @brief イネーブルのノード番号を返す．
  virtual
  int
  enable() const;

  /// @brief クリア信号のノード番号を返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  int
  clear() const;

  /// @brief プリセット信号のノード番号を返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  int
  preset() const;

  /// @brief セル番号を返す．
  ///
  /// -1 の場合もある．
  virtual
  int
  cell_id() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  int mId;

  // 名前
  string mName;

  // データ出力ノード
  int mOutput;

  // データ反転出力ノード
  int mXoutput;

  // データ入力ノード
  int mInput;

  // イネーブルノード
  int mEnable;

  // クリア信号ノード
  int mClear;

  // プリセット信号ノード
  int mPreset;

  // セル番号
  int mCellId;

};

END_NAMESPACE_YM_BNET

#endif // BNLATCHIMPL_H
