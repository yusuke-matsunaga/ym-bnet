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
    SizeType preset,    ///< [in] プリセット端子のノード番号
    BnCPV cpv           ///< [in] クリアとプリセットが衝突した場合の挙動
  ) : mId{id},
      mName{name},
      mInput{input},
      mOutput{output},
      mEnable{enable},
      mClear{clear},
      mPreset{preset},
      mCPV{cpv}
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

  /// @brief クリアとプリセットが衝突した場合の挙動
  BnCPV
  clear_preset_value() const override;

  /// @brief セルに割り当てられている場合のセル番号を返す．
  ///
  /// セルが割り当てられていない場合には -1 を返す．
  int
  cell_id() const override;

  /// @brief セルに割り当てられている場合の入力端子に対応するノード番号を返す．
  SizeType
  cell_input(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < cell.input_num() )
  ) const override;

  /// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
  SizeType
  cell_output(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < cell.output_num() )
  ) const override;


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

  // クリアとプリセットが衝突した場合の挙動
  BnCPV mCPV;

};


//////////////////////////////////////////////////////////////////////
/// @class BnLatchCell BnLatchCell.h "BnLatchCell.h"
/// @brief セルに対応する BnLatchImpl の継承クラス
//////////////////////////////////////////////////////////////////////
class BnLatchCell :
  public BnLatchImpl
{
public:

  /// @brief コンストラクタ
  BnLatchCell(
    SizeType id,                    ///< [in] ID番号
    const string& name,             ///< [in] 名前
    SizeType input,                 ///< [in] 入力端子のノード番号
    SizeType output,                ///< [in] 出力端子のノード番号
    SizeType enable,                ///< [in] イネーブル端子のノード番号
    SizeType clear,                 ///< [in] クリア端子のノード番号
    SizeType preset,                ///< [in] プリセット端子のノード番号
    BnCPV cpv,                      ///< [in] クリアとプリセットが衝突した場合の挙動
    int cell_id,                    ///< [in] セルのID番号
    const vector<SizeType>& inputs, ///< [in] セルの入力ノード番号のリスト
    const vector<SizeType>& outputs ///< [in] セルの入力ノード番号のリスト
  ) : BnLatchImpl{id, name, input, output,
		  enable, clear, preset, cpv},
      mCellId{cell_id},
      mInputs{inputs},
      mOutputs{outputs}
  {
  }

  /// @brief デストラクタ
  ~BnLatchCell() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief セルに割り当てられている場合のセル番号を返す．
  ///
  /// セルが割り当てられていない場合には -1 を返す．
  int
  cell_id() const override;

  /// @brief セルに割り当てられている場合の入力端子に対応するノード番号を返す．
  SizeType
  cell_input(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < cell.input_num() )
  ) const override;

  /// @brief セルに割り当てられている場合の出力端子に対応するノード番号を返す．
  SizeType
  cell_output(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < cell.output_num() )
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // セル番号
  int mCellId;

  // セルの入力端子に対応するノード番号のリスト
  vector<SizeType> mInputs;

  // セルの出力端子に対応するノード番号のリスト
  vector<SizeType> mOutputs;

};

END_NAMESPACE_YM_BNET

#endif // BNLATCHIMPL_H
