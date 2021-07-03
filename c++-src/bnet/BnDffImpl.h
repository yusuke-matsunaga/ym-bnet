#ifndef BNDFFIMPL_H
#define BNDFFIMPL_H

/// @file BnDffImpl.h
/// @brief BnDffImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2021 Yusuke Matsunaga
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
  BnDffImpl(
    int id,             ///< [in] ID番号
    const string& name, ///< [in] 名前
    int input,          ///< [in] 入力端子のノード番号
    int output,         ///< [in] 出力端子のノード番号
    int xoutput,        ///< [in] 反転出力端子のノード番号
    int clock,          ///< [in] クロック端子のノード番号
    int clear,          ///< [in] クリア端子のノード番号
    int preset,         ///< [in] プリセット端子のノード番号
    int cell_id         ///< [in] セル番号
  ) : mId(id),
    mName(name),
    mInput(input),
    mOutput(output),
    mXOutput(xoutput),
    mClock(clock),
    mClear(clear),
    mPreset(preset),
    mCellId(cell_id)
  {
  }

  /// @brief デストラクタ
  ~BnDffImpl() = default;


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

  /// @brief 反転データ出力のノード番号を返す．
  virtual
  int
  xoutput() const;

  /// @brief データ入力のノード番号を返す．
  virtual
  int
  input() const;

  /// @brief クロックのノード番号を返す．
  virtual
  int
  clock() const;

  /// @brief クリア信号のノード番号を返す．
  ///
  /// kBnNullId の場合もある．
  virtual
  int
  clear() const;

  /// @brief プリセット信号のノードを返す．
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

  // 反転データ出力ノード
  int mXOutput;

  // データ入力ノード
  int mInput;

  // クロックノード
  int mClock;

  // クリア信号ノード
  int mClear;

  // プリセット信号ノード
  int mPreset;

  // セル番号
  int mCellId;

};

END_NAMESPACE_YM_BNET

#endif // BNDFFIMPL_H
