#ifndef BNNETWORKIMPL_H
#define BNNETWORKIMPL_H

/// @file BnNetworkImpl.h
/// @brief BnNetworkImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/BnFuncType.h"
#include "ym/ym_cell.h"
#include "ym/SimpleAlloc.h"
#include "BnNodeImpl.h"
#include "BnFuncTypeMgr.h"


BEGIN_NAMESPACE_YM_BNET

class BnNetworkHandler;

//////////////////////////////////////////////////////////////////////
/// @class BnNetworkImpl BnNetworkImpl.h "BnNetworkImpl.h"
/// @brief BnNetwork の実装クラス
//////////////////////////////////////////////////////////////////////
class BnNetworkImpl
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  BnNetworkImpl();

  /// @brief デストラクタ
  ~BnNetworkImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  // ほとんど BnNetwork と同じインターフェイスを持つ．
  //////////////////////////////////////////////////////////////////////

  /// @brief model 名を得る．
  const char*
  name() const;

  /// @brief ポート数を返す．
  ymuint
  port_num() const;

  /// @brief ポートを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
  const BnPort*
  port(ymuint pos) const;

  /// @brief ノード数を返す．
  ymuint
  node_num() const;

  /// @brief ID 番号からノードを得る．
  /// @param[in] id ID 番号 ( 0 <= id < node_num() )
  const BnNode*
  node(ymuint id) const;

  /// @brief 外部入力数を得る．
  ymuint
  input_num() const;

  /// @brief 外部入力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
  const BnNode*
  input(ymuint pos) const;

  /// @brief 外部出力数を得る．
  ymuint
  output_num() const;

  /// @brief 外部出力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  const BnNode*
  output(ymuint pos) const;

  /// @brief D-FF数を得る．
  ymuint
  dff_num() const;

  /// @brief D-FFを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < ff_num() )
  const BnNode*
  dff(ymuint pos) const;

  /// @brief 論理ノード数を得る．
  ymuint
  logic_num() const;

  /// @brief 論理ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
  const BnNode*
  logic(ymuint pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // blif 形式のファイルとの間で入出力を行なう関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を blif 形式で出力する．
  /// @param[in] s 出力先のストリーム
  void
  write_blif(ostream& s) const;


public:
  //////////////////////////////////////////////////////////////////////
  // iscas89 形式のファイルとの間で入出力を行なう関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を iscas89 形式で出力する．
  /// @param[in] s 出力先のストリーム
  void
  write_iscas89(ostream& s) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  void
  clear();

  /// @brief .model 名の設定
  void
  set_model(const char* name);

  /// @brief ポートを生成する．
  /// @param[in] port_name ポート名
  /// @param[in] bits 内容のノードIDのベクタ
  void
  new_port(const char* port_name,
	   const vector<ymuint>& bits);

  /// @brief 外部入力ノードを生成する．
  /// @param[in] node_id ノードID
  /// @param[in] node_name ノード名
  /// @return 生成されたノードID (= node_id)を返す．
  ymuint
  new_input(ymuint node_id,
	    const char* node_name);

  /// @brief 外部出力ノードの番号を登録する．
  /// @param[in] node_id ノードID
  /// @param[in] node_name ノード名
  /// @return 生成されたノードID (!= node_id)を返す．
  ymuint
  new_output(ymuint node_id,
	     const char* node_name);

  /// @brief D-FFノードを生成する．
  /// @param[in] node_id ノードID
  /// @param[in] node_name ノード名
  /// @param[in] inode_id ファンインのID番号
  /// @param[in] rval リセット値 ( '0', '1', ' ' のいずれか )
  /// @return 生成されたノードID (= node_id)を返す．
  ymuint
  new_dff(ymuint node_id,
	  const char* node_name,
	  ymuint inode_id,
	  char rval);

  /// @brief 論理ノードを生成する．
  /// @param[in] node_id ノードID
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_array ファンインのID番号の配列
  /// @param[in] func_type 関数の型
  /// @return 生成されたノードID (= node_id)を返す．
  ymuint
  new_logic(ymuint node_id,
	    const char* node_name,
	    const vector<ymuint>& inode_id_array,
	    const BnFuncType* func_type);

  /// @brief プリミティブ型を生成する．
  const BnFuncType*
  new_primitive_type(BnFuncType::Type prim_type,
		     ymuint input_num);

  /// @brief セル型を生成する．
  const BnFuncType*
  new_cell_type(const Cell* cell);

  /// @brief 論理式型を生成する．
  const BnFuncType*
  new_expr_type(const Expr& expr,
		ymuint input_num);

  /// @brief 真理値表型を生成する．
  const BnFuncType*
  new_tv_type(const TvFunc& tv);

  /// @brief 各ノードのファンアウト情報を設定する．
  void
  wrap_up();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインのノード番号の配列を作る．
  ymuint*
  new_fanin_array(const vector<ymuint>& fanin_array);

  /// @brief ノードをセットする．
  /// @param[in] node_id ノードID
  /// @param[in] node 対象のノード
  void
  set_node(ymuint node_id,
	   BnNodeImpl* ndoe);

  /// @brief 文字列領域を確保する．
  const char*
  alloc_string(const char* src_str);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // メモリアロケータ
  SimpleAlloc mAlloc;

  // 名前 (.model)
  // 領域は mAlloc で確保する．
  const char* mName;

  // ポートの配列
  vector<BnPort*> mPortArray;

  // ID をキーにしてノードを収めた配列
  vector<BnNodeImpl*> mNodeArray;

  // 外部入力ノードの配列
  vector<BnNode*> mPIArray;

  // 外部出力ノードの配列
  vector<BnNode*> mPOArray;

  // ラッチノードの配列
  vector<BnNode*> mFFArray;

  // 論理ノードの配列
  vector<BnNode*> mLogicArray;

  // FuncType を管理するオブジェクト
  BnFuncTypeMgr mFuncTypeMgr;

};


