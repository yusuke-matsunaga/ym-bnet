
/// @file read_aig_test.cc
/// @brief read_aig_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

int
read_aig_test(
  int argc,
  char** argv
)
{
  if ( argc != 2 ) {
    return 1;
  }

  string filename = argv[1];
  auto pos = filename.find_last_of(".");
  string ext;
  if ( pos != string::npos ) {
    ext = filename.substr(pos + 1);
  }
  BnNetwork network;
  if ( ext == "aig" ) {
    network = BnNetwork::read_aig(filename);
  }
  else if ( ext == "aag" ) {
    network = BnNetwork::read_aag(filename);
  }

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
  return YM_NAMESPACE::read_aig_test(argc, argv);
}
