
/// @file BnNetwork.cc
/// @brief BnNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "BnNetworkImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
//
// 空の状態で初期化される．
BnNetwork::BnNetwork(
) : mImpl{new BnNetworkImpl{}}
{
}

// @brief コピーコンストラクタ
BnNetwork::BnNetwork(
  const BnNetwork& src
) : mImpl{new BnNetworkImpl{}}
{
  mImpl->copy(src.mImpl.get());
}

// @brief ムーブコンストラクタ
BnNetwork::BnNetwork(
  BnNetwork&& src
)
{
  move(std::move(src));
}

// @brief コピー代入演算子
BnNetwork&
BnNetwork::operator=(
  const BnNetwork& src
)
{
  mImpl->copy(src.mImpl.get());

  return *this;
}

// @brief ムーブ代入演算子
BnNetwork&
BnNetwork::operator=(
  BnNetwork&& src
)
{
  move(std::move(src));

  return *this;
}

// @brief デストラクタ
BnNetwork::~BnNetwork()
{
  clear();
}

// @brief 内容をクリアする．
//
// コンストラクタ直後と同じ状態になる．
void
BnNetwork::clear()
{
  if ( mImpl ) {
    mImpl->clear();
  }
}

// @brief 内容をコピーする．
void
BnNetwork::copy(
  const BnNetwork& src
)
{
  if ( &src == this ) {
    // 自分自身がソースならなにもしない．
    return;
  }

  ASSERT_COND( mImpl != nullptr );

  mImpl->copy(src.mImpl.get());
}

// @brief 内容をムーブする．
void
BnNetwork::move(
  BnNetwork&& src
)
{
  swap(mImpl, src.mImpl);
  mImpl->wrap_up();
}

// @brief ネットワーク名を得る．
string
BnNetwork::name() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->name();
}

// @brief 関連するセルライブラリを得る．
const ClibCellLibrary&
BnNetwork::library() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->library();
}

// @brief ポート数を得る．
SizeType
BnNetwork::port_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->port_num();
}

// @brief ポートの情報を得る．
BnPort
BnNetwork::port(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return BnPort{mImpl.get(), pos};
}

// @brief ポート名からポートを得る．
BnPort
BnNetwork::find_port(
  const string& name
) const
{
  ASSERT_COND( mImpl != nullptr );

  return BnPort{mImpl.get(), mImpl->find_port(name)};
}

// @brief ポートのリストを得る．
BnPortList
BnNetwork::port_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return BnPortList{mImpl.get()};
}

// @brief DFF数を得る．
SizeType
BnNetwork::dff_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->dff_num();
}

// @brief DFFを得る．
BnDff
BnNetwork::dff(
  SizeType id
) const
{
  ASSERT_COND( mImpl != nullptr );

  return BnDff{mImpl.get(), id};
}

// @brief DFFのリストを得る．
BnDffList
BnNetwork::dff_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return BnDffList{mImpl.get()};
}

// @brief ノード数を得る．
SizeType
BnNetwork::node_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->node_num();
}

// @brief ノードを得る．
BnNode
BnNetwork::node(
  SizeType id
) const
{
  ASSERT_COND( mImpl != nullptr );

  return BnNode{mImpl.get(), id};
}

// @brief 全てのノードのリストを得る．
BnAllNodeList
BnNetwork::all_node_list() const
{
  return BnAllNodeList{mImpl.get()};
}

// @brief 入力数を得る．
SizeType
BnNetwork::input_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->input_num();
}

// @brief 入力ノードのノード番号を得る．
SizeType
BnNetwork::input_id(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->input_id(pos);
}

// @brief 入力ノードのノードのリストを得る．
BnNodeList
BnNetwork::input_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return BnNodeList{mImpl.get(), mImpl->input_id_list()};
}

// @brief 外部入力ノードのノードのリストを得る．
BnNodeList
BnNetwork::primary_input_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return BnNodeList{mImpl.get(), mImpl->primary_input_id_list()};
}

// @brief 出力数を得る．
SizeType
BnNetwork::output_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output_num();
}

// @brief 出力ノードのノード番号を得る．
SizeType
BnNetwork::output_id(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output_id(pos);
}

// @brief 出力ノードのノードのリストを得る．
BnNodeList
BnNetwork::output_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return BnNodeList{mImpl.get(), mImpl->output_id_list()};
}

