
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
BlifCover::Pat
BlifCover::input_pat(int ipos,
		     int cpos) const
{
  ASSERT_COND( ipos >= 0 && ipos < input_num() );
  ASSERT_COND( cpos >= 0 && cpos < cube_num() );

  // キューブ1つ分のブロック数
  int nb1 = ((input_num() * 2) + 63) / 64;

  int blk = (ipos * 2) / 64;
  int sft = (ipos * 2) % 64;
  ymuint64 tmp = (mPatArray[nb1 * cpos + blk] >> sft) & 3U;
  switch ( tmp ) {
  case 0: return kPat_0;
  case 1: return kPat_1;
  case 2: return kPat_d;
  }
  ASSERT_NOT_REACHED;
  return kPat_d;
}

// @brief 内容を出力する．
// @param[in] s 出力先のストリーム
void
BlifCover::print(ostream& s) const
{
  for ( int c = 0; c < cube_num(); ++ c ) {
    for ( int i = 0; i < input_num(); ++ i ) {
      s << input_pat(i, c);
    }
    if ( input_num() > 0 ) {
      s << ' ';
    }
    s << output_pat() << endl;
  }
}

END_NAMESPACE_YM_BNET
