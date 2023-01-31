
/// @file write_aig_test.cc
/// @brief write_aig_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

int
write_aig_test(
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
  else if ( ext == "blif" ) {
    network = BnNetwork::read_blif(filename);
  }
  else if ( ext == "bench" ) {
    network = BnNetwork::read_iscas89(filename);
  }

  network.write_aig(cout);

  return 0;
}

END_NAMESPACE_YM


int
main(
  int argc,
  char** argv
)
{
  return YM_NAMESPACE::write_aig_test(argc, argv);
}
