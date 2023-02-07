
/// @file BlifModel.cc
/// @brief BlifModel の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BlifModel.h"
#include "ModelImpl.h"
#include "BlifParser.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
// クラス BlifModel
//////////////////////////////////////////////////////////////////////

// @brief 読み込みを行う．
bool
BlifModel::read(
  const string& filename
)
{
  return read(filename, ClibCellLibrary{});
}

// @brief 読み込みを行う(セルライブラリ付き)．
bool
BlifModel::read(
  const string& filename,
  const ClibCellLibrary& cell_library
)
{
  delete mImpl;
  mImpl = new ModelImpl;

  BlifParser parser;
  return parser.read(filename, cell_library, mImpl);
}

// @brief コンストラクタ
BlifModel::BlifModel()
{
  mImpl = new ModelImpl;
}

// @brief デストラクタ
BlifModel::~BlifModel()
{
  delete mImpl;
}

// @brief 名前を返す．
const string&
BlifModel::name() const
{
  return mImpl->name();
}

// @brief 入力のノード番号のリストを返す．
const vector<SizeType>&
BlifModel::input_list() const
{
  return mImpl->input_list();
}

// @brief 出力のノード番号のリストを返す．
const vector<SizeType>&
BlifModel::output_list() const
{
  return mImpl->output_list();
}

// @brief DFFのノード番号のリストを返す．
const vector<SizeType>&
BlifModel::dff_list() const
{
  return mImpl->dff_list();
}

// @brief 論理ノード番号のリストを返す．
const vector<SizeType>&
BlifModel::logic_list() const
{
  return mImpl->logic_list();
}

// @brief ノード名を返す．
const string&
BlifModel::node_name(
  SizeType node_id
) const
{
  return mImpl->node_name(node_id);
}

// @brief ノードの種類を返す．
BlifType
BlifModel::node_type(
  SizeType node_id
) const
{
  return mImpl->node_type(node_id);
}

// @brief ノードのファンインのノード番号のリストを返す．
const vector<SizeType>&
BlifModel::node_fanin_list(
  SizeType node_id
) const
{
  return mImpl->node_fanin_list(node_id);
}

// @brief ノードのカバー番号を返す．
SizeType
BlifModel::node_cover_id(
  SizeType node_id
) const
{
  return mImpl->node_cover_id(node_id);
}

// @brief ノードのカバー番号を返す．
const BlifCover&
BlifModel::node_cover(
  SizeType node_id
) const
{
  SizeType id = node_cover_id(node_id);
  return cover(id);
}

// @brief ノードのセル番号を返す．
SizeType
BlifModel::node_cell_id(
  SizeType node_id
) const
{
  return mImpl->node_cell_id(node_id);
}

// @brief ノードの入力ノード番号を返す．
SizeType
BlifModel::node_input(
  SizeType node_id
) const
{
  return mImpl->node_input(node_id);
}

// @brief ノードのリセット値を返す．
char
BlifModel::node_rval(
  SizeType node_id
) const
{
  return mImpl->node_rval(node_id);
}

// @brief カバーの種類の数を返す．
SizeType
BlifModel::cover_num() const
{
  return mImpl->cover_num();
}

// @brief カバーを取り出す．
const BlifCover&
BlifModel::cover(
  SizeType cover_id
) const
{
  return mImpl->cover(cover_id);
}

// @brief 内容を出力する．
void
BlifModel::print(
  ostream& s
) const
{
  s << "Name: " << name() << endl;
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
    s << " " << id << " = DFF(" << node_input(id) << ")"
      << ": rval = " << node_rval(id) << endl;
  }

  SizeType max_cover = 0;
  for ( auto id: logic_list() ) {
    s << " " << id << " = Logic(";
    for ( auto iid: node_fanin_list(id) ) {
      s << " " << iid;
    }
    s << "): ";
    if ( node_type(id) == Cover ) {
      SizeType cover_id = node_cover_id(id);
      s << "cover = " << cover_id;
      max_cover = std::max(max_cover, cover_id);
    }
    else if ( node_type(id) == Cell ) {
      s << "cell = " << node_cell_id(id);
    }
    else {
      ASSERT_NOT_REACHED;
    }
    s << endl;
  }

  s << endl;
  for ( SizeType id = 0; id <= max_cover; ++ id ) {
    s << "Cover#" << id << ":" << endl;
    auto& cover = this->cover(id);
    cover.print(s);
  }
}

END_NAMESPACE_YM_BLIF
