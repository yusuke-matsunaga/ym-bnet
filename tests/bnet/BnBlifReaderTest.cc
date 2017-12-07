
/// @file BnBlifReaderTest.cc
/// @brief BnBlifReaderTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnBlifReader.h"
#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

int
BnBlifReaderTest(int argc,
		 char** argv)
{
  if ( argc != 2 ) {
    cerr << "USAGE: BnBlifReaderTest <blif-filename>" << endl;
    return -1;
  }

  BnNetwork network;

  string filename = argv[1];
  bool stat = BnBlifReader::read(network, filename);
  if ( !stat ) {
    cerr << "read_blif(" << filename << ") failed" << endl;
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
  return nsYm::BnBlifReaderTest(argc, argv);
}
