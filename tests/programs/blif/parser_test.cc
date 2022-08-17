
/// @file parser_test.cc
/// @brief BlifParser のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "BlifParser.h"
#include "ym/ClibCellLibrary.h"
#include "TestBlifHandler.h"
#include "NullBlifHandler.h"
#include "ym/MsgMgr.h"
#include "ym/StreamMsgHandler.h"


void
usage(
  const char* argv0
)
{
  using namespace std;

  cerr << "USAGE : " << argv0 << " [-null] blif-file" << endl;
}

int
main(
  int argc,
  char** argv
)
{
  using namespace std;
  using namespace nsYm;
  using namespace nsYm::nsBnet;

  bool null = false;

  int base = 1;
  if ( argc == 3 && strcmp(argv[1], "-null") == 0 ) {
    null = true;
    base = 2;
  }

  if ( base + 1 != argc ) {
    usage(argv[0]);
    return 2;
  }

  string filename = argv[base];

#if !defined(YM_DEBUG)
  try {
#endif

    StreamMsgHandler msg_handler(cerr);
    MsgMgr::attach_handler(&msg_handler);

    BlifParser parser;
    bool stat = false;

    if ( null ) {
      NullBlifHandler __dummy(parser);

      stat = parser.read(filename);
    }
    else {
      TestBlifHandler __dummy(parser, cout);

      stat = parser.read(filename);
    }

    if ( !stat ) {
      cerr << "Error in reading " << filename << endl;
      return 4;
    }
#if !defined(YM_DEBUG)
  }
  catch ( const AssertError& x) {
    cout << x << endl;
  }
#endif

  return 0;
}
