#ifndef BNOUTPUTNODE_H
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
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  BnOutputNode(
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id  ///< [in] 出力番号
  ) : BnNodeImpl(id, name),
      mOutputPos{output_id},
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
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id, ///< [in] 出力番号
    SizeType port_id,   ///< [in] ポート番号
    SizeType port_bit   ///< [in] ポート中のビット位置
  ) : BnOutputNode(id, name, output_id),
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
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id, ///< [in] 出力番号
    SizeType dff_id     ///< [in] DFF番号
  ) : BnOutputNode(id, name, output_id),
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
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id, ///< [in] 出力番号
    SizeType dff_id     ///< [in] DFF番号
  ) : BnDffControl(id, name, output_id, dff_id)
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
/// @class BnDffClock BnOutputNode.h "BnOutputNode.h"
/// @brief DFf のクロック入力端子を表すクラス
//////////////////////////////////////////////////////////////////////
class BnDffClock :
  public BnDffControl
{
public:

  /// @brief コンストラクタ
  BnDffClock(
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id, ///< [in] 出力番号
    SizeType dff_id     ///< [in] DFF番号
  ) : BnDffControl(id, name, output_id, dff_id)
  {
  }

  /// @brief デストラクタ
  ~BnDffClock() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFFのクロック端子の時に true を返す．
  bool
  is_dff_clock() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BnDffClear BnOutputNode.h "BnOutputNode.h"
/// @brief DFf のクリア端子を表すクラス
//////////////////////////////////////////////////////////////////////
class BnDffClear :
  public BnDffControl
{
public:

  /// @brief コンストラクタ
  BnDffClear(
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id, ///< [in] 出力番号
    SizeType dff_id     ///< [in] DFF番号
  ) : BnDffControl(id, name, output_id, dff_id)
  {
  }

  /// @brief デストラクタ
  ~BnDffClear() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFFのクリア端子の時に true を返す．
  bool
  is_dff_clear() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BnDffPreset BnOutputNode.h "BnOutputNode.h"
/// @brief DFf のプリセット端子を表すクラス
//////////////////////////////////////////////////////////////////////
class BnDffPreset :
  public BnDffControl
{
public:

  /// @brief コンストラクタ
  BnDffPreset(
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id, ///< [in] 出力番号
    SizeType dff_id     ///< [in] DFF番号
  ) : BnDffControl(id, name, output_id, dff_id)
  {
  }

  /// @brief デストラクタ
  ~BnDffPreset() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFFのプリセット端子の時に true を返す．
  bool
  is_dff_preset() const override;

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
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id, ///< [in] 出力番号
    SizeType latch_id   ///< [in] ラッチ番号
  ) : BnOutputNode(id, name, output_id),
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
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id, ///< [in] 出力番号
    SizeType latch_id   ///< [in] ラッチ番号
  ) : BnLatchControl(id, name, output_id, latch_id)
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


//////////////////////////////////////////////////////////////////////
/// @class BnLatchEnable BnOutputNode.h "BnOutputNode.h"
/// @brief ラッチのイネーブル端子を表すクラス
//////////////////////////////////////////////////////////////////////
class BnLatchEnable :
  public BnLatchControl
{
public:

  /// @brief コンストラクタ
  BnLatchEnable(
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id, ///< [in] 出力番号
    SizeType latch_id   ///< [in] ラッチ番号
  ) : BnLatchControl(id, name, output_id, latch_id)
  {
  }

  /// @brief デストラクタ
  ~BnLatchEnable() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ラッチのイネーブル端子の時に true を返す．
  bool
  is_latch_enable() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BnLatchClear BnOutputNode.h "BnOutputNode.h"
/// @brief ラッチのクリア端子を表すクラス
//////////////////////////////////////////////////////////////////////
class BnLatchClear :
  public BnLatchControl
{
public:

  /// @brief コンストラクタ
  BnLatchClear(
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id, ///< [in] 出力番号
    SizeType latch_id   ///< [in] ラッチ番号
  ) : BnLatchControl(id, name, output_id, latch_id)
  {
  }

  /// @brief デストラクタ
  ~BnLatchClear() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ラッチのクリア端子の時に true を返す．
  bool
  is_latch_clear() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BnLatchPreset BnOutputNode.h "BnOutputNode.h"
/// @brief ラッチのプリセット端子を表すクラス
//////////////////////////////////////////////////////////////////////
class BnLatchPreset :
  public BnLatchControl
{
public:

  /// @brief コンストラクタ
  BnLatchPreset(
    SizeType id,        ///< [in] ID 番号
    const string& name, ///< [in] ノード名
    SizeType output_id, ///< [in] 出力番号
    SizeType latch_id   ///< [in] ラッチ番号
  ) : BnLatchControl(id, name, output_id, latch_id)
  {
  }

  /// @brief デストラクタ
  ~BnLatchPreset() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ラッチのプリセット端子の時に true を返す．
  bool
  is_latch_preset() const override;

};

END_NAMESPACE_YM_BNET

#endif // BNOUTPUTNODE_H
