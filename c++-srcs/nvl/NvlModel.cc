
/// @file NvlModel.cc
/// @brief NvlModel の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/NvlModel.h"
#include "ym/NvlParser.h"
#include "ModelImpl.h"


BEGIN_NAMESPACE_YM_NVL

// @brief コンストラクタ
NvlModel::NvlModel()
{
  mImpl = new ModelImpl;
}

// @brief デストラクタ
NvlModel::~NvlModel()
{
  delete mImpl;
}

// @brief 入力のノード番号のリストを返す．
const vector<SizeType>&
NvlModel::input_list() const
{
  return mImpl->input_list();
}

// @brief 出力のノード番号のリストを返す．
const vector<SizeType>&
NvlModel::output_list() const
{
  return mImpl->output_list();
}

// @brief 論理ノード番号のリストを返す．
const vector<SizeType>&
NvlModel::gate_list() const
{
  return mImpl->gate_list();
}

// @brief ノード名を返す．
const string&
NvlModel::node_name(
  SizeType node_id ///< [in] ノード番号
) const
{
  return mImpl->node_name(node_id);
}

// @brief ノードの種類を返す．
NvlType
NvlModel::node_type(
  SizeType node_id ///< [in] ノード番号
) const
{
  return mImpl->node_type(node_id);
}

// @brief ノードのファンインのノード番号のリストを返す．
SizeType
NvlModel::node_fanin_num(
  SizeType node_id
) const
{
  return mImpl->node_fanin_num(node_id);
}

// @brief ノードのファンインのノード番号のリストを返す．
const vector<SizeType>&
NvlModel::node_fanin_list(
  SizeType node_id
) const
{
  return mImpl->node_fanin_list(node_id);
}

// @brief 論理ノードの種類を返す．
PrimType
NvlModel::node_gate_type(
  SizeType node_id ///< [in] ノード番号
) const
{
  return mImpl->node_gate_type(node_id);
}

// @brief 内容を出力する．
void
NvlModel::print(
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

END_NAMESPACE_YM_NVL
