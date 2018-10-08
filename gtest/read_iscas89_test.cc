
/// @file ReadIscas89Test.cc
/// @brief ReadIscas89Test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

TEST(ReadIscas89Test, test)
{
  BnNetwork network;

  string filename = "b10.bench";
  string path = DATAPATH + filename;
  bool stat = read_iscas89(network, path);
  EXPECT_TRUE( stat );
  EXPECT_EQ( 11 + 17 + 1, network.input_num() );
  EXPECT_EQ( 6 + 17 + 17, network.output_num() );
  EXPECT_EQ( 172, network.logic_num() );
  EXPECT_EQ( 11 + 6 + 1, network.port_num() );
  EXPECT_EQ( 17, network.dff_num() );
}

END_NAMESPACE_YM
