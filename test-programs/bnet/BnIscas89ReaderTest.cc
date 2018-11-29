
/// @file BnIscas89ReaderTest.cc
/// @brief BnIscas89ReaderTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

int
BnIscas89ReaderTest(int argc,
		    char** argv)
{
  bool blif = false;
  bool iscas89 = false;

  int argbase = 1;
  if ( strcmp(argv[argbase], "--blif") == 0 ) {
    blif = true;
    ++ argbase;
  }
  else if ( strcmp(argv[argbase], "--iscas89") == 0 ) {
    iscas89 = true;
    ++ argbase;
  }

  if ( argc - argbase != 1 ) {
    cerr << "USAGE: BnIscas89ReaderTest [--blif|--iscas89] <iscas89-filename>" << endl;
    return -1;
  }

  string filename = argv[argbase];
  BnNetwork network = BnNetwork::read_iscas89(filename);
  if ( network.node_num() == 0 ) {
    cerr << "read_iscas89(" << filename << ") failed" << endl;
    return -1;
  }

  if ( blif ) {
    network.write_blif(cout);
  }
  else if ( iscas89 ) {
    network.write_iscas89(cout);
  }
  else {
    network.write(cout);
  }

  return 0;
}

END_NAMESPACE_YM

int
main(int argc,
     char** argv)
{
  return nsYm::BnIscas89ReaderTest(argc, argv);
}
