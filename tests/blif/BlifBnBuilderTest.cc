
/// @file BlifBnBuilderTest.cc
/// @brief BlifBnBuilderTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnBlifReader.h"
#include "ym/BnBuilder.h"


BEGIN_NAMESPACE_YM

int
BlifBnBuilderTest(int argc,
		  char** argv)
{
  if ( argc < 2 ) {
    return -1;
  }

  BnBuilder builder;
  BnBlifReader reader;

  string filename = argv[1];
  const CellLibrary* cell_library = nullptr;
  bool stat = reader.read(builder, filename, cell_library);
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
  return nsYm::BlifBnBuilderTest(argc, argv);
}
