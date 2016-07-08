
/// @file BnPortImpl.cc
/// @brief BnPortImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnPortImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnPortImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] name 名前
// @param[in] dir 方向
// @param[in] bits 内容のベクタ
BnPortImpl::BnPortImpl(const string& name,
		       Direction dir,
		       const vector<ymuint>& bits) :
  mName(name),
  mDirection(dir),
  mBits(bits)
{
}

// @brief デストラクタ
BnPortImpl::~BnPortImpl()
{
}

// @brief 名前を得る．
string
BnPortImpl::name() const
{
  return mName;
}

// @brief 方向
BnPort::Direction
BnPortImpl::direction() const
{
  return mDirection;
}

// @brief ビット数を得る．
ymuint
BnPortImpl::bit_width() const
{
  return mBits.size();
}

// @brief pos ビット目の内容を得る．
// @param[in] pos ビット位置 ( 0 <= pos < bit_width() )
// @return 対応するノードのIDを返す．
ymuint
BnPortImpl::bit(ymuint pos) const
{
  ASSERT_COND( pos < bit_width() );
  return mBits[pos];
}

END_NAMESPACE_YM_BNET
