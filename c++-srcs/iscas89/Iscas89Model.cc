
/// @file Iscas89Model.cc
/// @brief Iscas89Model の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Model.h"
#include "ym/Iscas89Parser.h"
#include "ModelImpl.h"


BEGIN_NAMESPACE_YM_ISCAS89

// @brief コンストラクタ
Iscas89Model::Iscas89Model()
{
  mImpl = new ModelImpl;
}

// @brief デストラクタ
Iscas89Model::~Iscas89Model()
{
  delete mImpl;
}

// @brief 入力のノード番号のリストを返す．
const vector<SizeType>&
Iscas89Model::input_list() const
{
  return mImpl->input_list();
}

// @brief 出力のノード番号のリストを返す．
const vector<SizeType>&
Iscas89Model::output_list() const
{
  return mImpl->output_list();
}

// @brief DFFのノード番号のリストを返す．
const vector<SizeType>&
Iscas89Model::dff_list() const
{
  return mImpl->dff_list();
}

// @brief 論理ノード番号のリストを返す．
const vector<SizeType>&
Iscas89Model::gate_list() const
{
  return mImpl->gate_list();
}

// @brief 論理式のリストを返す．
const vector<Expr>&
Iscas89Model::expr_list() const
{
  return mImpl->expr_list();
}

// @brief ノード名を返す．
const string&
Iscas89Model::node_name(
  SizeType node_id ///< [in] ノード番号
) const
{
  return mImpl->node_name(node_id);
}

// @brief ノードの種類を返す．
Iscas89Type
Iscas89Model::node_type(
  SizeType node_id ///< [in] ノード番号
) const
{
  return mImpl->node_type(node_id);
}

// @brief ノードのファンインのノード番号のリストを返す．
SizeType
Iscas89Model::node_fanin_num(
  SizeType node_id
) const
{
  return mImpl->node_fanin_num(node_id);
}

// @brief ノードのファンインのノード番号のリストを返す．
const vector<SizeType>&
Iscas89Model::node_fanin_list(
  SizeType node_id
) const
{
  return mImpl->node_fanin_list(node_id);
}

// @brief 論理ノードの種類を返す．
PrimType
Iscas89Model::node_gate_type(
  SizeType node_id ///< [in] ノード番号
) const
{
  return mImpl->node_gate_type(node_id);
}

// @brief 論理ノードに対する論理式番号を返す．
SizeType
Iscas89Model::node_expr_id(
  SizeType node_id ///< [in] ノード番号
) const
{
  return mImpl->node_expr_id(node_id);
}

// @brief 論理ノードに対する論理式を返す．
Expr
Iscas89Model::node_expr(
  SizeType node_id ///< [in] ノード番号
) const
{
  return mImpl->node_expr(node_id);
}

// @brief ノードの入力ノード番号を返す．
SizeType
Iscas89Model::node_input(
  SizeType node_id
) const
{
  return mImpl->node_input(node_id);
}

// @brief 内容を出力する．
void
Iscas89Model::print(
  ostream& s
) const
{
  s << "Input: ";
  for ( auto id: input_list() ) {
    s << " " << id;
  }
  s << endl;
  s << "Output: ";
  for ( auto id: output_list() ) {
    s << " " << id;
  }
  s << endl;
  for ( auto id: dff_list() ) {
    s << " " << id << " = DFF(" << node_input(id) << ")" << endl;
  }

  for ( auto id: gate_list() ) {
    auto gtype = node_gate_type(id);
    s << " " << id << " = "
      << gtype << "(";
    for ( auto iid: node_fanin_list(id) ) {
      s << " " << iid;
    }
    s << ")" << endl;
  }
}

END_NAMESPACE_YM_ISCAS89
