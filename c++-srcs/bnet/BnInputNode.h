﻿#ifndef BNINPUTNODE_H
#define BNINPUTNODE_H

/// @file BnInputNode.h
/// @brief BnInputNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "BnNodeImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnInputNode BnNodeImpl.h "BnNodeImpl.h"
/// @brief 入力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class BnInputNode :
  public BnNodeImpl
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  BnInputNode(
    const string& name ///< [in] ノード名
  ) : BnNodeImpl{name}
  {
  }

  /// @brief デストラクタ
  ~BnInputNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnNodeType
  type() const override;

  /// @brief 外部入力の時 true を返す．
  bool
  is_input() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力番号を返す．
  ///
  /// is_input() == false の時の動作は不定<br>
  /// node_id = BnNetwork::input_id(pos) の時 node->input_pos() = pos となる．
  SizeType
  input_pos() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用のインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力位置を設定する．
  ///
  /// 入力ノード専用
  void
  set_input_pos(
    SizeType ipos ///< [in] 入力位置
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力位置
  SizeType mInputPos;

};


//////////////////////////////////////////////////////////////////////
/// @class BnPortInput BnPortInput.h "BnPortInput.h"
/// @brief 入力端子を表すクラス
//////////////////////////////////////////////////////////////////////
class BnPortInput :
  public BnInputNode
{
public:

  /// @brief コンストラクタ
  BnPortInput(
    const string& name, ///< [in] ノード名
    SizeType port_id,   ///< [in] ポート番号
    SizeType port_bit   ///< [in] ポート中のビット位置
  ) : BnInputNode{name},
      mPortId{port_id},
      mPortBit{port_bit},
      mPrimaryInputPos{BNET_NULLID}
  {
  }

  /// @brief デストラクタ
  ~BnPortInput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力端子の時 true を返す．
  bool
  is_port_input() const override;

  /// @brief 外部入力番号を返す．
  ///
  /// is_port_input() == true の時のみ意味を持つ<br>
  /// node_id = BnNetwork::primary_input_id(pos) の時
  /// node->primary_input_pos() = pos となる．
  SizeType
  primary_input_pos() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているポート番号を返す．
  ///
  /// is_port_input() == true || is_port_output() == true の時のみ意味を持つ．
  SizeType
  port_id() const override;

  /// @brief 接続しているポート中のビット番号を返す．
  ///
  /// is_port_input() || is_port_output() の時のみ意味を持つ．
  SizeType
  port_bit() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用のインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力端子番号を設定する．
  ///
  /// 外部入力ノード専用
  void
  set_primary_input_pos(
    SizeType pos ///< [in] 位置番号
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 外部入力番号
  SizeType mPrimaryInputPos;

  // ポート番号
  SizeType mPortId;

  // ポート中のビット位置
  SizeType mPortBit;

};


//////////////////////////////////////////////////////////////////////
/// @class BnDffOut BnInputNode.h "BnInputNode.h"
/// @brief DFF の出力端子を表すクラス
///
/// 名前が紛らわしいが入力ノードである．
//////////////////////////////////////////////////////////////////////
class BnDffOut :
  public BnInputNode
{
public:

  /// @brief コンストラクタ
  BnDffOut(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  ) : BnInputNode{name},
      mDffId{dff_id}
  {
  }

  /// @brief デストラクタ
  ~BnDffOut() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているDFFの番号を返す．
  ///
  /// is_data_in() || is_data_out()
  /// の時のみ意味を持つ．
  SizeType
  dff_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // DFF 番号
  SizeType mDffId;

};


//////////////////////////////////////////////////////////////////////
/// @class BnDataOut BnDffOutput.h "BnDffOutput.h"
/// @brief DFF のデータ出力を表すクラス
///
/// 名前が紛らわしいが入力ノードである．
//////////////////////////////////////////////////////////////////////
class BnDataOut :
  public BnDffOut
{
public:

  /// @brief コンストラクタ
  BnDataOut(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  ) : BnDffOut{name, dff_id}
  {
  }

  /// @brief デストラクタ
  ~BnDataOut() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFFの出力端子の時 true を返す．
  bool
  is_data_out() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BnCellOutput BnDffOutput.h "BnDffOutput.h"
/// @brief DFFセルの出力端子を表すクラス
///
/// 名前が紛らわしいが入力ノードである．
//////////////////////////////////////////////////////////////////////
class BnCellOutput :
  public BnDffOut
{
public:

  /// @brief コンストラクタ
  BnCellOutput(
    const string& name, ///< [in] ノード名
    SizeType dff_id,    ///< [in] DFF番号
    SizeType oid        ///< [in] 出力ピン番号
  ) : BnDffOut{name, dff_id},
      mOid{oid}
  {
  }

  /// @brief デストラクタ
  ~BnCellOutput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFFセルの出力端子の時 true を返す．
  bool
  is_cell_output() const override;

  /// @brief DFFセルの出力ピン番号を返す．
  SizeType
  cell_output_pos() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力ピン番号
  SizeType mOid;

};

END_NAMESPACE_YM_BNET

#endif // BNINPUTNODE_H
