
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
  BnNode and1;
  BnNode input1;
  BnNode input2;
  {
    BnModifier mod1;
    auto port1 = mod1.new_input_port("port1");
    auto port2 = mod1.new_input_port("port2");
    auto port3 = mod1.new_input_port("port3");
    auto port4 = mod1.new_output_port("port4");
    auto port5 = mod1.new_output_port("port5");

    input1 = port1.bit(0);
    input2 = port2.bit(0);
    auto input3 = port3.bit(0);

    and1 = mod1.new_and(string{}, {input1, input2});
    auto xor1 = mod1.new_xor(string(), {and1, input3});

    auto output1 = port4.bit(0);
    mod1.set_output_src(output1, and1);

    auto output2 = port5.bit(0);
    mod1.set_output_src(output2, xor1);
    network1.move(std::move(mod1));
  }

  EXPECT_EQ( 2, and1.fanout_num() );

  // 新たに２入力ORを追加
  BnNode or1;
  {
    BnModifier mod2{std::move(network1)};
    or1 = mod2.new_or(string{}, {input1, input2});

    EXPECT_EQ( 0, or1.fanout_num() );

    // and1 のファンアウトを or1 につなぎ替える．
    mod2.substitute_fanout(and1, or1);
    network1.move(std::move(mod2));
  }

  EXPECT_EQ( 0, and1.fanout_num() );
  EXPECT_EQ( 2, or1.fanout_num() );
}

TEST(SubstituteFanoutTest, test2)
{
  BnModifier mod1;
  auto port1 = mod1.new_input_port("port1");
  auto port2 = mod1.new_input_port("port2");
  auto port3 = mod1.new_input_port("port3");
  auto port4 = mod1.new_output_port("port4");
  auto port5 = mod1.new_output_port("port5");

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

  EXPECT_EQ( 2, and1.fanout_num() );

  // 新たに２入力ORを追加
  BnModifier mod2{std::move(network1)};
  auto or1 = mod2.new_or(string{}, {input1, input2});

  // and1 のファンアウトを or1 につなぎ替える．
  mod2.substitute_fanout(and1, or1);

  network1 = std::move(mod2);

  EXPECT_EQ( 0, and1.fanout_num() );
  EXPECT_EQ( 2, or1.fanout_num() );
  EXPECT_EQ( or1, network1.output_node(0).output_src() );
}

END_NAMESPACE_YM
