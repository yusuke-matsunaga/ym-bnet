
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
// @param[in] id ポート番号
// @param[in] name 名前
BnPortImpl::BnPortImpl(int id,
		       const string& name) :
  mId(id),
  mName(name)
{
}

// @brief デストラクタ
BnPortImpl::~BnPortImpl()
{
}

// @brief ポート番号を返す．
int
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

// @brief コンストラクタ
// @param[in] id ポート番号
// @param[in] name 名前
// @param[in] bit ノード番号
BnPort1::BnPort1(int id,
		 const string& name,
		 int bit) :
  BnPortImpl(id, name),
  mBit(bit)
{
}

// @brief デストラクタ
BnPort1::~BnPort1()
{
}

// @brief ビット数を得る．
int
BnPort1::bit_width() const
{
  return 1;
}

// @brief pos ビット目の内容を得る．
// @param[in] pos ビット位置 ( 0 <= pos < bit_width() )
// @return 対応するノード番号を返す．
int
BnPort1::bit(int pos) const
{
  ASSERT_COND( pos == 0 );
  return mBit;
}


//////////////////////////////////////////////////////////////////////
// クラス BnPortN
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ポート番号
// @param[in] name 名前
// @param[in] bits 内容のベクタ
BnPortN::BnPortN(int id,
		 const string& name,
		 const vector<int>& bits) :
  BnPortImpl(id, name),
  mBitWidth(bits.size())
{
  mBits = new int[mBitWidth];
  for ( int i = 0; i < mBitWidth; ++ i ) {
    mBits[i] = bits[i];
  }
}

// @brief デストラクタ
BnPortN::~BnPortN()
{
  delete [] mBits;
}

// @brief ビット数を得る．
int
BnPortN::bit_width() const
{
  return mBitWidth;
}

// @brief pos ビット目の内容を得る．
// @param[in] pos ビット位置 ( 0 <= pos < bit_width() )
// @return 対応するノード番号を返す．
int
BnPortN::bit(int pos) const
{
  ASSERT_COND( pos < bit_width() );
  return mBits[pos];
}

END_NAMESPACE_YM_BNET
