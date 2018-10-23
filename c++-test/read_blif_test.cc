
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
  int ni = 35;   // 入力数
  int no = 49;   // 出力数
  int nd = 179;  // D-FF数
  int ng = 2779; // ゲート数
  EXPECT_EQ( ni + nd + 1, network.input_num() ); // +1 はクロック入力
  EXPECT_EQ( no + nd + nd, network.output_num() ); // + nd はクロック出力(D-FFへの入力)
  EXPECT_EQ( ng, network.logic_num() );
  EXPECT_EQ( ni + no + 1, network.port_num() ); // +1 はクロック
  EXPECT_EQ( nd, network.dff_num() );
}

END_NAMESPACE_YM
