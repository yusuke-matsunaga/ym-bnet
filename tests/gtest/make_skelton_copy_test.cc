
/// @file make_skelton_test.cc
/// @brief make_skelton_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "ym/BnNetwork.h"
#include "ym/BnModifier.h"
#include "ym/BnPort.h"


BEGIN_NAMESPACE_YM

TEST(BnNetworkTest, make_skelton_copy)
{
  string filename = "s5378.blif";
  string path = DATAPATH + filename;
  BnNetwork src_network = BnNetwork::read_blif(path);

  BnModifier mod;
  auto node_map = mod.make_skelton_copy(src_network);
  BnNetwork dst_network{std::move(mod)};

  SizeType np = src_network.port_num();
  for ( SizeType i = 0; i < np; ++ i ) {
    auto& src_port = src_network.port(i);
    auto& dst_port = dst_network.port(i);
    EXPECT_EQ( src_port.id(), dst_port.id() );
    SizeType nb = src_port.bit_width();
    EXPECT_EQ( nb, dst_port.bit_width() );
    for ( SizeType j = 0; j < nb; ++ j ) {
      SizeType src_id = src_port.bit(j);
      SizeType dst_id = dst_port.bit(j);
      EXPECT_TRUE( node_map.count(src_id) > 0 );
      EXPECT_EQ( dst_id, node_map.at(src_id) );
    }
  }
}

END_NAMESPACE_YM
