
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
  string filename = "b10.bench";
  string path = DATAPATH + filename;
  BnNetwork network = BnNetwork::read_iscas89(path);
  int ni = 11;
  int no = 6;
  int nd = 17;
  int ng = 172;
  EXPECT_EQ( ni + nd + 1, network.input_num() );
  EXPECT_EQ( no + nd + nd, network.output_num() );
  EXPECT_EQ( ng, network.logic_num() );
  EXPECT_EQ( ni + no + 1, network.port_num() );
  EXPECT_EQ( nd, network.dff_num() );
}

END_NAMESPACE_YM
