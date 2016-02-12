
/// @file BnNetwork.cc
/// @brief BnNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNetwork.h"
#include "BnNetworkImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BnNetwork::BnNetwork() :
  mImpl(new BnNetworkImpl)
{
}

// @brief デストラクタ
BnNetwork::~BnNetwork()
{
  delete mImpl;
}

// @brief model 名を得る．
string
BnNetwork::name() const
{
  return mImpl->name();
}

// @brief ノード数を得る．
ymuint
BnNetwork::node_num() const
{
  return mImpl->node_num();
}

// @brief ID 番号からノードを得る．
// @param[in] id ID 番号 ( 0 <= id < node_num() )
const BnNode*
BnNetwork::node(ymuint id) const
{
  return mImpl->node(id);
}

// @brief 外部入力数を得る．
ymuint
BnNetwork::input_num() const
{
  return mImpl->input_num();
}

// @brief 外部入力を得る．
// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
const BnNode*
BnNetwork::input(ymuint pos) const
{
  return mImpl->input(pos);
}

// @brief 外部出力数を得る．
ymuint
BnNetwork::output_num() const
{
  return mImpl->output_num();
}

// @brief 外部出力を得る．
// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
const BnNode*
BnNetwork::output(ymuint pos) const
{
  return mImpl->output(pos);
}

// @brief ラッチ数を得る．
ymuint
BnNetwork::ff_num() const
{
  return mImpl->ff_num();
}

// @brief ラッチを得る．
// @param[in] pos 位置番号 ( 0 <= pos < ff_num() )
const BnNode*
BnNetwork::ff(ymuint pos) const
{
  return mImpl->ff(pos);
}

// @brief 論理ノード数を得る．
ymuint
BnNetwork::logic_num() const
{
  return mImpl->logic_num();
}

// @brief 論理ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
const BnNode*
BnNetwork::logic(ymuint pos) const
{
  return mImpl->logic(pos);
}

// @brief blif 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @retval true 正常に読み込めた
// @retval false 読み込み中にエラーが起こった．
bool
BnNetwork::read_blif(const string& filename,
		     const CellLibrary* cell_library)
{
  return mImpl->read_blif(filename, cell_library);
}

// @brief 内容を blif 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnNetwork::write_blif(ostream& s) const
{
  mImpl->write_blif(s);
}


END_NAMESPACE_YM_BNET
