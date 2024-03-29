
/// @file BnIscas89ReaderTest.cc
/// @brief BnIscas89ReaderTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/MsgMgr.h"
#include "ym/StreamMsgHandler.h"


BEGIN_NAMESPACE_YM

void
write_network(
  const BnNetwork& network,
  ostream& s,
  bool blif,
  bool iscas89,
  bool verilog
)
{
  if ( blif ) {
    network.write_blif(s);
  }
  else if ( iscas89 ) {
    network.write_iscas89(s);
  }
  else if ( verilog ) {
    network.write_verilog(s);
  }
  else {
    network.write(s);
  }
}

int
BnIscas89ReaderTest(
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
    else if ( strcmp(argv[argbase], "--copy") == 0 ) {
      copy = true;
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
    cerr << "USAGE: BnIscas89ReaderTest [--blif|--iscas89] <iscas89-filename>" << endl;
    return -1;
  }

  StreamMsgHandler handler{cout};
  MsgMgr::attach_handler(&handler);

  string filename = argv[argbase];
  BnNetwork network = BnNetwork::read_iscas89(filename);
  if ( network.node_num() == 0 ) {
    cerr << "read_iscas89(" << filename << ") failed" << endl;
    return -1;
  }

  if ( copy ) {
    BnNetwork network2{network};
    write_network(network2, cout, blif, iscas89, verilog);
  }
  else {
    write_network(network, cout, blif, iscas89, verilog);
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
  return nsYm::BnIscas89ReaderTest(argc, argv);
}
