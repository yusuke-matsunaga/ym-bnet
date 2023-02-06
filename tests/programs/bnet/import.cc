
/// @file import_test.cc
/// @brief import_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnModifier.h"


BEGIN_NAMESPACE_YM

int
import_test(
  int argc,
  char** argv
)
{
  BnNetwork network1;
  { // network1 は２入力ANDからなるネットワーク
    BnModifier mod1;
    auto port1 = mod1.new_input_port("port1");
    auto port2 = mod1.new_input_port("port2");
    auto port3 = mod1.new_output_port("port3");

    auto input1 = port1.bit(0);
    auto input2 = port2.bit(0);

    auto and1 = mod1.new_and(string(), {input1, input2});

    auto output1 = port3.bit(0);
    mod1.set_output_src(output1, and1);
    network1.move(std::move(mod1));
  }

  BnNetwork network2;
  {
    BnModifier mod2;
    auto port1 = mod2.new_input_port("port1");
    auto port2 = mod2.new_input_port("port2");
    auto port3 = mod2.new_input_port("port3");
    auto port4 = mod2.new_input_port("port4");

    auto input1 = port1.bit(0);
    auto input2 = port2.bit(0);
    auto input3 = port3.bit(0);
    auto input4 = port4.bit(0);

    vector<BnNode> input_list1{input1, input2};
    auto output_list1 = mod2.import_subnetwork(network1, input_list1);
    ASSERT_COND( output_list1.size() == 1 );

    vector<BnNode> input_list2{input3, input4};
    auto output_list2 = mod2.import_subnetwork(network1, input_list2);
    ASSERT_COND( output_list2.size() == 1 );

    auto or_node = mod2.new_or(string(), {output_list1[0], output_list2[0]});

    auto port5 = mod2.new_output_port("port5");
    auto output = port5.bit(0);

    mod2.set_output_src(output, or_node);
    network2.move(std::move(mod2));
  }

  return 0;
}

END_NAMESPACE_YM

int
main(
  int argc,
  char** argv
)
{
  return YM_NAMESPACE::import_test(argc, argv);
}
