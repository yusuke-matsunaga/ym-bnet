
/// @file Iscas89Model.cc
/// @brief Iscas89Model の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Model.h"
#include "ModelImpl.h"
#include "Iscas89Parser.h"


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

// @brief 読み込みを行う．
bool
Iscas89Model::read(
  const string& filename
)
{
  delete mImpl;
  mImpl = new ModelImpl;

  Iscas89Parser parser;
  return parser.read(filename, mImpl);
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
Iscas89Model::logic_list() const
{
  return mImpl->logic_list();
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
const vector<SizeType>&
Iscas89Model::node_fanin_list(
  SizeType node_id
) const
{
  return mImpl->node_fanin_list(node_id);
}

// @brief 論理ノードの種類を返す．
Iscas89GateType
Iscas89Model::node_gate_type(
  SizeType node_id ///< [in] ノード番号
) const
{
  return mImpl->node_gate_type(node_id);
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
}

END_NAMESPACE_YM_ISCAS89
