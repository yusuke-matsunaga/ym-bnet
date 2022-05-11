
/// @file loop_test.cc
/// @brief BnBuilderTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNetwork.h"
#include "ym/Iscas89Parser.h"


BEGIN_NAMESPACE_YM

void
blif_loop_test(int loop_count,
	       const string& filename)
{
  for (int i = 0; i < loop_count; ++ i) {
    BnNetwork network = BnNetwork::read_blif(filename);
  }
}

void
iscas89_loop_test(int loop_count,
		  const string& filename)
{
  for (int i = 0; i < loop_count; ++ i) {
    BnNetwork network = BnNetwork::read_iscas89(filename);
  }
}

void
iscas89parser_loop_test(int loop_count,
		  const string& filename)
{
  for (int i = 0; i < loop_count; ++ i) {
    nsBnet::Iscas89Parser parser;
    parser.read(filename);
  }
}

void
usage(char* progname)
{
  cerr << "USAGE: " << progname << " <blif|iscas89> <# of loop counts> <iscas89 filename>" << endl;
}

END_NAMESPACE_YM

int
main(int argc,
     char** argv)
{
  using namespace std;
  using namespace nsYm;

  if ( argc != 4 ) {
    usage(argv[0]);
    return 1;
  }

  int loop_count = atoi(argv[2]);
  string filename = argv[3];

  if ( strcmp(argv[1], "blif") == 0 ) {
    blif_loop_test(loop_count, filename);
  }
  else if ( strcmp(argv[1], "iscas89") == 0 ) {
    iscas89_loop_test(loop_count, filename);
  }
  else if ( strcmp(argv[1], "iscas89parser") == 0 ) {
    iscas89parser_loop_test(loop_count, filename);
  }
  else {
    usage(argv[0]);
    return 1;
  }

  return 0;
}
