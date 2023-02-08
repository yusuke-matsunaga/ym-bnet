
/// @file read_iscas89_test.cc
/// @brief Iscas89Model::read() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Parser.h"
#include "ym/Iscas89Model.h"
#include "ym/MsgMgr.h"
#include "ym/StreamMsgHandler.h"


int
main(
  int argc,
  char** argv
)
{
  using namespace std;
  using namespace nsYm;

  if ( argc != 2 ) {
    cerr << "USAGE : " << argv[0] << " iscas89-file" << endl;
    return 2;
  }
  string filename = argv[1];

  try {
    Iscas89Parser parser;
    Iscas89Model model;

    StreamMsgHandler msg_handler(cerr);
    MsgMgr::attach_handler(&msg_handler);

    if ( !parser.read(filename, model) ) {
      cerr << "Error in reading " << filename << endl;
      return 4;
    }
    model.print(cout);
  }
  catch ( const AssertError& x) {
    cout << x << endl;
  }

  return 0;
}
