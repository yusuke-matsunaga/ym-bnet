
/// @file BlifBnBuilderTest.cc
/// @brief BlifBnBuilderTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BlifBnBuilder.h"


BEGIN_NAMESPACE_YM_BNET

int
BlifBnBuilderTest(int argc,
		  char** argv)
{
  if ( argc < 2 ) {
    return -1;
  }

  BlifBnBuilder builder;

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

END_NAMESPACE_YM_BNET

int
main(int argc,
     char** argv)
{
  return nsYm::nsBnet::BlifBnBuilderTest(argc, argv);
}
