
/// @file ymbnet_module.cc
/// @brief Python 用の bnet モジュールを定義する．
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ym/PyBnNetwork.h"
#include "ym/PyBnNode.h"
#include "ym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// メソッド定義構造体
PyMethodDef ymbnet_methods[] = {
  {nullptr, nullptr, 0, nullptr},
};

// モジュール定義構造体
PyModuleDef ymbnet_module = {
  PyModuleDef_HEAD_INIT,
  "ymbnet",
  PyDoc_STR("ymbnet: Extension module for bnet"),
  -1,
  ymbnet_methods,
};

END_NONAMESPACE

PyMODINIT_FUNC
PyInit_ymbnet()
{
  auto m = PyModule::init(&ymbnet_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyBnNetwork::init(m) ) {
    goto error;
  }

  if ( !PyBnNode::init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NAMESPACE_YM
