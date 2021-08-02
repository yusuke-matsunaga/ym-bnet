#ifndef BNINPUTNODE_H
#define BNINPUTNODE_H

/// @file BnInputNode.h
/// @brief BnInputNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
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
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType input_id   ///< [in] 入力番号
  ) : BnNodeImpl(id, name),
      mInputPos{input_id}
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
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType input_id,  ///< [in] 入力番号
    SizeType port_id,   ///< [in] ポート番号
    SizeType port_bit   ///< [in] ポート中のビット位置
  ) : BnInputNode(id, name, input_id),
      mPortId{port_id},
      mPortBit{port_bit}
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
/// @class BnDffOutput BnDffOutput.h "BnDffOutput.h"
/// @brief DFf の出力端子を表すクラス
///
/// 名前が紛らわしいが入力ノードである．
//////////////////////////////////////////////////////////////////////
class BnDffOutput :
  public BnInputNode
{
public:

  /// @brief コンストラクタ
  BnDffOutput(
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType input_id,  ///< [in] 入力番号
    SizeType dff_id     ///< [in] DFF番号
  ) : BnInputNode(id, name, input_id),
      mDffId(dff_id)
  {
  }

  /// @brief デストラクタ
  ~BnDffOutput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFFの出力端子の時 true を返す．
  bool
  is_dff_output() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているDFFの番号を返す．
  ///
  /// is_dff_input() || is_dff_output() || is_dff_xoutput() ||
  /// is_dff_clock() || is_dff_clear() || is_dff_preset()
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
/// @class BnDffXOutput BnInputNode.h "BnInputNode.h"
/// @brief DFf の反転出力端子を表すクラス
///
/// 名前が紛らわしいが入力ノードである．
//////////////////////////////////////////////////////////////////////
class BnDffXOutput :
  public BnInputNode
{
public:

  /// @brief コンストラクタ
  BnDffXOutput(
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType input_id,  ///< [in] 入力番号
    SizeType dff_id     ///< [in] DFF番号
  ) : BnInputNode(id, name, input_id),
      mDffId{dff_id}
  {
  }

  /// @brief デストラクタ
  ~BnDffXOutput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFFの反転出力端子の時 true を返す．
  bool
  is_dff_xoutput() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているDFFの番号を返す．
  ///
  /// is_dff_input() || is_dff_output() || is_dff_xoutput() ||
  /// is_dff_clock() || is_dff_clear() || is_dff_preset()
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
/// @class BnLatchOutput BnLatchOutput.h "BnLatchOutput.h"
/// @brief ラッチの出力端子を表すクラス
///
/// 名前が紛らわしいが入力ノードである．
//////////////////////////////////////////////////////////////////////
class BnLatchOutput :
  public BnInputNode
{
public:

  /// @brief コンストラクタ
  BnLatchOutput(
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType input_id,  ///< [in] 入力番号
    SizeType latch_id   ///< [in] ラッチ番号
  ) : BnInputNode(id, name, input_id),
      mLatchId{latch_id}
  {
  }

  /// @brief デストラクタ
  ~BnLatchOutput() override = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ラッチの出力端子の時 true を返す．
  bool
  is_latch_output() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているラッチの番号を返す．
  ///
  /// is_latch_input() || is_latch_output() || is_latch_xoutput() ||
  /// is_latch_enable() || is_latch_clear() || is_latch_preset()
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
/// @class BnLatchXOutput BnLatchOutput.h "BnLatchOutput.h"
/// @brief ラッチの反転出力端子を表すクラス
///
/// 名前が紛らわしいが入力ノードである．
//////////////////////////////////////////////////////////////////////
class BnLatchXOutput :
  public BnInputNode
{
public:

  /// @brief コンストラクタ
  BnLatchXOutput(
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType input_id,  ///< [in] 入力番号
    SizeType latch_id   ///< [in] ラッチ番号
  ) : BnInputNode(id, name, input_id),
      mLatchId{latch_id}
  {
  }

  /// @brief デストラクタ
  ~BnLatchXOutput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ラッチの反転出力端子の時 true を返す．
  bool
  is_latch_xoutput() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているラッチの番号を返す．
  ///
  /// is_latch_input() || is_latch_output() || is_latch_xoutput ||
  /// is_latch_enable() || is_latch_clear() || is_latch_preset()
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

END_NAMESPACE_YM_BNET

#endif // BNINPUTNODE_H
