
/// @file TestBlifHandler.cc
/// @brief TestBlifHandler の実装プログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "TestBlifHandler.h"
#include "BlifCover.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/ClibPin.h"
#include "ym/FileRegion.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
// @param[in] parser パーサー
TestBlifHandler::TestBlifHandler(
  BlifParser& parser,
  ostream& stream
) : BlifHandler{parser},
    mStream{stream}
{
}

// @brief 初期化
bool
TestBlifHandler::init()
{
  return true;
}

// @brief セルライブラリの設定
void
TestBlifHandler::set_cell_library(
  const ClibCellLibrary& library
)
{
  mStream << "set_cell_library(" << library.name() << ")" << endl;
}

// @brief .model 文の開始
bool
TestBlifHandler::model(
  const FileRegion& loc1,
  const FileRegion& loc2,
  const string& name
)
{
  mStream << ".model " << name << endl
	  << "\t[" << loc1 << "]" << endl
	  << "\t[" << loc2 << "]" << endl;
  return true;
}

// @brief .inputs 文中の文字列の処理
bool
TestBlifHandler::inputs_elem(
  SizeType name_id,
  const string& name
)
{
  mStream << "  inputs elem: " << name << endl
	  << "\t[" << id2loc(name_id) << "]" << endl;
  return true;
}

// @brief .outputs 文中の文字列の処理
bool
TestBlifHandler::outputs_elem(
  SizeType name_id,
  const string& name
)
{
  mStream << "  outputs elem: " << name_id << endl
	  << "\t[" << id2loc(name_id) << "]" << endl;
  return true;
}

// @brief .names 文の処理
bool
TestBlifHandler::names(
  SizeType onode_id,
  const string& oname,
  const vector<SizeType>& inode_id_array,
  SizeType cover_id
)
{
  mStream << ".names" << endl;
  for ( auto id: inode_id_array ) {
    mStream << id2str(id) << endl
	    << "\t[" << id2loc(id) << "]" << endl;
  }
  mStream << oname << endl
	  << "\t[" << id2loc(onode_id) << "]" << endl;
  mStream << "Cover#" << cover_id << endl;
  const BlifCover& cover = id2cover(cover_id);
  auto nc = cover.cube_num();
  auto ni = inode_id_array.size();
  for ( auto c = 0; c < nc; ++ c ) {
    for ( auto i = 0; i < ni; ++ i ) {
      mStream << cover.input_pat(c, i);
    }
    if ( ni > 0 ) {
      mStream << ' ';
    }
    mStream << cover.output_pat() << endl;
  }
  return true;
}

// @brief .gate 文の処理
bool
TestBlifHandler::gate(
  SizeType onode_id,
  const string& oname,
  const vector<SizeType>& inode_id_array,
  SizeType cell_id
)
{
  mStream << ".gate " << cell_id << " " << oname
	  << "\t[" << id2loc(onode_id) << "]" << endl;
  auto ni = inode_id_array.size();
  for ( auto i = 0; i < ni; ++ i ) {
    auto id = inode_id_array[i];
    mStream << "  #" << i << ": " << id2str(id)
	    << "\t[" << id2loc(id) << "]" << endl;
  }
  return true;
}

// @brief .latch 文の処理
bool
TestBlifHandler::latch(
  SizeType onode_id,
  const string& oname,
  SizeType inode_id,
  const FileRegion& loc4,
  char rval)
{
  mStream << ".latch "
	  << id2str(inode_id) << " "
	  << oname << " " << rval << endl
	  << "\t[" << id2loc(inode_id) << "]" << endl
	  << "\t[" << id2loc(onode_id) << "]" << endl
	  << "\t[" << loc4 << "]" << endl;
  return true;
}

// @brief .end 文の処理
bool
TestBlifHandler::end(
  const FileRegion& loc
)
{
  mStream << ".end" << endl
	  << "\t[" << loc << "]" << endl;
  return true;
}

// @brief 通常終了時の処理
void
TestBlifHandler::normal_exit()
{
  mStream << "normal_exit" << endl;
}

// @brief エラー終了時の処理
void
TestBlifHandler::error_exit()
{
  mStream << "error_exit" << endl;
}

END_NAMESPACE_YM_BNET
