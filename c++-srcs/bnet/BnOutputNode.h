#ifndef BNOUTPUTNODE_H
#define BNOUTPUTNODE_H

/// @file BnOutputNode.h
/// @brief BnOutputNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021, 2022 Yusuke Matsunaga
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

  /// @brief 出力位置を設定する．
  void
  set_output_pos(
    SizeType opos ///< [in] 出力位置
  ) override;

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
      mPortBit{port_bit},
      mPrimaryOutputPos{BNET_NULLID}
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

  /// @brief 外部出力端子番号を返す．
  ///
  /// is_port_output() == true の時のみ意味を持つ．<br>
  /// node_id = BnNetwork::primary_output_id(pos) の時
  /// node->primary_output_pos() = pos となる．
  SizeType
  primary_output_pos() const override;


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

  /// @brief 外部出力端子番号を設定する．
  ///
  /// 外部出力ノード専用
  void
  set_primary_output_pos(
    SizeType pos ///< [in] 位置番号
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 外部出力番号
  SizeType mPrimaryOutputPos;

  // ポート番号
  SizeType mPortId;

  // ポート中のビット位置
  SizeType mPortBit;

};


//////////////////////////////////////////////////////////////////////
/// @class BnControl BnOutputNode.h "BnOutputNode.h"
/// @brief DFf の入力系端子を表すクラス
///
/// 名前が紛らわしいが出力ノードである．
//////////////////////////////////////////////////////////////////////
class BnControl :
  public BnOutputNode
{
public:

  /// @brief コンストラクタ
  BnControl(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  ) : BnOutputNode{name},
      mDffId{dff_id}
  {
  }

  /// @brief デストラクタ
  ~BnControl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 入力タイプ/出力タイプに共通なインターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続しているDFFの番号を返す．
  ///
  /// is_dff_input() || is_dff_output() || is_clock() || is_enable() ||
  /// is_clear() || is_preset()
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
/// @class BnDataIn BnOutputNode.h "BnOutputNode.h"
/// @brief DFf の入力端子を表すクラス
///
/// 名前が紛らわしいが出力ノードである．
//////////////////////////////////////////////////////////////////////
class BnDataIn:
  public BnControl
{
public:

  /// @brief コンストラクタ
  BnDataIn(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  ) : BnControl{name, dff_id}
  {
  }

  /// @brief デストラクタ
  ~BnDataIn() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brie DFFの入力端子の時に true を返す．
  bool
  is_data_in() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BnClock BnOutputNode.h "BnOutputNode.h"
/// @brief DFf のクロック端子を表すクラス
///
/// 名前が紛らわしいが出力ノードである．
//////////////////////////////////////////////////////////////////////
class BnClock :
  public BnControl
{
public:

  /// @brief コンストラクタ
  BnClock(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  ) : BnControl{name, dff_id}
  {
  }

  /// @brief デストラクタ
  ~BnClock() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brie DFFのクロック端子の時に true を返す．
  bool
  is_clock() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BnClear BnOutputNode.h "BnOutputNode.h"
/// @brief DFf のクリア端子を表すクラス
///
/// 名前が紛らわしいが出力ノードである．
//////////////////////////////////////////////////////////////////////
class BnClear :
  public BnControl
{
public:

  /// @brief コンストラクタ
  BnClear(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  ) : BnControl{name, dff_id}
  {
  }

  /// @brief デストラクタ
  ~BnClear() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brie DFFのクリア端子の時に true を返す．
  bool
  is_clear() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BnPreset BnOutputNode.h "BnOutputNode.h"
/// @brief DFf のプリセット端子を表すクラス
///
/// 名前が紛らわしいが出力ノードである．
//////////////////////////////////////////////////////////////////////
class BnPreset :
  public BnControl
{
public:

  /// @brief コンストラクタ
  BnPreset(
    const string& name, ///< [in] ノード名
    SizeType dff_id     ///< [in] DFF番号
  ) : BnControl{name, dff_id}
  {
  }

  /// @brief デストラクタ
  ~BnPreset() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brie DFFのプリセット端子の時に true を返す．
  bool
  is_preset() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BnCellInput BnOutputNode.h "BnOutputNode.h"
/// @brief DFFセル の入力端子を表すクラス
///
/// 名前が紛らわしいが出力ノードである．
//////////////////////////////////////////////////////////////////////
class BnCellInput :
  public BnControl
{
public:

  /// @brief コンストラクタ
  BnCellInput(
    const string& name, ///< [in] ノード名
    SizeType dff_id,    ///< [in] DFF番号
    SizeType iid        ///< [in] セルの入力ピン番号
  ) : BnControl{name, dff_id},
      mIid{iid}
  {
  }

  /// @brief デストラクタ
  ~BnCellInput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ノードの外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFFセルの入力端子の時 true を返す．
  bool
  is_cell_input() const override;

  /// @brief DFFセルの入力ピン番号を返す．
  SizeType
  cell_input_pos() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力ピン番号
  SizeType mIid;

};

END_NAMESPACE_YM_BNET

#endif // BNOUTPUTNODE_H
