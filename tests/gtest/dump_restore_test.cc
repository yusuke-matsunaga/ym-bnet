
/// @file dump_restore_test.cc
/// @brief dump_restore_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

TEST(DumpRestoreTest, test1)
{
  string filename = "s5378.blif";
  string path = DATAPATH + filename;
  BnNetwork network = BnNetwork::read_blif(path);

  ostringstream obuf;
  BinEnc enc{obuf};
  network.dump(enc);

  istringstream ibuf{obuf.str()};
  BinDec dec{ibuf};
  BnNetwork network2 = BnNetwork::restore(dec);

  int ni = 35;   // 入力数
  int no = 49;   // 出力数
  int nd = 179;  // D-FF数
  int ng = 2779; // ゲート数
  EXPECT_EQ( ni + nd + 1, network2.input_num() ); // +1 はクロック入力
  EXPECT_EQ( no + nd + nd, network2.output_num() ); // + nd はクロック出力(D-FFへの入力)
  EXPECT_EQ( ng, network2.logic_num() );
  EXPECT_EQ( ni + no + 1, network2.port_num() ); // +1 はクロック
  EXPECT_EQ( nd, network2.dff_num() );
}

END_NAMESPACE_YM
