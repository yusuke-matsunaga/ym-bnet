#ifndef BNNETWORKIMPL_H
#define BNNETWORKIMPL_H

/// @file BnNetworkImpl.h
/// @brief BnNetworkImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/ym_cell.h"
#include "ym/SimpleAlloc.h"
#include "BnNodeImpl.h"
#include "ym/BlifCover.h"


BEGIN_NAMESPACE_YM_BNET

class BnNetworkHandler;

//////////////////////////////////////////////////////////////////////
/// @class BnNetworkImpl BnNetworkImpl.h "BnNetworkImpl.h"
/// @brief BnNetwork の実装クラス
//////////////////////////////////////////////////////////////////////
class BnNetworkImpl
{
  friend class BlifBnNetworkHandler;

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

  /// @brief ラッチ数を得る．
  ymuint
  ff_num() const;

  /// @brief ラッチを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < ff_num() )
  const BnNode*
  ff(ymuint pos) const;

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

  /// @brief blif 形式のファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @param[in] cell_library セルライブラリ
  /// @retval true 正常に読み込めた
  /// @retval false 読み込み中にエラーが起こった．
  bool
  read_blif(const string& filename,
	    const CellLibrary* cell_library = nullptr);

  /// @brief 内容を blif 形式で出力する．
  /// @param[in] s 出力先のストリーム
  void
  write_blif(ostream& s) const;


private:
  //////////////////////////////////////////////////////////////////////
  // BnNetworkHandler のみが用いる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  void
  clear();

  /// @brief .model 名の設定
  void
  set_model(const char* name);

  /// @brief 外部入力ノードを生成する．
  /// @param[in] node_id ノードID
  /// @param[in] node_name ノード名
  void
  new_input(ymuint node_id,
	    const char* node_name);

  /// @brief 外部出力ノードの番号を登録する．
  /// @param[in] node_id ノードID
  void
  new_output(ymuint node_id);

  /// @brief ラッチノードを生成する．
  /// @param[in] node_id ノードID
  /// @param[in] node_name ノード名
  /// @param[in] inode_id ファンインのID番号
  /// @param[in] rval リセット値 ( '0', '1', ' ' のいずれか )
  void
  new_latch(ymuint node_id,
	    const char* node_name,
	    ymuint inode_id,
	    char rval);

  /// @brief プリミティブノードを生成する．
  /// @param[in] node_id ノードID
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_array ファンインのID番号の配列
  /// @param[in] gate_type ゲートの型
  void
  new_logic(ymuint node_id,
	    const char* node_name,
	    const vector<ymuint>& inode_id_array,
	    GateType gate_type);

  /// @brief セルノードを生成する．
  /// @param[in] node_id ノードID
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_array ファンインのID番号の配列
  /// @param[in] cell セルへのポインタ
  void
  new_logic(ymuint node_id,
	    const char* node_name,
	    const vector<ymuint>& inode_id_array,
	    const Cell* cell);

  /// @brief カバーノードを生成する．
  /// @param[in] node_id ノードID
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_array ファンインのID番号の配列
  /// @param[in] cover_id カバー
  void
  new_logic(ymuint node_id,
	    const char* node_name,
	    const vector<ymuint>& inode_id_array,
	    ymuint cover_id);

  /// @brief 論理式ノードを生成する．
  /// @param[in] node_id ノードID
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_array ファンインのID番号の配列
  /// @param[in] expr 論理式
  void
  new_logic(ymuint node_id,
	    const char* node_name,
	    const vector<ymuint>& inode_id_array,
	    Expr expr);

  /// @brief 真理値表ノードを生成する．
  /// @param[in] node_id ノードID
  /// @param[in] node_name ノード名
  /// @param[in] inode_id_array ファンインのID番号の配列
  /// @param[in] tv 真理値表
  void
  new_logic(ymuint node_id,
	    const char* node_name,
	    const vector<ymuint>& inode_id_array,
	    TvFunc tv);

  /// @brief ファンインのノード番号の配列を作る．
  ymuint*
  new_fanin_array(const vector<ymuint>& fanin_array);

  /// @brief ノードをセットする．
  /// @param[in] node_id ノードID
  /// @param[in] node 対象のノード
  void
  set_node(ymuint node_id,
	   BnNodeImpl* ndoe);

  /// @brief カバーを登録する．
  void
  new_cover(ymuint cover_id,
	    ymuint input_num,
	    ymuint cube_num,
	    const string& ipat_str,
	    BlifPat opat);

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

  // ID をキーにしてノードを収めた配列
  vector<BnNodeImpl*> mNodeArray;

  // 外部入力の配列
  vector<BnNode*> mPIArray;

  // 外部出力ノードのIDの配列
  vector<ymuint> mPOArray;

  // ラッチノードの配列
  vector<BnNode*> mFFArray;

  // 論理ノードの配列
  vector<BnNode*> mLogicArray;

  // カバーの配列
  vector<BlifCover*> mCoverArray;

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
  return node(mPOArray[pos]);
}

// @brief ラッチ数を得る．
inline
ymuint
BnNetworkImpl::ff_num() const
{
  return mFFArray.size();
}

// @brief ラッチを得る．
// @param[in] pos 位置番号 ( 0 <= pos < ff_num() )
inline
const BnNode*
BnNetworkImpl::ff(ymuint pos) const
{
  ASSERT_COND( pos < ff_num() );
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
