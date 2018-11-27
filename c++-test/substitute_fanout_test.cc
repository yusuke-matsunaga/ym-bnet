
/// @file substitute_fanout_test.cc
/// @brief substitute_fanout_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include <gtest/gtest.h>
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnNode.h"


BEGIN_NAMESPACE_YM

TEST(SubstituteFanoutTest, test1)
{
  BnNetwork network1;

  int port1_id = network1.new_input_port("port1");
  int port2_id = network1.new_input_port("port2");
  int port3_id = network1.new_input_port("port3");
  int port4_id = network1.new_output_port("port4");
  int port5_id = network1.new_output_port("port5");

  auto& port1 = network1.port(port1_id);
  auto& port2 = network1.port(port2_id);
  auto& port3 = network1.port(port3_id);
  auto& port4 = network1.port(port4_id);
  auto& port5 = network1.port(port5_id);

  int input1 = port1.bit(0);
  int input2 = port2.bit(0);
  int input3 = port3.bit(0);

  int and1 = network1.new_and(string(), 2);
  network1.connect(input1, and1, 0);
  network1.connect(input2, and1, 1);

  int xor1 = network1.new_xor(string(), 2);
  network1.connect(and1, xor1, 0);
  network1.connect(input3, xor1, 1);

  int output1 = port4.bit(0);
  network1.connect(and1, output1, 0);

  int output2 = port5.bit(0);
  network1.connect(xor1, output2, 0);

  network1.wrap_up();

  auto& node1 = network1.node(and1);
  EXPECT_EQ( 2, node1.fanout_num() );

  // 新たに２入力ORを追加
  int or1 = network1.new_or(string(), 2);
  network1.connect(input1, or1, 0);
  network1.connect(input2, or1, 1);

  auto& node2 = network1.node(or1);
  EXPECT_EQ( 0, node2.fanout_num() );

  // and1 のファンアウトを or1 につなぎ替える．
  network1.substitute_fanout(and1, or1);

  network1.wrap_up();

  EXPECT_EQ( 0, node1.fanout_num() );
  EXPECT_EQ( 2, node2.fanout_num() );
}

END_NAMESPACE_YM
