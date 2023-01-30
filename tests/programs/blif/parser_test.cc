
/// @file parser_test.cc
/// @brief BlifParser のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BlifParser.h"
#include "ym/BlifModel.h"
#include "ym/ClibCellLibrary.h"
#include "ym/MsgMgr.h"
#include "ym/StreamMsgHandler.h"


void
usage(
  const char* argv0
)
{
  using namespace std;

  cerr << "USAGE : " << argv0 << " blif-file" << endl;
}

int
main(
  int argc,
  char** argv
)
{
  using namespace std;
  using namespace nsYm;
  using namespace nsYm::nsBlif;

  int base = 1;

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
    BlifModel model;
    bool stat = parser.read(filename, model);
    if ( !stat ) {
      cerr << "Error in reading " << filename << endl;
      return 4;
    }
    model.print(cout);
#if !defined(YM_DEBUG)
  }
  catch ( const AssertError& x) {
    cout << x << endl;
  }
#endif

  return 0;
}
