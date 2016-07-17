
/// @file BnBuilder_iscas89_test.cc
/// @brief BnBuilder::read_iscas89() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnBuilder.h"


BEGIN_NAMESPACE_YM

int
BnBuilder_iscas89_test(int argc,
		       char** argv)
{
  if ( argc < 2 ) {
    return -1;
  }

  BnBuilder builder;

  string filename = argv[1];
  if ( !builder.read_iscas89(filename) ) {
    cerr << "read_iscas(" << filename << ") failed" << endl;
    return -1;
  }

  builder.write(cout);

  return 0;
}

END_NAMESPACE_YM

int
main(int argc,
     char** argv)
{
  return nsYm::BnBuilder_iscas89_test(argc, argv);
}
