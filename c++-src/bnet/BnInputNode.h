﻿#ifndef BNINPUTNODE_H
#define BNINPUTNODE_H

/// @file BnInputNode.h
/// @brief BnInputNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
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
  /// @param[in] id ID 番号
  /// @param[in] name ノード名
  /// @param[in] input_id 入力番号
  BnInputNode(int id,
	      const string& name,
	      int input_id);

  /// @brief デストラクタ
  ~BnInputNode();


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
  /// node = BnNetwork::input(id) の時 node->input_id() = id となる．
  int
  input_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力番号
  int mInputId;

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
  /// @param[in] id ID 番号
  /// @param[in] name ノード名
  /// @param[in] input_id 入力番号
  /// @param[in] port_id ポート番号
  /// @param[in] port_bit ポート中のビット位置
  BnPortInput(int id,
	      const string& name,
	      int input_id,
	      int port_id,
	      int port_bit);

  /// @brief デストラクタ
  ~BnPortInput();


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
  int
  port_id() const override;

  /// @brief 接続しているポート中のビット番号を返す．
  ///
  /// is_port_input() || is_port_output() の時のみ意味を持つ．
  int
  port_bit() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ポート番号
  int mPortId;

  // ポート中のビット位置
  int mPortBit;

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
  /// @param[in] id ID 番号
  /// @param[in] name ノード名
  /// @param[in] input_id 入力番号
  /// @param[in] dff_id DFF番号
  BnDffOutput(int id,
	      const string& name,
	      int inputid,
	      int dff_id);

  /// @brief デストラクタ
  ~BnDffOutput();


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
  /// is_dff_input() || is_dff_output() || is_dff_clock() || is_dff_clear() || is_dff_preset()
  /// の時のみ意味を持つ．
  int
  dff_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // DFF 番号
  int mDffId;

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
  /// @param[in] id ID 番号
  /// @param[in] name ノード名
  /// @param[in] input_id 入力番号
  /// @param[in] dff_id DFF番号
  BnDffXOutput(int id,
	      const string& name,
	      int inputid,
	      int dff_id);

  /// @brief デストラクタ
  ~BnDffXOutput();


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
  /// is_dff_input() || is_dff_output() || is_dff_clock() || is_dff_clear() || is_dff_preset()
  /// の時のみ意味を持つ．
  int
  dff_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // DFF 番号
  int mDffId;

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
  /// @param[in] id ID 番号
  /// @param[in] name ノード名
  /// @param[in] input_id 入力番号
  /// @param[in] latch_id ラッチ番号
  BnLatchOutput(int id,
		const string& name,
		int input_id,
		int latch_id);

  /// @brief デストラクタ
  ~BnLatchOutput() override;


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
  /// is_latch_input() || is_latch_output() || is_latch_enable() || is_latch_clear() || is_latch_preset()
  /// の時のみ意味を持つ．
  int
  latch_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ラッチ番号
  int mLatchId;

};

END_NAMESPACE_YM_BNET

#endif // BNINPUTNODE_H
