
/// @file ReadBlifTest.cc
/// @brief ReadBlifTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

TEST(ReadBlifTest, test)
{
  string filename = "s5378.blif";
  string path = DATAPATH + filename;
  auto network = BnNetwork::read_blif(path);
  int ni = 35;   // 入力数
  int no = 49;   // 出力数
  int nd = 179;  // D-FF数
  int ng = 2779; // ゲート数
  EXPECT_EQ( ni + nd + 1, network.input_num() ); // +1 はクロック入力
  EXPECT_EQ( no + nd + nd, network.output_num() ); // + nd はクロック出力(D-FFへの入力)
  EXPECT_EQ( ng, network.logic_num() );
  EXPECT_EQ( ni + no + 1, network.port_num() ); // +1 はクロック
  EXPECT_EQ( nd, network.dff_num() );

  // 出力結果の回帰テスト
  ostringstream s1;
  network.write(s1);

  string ref_filename = "s5378.bnet";
  string ref_path = DATAPATH + ref_filename;
  ifstream s2{ref_path};
  ASSERT_TRUE( s2 );
  string ref_contents;
  string buff;
  while ( getline(s2, buff) ) {
    ref_contents += buff + '\n';
  }

  EXPECT_EQ( ref_contents, s1.str() );
}

TEST(ReadBlifTest, file_not_found)
{
  EXPECT_THROW( {
      auto _ = BnNetwork::read_blif("not_exist_file");
    }, std::invalid_argument );
}

TEST(ReadBlifTest, wrong_data)
{
  string filename = "broken.blif";
  string path = DATAPATH + filename;
  EXPECT_THROW( {
      auto _ = BnNetwork::read_blif(path);
    }, std::invalid_argument );
}
END_NAMESPACE_YM
