
/// @file BnNetwork_dump_restore.cc
/// @brief BnNetwork::dump(), restore() の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/Range.h"
#include "BinIO.h"


BEGIN_NAMESPACE_YM_BNET


// @brief 内容を独自形式でバイナリダンプする．
void
BnNetwork::dump(
  BinEnc& s
) const
{
  BinIO bio;
  bio.dump(s, *this);
}

// @brief バイナリダンプされた内容を復元する．
BnNetwork
BnNetwork::restore(
  BinDec& s
)
{
  BinIO bio;
  BnNetwork network;
  bio.restore(s, network.mImpl.get());
  return network;
}

END_NAMESPACE_YM_BNET
