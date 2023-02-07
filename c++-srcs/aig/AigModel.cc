
/// @file AigModel.cc
/// @brief AigModel の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/AigModel.h"
#include "ModelImpl.h"


BEGIN_NAMESPACE_YM_AIG

//////////////////////////////////////////////////////////////////////
// クラス AigModel
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
AigModel::AigModel(
) : mImpl{new ModelImpl}
{
}

// @brief デストラクタ
AigModel::~AigModel()
{
  delete mImpl;
}

// @brief Ascii AIG フォーマットを読み込む．
bool
AigModel::read_aag(
  const string& filename
)
{
  ifstream s{filename};
  if ( s ) {
    return read_aag(s);
  }
  return false;
}

// @brief Ascii AIG フォーマットを読み込む．
bool
AigModel::read_aag(
  istream& s
)
{
  return mImpl->read_aag(s);
}

// @brief AIG フォーマットを読み込む．
bool
AigModel::read_aig(
  const string& filename
)
{
  ifstream s{filename};
  if ( s ) {
    return read_aig(s);
  }
  return false;
}

// @brief AIG フォーマットを読み込む．
bool
AigModel::read_aig(
  istream& s
)
{
  return mImpl->read_aig(s);
}

// @brief 変数番号の最大値を返す．
SizeType
AigModel::M() const
{
  return mImpl->M();
}

// @brief 入力数を得る．
SizeType
AigModel::I() const
{
  return mImpl->I();
}

// @brief ラッチ数を得る．
SizeType
AigModel::L() const
{
  return mImpl->L();
}

// @brief 出力数を得る．
SizeType
AigModel::O() const
{
  return mImpl->O();
}

// @brief ANDノード数を返す．
SizeType
AigModel::A() const
{
  return mImpl->A();
}

// @brief 入力ノードのリテラルを得る．
SizeType
AigModel::input(
  SizeType pos
) const
{
  return mImpl->input(pos);
}

// @brief ラッチノードのリテラルを得る．
SizeType
AigModel::latch(
  SizeType pos ///< [in] ラッチ番号 ( 0 <= pos < L() )
) const
{
  return mImpl->latch(pos);
}

// @brief ラッチのソースリテラルを得る．
SizeType
AigModel::latch_src(
  SizeType pos
) const
{
  return mImpl->latch_src(pos);
}

// @brief 出力のソースリテラルを得る．
SizeType
AigModel::output_src(
  SizeType pos
) const
{
  return mImpl->output_src(pos);
}

// @brief ANDノードのリテラルを得る．
SizeType
AigModel::and_node(
  SizeType pos
) const
{
  return mImpl->and_node(pos);
}

// @brief ANDノードのソース1のリテラルを得る．
SizeType
AigModel::and_src1(
  SizeType pos
) const
{
  return mImpl->and_src1(pos);
}

// @brief ANDノードのソース2のリテラルを得る．
SizeType
AigModel::and_src2(
  SizeType pos
) const
{
  return mImpl->and_src2(pos);
}

// @brief 入力のシンボルを得る．
const string&
AigModel::input_symbol(
  SizeType pos
) const
{
  return mImpl->input_symbol(pos);
}

// @brief ラッチのシンボルを得る．
const string&
AigModel::latch_symbol(
  SizeType pos
) const
{
  return mImpl->latch_symbol(pos);
}

// @brief 出力のシンボルを得る．
const string&
AigModel::output_symbol(
  SizeType pos
) const
{
  return mImpl->output_symbol(pos);
}

// @brief コメントを得る．
string
AigModel::comment() const
{
  return mImpl->comment();
}

END_NAMESPACE_YM_AIG
