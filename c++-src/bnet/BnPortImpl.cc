
/// @file BnPortImpl.cc
/// @brief BnPortImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BnPortImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnPortImpl
//////////////////////////////////////////////////////////////////////

// @brief ポート番号を返す．
SizeType
BnPortImpl::id() const
{
  return mId;
}

// @brief 名前を得る．
string
BnPortImpl::name() const
{
  return mName;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPort1
//////////////////////////////////////////////////////////////////////

// @brief ビット数を得る．
SizeType
BnPort1::bit_width() const
{
  return 1;
}

// @brief pos ビット目の内容を得る．
// @return 対応するノード番号を返す．
SizeType
BnPort1::bit(
  SizeType pos
) const
{
  ASSERT_COND( pos == 0 );
  return mBit;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPortN
//////////////////////////////////////////////////////////////////////

// @brief ビット数を得る．
SizeType
BnPortN::bit_width() const
{
  return mBits.size();
}

// @brief pos ビット目の内容を得る．
// @param[in] pos ビット位置 ( 0 <= pos < bit_width() )
// @return 対応するノード番号を返す．
SizeType
BnPortN::bit(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < bit_width() );
  return mBits[pos];
}

END_NAMESPACE_YM_BNET
