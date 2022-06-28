#ifndef BNLATCHIMPL_H
#define BNLATCHIMPL_H

/// @file BnLatchImpl.h
/// @brief BnLatchImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2021 Yusuke Matsunaga
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
  BnLatchImpl(
    SizeType id,        ///< [in] ID番号
    const string& name, ///< [in] 名前
    SizeType input,     ///< [in] 入力端子のノード番号
    SizeType output,    ///< [in] 出力端子のノード番号
    SizeType enable,    ///< [in] イネーブル端子のノード番号
    SizeType clear,     ///< [in] クリア端子のノード番号
    SizeType preset     ///< [in] プリセット端子のノード番号
  ) : mId{id},
      mName{name},
      mInput{input},
      mOutput{output},
      mEnable{enable},
      mClear{clear},
      mPreset{preset}
  {
  }

  /// @brief デストラクタ
  ~BnLatchImpl() = default;


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

  /// @brief 入力端子のノード番号を返す．
  SizeType
  input() const override;

  /// @brief 出力端子のノード番号を返す．
  SizeType
  output() const override;

  /// @brief ラッチイネーブル端子のノード番号を返す．
  SizeType
  enable() const override;

  /// @brief クリア端子のノード番号を返す．
  SizeType
  clear() const override;

  /// @brief プリセット端子のノード番号を返す．
  SizeType
  preset() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId;

  // 名前
  string mName;

  // 入力のノード番号
  SizeType mInput;

  // 出力のノード番号
  SizeType mOutput;

  // イネーブルのノード番号
  SizeType mEnable;

  // クリアのノード番号
  SizeType mClear;

  // プリセットのノード番号
  SizeType mPreset;

};

END_NAMESPACE_YM_BNET

#endif // BNLATCHIMPL_H
