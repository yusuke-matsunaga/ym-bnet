
/// @file py_ymbnet.cc
/// @brief py_ymbnet の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "ym_config.h"


BEGIN_NAMESPACE_YM

extern "C"
PyObject* PyInit_ymbnet();

END_NAMESPACE_YM

int
main(
  int argc,
  char** argv
)
{
  PyImport_AppendInittab("ymbnet", &YM_NAMESPACE::PyInit_ymbnet);

  return Py_BytesMain(argc, argv);
}
