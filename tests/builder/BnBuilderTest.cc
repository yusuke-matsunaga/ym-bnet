
/// @file BnBuilderTest.cc
/// @brief BnBuilderTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnBuilder.h"


BEGIN_NAMESPACE_YM

int
BnBuilderTest(int argc,
	      char** argv)
{
  if ( argc < 2 ) {
    return -1;
  }

  BnBuilder builder;

  string filename = argv[1];
  const CellLibrary* cell_library = nullptr;
  bool stat = builder.read_blif(filename, cell_library);

  if ( !stat ) {
    cerr << "read_blif(" << filename << ") failed" << endl;
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
  return nsYm::BnBuilderTest(argc, argv);
}
