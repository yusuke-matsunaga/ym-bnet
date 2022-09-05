
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
  mImpl->copy(*src.mImpl);
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
  mImpl->copy(*src.mImpl);

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

  mImpl->copy(*src.mImpl);
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
const BnPort&
BnNetwork::port(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->port(pos);
}

// @brief ポートのリストを得る．
BnPortList
BnNetwork::port_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return BnPortList{*this};
}

// @brief DFF数を得る．
SizeType
BnNetwork::dff_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->dff_num();
}

// @brief DFFを得る．
const BnDff&
BnNetwork::dff(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->dff(pos);
}

// @brief DFFのリストを得る．
BnDffList
BnNetwork::dff_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return BnDffList{*this};
}

// @brief ノード数を得る．
SizeType
BnNetwork::node_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->node_num();
}

// @brief ノードを得る．
const BnNode&
BnNetwork::node(
  SizeType id
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->node(id);
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

  return BnNodeList{*this, mImpl->input_id_list()};
}

// @brief 外部入力ノードのノードのリストを得る．
BnNodeList
BnNetwork::primary_input_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return BnNodeList{*this, mImpl->primary_input_id_list()};
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

  return BnNodeList{*this, mImpl->output_id_list()};
}

// @brief 外部出力ノードのノードのリストを得る．
BnNodeList
BnNetwork::primary_output_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return BnNodeList{*this, mImpl->primary_output_id_list()};
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

  return BnNodeList{*this, mImpl->logic_id_list()};
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
// BnNode
//////////////////////////////////////////////////////////////////////

// @brief ファンインのノードを返す．
const BnNode&
BnNode::fanin(
  SizeType pos,            ///< [in] 位置番号 ( 0 <= pos < fanin_num() )
  const BnNetwork& network ///< [in] 対象のネットワーク
) const
{
  auto id = fanin_id(pos);
  ASSERT_COND( id != BNET_NULLID );
  return network.node(id);
}

// @brief ファンインのノードのリストを返す．
BnNodeList
BnNode::fanin_list(
  const BnNetwork& network
) const
{
  return BnNodeList{network, fanin_id_list()};
}


//////////////////////////////////////////////////////////////////////
// BnPortListIter
//////////////////////////////////////////////////////////////////////

// @brief 内容を取り出す．
const BnPort&
BnPortListIter::operator*() const
{
  return mNetwork.port(mPos);
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
  return iterator{mNetwork, mNetwork.port_num()};
}


//////////////////////////////////////////////////////////////////////
// BnDffListIter
//////////////////////////////////////////////////////////////////////

// @brief 内容を取り出す．
const BnDff&
BnDffListIter::operator*() const
{
  return mNetwork.dff(mPos);
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
  return iterator{mNetwork, mNetwork.dff_num()};
}


//////////////////////////////////////////////////////////////////////
// BnNodeListIter
//////////////////////////////////////////////////////////////////////

// @brief 内容を取り出す．
const BnNode&
BnNodeListIter::operator*() const
{
  auto id = *mIter;
  ASSERT_COND( id != BNET_NULLID );
  return mNetwork.node(id);
}


//////////////////////////////////////////////////////////////////////
// BnNodeList
//////////////////////////////////////////////////////////////////////

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
// BnNodeType
//////////////////////////////////////////////////////////////////////

// cython 用の変換関数
int
__bnnodetype_to_int(
  BnNodeType type
)
{
  switch ( type ) {
  case BnNodeType::None:   return 0;
  case BnNodeType::Input:  return 1;
  case BnNodeType::Output: return 2;
  case BnNodeType::C0:     return 3;
  case BnNodeType::C1:     return 4;
  case BnNodeType::Buff:   return 5;
  case BnNodeType::Not:    return 6;
  case BnNodeType::And:    return 7;
  case BnNodeType::Nand:   return 8;
  case BnNodeType::Or:     return 9;
  case BnNodeType::Nor:    return 10;
  case BnNodeType::Xor:    return 11;
  case BnNodeType::Xnor:   return 12;
  case BnNodeType::Expr:   return 13;
  case BnNodeType::TvFunc: return 14;
  case BnNodeType::Bdd:    return 15;
  case BnNodeType::Cell:   return 16;
  defult: ASSERT_NOT_REACHED; break;
  }
  return 0;
}

BnNodeType
__int_to_bnnodetype(
  int val
)
{
  static BnNodeType tbl[] {
    BnNodeType::None,
    BnNodeType::Input,
    BnNodeType::Output,
    BnNodeType::C0,
    BnNodeType::C1,
    BnNodeType::Buff,
    BnNodeType::Not,
    BnNodeType::And,
    BnNodeType::Nand,
    BnNodeType::Or,
    BnNodeType::Nor,
    BnNodeType::Xor,
    BnNodeType::Xnor,
    BnNodeType::Expr,
    BnNodeType::TvFunc,
    BnNodeType::Bdd,
    BnNodeType::Cell
  };

  ASSERT_COND( 0 <= val && val < sizeof(tbl) / sizeof(BnNodeType) );
  return tbl[val];
}

END_NAMESPACE_YM_BNET
