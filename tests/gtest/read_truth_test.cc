
/// @file read_truth_test.cc
/// @brief BnNetwork::read_truth() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

TEST(ReadTruthTest, test)
{
  string filename = "ex00.truth";
  string path = DATAPATH + filename;
  BnNetwork network = BnNetwork::read_truth(path);
  int ni = 6;   // 入力数
  int no = 1;   // 出力数
  int nd = 0;  // D-FF数
  EXPECT_EQ( ni + nd, network.input_num() ); // +1 はクロック入力
  EXPECT_EQ( no + nd + nd, network.output_num() ); // + nd はクロック出力(D-FFへの入力)
  EXPECT_EQ( no, network.logic_num() );
  EXPECT_EQ( ni + no, network.port_num() ); // +1 はクロック
  EXPECT_EQ( nd, network.dff_num() );
}

TEST(ReadTruthTest, file_not_found)
{
  EXPECT_THROW( {
      auto _ = BnNetwork::read_truth("file_not_found.trueh");
    }, BnetError );
}

TEST(ReadTruthTest, wrong_data)
{
  string filename = "broken.truth";
  string path = DATAPATH + filename;
  EXPECT_THROW( {
      auto _ = BnNetwork::read_truth(path);
    }, BnetError );
}

END_NAMESPACE_YM
