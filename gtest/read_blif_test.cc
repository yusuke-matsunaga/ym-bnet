
/// @file ReadBlifTest.cc
/// @brief ReadBlifTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

TEST(ReadBlifTest, test)
{
  BnNetwork network;

  string filename = "s5378.blif";
  string path = DATAPATH + filename;
  bool stat = read_blif(network, path);
  EXPECT_TRUE( stat );
  EXPECT_EQ( 215, network.input_num() );
  EXPECT_EQ( 407, network.output_num() );
  EXPECT_EQ( 2779, network.logic_num() );
  EXPECT_EQ( 35 + 49 + 1, network.port_num() );
  EXPECT_EQ( 179, network.dff_num() );
}

END_NAMESPACE_YM
