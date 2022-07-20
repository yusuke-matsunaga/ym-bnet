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
    SizeType preset     ///< [in] プリセット端子のノード番号
  ) : mId{id},
      mName{name},
      mInput{input},
      mOutput{output},
      mClock{clock},
      mClear{clear},
      mPreset{preset}
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

  /// @brief 入力端子のノード番号を返す．
  SizeType
  input() const override;

  /// @brief 出力端子のノード番号を返す．
  SizeType
  output() const override;

  /// @brief クロック端子のノード番号を返す．
  SizeType
  clock() const override;

  /// @brief クリア端子のノード番号を返す．
  SizeType
  clear() const override;

  /// @brief プリセット端子のノード番号を返す．
  SizeType
  preset() const override;

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

  // クロックのノード番号
  SizeType mClock;

  // クリアのノード番号
  SizeType mClear;

  // プリセットのノード番号
  SizeType mPreset;

};


//////////////////////////////////////////////////////////////////////
/// @class BnDffCell BnDffImpl.h "BnDffImpl.h"
/// @brief セルに対応する BnDffImpl の継承クラス
//////////////////////////////////////////////////////////////////////
class BnDffCell :
  public BnDffImpl
{
public:

  /// @brief コンストラクタ
  BnDffCell(
    SizeType id,                    ///< [in] ID番号
    const string& name,             ///< [in] 名前
    SizeType input,                 //< [in] 入力端子のノード番号
    SizeType output,                ///< [in] 出力端子のノード番号
    SizeType clock,                 ///< [in] クロック端子のノード番号
    SizeType clear,                 ///< [in] クリア端子のノード番号
    SizeType preset,                ///< [in] プリセット端子のノード番号
    int cell_id,                    ///< [in] セルのID番号
    const vector<SizeType>& inputs, ///< [in] セルの入力ノード番号のリスト
    const vector<SizeType>& outputs ///< [in] セルの入力ノード番号のリスト
  ) : BnDffImpl{id, name, input, output,
		clock, clear, preset},
      mCellId{cell_id},
      mInputs{inputs},
      mOutputs{outputs}
  {
  }

  /// @brief デストラクタ
  ~BnDffCell() = default;


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

#endif // BNDFFIMPL_H
