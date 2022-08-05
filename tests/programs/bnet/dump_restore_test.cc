
/// @file dump_restore_test.cc
/// @brief BnNetwork::dump(), restore() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

int
dump_restore_test(
  int argc,
  char** argv
)
{
  bool blif = false;
  bool iscas89 = false;
  bool verilog = false;
  bool copy = false;

  int argbase = 1;
  while ( argc > argbase ) {
    if ( strcmp(argv[argbase], "--blif") == 0 ) {
      blif = true;
      ++ argbase;
    }
    else if ( strcmp(argv[argbase], "--iscas89") == 0 ) {
      iscas89 = true;
      ++ argbase;
    }
    else if ( strcmp(argv[argbase], "--verilog") == 0 ) {
      verilog = true;
      ++ argbase;
    }
    else if ( strncmp(argv[argbase], "--", 2) == 0 ) {
      cerr << argv[argbase] << ": Unknown option, ignoared" << endl;
      ++ argbase;
    }
    else {
      break;
    }
  }

  if ( argc - argbase != 1 ) {
    cerr << "USAGE: dump_restore_test [--blif|--iscas89] <iscas89-filename>" << endl;
    return -1;
  }

  string filename = argv[argbase];
  BnNetwork network = BnNetwork::read_blif(filename);
  if ( network.node_num() == 0 ) {
    cerr << "read_blif(" << filename << ") failed" << endl;
    return -1;
  }

  ostringstream obuf;
  BinEnc enc{obuf};
  network.dump(enc);

  istringstream ibuf{obuf.str()};
  BinDec dec{ibuf};
  auto network2 = BnNetwork::restore(dec);

  network2.write_blif(cout);

  return 0;
}

END_NAMESPACE_YM

int
main(
  int argc,
  char** argv
)
{
  return nsYm::dump_restore_test(argc, argv);
}
