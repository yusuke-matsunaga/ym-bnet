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
/// @class BnDffBase BnDffBase.h "BnDffBase.h"
/// @brief BnDff の基底クラス
//////////////////////////////////////////////////////////////////////
class BnDffBase :
  public BnDff
{
public:

  /// @brief コンストラクタ
  BnDffBase(
    SizeType id,       ///< [in] ID番号
    const string& name ///< [in] 名前
  ) : mId{id},
      mName{name}
  {
  }

  /// @brief デストラクタ
  ~BnDffBase() = default;


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
  data_in() const override;

  /// @brief 出力端子のノード番号を返す．
  SizeType
  data_out() const override;

  /// @brief クロック端子のノード番号を返す．
  SizeType
  clock() const override;

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
  /// セルが割り当てられていない場合には CLIB_NULLID を返す．
  SizeType
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

};


//////////////////////////////////////////////////////////////////////
/// @class BnDLBase BnDLBase.h "BnDLBase.h"
/// @brief BnDff の実装クラス
//////////////////////////////////////////////////////////////////////
class BnDLBase :
  public BnDffBase
{
public:

  /// @brief コンストラクタ
  BnDLBase(
    SizeType id,        ///< [in] ID番号
    const string& name, ///< [in] 名前
    SizeType input,     ///< [in] 入力端子のノード番号
    SizeType output,    ///< [in] 出力端子のノード番号
    SizeType clear,     ///< [in] クリア端子のノード番号
    SizeType preset,    ///< [in] プリセット端子のノード番号
    BnCPV cpv           ///< [in] クリアとプリセットが衝突した場合の挙動
  ) : BnDffBase{id, name},
      mInput{input},
      mOutput{output},
      mClear{clear},
      mPreset{preset},
      mCPV{cpv}
  {
  }

  /// @brief デストラクタ
  ~BnDLBase() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力端子のノード番号を返す．
  SizeType
  data_in() const override;

  /// @brief 出力端子のノード番号を返す．
  SizeType
  data_out() const override;

  /// @brief クリア端子のノード番号を返す．
  SizeType
  clear() const override;

  /// @brief プリセット端子のノード番号を返す．
  SizeType
  preset() const override;

  /// @brief クリアとプリセットが衝突した場合の挙動
  BnCPV
  clear_preset_value() const override;


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

  // クリアのノード番号
  SizeType mClear;

  // プリセットのノード番号
  SizeType mPreset;

  // クリアとプリセットが衝突した場合の挙動
  BnCPV mCPV;

};


//////////////////////////////////////////////////////////////////////
/// @class BnDffImpl BnDffImpl.h "BnDffImpl.h"
/// @brief BnDff の実装クラス
//////////////////////////////////////////////////////////////////////
class BnDffImpl :
  public BnDLBase
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
    BnCPV cpv           ///< [in] クリアとプリセットが衝突した場合の挙動
  ) : BnDLBase{id, name, input, output, clear, preset, cpv},
      mClock{clock}
  {
  }

  /// @brief デストラクタ
  ~BnDffImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnDffType
  type() const override;

  /// @brief クロック端子のノード番号を返す．
  SizeType
  clock() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // クロックのノード番号
  SizeType mClock;

};


//////////////////////////////////////////////////////////////////////
/// @class BnLatchImpl BnLatchImpl.h "BnLatchImpl.h"
/// @brief BnLatch の実装クラス
//////////////////////////////////////////////////////////////////////
class BnLatchImpl :
  public BnDLBase
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
  ) : BnDLBase{id, name, input, output, clear, preset, cpv},
      mEnable{enable}
  {
  }

  /// @brief デストラクタ
  ~BnLatchImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnDffType
  type() const override;

  /// @brief ラッチイネーブル端子のノード番号を返す．
  SizeType
  enable() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // イネーブルのノード番号
  SizeType mEnable;

};


//////////////////////////////////////////////////////////////////////
/// @class BnDLCell BnDLCell.h "BnDLCell.h"
/// @brief BnDff の実装クラス
//////////////////////////////////////////////////////////////////////
class BnDLCell :
  public BnDffBase
{
public:

  /// @brief コンストラクタ
  BnDLCell(
    SizeType id,                        ///< [in] ID番号
    const string& name,                 ///< [in] 名前
    SizeType cell_id,                   ///< [in] セル番号
    const vector<SizeType>& input_list, ///< [in] 入力ノード番号のリスト
    const vector<SizeType>& output_list ///< [in] 出力ノード番号のリスト
  ) : BnDffBase{id, name},
      mCellId{cell_id},
      mInputList{input_list},
      mOutputList{output_list}
  {
  }

  /// @brief デストラクタ
  ~BnDLCell() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief セルに割り当てられている場合のセル番号を返す．
  ///
  /// セルが割り当てられていない場合には -1 を返す．
  SizeType
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
  SizeType mCellId;

  // 入力端子のノード番号のリスト
  vector<SizeType> mInputList;

  // 出力端子のノード番号のリスト
  vector<SizeType> mOutputList;

};


//////////////////////////////////////////////////////////////////////
/// @class BnDffCell BnDffCell.h "BnDffCell.h"
/// @brief BnDff の実装クラス
//////////////////////////////////////////////////////////////////////
class BnDffCell :
  public BnDLCell
{
public:

  /// @brief コンストラクタ
  BnDffCell(
    SizeType id,                        ///< [in] ID番号
    const string& name,                 ///< [in] 名前
    SizeType cell_id,                   ///< [in] セル番号
    const vector<SizeType>& input_list, ///< [in] 入力ノード番号のリスト
    const vector<SizeType>& output_list ///< [in] 出力ノード番号のリスト
  ) : BnDLCell{id, name, cell_id, input_list, output_list}
  {
  }

  /// @brief デストラクタ
  ~BnDffCell() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnDffType
  type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BnLatchCell BnLatchCell.h "BnLatchCell.h"
/// @brief BnDff の実装クラス
//////////////////////////////////////////////////////////////////////
class BnLatchCell :
  public BnDLCell
{
public:

  /// @brief コンストラクタ
  BnLatchCell(
    SizeType id,                        ///< [in] ID番号
    const string& name,                 ///< [in] 名前
    SizeType cell_id,                   ///< [in] セル番号
    const vector<SizeType>& input_list, ///< [in] 入力ノード番号のリスト
    const vector<SizeType>& output_list ///< [in] 出力ノード番号のリスト
  ) : BnDLCell{id, name, cell_id, input_list, output_list}
  {
  }

  /// @brief デストラクタ
  ~BnLatchCell() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnDffType
  type() const override;

};

END_NAMESPACE_YM_BNET

#endif // BNDFFIMPL_H
