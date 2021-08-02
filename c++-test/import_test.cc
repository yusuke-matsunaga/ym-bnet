
/// @file import_test.cc
/// @brief import_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"


BEGIN_NAMESPACE_YM

TEST(ImportTest, test1)
{
  BnNetwork network1;

  { // network1 は２入力ANDからなるネットワーク
    auto port1_id = network1.new_input_port("port1");
    auto port2_id = network1.new_input_port("port2");
    auto port3_id = network1.new_output_port("port3");

    auto& port1 = network1.port(port1_id);
    auto& port2 = network1.port(port2_id);
    auto& port3 = network1.port(port3_id);

    auto input1 = port1.bit(0);
    auto input2 = port2.bit(0);

    auto and1 = network1.new_and(string(), 2);
    network1.connect(input1, and1, 0);
    network1.connect(input2, and1, 1);

    auto output1 = port3.bit(0);
    network1.connect(and1, output1, 0);

    network1.wrap_up();
  }

  BnNetwork network2;

  {
    auto port1_id = network2.new_input_port("port1");
    auto port2_id = network2.new_input_port("port2");
    auto port3_id = network2.new_input_port("port3");
    auto port4_id = network2.new_input_port("port4");

    auto& port1 = network2.port(port1_id);
    auto& port2 = network2.port(port1_id);
    auto& port3 = network2.port(port1_id);
    auto& port4 = network2.port(port1_id);

    auto input1 = port1.bit(0);
    auto input2 = port2.bit(0);
    auto input3 = port3.bit(0);
    auto input4 = port4.bit(0);

    vector<SizeType> input_list1{input1, input2};
    vector<SizeType> output_list1;
    network2.import_subnetwork(network1, input_list1, output_list1);
    ASSERT_COND( output_list1.size() == 1 );

    vector<SizeType> input_list2{input3, input4};
    vector<SizeType> output_list2;
    network2.import_subnetwork(network1, input_list2, output_list2);
    ASSERT_COND( output_list2.size() == 1 );

    auto or_id = network2.new_or(string(), 2);
    network2.connect(output_list1[0], or_id, 0);
    network2.connect(output_list2[0], or_id, 1);

    auto port5_id = network2.new_output_port("port5");
    auto& port5 = network2.port(port5_id);
    auto output = port5.bit(0);

    network2.connect(or_id, output, 0);

    network2.wrap_up();
  }
}

END_NAMESPACE_YM
