
/// @file simple_decomp_test.cc
/// @brief simple_decomp_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include <gtest/gtest.h>
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnNode.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM

TEST(SimpleDecompTest, test1)
{
  BnNetwork network1;

  int port1_id = network1.new_input_port("port1");
  int port2_id = network1.new_input_port("port2");
  int port3_id = network1.new_input_port("port3");
  int port4_id = network1.new_output_port("port4");

  auto& port1 = network1.port(port1_id);
  auto& port2 = network1.port(port2_id);
  auto& port3 = network1.port(port3_id);
  auto& port4 = network1.port(port4_id);

  int input1 = port1.bit(0);
  int input2 = port2.bit(0);
  int input3 = port3.bit(0);

  VarId var0{0};
  VarId var1{1};
  VarId var2{2};
  Expr lit0{Expr::make_posi_literal(var0)};
  Expr lit1{Expr::make_posi_literal(var1)};
  Expr lit2{Expr::make_posi_literal(var2)};

  Expr expr = (lit0 & ~lit1) | (~lit1 & lit2);
  vector<int> fanin_id_list{input1, input2, input3};
  int id1 = network1.new_logic(string(), expr, fanin_id_list);

  int output1 = port4.bit(0);
  network1.connect(id1, output1, 0);

  network1.wrap_up();

  network1.write(cout);

  network1.simple_decomp();

  network1.write(cout);
}

END_NAMESPACE_YM
