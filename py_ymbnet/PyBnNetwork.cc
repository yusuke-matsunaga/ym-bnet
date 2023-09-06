
/// @file PyBnNetwork.cc
/// @brief Python BnNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyBnNetwork.h"
#include "pym/PyClibCellLibrary.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct BnNetworkObject
{
  PyObject_HEAD
  BnNetwork* mPtr;
};

// Python 用のタイプ定義
PyTypeObject BnNetworkType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
BnNetwork_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'BnNetwork' is disabled");
  return nullptr;
}

// 終了関数
void
BnNetwork_dealloc(
  PyObject* self
)
{
  auto bnnetwork_obj = reinterpret_cast<BnNetworkObject*>(self);
  delete bnnetwork_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
BnNetwork_read_blif(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "",
    "cell_library",
    nullptr
  };
  const char* filename = nullptr;
  PyObject* lib_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|$O!",
				    const_cast<char**>(kwlist),
				    &filename,
				    PyClibCellLibrary::_typeobject(), &lib_obj) ) {
    return nullptr;
  }
  ClibCellLibrary cell_library;
  if ( lib_obj != nullptr ) {
    cell_library = PyClibCellLibrary::Get(lib_obj);
  }
  try {
    auto network = BnNetwork::read_blif(filename, cell_library);
    auto obj = BnNetworkType.tp_alloc(&BnNetworkType, 0);
    auto bnet_obj = reinterpret_cast<BnNetworkObject*>(obj);
    bnet_obj->mPtr = new BnNetwork{std::move(network)};
    return obj;
  }
  catch ( std::invalid_argument ) {
    ostringstream buff;
    buff << "read_blif(\"" << filename << "\") failed";
    PyErr_SetString(PyExc_ValueError, buff.str().c_str());
    return nullptr;
  }
}

PyObject*
BnNetwork_read_iscas89(
  PyObject* Py_UNUSED(self),
  PyObject* args
)
{
  const char* filename = nullptr;
  PyObject* lib_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "s", &filename) ) {
    return nullptr;
  }
  try {
    auto network = BnNetwork::read_iscas89(filename);
    auto obj = BnNetworkType.tp_alloc(&BnNetworkType, 0);
    auto bnet_obj = reinterpret_cast<BnNetworkObject*>(obj);
    bnet_obj->mPtr = new BnNetwork{std::move(network)};
    return obj;
  }
  catch ( std::invalid_argument ) {
    ostringstream buff;
    buff << "read_iscas89(\"" << filename << "\") failed";
    PyErr_SetString(PyExc_ValueError, buff.str().c_str());
    return nullptr;
  }
}

PyObject*
BnNetwork_write(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "file",
    nullptr
  };
  const char* filename = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|$s",
				    const_cast<char**>(kwlist),
				    &filename) ) {
    return nullptr;
  }
  auto& network = PyBnNetwork::Get(self);
  if ( filename == nullptr ) {
    network.write(cout);
  }
  else {
    ofstream fout{filename};
    if ( fout ) {
      network.write(fout);
    }
    else {
      ostringstream buff;
      buff << filename << ": Could not open file";
      PyErr_SetString(PyExc_ValueError, buff.str().c_str());
      return nullptr;
    }
  }
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef BnNetwork_methods[] = {
  {"read_blif", reinterpret_cast<PyCFunction>(BnNetwork_read_blif),
   METH_VARARGS | METH_KEYWORDS | METH_STATIC,
   PyDoc_STR("read 'blif' file")},
  {"read_iscas89", reinterpret_cast<PyCFunction>(BnNetwork_read_iscas89),
   METH_VARARGS | METH_KEYWORDS | METH_STATIC,
   PyDoc_STR("read 'iscas89(.bench)' file")},
  {"write", reinterpret_cast<PyCFunction>(BnNetwork_write),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("write contents")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
BnNetwork_port_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyBnNetwork::Get(self);
  int val = network.port_num();
  return Py_BuildValue("i", val);
}

PyObject*
BnNetwork_dff_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyBnNetwork::Get(self);
  int val = network.dff_num();
  return Py_BuildValue("i", val);
}

PyObject*
BnNetwork_input_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyBnNetwork::Get(self);
  int val = network.input_num();
  return Py_BuildValue("i", val);
}

PyObject*
BnNetwork_output_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyBnNetwork::Get(self);
  int val = network.output_num();
  return Py_BuildValue("i", val);
}

PyObject*
BnNetwork_logic_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyBnNetwork::Get(self);
  int val = network.logic_num();
  return Py_BuildValue("i", val);
}

// getsetter 定義
PyGetSetDef BnNetwork_getsetters[] = {
  {"port_num", BnNetwork_port_num, nullptr, PyDoc_STR("port num"), nullptr},
  {"dff_num", BnNetwork_dff_num, nullptr, PyDoc_STR("DFF num"), nullptr},
  {"input_num", BnNetwork_input_num, nullptr, PyDoc_STR("input num"), nullptr},
  {"output_num", BnNetwork_output_num, nullptr, PyDoc_STR("output num"), nullptr},
  {"logic_num", BnNetwork_logic_num, nullptr, PyDoc_STR("logic gate num"), nullptr},
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief 'BnNetwork' オブジェクトを使用可能にする．
bool
PyBnNetwork::init(
  PyObject* m
)
{
  BnNetworkType.tp_name = "BnNetwork";
  BnNetworkType.tp_basicsize = sizeof(BnNetworkObject);
  BnNetworkType.tp_itemsize = 0;
  BnNetworkType.tp_dealloc = BnNetwork_dealloc;
  BnNetworkType.tp_flags = Py_TPFLAGS_DEFAULT;
  BnNetworkType.tp_doc = PyDoc_STR("BnNetwork object");
  BnNetworkType.tp_methods = BnNetwork_methods;
  BnNetworkType.tp_getset = BnNetwork_getsetters;
  BnNetworkType.tp_new = BnNetwork_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "BnNetwork", &BnNetworkType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief BnNetwork を PyObject に変換する．
PyObject*
PyBnNetwork::ToPyObject(
  const BnNetwork& val
)
{
  auto obj = BnNetworkType.tp_alloc(&BnNetworkType, 0);
  auto bnnetwork_obj = reinterpret_cast<BnNetworkObject*>(obj);
  bnnetwork_obj->mPtr = new BnNetwork{val};
  return obj;
}

// @brief PyObject が BnNetwork タイプか調べる．
bool
PyBnNetwork::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief BnNetwork を表す PyObject から BnNetwork を取り出す．
const BnNetwork&
PyBnNetwork::Get(
  PyObject* obj
)
{
  auto bnnetwork_obj = reinterpret_cast<BnNetworkObject*>(obj);
  return *bnnetwork_obj->mPtr;
}

// @brief BnNetwork を表すオブジェクトの型定義を返す．
PyTypeObject*
PyBnNetwork::_typeobject()
{
  return &BnNetworkType;
}

END_NAMESPACE_YM
