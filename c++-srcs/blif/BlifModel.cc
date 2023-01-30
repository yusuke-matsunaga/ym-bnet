
/// @file BlifModel.cc
/// @brief BlifModel の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BlifModel.h"
#include "ym/BlifNode.h"
#include "ym/BlifCover.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
// クラス BlifModel
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifModel::BlifModel()
{
}

// @brief デストラクタ
BlifModel::~BlifModel()
{
  for ( auto node: mNodeList ) {
    delete node;
  }
}

// @brief カバーを取り出す．
const BlifCover&
BlifModel::cover(
  SizeType cover_id
) const
{
  return mCoverArray[cover_id];
}

// @brief 内容を出力する．
void
BlifModel::print(
  ostream& s
) const
{
  s << "Name: " << name() << endl;
  s << "Input: ";
  for ( auto id = 0; id < input_num(); ++ id ) {
    s << " " << id;
  }
  s << endl;
  s << "Output: ";
  for ( auto id: output_list() ) {
    s << " " << id;
  }
  s << endl;
  s << "Latch: ";
  for ( auto id = 0; id < latch_num(); ++ id ) {
    s << " " << (id + input_num());
  }
  s << endl;
  for ( auto id = 0; id < node_num(); ++ id ) {
    auto node = this->node(id);
    s << "Node#" << node->id() << "(" << node->name() << ")";
    switch ( node->type() ) {
    case BlifNode::Input:
      s << ": Input";
      break;
    case BlifNode::Names:
      s << ": Names (";
      for ( auto id: node->inode_list() ) {
	s << " " << id;
      }
      s << "): Cover#" << node->cover_id();
      break;
    case BlifNode::Gate:
      s << ": Gate (";
      for ( auto id: node->inode_list() ) {
	s << " " << id;
      }
      s << "): Cell#" << node->cover_id();
      break;
    case BlifNode::Latch:
      s << ": Latch ("
	<< node->inode()
	<< "): " << node->rval();
      break;
    }
    s << endl;
  }
}

END_NAMESPACE_YM_BLIF
