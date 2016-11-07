
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
BnPortImpl::BnPortImpl(const string& name) :
  mName(name)
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


//////////////////////////////////////////////////////////////////////
// クラス BnPort1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] name 名前
// @param[in] bit ノード番号
BnPort1::BnPort1(const string& name,
		 ymuint bit) :
  BnPortImpl(name),
  mBit(bit)
{
}

// @brief デストラクタ
BnPort1::~BnPort1()
{
}

// @brief ビット数を得る．
ymuint
BnPort1::bit_width() const
{
  return 1;
}

// @brief pos ビット目の内容を得る．
// @param[in] pos ビット位置 ( 0 <= pos < bit_width() )
// @return 対応するノード番号を返す．
ymuint
BnPort1::bit(ymuint pos) const
{
  ASSERT_COND( pos == 0 );
  return mBit;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPortN
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] name 名前
// @param[in] bits 内容のベクタ
BnPortN::BnPortN(const string& name,
		 const vector<ymuint>& bits) :
  BnPortImpl(name),
  mBits(bits)
{
}

// @brief デストラクタ
BnPortN::~BnPortN()
{
}

// @brief ビット数を得る．
ymuint
BnPortN::bit_width() const
{
  return mBits.size();
}

// @brief pos ビット目の内容を得る．
// @param[in] pos ビット位置 ( 0 <= pos < bit_width() )
// @return 対応するノード番号を返す．
ymuint
BnPortN::bit(ymuint pos) const
{
  ASSERT_COND( pos < bit_width() );
  return mBits[pos];
}

END_NAMESPACE_YM_BNET
