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
  /// @param[in] enable イネーブル端子のノード番号
  /// @param[in] clear クリア端子のノード番号
  /// @param[in] preset プリセット端子のノード番号
  /// @param[in] cell セル
  BnLatchImpl(ymuint id,
	      const string& name,
	      ymuint input,
	      ymuint output,
	      ymuint enable,
	      ymuint clear,
	      ymuint preset,
	      const ClibCell* cell = nullptr);

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

  /// @brief データ出力のノード番号を返す．
  virtual
  ymuint
  output() const;

  /// @brief データ入力のノード番号を返す．
  virtual
  ymuint
  input() const;

  /// @brief イネーブルのノード番号を返す．
  virtual
  ymuint
  enable() const;

  /// @brief クリア信号のノード番号を返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  ymuint
  clear() const;

  /// @brief プリセット信号のノード番号を返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  ymuint
  preset() const;

  /// @brief セルを返す．
  ///
  /// nullptr の場合もある．
  virtual
  const ClibCell*
  cell() const;

  /// @brief データ出力のピン番号を返す．
  ///
  /// cell() == nullptr の場合の値は不定
  virtual
  ymuint
  output_pin_id() const;

  /// @brief データ入力のピン番号を返す．
  ///
  /// cell() == nullptr の場合の値は不定
  virtual
  ymuint
  input_pin_id() const;

  /// @brief イネーブル端子のピン番号を返す．
  ///
  /// cell() == nullptr の場合の値は不定
  virtual
  ymuint
  enable_pin_id() const;

  /// @brief クリア信号のピン番号を返す．
  ///
  /// kBnNullId の場合もある．
  /// cell() == nullptr の場合の値は不定
  virtual
  ymuint
  clear_pin_id() const;

  /// @brief プリセット信号のピン番号を返す．
  ///
  /// kBnNullId の場合もある．
  /// cell() == nullptr の場合の値は不定
  virtual
  ymuint
  preset_pin_id() const;


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

  // イネーブルノード
  ymuint mEnable;

  // クリア信号ノード
  ymuint mClear;

  // プリセット信号ノード
  ymuint mPreset;

  // セル
  const ClibCell* mCell;

};

END_NAMESPACE_YM_BNET

#endif // BNLATCHIMPL_H
