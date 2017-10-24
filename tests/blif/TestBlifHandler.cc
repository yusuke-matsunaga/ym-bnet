
/// @file TestBlifHandler.cc
/// @brief TestBlifHandler の実装プログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "TestBlifHandler.h"
#include "ym/BlifCover.h"
#include "ym/ClibCell.h"
#include "ym/ClibCellPin.h"
#include "ym/FileRegion.h"


BEGIN_NAMESPACE_YM

// @brief コンストラクタ
TestBlifHandler::TestBlifHandler(ostream* streamptr) :
  mStreamPtr(streamptr)
{
}

// @brief デストラクタ
TestBlifHandler::~TestBlifHandler()
{
}

// @brief 初期化
bool
TestBlifHandler::init()
{
  return true;
}

// @brief .model 文の開始
bool
TestBlifHandler::model(const FileRegion& loc1,
		       const FileRegion& loc2,
		       const char* name)
{
  (*mStreamPtr) << ".model " << name << endl
		<< "\t[" << loc1 << "]" << endl
		<< "\t[" << loc2 << "]" << endl;
  return true;
}

// @brief .inputs 文中の文字列の処理
bool
TestBlifHandler::inputs_elem(ymuint name_id,
			     const char* name)
{
  (*mStreamPtr) << "  inputs elem: " << name << endl
		<< "\t[" << id2loc(name_id) << "]" << endl;
  return true;
}

// @brief .outputs 文中の文字列の処理
bool
TestBlifHandler::outputs_elem(ymuint name_id,
			      const char* name)
{
  (*mStreamPtr) << "  outputs elem: " << name_id << endl
		<< "\t[" << id2loc(name_id) << "]" << endl;
  return true;
}

// @brief .names 文の処理
// @param[in] onode_id 出力ノードのID番号
// @param[in] inode_id_array 各識別子のID番号の配列
// @param[in] cover カバー
// @retval true 処理が成功した．
// @retval false エラーが起こった．
// @note cover_pat は ni 個ごとに1行のパタンを表す．
// 各要素のとりうる値は '0', '1', '-' を表す．
// @note opat は '0' か '1' のどちらか
bool
TestBlifHandler::names(ymuint onode_id,
		       const char* oname,
		       const vector<ymuint>& inode_id_array,
		       ymuint cover_id)
{
  (*mStreamPtr) << ".names" << endl;
  ymuint ni = inode_id_array.size();
  for (ymuint i = 0; i < ni; ++ i) {
    ymuint id = inode_id_array[i];
    (*mStreamPtr) << id2str(id) << endl
		  << "\t[" << id2loc(id) << "]" << endl;
  }
  (*mStreamPtr) << oname << endl
		<< "\t[" << id2loc(onode_id) << "]" << endl;
  (*mStreamPtr) << "Cover#" << cover_id << endl;
  const BlifCover* cover = id2cover(cover_id);
  ymuint pos = 0;
  for (ymuint c = 0; c < cover->cube_num(); ++ c) {
    for (ymuint i = 0; i < ni; ++ i, ++ pos) {
      (*mStreamPtr) << cover->input_pat(i, c);
    }
    if ( ni > 0 ) {
      (*mStreamPtr) << ' ';
    }
    (*mStreamPtr) << cover->output_pat() << endl;
  }
  return true;
}

// @brief .gate 文の処理
// @param[in] onode_id 出力ノードのID番号
// @param[in] inode_id_array 入力ノードのID番号の配列
// @param[in] cell セル
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
TestBlifHandler::gate(ymuint onode_id,
		      const char* oname,
		      const vector<ymuint>& inode_id_array,
		      const ClibCell* cell)
{
  (*mStreamPtr) << ".gate " << cell->name() << " " << oname
		<< "\t[" << id2loc(onode_id) << "]" << endl;
  ymuint ni = inode_id_array.size();
  for (ymuint i = 0; i < ni; ++ i) {
    ymuint id = inode_id_array[i];
    (*mStreamPtr) << "  " << cell->input(i)->name() << " " << id2str(id)
		  << "\t[" << id2loc(id) << "]" << endl;
  }
  return true;
}

// @brief .latch 文の処理
// @param[in] onode_id 出力ノードのID番号
// @param[in] inode_id 入力ノードのID番号
// @param[in] loc4 リセット値の位置情報
// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
TestBlifHandler::latch(ymuint onode_id,
		       const char* oname,
		       ymuint inode_id,
		       const FileRegion& loc4,
		       char rval)
{
  (*mStreamPtr) << ".latch "
		<< id2str(inode_id) << " "
		<< oname << " " << rval << endl
		<< "\t[" << id2loc(inode_id) << "]" << endl
		<< "\t[" << id2loc(onode_id) << "]" << endl
		<< "\t[" << loc4 << "]" << endl;
  return true;
}

// @brief .end 文の処理
bool
TestBlifHandler::end(const FileRegion& loc)
{
  (*mStreamPtr) << ".end" << endl
		<< "\t[" << loc << "]" << endl;
  return true;
}

// @brief 通常終了時の処理
void
TestBlifHandler::normal_exit()
{
  (*mStreamPtr) << "normal_exit" << endl;
}

// @brief エラー終了時の処理
void
TestBlifHandler::error_exit()
{
  (*mStreamPtr) << "error_exit" << endl;
}

END_NAMESPACE_YM