// @brief 外部出力ノードのノードのリストを得る．
BnNodeList
BnNetwork::primary_output_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return BnNodeList{mImpl.get(), mImpl->primary_output_id_list()};
}

// @brief 論理ノード数を得る．
SizeType
BnNetwork::logic_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->logic_num();
}

// @brief 論理ノードのノード番号を得る．
SizeType
BnNetwork::logic_id(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->logic_id(pos);
}

// @brief 論理ノードのリストを得る．
BnNodeList
BnNetwork::logic_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return BnNodeList{mImpl.get(), mImpl->logic_id_list()};
}

// @brief 実装可能な構造を持っている時 true を返す．
bool
BnNetwork::is_concrete() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->is_concrete();
}

// @brief 全てのノードがセル割当情報を持つ時 true を返す．
bool
BnNetwork::is_mapped() const
{
  ASSERT_COND( mImpl != nullptr );
  return mImpl->is_mapped();
}

// @brief 関数の数を得る．
SizeType
BnNetwork::func_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->func_num();
}

// @brief 関数番号から関数を得る．
const TvFunc&
BnNetwork::func(
  SizeType func_id
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->func(func_id);
}

// @brief 論理式の数を得る．
SizeType
BnNetwork::expr_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->expr_num();
}

// @brief 論理式番号から論理式を得る．
Expr
BnNetwork::expr(
  SizeType expr_id
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->expr(expr_id);
}


//////////////////////////////////////////////////////////////////////
// BnPortListIter
//////////////////////////////////////////////////////////////////////

// @brief 内容を取り出す．
BnPort
BnPortListIter::operator*() const
{
  return BnPort{mNetwork, mPos};
}


//////////////////////////////////////////////////////////////////////
// BnPortList
//////////////////////////////////////////////////////////////////////

// @brief 先頭の反復子を返す．
BnPortList::iterator
BnPortList::begin() const
{
  return iterator{mNetwork, 0};
}

// @brief 末尾の反復子を返す．
BnPortList::iterator
BnPortList::end() const
{
  return iterator{mNetwork, mNetwork->port_num()};
}


//////////////////////////////////////////////////////////////////////
// BnDffListIter
//////////////////////////////////////////////////////////////////////

// @brief 内容を取り出す．
BnDff
BnDffListIter::operator*() const
{
  return BnDff{mNetwork, mPos};
}


//////////////////////////////////////////////////////////////////////
// BnDffList
//////////////////////////////////////////////////////////////////////

// @brief 先頭の反復子を返す．
BnDffList::iterator
BnDffList::begin() const
{
  return iterator{mNetwork, 0};
}

// @brief 末尾の反復子を返す．
BnDffList::iterator
BnDffList::end() const
{
  return iterator{mNetwork, mNetwork->dff_num()};
}


//////////////////////////////////////////////////////////////////////
// BnNodeList
//////////////////////////////////////////////////////////////////////

// @brief 要素を返す．
BnNode
BnNodeList::operator[](
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < size() );
  return BnNode{mNetwork, mIdList[pos]};
}

// @brief 先頭の反復子を返す．
BnNodeList::iterator
BnNodeList::begin() const
{
  return iterator{mNetwork, mIdList.begin()};
}

// @brief 末尾の反復子を返す．
BnNodeList::iterator
BnNodeList::end() const
{
  return iterator{mNetwork, mIdList.end()};
}


//////////////////////////////////////////////////////////////////////
// BnAllNodeListIter
//////////////////////////////////////////////////////////////////////

// @brief 内容を取り出す．
BnNode
BnAllNodeListIter::operator*() const
{
  return BnNode{mNetwork, mPos + 1};
}

// @brief 一つ進める．
BnAllNodeListIter&
BnAllNodeListIter::operator++()
{
  if ( mPos < mNetwork->node_num() ) {
    ++ mPos;
  }
  return *this;
}

//////////////////////////////////////////////////////////////////////
// BnAllNodeList
//////////////////////////////////////////////////////////////////////

/// @brief 先頭の反復子を返す．
BnAllNodeList::iterator
BnAllNodeList::begin() const
{
  return iterator{mNetwork, 0};
}

// @brief 末尾の反復子を返す．
BnAllNodeList::iterator
BnAllNodeList::end() const
{
  return iterator{mNetwork, mNetwork->node_num()};
}

END_NAMESPACE_YM_BNET
