
/// @file ReadBlifTest.cc
/// @brief ReadBlifTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/BnNetwork.h"
#include "ym/BnBlifReader.h"


BEGIN_NAMESPACE_YM

TEST(ReadBlifTest, test)
{
  BnNetwork network;

  string filename = "s5378.blif";
  string path = DATAPATH + filename;
  bool stat = BnBlifReader::read(network, path);
  EXPECT_TRUE( stat );
}

END_NAMESPACE_YM
