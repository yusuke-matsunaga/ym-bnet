
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

  SizeType port1_id = network1.new_input_port("port1");
  SizeType port2_id = network1.new_input_port("port2");
  SizeType port3_id = network1.new_input_port("port3");
  SizeType port4_id = network1.new_output_port("port4");
  SizeType port5_id = network1.new_output_port("port5");

  auto& port1 = network1.port(port1_id);
  auto& port2 = network1.port(port2_id);
  auto& port3 = network1.port(port3_id);
  auto& port4 = network1.port(port4_id);
  auto& port5 = network1.port(port5_id);

  SizeType input1 = port1.bit(0);
  SizeType input2 = port2.bit(0);
  SizeType input3 = port3.bit(0);

  SizeType and1 = network1.new_and(string(), {input1, input2});
  SizeType xor1 = network1.new_xor(string(), {and1, input3});

  SizeType output1 = port4.bit(0);
  network1.set_output(output1, and1);

  SizeType output2 = port5.bit(0);
  network1.set_output(output2, xor1);

  network1.wrap_up();

  auto& node1 = network1.node(and1);
  EXPECT_EQ( 2, node1.fanout_num() );

  // 新たに２入力ORを追加
  SizeType or1 = network1.new_or(string(), {input1, input2});

  auto& node2 = network1.node(or1);
  EXPECT_EQ( 0, node2.fanout_num() );

  // and1 のファンアウトを or1 につなぎ替える．
  network1.substitute_fanout(and1, or1);

  network1.wrap_up();

  EXPECT_EQ( 0, node1.fanout_num() );
  EXPECT_EQ( 2, node2.fanout_num() );
}

END_NAMESPACE_YM
