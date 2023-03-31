
/// @file PyBnNode.cc
/// @brief Python BnNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyBnNode.h"
#include "pym/PyExpr.h"
#include "pym/PyTvFunc.h"
#include "pym/PyBdd.h"
#include "pym/PyModule.h"
#include "ym/BnNetwork.h"
#include "ym/ClibCell.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct BnNodeObject
{
  PyObject_HEAD
  SizeType mId;
  BnNetwork* mNetwork;
};

// Python 用のタイプ定義
PyTypeObject BnNode_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
BnNode_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'BnNode' is disabled");
  return nullptr;
}

// 終了関数
void
BnNode_dealloc(
  PyObject* self
)
{
  // mPtr は borrowed reference だからなにもしない．
  Py_TYPE(self)->tp_free(self);
}

PyObject*
BnNode_is_input(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_input();
  return PyBool_FromLong(r);
}

PyObject*
BnNode_is_port_input(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_port_input();
  return PyBool_FromLong(r);
}

PyObject*
BnNode_is_data_out(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_data_out();
  return PyBool_FromLong(r);
}

PyObject*
BnNode_is_cell_output(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_cell_output();
  return PyBool_FromLong(r);
}

PyObject*
BnNode_is_output(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_output();
  return PyBool_FromLong(r);
}

PyObject*
BnNode_is_port_output(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_port_output();
  return PyBool_FromLong(r);
}

PyObject*
BnNode_is_data_in(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_data_in();
  return PyBool_FromLong(r);
}

PyObject*
BnNode_is_clock(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_clock();
  return PyBool_FromLong(r);
}

PyObject*
BnNode_is_clear(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_clear();
  return PyBool_FromLong(r);
}

PyObject*
BnNode_is_preset(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_preset();
  return PyBool_FromLong(r);
}

PyObject*
BnNode_is_cell_input(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_cell_input();
  return PyBool_FromLong(r);
}

PyObject*
BnNode_is_logic(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto node = PyBnNode::Get(self);
  auto r = node.is_logic();
  return PyBool_FromLong(r);
}

// メソッド定義
PyMethodDef BnNode_methods[] = {
  {"is_input", BnNode_is_input, METH_NOARGS,
   PyDoc_STR("return True if input")},
  {"is_port_input", BnNode_is_port_input, METH_NOARGS,
   PyDoc_STR("return True if primary input")},
  {"is_data_out", BnNode_is_data_out, METH_NOARGS,
   PyDoc_STR("return True if FF's data-out")},
  {"is_cell_output", BnNode_is_cell_output, METH_NOARGS,
   PyDoc_STR("return True if FF cell's output")},
  {"is_port_output", BnNode_is_port_output, METH_NOARGS,
   PyDoc_STR("return True if primary output")},
  {"is_data_in", BnNode_is_data_in, METH_NOARGS,
   PyDoc_STR("return True if FF's data-in")},
  {"is_clock", BnNode_is_clock, METH_NOARGS,
   PyDoc_STR("return True if FF's clock")},
  {"is_clear", BnNode_is_clear, METH_NOARGS,
   PyDoc_STR("return True if FF's clear")},
  {"is_preset", BnNode_is_clock, METH_NOARGS,
   PyDoc_STR("return True if FF's preset")},
  {"is_cell_input", BnNode_is_cell_input, METH_NOARGS,
   PyDoc_STR("return True if FF cell's input")},
  {"is_logic", BnNode_is_logic, METH_NOARGS,
   PyDoc_STR("return True if logic node")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
BnNode_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto id = node.id();
  return PyLong_FromLong(id);
}

PyObject*
BnNode_name(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto name = node.name();
  return Py_BuildValue("s", name.c_str());
}

PyObject*
BnNode_fanout_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto network = PyBnNode::_network(self);
  auto fanout_list = node.fanout_list();
  auto n = fanout_list.size();
  auto obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto id = fanout_list[i].id();
    auto node1_obj = PyBnNode::ToPyObject(id, network);
    PyList_SET_ITEM(obj, i, node1_obj);
  }
  return obj;
}

PyObject*
BnNode_input_pos(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto val = node.input_pos();
  return PyLong_FromLong(val);
}

PyObject*
BnNode_primary_input_pos(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto val = node.primary_input_pos();
  return PyLong_FromLong(val);
}

PyObject*
BnNode_cell_output_pos(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto val = node.cell_output_pos();
  return PyLong_FromLong(val);
}

PyObject*
BnNode_output_pos(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto val = node.output_pos();
  return PyLong_FromLong(val);
}

PyObject*
BnNode_output_src(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto network = PyBnNode::_network(self);
  auto id = node.output_src().id();
  return PyBnNode::ToPyObject(id, network);
}

PyObject*
BnNode_primary_output_pos(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto val = node.primary_output_pos();
  return PyLong_FromLong(val);
}

PyObject*
BnNode_cell_input_pos(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto val = node.cell_input_pos();
  return PyLong_FromLong(val);
}

PyObject*
BnNode_port_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto val = node.port_id();
  return PyLong_FromLong(val);
}

PyObject*
BnNode_port_bit(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto val = node.port_bit();
  return PyLong_FromLong(val);
}

PyObject*
BnNode_dff_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto val = node.dff_id();
  return PyLong_FromLong(val);
}

