
/// @file read_truth_test.cc
/// @brief read_truth_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

int
read_truth_test(
  int argc,
  char** argv
)
{
  if ( argc != 2 ) {
    return 1;
  }

  string filename = argv[1];

  auto network = BnNetwork::read_truth(filename);

  network.write(cout);

  return 0;
}

END_NAMESPACE_YM


int
main(
  int argc,
  char** argv
)
{
  return YM_NAMESPACE::read_truth_test(argc, argv);
}
