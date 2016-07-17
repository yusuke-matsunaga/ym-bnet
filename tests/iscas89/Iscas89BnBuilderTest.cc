
/// @file Iscas89BnBuilderTest.cc
/// @brief Iscas89BnBuilder::read_iscas89() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "Iscas89BnBuilder.h"


BEGIN_NAMESPACE_YM_BNET

int
Iscas89BnBuilderTest(int argc,
		       char** argv)
{
  if ( argc < 2 ) {
    return -1;
  }

  Iscas89BnBuilder builder;

  string filename = argv[1];
  if ( !builder.read_iscas89(filename) ) {
    cerr << "read_iscas(" << filename << ") failed" << endl;
    return -1;
  }

  builder.write(cout);

  return 0;
}

END_NAMESPACE_YM_BNET

int
main(int argc,
     char** argv)
{
  return nsYm::nsBnet::Iscas89BnBuilderTest(argc, argv);
}
