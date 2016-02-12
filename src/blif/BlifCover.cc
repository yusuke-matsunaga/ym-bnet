
/// @file BlifCover.cc
/// @brief BlifCover の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BlifCover.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BlifCover
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifCover::BlifCover()
{
}

// @brief デストラクタ
BlifCover::~BlifCover()
{
}

// @brief 入力パタンを返す．
// @param[in] ipos 入力番号 ( 0 <= ipos < input_num() )
// @param[in] cpos キューブ番号 ( 0 <= cpos < cube_num() )
// @return パタンを返す．
BlifPat
BlifCover::input_pat(ymuint ipos,
		     ymuint cpos) const
{
  // キューブ1つ分のブロック数
  ymuint nb1 = ((input_num() * 2) + 63) / 64;

  // キューブの先頭のブロック
  const ymuint64* block = &mPatArray[nb1 * cpos];

  ymuint blk = (ipos * 2) / 64;
  ymuint sft = (ipos * 2) % 64;
  ymuint64 tmp = (block[blk] >> sft) & 3U;
  switch ( tmp ) {
  case 0: return kBlifPat_0;
  case 1: return kBlifPat_1;
  case 2: return kBlifPat_d;
  }
  ASSERT_NOT_REACHED;
  return kBlifPat_d;
}

// @brief 内容を出力する．
// @param[in] s 出力先のストリーム
void
BlifCover::print(ostream& s) const
{
  for (ymuint c = 0; c < cube_num(); ++ c) {
    for (ymuint i = 0; i < input_num(); ++ i) {
      BlifPat ipat = input_pat(i, c);
      switch ( ipat ) {
      case kBlifPat_0: s << '0'; break;
      case kBlifPat_1: s << '1'; break;
      case kBlifPat_d: s << '-'; break;
      }
    }
    if ( input_num() > 0 ) {
      s << ' ';
    }
    BlifPat opat = output_pat();
    switch ( opat ) {
    case kBlifPat_0: s << '0'; break;
    case kBlifPat_1: s << '1'; break;
    case kBlifPat_d: ASSERT_NOT_REACHED;
    }
    s << endl;
  }
}

END_NAMESPACE_YM_BNET
