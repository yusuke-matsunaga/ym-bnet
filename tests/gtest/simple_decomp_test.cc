
/// @file simple_decomp_test.cc
/// @brief simple_decomp_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnNode.h"
#include "ym/Expr.h"
#include "ym/BnModifier.h"


BEGIN_NAMESPACE_YM

TEST(SimpleDecompTest, test1)
{
  BnNetwork network1;
  {
    BnModifier mod1;
    auto port1 = mod1.new_input_port("port1");
    auto port2 = mod1.new_input_port("port2");
    auto port3 = mod1.new_input_port("port3");
    auto port4 = mod1.new_output_port("port4");

    auto input1 = port1.bit(0);
    auto input2 = port2.bit(0);
    auto input3 = port3.bit(0);

    SizeType var0{0};
    SizeType var1{1};
    SizeType var2{2};
    auto lit0 = Expr::make_posi_literal(var0);
    auto lit1 = Expr::make_posi_literal(var1);
    auto lit2 = Expr::make_posi_literal(var2);

    auto expr = (lit0 & ~lit1) | (~lit1 & lit2);
    auto id1 = mod1.new_logic_expr({}, expr, {input1, input2, input3});

    auto output1 = port4.bit(0);
    mod1.set_output_src(output1, id1);
    network1.move(std::move(mod1));
  }

  auto network2 = network1.simple_decomp();

  EXPECT_EQ( network1.input_num(), network2.input_num() );
  EXPECT_EQ( network1.output_num(), network2.output_num() );
  for ( auto node: network2.logic_list() ) {
    EXPECT_TRUE( node.type() == BnNodeType::Prim );
  }
}

END_NAMESPACE_YM
