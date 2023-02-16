
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
  auto network = BnNetwork::read_iscas89(path);
  int ni = 11;
  int no = 6;
  int nd = 17;
  int ng = 172;
  EXPECT_EQ( ni + nd + 1, network.input_num() );
  EXPECT_EQ( no + nd + nd, network.output_num() );
  EXPECT_EQ( ng, network.logic_num() );
  EXPECT_EQ( ni + no + 1, network.port_num() );
  EXPECT_EQ( nd, network.dff_num() );

  // 出力結果の回帰テスト
  ostringstream s1;
  network.write(s1);

  string ref_path = DATAPATH + string{"b10.bnet"};
  ifstream s2{ref_path};
  ASSERT_TRUE( s2 );
  string ref_contents;
  string buff;
  while ( getline(s2, buff) ) {
    ref_contents += buff + '\n';
  }

  EXPECT_EQ( ref_contents, s1.str() );
}

TEST(ReadIscas89Test, const_test)
{
  string filename = "const.bench";
  string path = DATAPATH + filename;
  auto network = BnNetwork::read_iscas89(path);
  int ni = 1;
  int no = 1;
  int nd = 0;
  int ng = 2;
  EXPECT_EQ( ni + nd, network.input_num() );
  EXPECT_EQ( no + nd + nd, network.output_num() );
  EXPECT_EQ( ng, network.logic_num() );
  EXPECT_EQ( ni + no, network.port_num() );
  EXPECT_EQ( nd, network.dff_num() );
}

TEST(ReadIscas89Test, mux_test)
{
  string filename = "mux2.bench";
  string path = DATAPATH + filename;
  auto network = BnNetwork::read_iscas89(path);
  int ni = 3;
  int no = 1;
  int nd = 0;
  int ng = 1;
  EXPECT_EQ( ni, network.input_num() );
  EXPECT_EQ( no, network.output_num() );
  EXPECT_EQ( ng, network.logic_num() );
  EXPECT_EQ( ni + no, network.port_num() );
  EXPECT_EQ( nd, network.dff_num() );

  // 出力結果の回帰テスト
  ostringstream s1;
  network.write(s1);

  string ref_path = DATAPATH + string{"mux2.bnet"};
  ifstream s2{ref_path};
  ASSERT_TRUE( s2 );
  string ref_contents;
  string buff;
  while ( getline(s2, buff) ) {
    ref_contents += buff + '\n';
  }

  EXPECT_EQ( ref_contents, s1.str() );
}

TEST(ReadIscas89Test, file_not_found)
{
  EXPECT_THROW({
      auto _ = BnNetwork::read_iscas89("file_not_found.bench");
    }, std::invalid_argument );
}

TEST(ReadIscas89Test, wrong_data)
{
  string filename = "broken.bench";
  string path = DATAPATH + filename;
  EXPECT_THROW({
      auto _ = BnNetwork::read_iscas89(path);
    }, std::invalid_argument );
}

END_NAMESPACE_YM
