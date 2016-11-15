
/// @file BnIscas89ReaderTest.cc
/// @brief BnIscas89ReaderTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnIscas89Reader.h"
#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

int
BnIscas89ReaderTest(int argc,
		 char** argv)
{
  if ( argc != 2 ) {
    cerr << "USAGE: BnIscas89ReaderTest <iscas89-filename>" << endl;
    return -1;
  }

  BnNetwork network;

  string filename = argv[1];
  bool stat = BnIscas89Reader::read(network, filename);
  if ( !stat ) {
    cerr << "read_iscas89(" << filename << ") failed" << endl;
    return -1;
  }

  network.write(cout);

  return 0;
}

END_NAMESPACE_YM

int
main(int argc,
     char** argv)
{
  return nsYm::BnIscas89ReaderTest(argc, argv);
}
