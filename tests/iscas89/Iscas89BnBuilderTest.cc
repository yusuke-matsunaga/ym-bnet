
/// @file Iscas89BnBuilderTest.cc
/// @brief Iscas89BnBuilder::read_iscas89() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnBuilder.h"
#include "ym/BnIscas89Reader.h"


BEGIN_NAMESPACE_YM

int
Iscas89BnBuilderTest(int argc,
		     char** argv)
{
  if ( argc < 2 ) {
    return -1;
  }

  BnBuilder builder;
  BnIscas89Reader reader;

  string filename = argv[1];
  bool stat = reader.read(builder, filename);
  if ( !stat ) {
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
  return nsYm::Iscas89BnBuilderTest(argc, argv);
}