PyObject*
BnNode_fanin_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto network = PyBnNode::_network(self);
  auto fanin_list = node.fanin_list();
  auto n = fanin_list.size();
  auto obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto id = fanin_list[i].id();
    auto node1_obj = PyBnNode::ToPyObject(id, network);
    PyList_SET_ITEM(obj, i, node1_obj);
  }
  return obj;
}

PyObject*
BnNode_expr(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto network = PyBnNode::_network(self);
  auto id = node.expr_id();
  auto expr = network->expr(id);
  return PyExpr::ToPyObject(expr);
}

PyObject*
BnNode_func(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto network = PyBnNode::_network(self);
  auto id = node.func_id();
  auto func = network->func(id);
  return PyTvFunc::ToPyObject(func);
}

PyObject*
BnNode_bdd(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto bdd = node.bdd();
  return PyBdd::ToPyObject(bdd);
}

PyObject*
BnNode_cell_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto node = PyBnNode::Get(self);
  auto id = node.cell().id();
  return PyLong_FromLong(id);
}

// getsetter 定義
PyGetSetDef BnNode_getsetters[] = {
  {"id", BnNode_id, nullptr,
   PyDoc_STR("ID"), nullptr},
  {"name", BnNode_name, nullptr,
   PyDoc_STR("name"), nullptr},
  {"fanout_list", BnNode_fanout_list, nullptr,
   PyDoc_STR("fanout list"), nullptr},
  {"input_pos", BnNode_input_pos, nullptr,
   PyDoc_STR("input position"), nullptr},
  {"primary_input_pos", BnNode_primary_input_pos, nullptr,
   PyDoc_STR("primary input position"), nullptr},
  {"cell_output_pos", BnNode_cell_output_pos, nullptr,
   PyDoc_STR("DFF cell's output position"), nullptr},
  {"output_pos", BnNode_output_pos, nullptr,
   PyDoc_STR("output position"), nullptr},
  {"output_src", BnNode_output_src, nullptr,
   PyDoc_STR("output source node"), nullptr},
  {"primary_output_pos", BnNode_primary_output_pos, nullptr,
   PyDoc_STR("primary output position"), nullptr},
  {"cell_input_pos", BnNode_cell_input_pos, nullptr,
   PyDoc_STR("DFF cell's input position"), nullptr},
  {"port_id", BnNode_port_id, nullptr,
   PyDoc_STR("port ID"), nullptr},
  {"port_bit", BnNode_port_bit, nullptr,
   PyDoc_STR("port bit ID"), nullptr},
  {"dff_id", BnNode_dff_id, nullptr,
   PyDoc_STR("DFF ID"), nullptr},
  {"fanin_list", BnNode_fanin_list, nullptr,
   PyDoc_STR("fanin list"), nullptr},
  {"expr", BnNode_expr, nullptr,
   PyDoc_STR("expr"), nullptr},
  {"func", BnNode_func, nullptr,
   PyDoc_STR("Truth Vector function"), nullptr},
  {"bdd", BnNode_bdd, nullptr,
   PyDoc_STR("BDD"), nullptr},
  {"cell_id", BnNode_cell_id, nullptr,
   PyDoc_STR("Cell ID"), nullptr},
  {nullptr, nullptr, nullptr,
   nullptr, nullptr}
};

END_NONAMESPACE


// @brief 'BnNode' オブジェクトを使用可能にする．
bool
PyBnNode::init(
  PyObject* m
)
{
  BnNode_Type.tp_name = "BnNode";
  BnNode_Type.tp_basicsize = sizeof(BnNodeObject);
  BnNode_Type.tp_itemsize = 0;
  BnNode_Type.tp_dealloc = BnNode_dealloc;
  BnNode_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  BnNode_Type.tp_doc = PyDoc_STR("BnNode object");
  BnNode_Type.tp_methods = BnNode_methods;
  BnNode_Type.tp_getset = BnNode_getsetters;
  BnNode_Type.tp_new = BnNode_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "BnNode", &BnNode_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief BnNode を PyObject に変換する．
PyObject*
PyBnNode::ToPyObject(
  SizeType id,
  BnNetwork* network
)
{
  auto obj = BnNode_Type.tp_alloc(&BnNode_Type, 0);
  auto bnnode_obj = reinterpret_cast<BnNodeObject*>(obj);
  bnnode_obj->mId = id;
  bnnode_obj->mNetwork = network;
  return obj;
}

// @brief PyObject が BnNode タイプか調べる．
bool
PyBnNode::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief BnNode を表す PyObject から BnNode を取り出す．
BnNode
PyBnNode::Get(
  PyObject* obj
)
{
  auto network_p = _network(obj);
  auto bnnode_obj = reinterpret_cast<BnNodeObject*>(obj);
  auto id = bnnode_obj->mId;
  return network_p->node(id);
}

// @brief 親のネットワークを取り出す．
BnNetwork*
PyBnNode::_network(
  PyObject* obj
)
{
  auto bnnode_obj = reinterpret_cast<BnNodeObject*>(obj);
  return bnnode_obj->mNetwork;
}

// @brief BnNode を表すオブジェクトの型定義を返す．
PyTypeObject*
PyBnNode::_typeobject()
{
  return &BnNode_Type;
}

END_NAMESPACE_YM
