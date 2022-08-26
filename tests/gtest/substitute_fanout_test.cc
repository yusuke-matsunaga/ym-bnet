
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
#include "ym/BnModifier.h"


BEGIN_NAMESPACE_YM

TEST(SubstituteFanoutTest, test1)
{
  BnNetwork network1;
  SizeType and1;
  SizeType input1;
  SizeType input2;
  {
    BnModifier mod1;
    SizeType port1_id = mod1.new_input_port("port1");
    SizeType port2_id = mod1.new_input_port("port2");
    SizeType port3_id = mod1.new_input_port("port3");
    SizeType port4_id = mod1.new_output_port("port4");
    SizeType port5_id = mod1.new_output_port("port5");

    auto& port1 = mod1.port(port1_id);
    auto& port2 = mod1.port(port2_id);
    auto& port3 = mod1.port(port3_id);
    auto& port4 = mod1.port(port4_id);
    auto& port5 = mod1.port(port5_id);

    input1 = port1.bit(0);
    input2 = port2.bit(0);
    SizeType input3 = port3.bit(0);

    and1 = mod1.new_and(string(), {input1, input2});
    SizeType xor1 = mod1.new_xor(string(), {and1, input3});

    SizeType output1 = port4.bit(0);
    mod1.set_output_src(output1, and1);

    SizeType output2 = port5.bit(0);
    mod1.set_output_src(output2, xor1);
    network1.move(std::move(mod1));
  }

  auto& node1 = network1.node(and1);
  EXPECT_EQ( 2, node1.fanout_num() );

  // 新たに２入力ORを追加
  SizeType or1;
  {
    BnModifier mod2{std::move(network1)};
    or1 = mod2.new_or(string(), {input1, input2});

    auto& node2 = mod2.node(or1);
    EXPECT_EQ( 0, node2.fanout_num() );

    // and1 のファンアウトを or1 につなぎ替える．
    mod2.substitute_fanout(and1, or1);
    network1.move(std::move(mod2));
  }
  auto& node2 = network1.node(or1);

  EXPECT_EQ( 0, node1.fanout_num() );
  EXPECT_EQ( 2, node2.fanout_num() );
}

TEST(SubstituteFanoutTest, test2)
{
  BnModifier mod1;
  auto port1_id = mod1.new_input_port("port1");
  auto port2_id = mod1.new_input_port("port2");
  auto port3_id = mod1.new_input_port("port3");
  auto port4_id = mod1.new_output_port("port4");
  auto port5_id = mod1.new_output_port("port5");

  auto& port1 = mod1.port(port1_id);
  auto& port2 = mod1.port(port2_id);
  auto& port3 = mod1.port(port3_id);
  auto& port4 = mod1.port(port4_id);
  auto& port5 = mod1.port(port5_id);

  auto input1 = port1.bit(0);
  auto input2 = port2.bit(0);
  auto input3 = port3.bit(0);

  auto and1 = mod1.new_and(string(), {input1, input2});
  auto xor1 = mod1.new_xor(string(), {and1, input3});

  auto output1 = port4.bit(0);
  mod1.set_output_src(output1, and1);

  auto output2 = port5.bit(0);
  mod1.set_output_src(output2, xor1);

  BnNetwork network1{std::move(mod1)};

  auto& node1 = network1.node(and1);
  EXPECT_EQ( 2, node1.fanout_num() );

  // 新たに２入力ORを追加
  BnModifier mod2{std::move(network1)};
  auto or1 = mod2.new_or(string(), {input1, input2});

  // and1 のファンアウトを or1 につなぎ替える．
  mod2.substitute_fanout(and1, or1);

  network1 = std::move(mod2);

  auto& node2 = network1.node(or1);

  EXPECT_EQ( 0, node1.fanout_num() );
  EXPECT_EQ( 2, node2.fanout_num() );
  EXPECT_EQ( or1, network1.output_src_id(0) );
}

END_NAMESPACE_YM
