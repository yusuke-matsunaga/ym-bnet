
/// @file BnPort.cc
/// @brief BnPort の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnPort.h"
#include "BnNetworkImpl.h"


BEGIN_NAMESPACE_YM_BNET

// @brief 名前を得る．
string
BnPort::name() const
{
  auto impl = mNetwork->_port(mId);
  return impl->name();
}

// @brief ビット数を得る．
SizeType
BnPort::bit_width() const
{
  auto impl = mNetwork->_port(mId);
  return impl->bit_width();
}

// @brief pos ビット目のノード番号を得る．
BnNode
BnPort::bit(
  SizeType pos
) const
{
  auto impl = mNetwork->_port(mId);
  return BnNode{mNetwork, impl->bit(pos)};
}

END_NAMESPACE_YM_BNET
