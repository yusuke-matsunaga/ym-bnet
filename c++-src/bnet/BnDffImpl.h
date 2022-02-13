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
    SizeType id,        ///< [in] ID番号
    const string& name, ///< [in] 名前
    SizeType input,     ///< [in] 入力端子のノード番号
    SizeType output,    ///< [in] 出力端子のノード番号
    SizeType clock,     ///< [in] クロック端子のノード番号
    SizeType clear,     ///< [in] クリア端子のノード番号
    SizeType preset,    ///< [in] プリセット端子のノード番号
    int cell_id         ///< [in] セル番号
  ) : mId{id},
      mName{name},
      mInput{input},
      mOutput{output},
      mClock{clock},
      mClear{clear},
      mPreset{preset},
      mCellId{cell_id}
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
  SizeType
  id() const override;

  /// @brief 名前を返す．
  string
  name() const override;

  /// @brief データ出力のノード番号を返す．
  SizeType
  output() const override;

  /// @brief データ入力のノード番号を返す．
  SizeType
  input() const override;

  /// @brief クロックのノード番号を返す．
  SizeType
  clock() const override;

  /// @brief クリア信号のノード番号を返す．
  ///
  /// BNET_NULLID の場合もある．
  SizeType
  clear() const override;

  /// @brief プリセット信号のノードを返す．
  ///
  /// BNET_NULLID の場合もある．
  SizeType
  preset() const override;

  /// @brief セル番号を返す．
  ///
  /// -1 の場合もある．
  int
  cell_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId;

  // 名前
  string mName;

  // データ出力ノード
  SizeType mOutput;

  // データ入力ノード
  SizeType mInput;

  // クロックノード
  SizeType mClock;

  // クリア信号ノード
  SizeType mClear;

  // プリセット信号ノード
  SizeType mPreset;

  // セル番号
  int mCellId;

};

END_NAMESPACE_YM_BNET

#endif // BNDFFIMPL_H