//////////////////////////////////////////////////////////////////////
//  BnNetworkImpl のインライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief model 名を得る．
inline
const char*
BnNetworkImpl::name() const
{
  return mName;
}

// @brief ポート数を返す．
inline
ymuint
BnNetworkImpl::port_num() const
{
  return mPortArray.size();
}

// @brief ポートを得る．
// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
inline
const BnPort*
BnNetworkImpl::port(ymuint pos) const
{
  ASSERT_COND( pos < port_num() );
  return mPortArray[pos];
}

// @brief ノード数を返す．
inline
ymuint
BnNetworkImpl::node_num() const
{
  return mNodeArray.size();
}

// @brief ID 番号からノードを得る．
// @param[in] id ID 番号
inline
const BnNode*
BnNetworkImpl::node(ymuint id) const
{
  ASSERT_COND( id < node_num() );
  return mNodeArray[id];
}

// @brief 外部入力数を得る．
inline
ymuint
BnNetworkImpl::input_num() const
{
  return mPIArray.size();
}

// @brief 外部入力を得る．
// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
inline
const BnNode*
BnNetworkImpl::input(ymuint pos) const
{
  ASSERT_COND( pos < input_num() );
  return mPIArray[pos];
}

// @brief 外部出力数を得る．
inline
ymuint
BnNetworkImpl::output_num() const
{
  return mPOArray.size();
}

// @brief 外部出力を得る．
// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
inline
const BnNode*
BnNetworkImpl::output(ymuint pos) const
{
  ASSERT_COND( pos < output_num() );
  return mPOArray[pos];
}

// @brief D-FF数を得る．
inline
ymuint
BnNetworkImpl::dff_num() const
{
  return mFFArray.size();
}

// @brief D-FFを得る．
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
inline
const BnNode*
BnNetworkImpl::dff(ymuint pos) const
{
  ASSERT_COND( pos < dff_num() );
  return mFFArray[pos];
}

// @brief 論理ノード数を得る．
inline
ymuint
BnNetworkImpl::logic_num() const
{
  return mLogicArray.size();
}

// @brief 論理ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
// @note 論理ノードはトポロジカル順に整列している．
inline
const BnNode*
BnNetworkImpl::logic(ymuint pos) const
{
  ASSERT_COND( pos < logic_num() );
  return mLogicArray[pos];
}

END_NAMESPACE_YM_BNET

#endif // BNNETWORKIMPL_H
