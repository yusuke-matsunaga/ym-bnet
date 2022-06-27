﻿#ifndef BNOUTPUTNODE_H
#define BNOUTPUTNODE_H

/// @file BnOutputNode.h
/// @brief BnOutputNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BnNodeImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnOutputNode BnNodeImpl.h "BnNodeImpl.h"
/// @brief 外部出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class BnOutputNode :
  public BnNodeImpl
{
  friend class BnNetworkImpl;

public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  BnOutputNode(
    const string& name ///< [in] ノード名
  ) : BnNodeImpl{name},
      mFanin{BNET_NULLID}
  {
  }

  /// @brief デストラクタ
  ~BnOutputNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 全タイプ共通の外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  BnNodeType
  type() const override;

  /// @brief 外部出力ノードの時 true を返す．
  bool
  is_output() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号を返す．
  ///
  /// is_output() == false の時の動作は不定<br>
  /// node_id = BnNetwork::output_id(pos) の時，node->output_pos() = pos となる．
  SizeType
  output_pos() const override;

  /// @brief ファンイン数を得る．
  SizeType
  fanin_num() const override;

  /// @brief ファンインのノード番号を返す．
  SizeType
  fanin_id(
    SizeType pos ///< [in] 入力位置 ( 0 <= pos < fanin_num() )
  ) const override;

  /// @brief ファンインのノード番号のリストを返す．
  vector<SizeType>
  fanin_id_list() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用のインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインを設定する．
  void
  set_fanin(
    SizeType ipos,    ///< [in] 入力位置
    SizeType fanin_id ///< [in] ファンインのノード番号
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力位置
  SizeType mOutputPos;

  // ファンインのノード番号
  SizeType mFanin;

};


//////////////////////////////////////////////////////////////////////
/// @class BnPortOutput BnPortOutput.h "BnPortOutput.h"
/// @brief 外部出力端子を表すクラス
//////////////////////////////////////////////////////////////////////
class BnPortOutput :
  public BnOutputNode
{
public:

  /// @brief コンストラクタ
  BnPortOutput(
    const string& name, ///< [in] ノード名
    SizeType port_id,   ///< [in] ポート番号
    SizeType port_bit   ///< [in] ポート中のビット位置
  ) : BnOutputNode{name},
      mPortId{port_id},
      mPortBit{port_bit}
  {
  }

  /// @brief デストラクタ
  ~BnPortOutput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部出力端子の時に true を返す．
  bool
  is_port_output() const override;


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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ポート番号
  SizeType mPortId;

  // ポート中のビット位置
  SizeType mPortBit;

};


//////////////////////////////////////////////////////////////////////
/// @class BnDffControl BnOutputNode.h "BnOutputNode.h"
/// @brief DFf の入力系端子を表すクラス
///
/// 名前が紛らわしいが出力ノードである．
//////////////////////////////////////////////////////////////////////
class BnDffControl :
  public BnOutputNode
{
public:

  /// @brief コンストラクタ
  BnDffControl(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  ) : BnOutputNode{name},
      mDffId{dff_id}
  {
  }

  /// @brief デストラクタ
  ~BnDffControl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているDFFの番号を返す．
  ///
  /// is_dff_input() || is_dff_output() || is_dff_clock() || is_dff_clear() || is_dff_preset()
  /// の時のみ意味を持つ．
  SizeType
  dff_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // DFF番号
  SizeType mDffId;

};


//////////////////////////////////////////////////////////////////////
/// @class BnDffInput BnOutputNode.h "BnOutputNode.h"
/// @brief DFf の入力端子を表すクラス
///
/// 名前が紛らわしいが出力ノードである．
//////////////////////////////////////////////////////////////////////
class BnDffInput :
  public BnDffControl
{
public:

  /// @brief コンストラクタ
  BnDffInput(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  ) : BnDffControl{name, dff_id}
  {
  }

  /// @brief デストラクタ
  ~BnDffInput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brie DFFの入力端子の時に true を返す．
  bool
  is_dff_input() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BnLatchControl BnOutputNode.h "BnOutputNode.h"
/// @brief ラッチの入力系端子を表すクラス
///
/// 名前が紛らわしいが出力ノードである．
//////////////////////////////////////////////////////////////////////
class BnLatchControl :
  public BnOutputNode
{
public:

  /// @brief コンストラクタ
  BnLatchControl(
    const string& name, ///< [in] ノード名
    SizeType latch_id   ///< [in] ラッチ番号
  ) : BnOutputNode{name},
      mLatchId{latch_id}
  {
  }

  /// @brief デストラクタ
  ~BnLatchControl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているラッチの番号を返す．
  ///
  /// is_latch_input() || is_latch_output() || is_latch_enable() ||
  /// is_latch_clear() || is_latch_preset()
  /// の時のみ意味を持つ．
  SizeType
  latch_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ラッチ番号
  SizeType mLatchId;

};


//////////////////////////////////////////////////////////////////////
/// @class BnLatchInput BnOutputNode.h "BnOutputNode.h"
/// @brief ラッチの入力端子を表すクラス
///
/// 名前が紛らわしいが出力ノードである．
//////////////////////////////////////////////////////////////////////
class BnLatchInput :
  public BnLatchControl
{
public:

  /// @brief コンストラクタ
  BnLatchInput(
    const string& name, ///< [in] ノード名
    SizeType latch_id   ///< [in] ラッチ番号
  ) : BnLatchControl{name, latch_id}
  {
  }

  /// @brief デストラクタ
  ~BnLatchInput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brie ラッチの入力端子の時に true を返す．
  bool
  is_latch_input() const override;

};

END_NAMESPACE_YM_BNET

#endif // BNOUTPUTNODE_H
