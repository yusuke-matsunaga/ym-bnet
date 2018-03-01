
/// @file Iscas89IdHash.cc
/// @brief Iscas89IdHash の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "Iscas89IdHash.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス Iscas89IdCell
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Iscas89IdCell::Iscas89IdCell(int id,
			     const char* str) :
  mId(id),
  mFlags(0U),
  mLink(nullptr)
{
  const char* s = str;
  char* d = mStr;
  while ( ((*d ++) = (*s ++)) ) ;
}

// @brief デストラクタ
Iscas89IdCell::~Iscas89IdCell()
{
}


//////////////////////////////////////////////////////////////////////
// クラス Iscas89IdHash
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Iscas89IdHash::Iscas89IdHash() :
  mAlloc(4096),
  mTableSize(0)
{
  alloc_table(1024);
}

// @brief デストラクタ
Iscas89IdHash::~Iscas89IdHash()
{
  clear();
  delete [] mTable;
}

// @brief 内容をクリアする．
void
Iscas89IdHash::clear()
{
  for ( int i = 0; i < mTableSize; ++ i ) {
    mTable[i] = nullptr;
  }
  mCellArray.clear();
  mAlloc.destroy();
}


BEGIN_NONAMESPACE

inline
int
hash_func(const char* str)
{
  int h = 0;
  for ( char c; (c = *str); ++ str ) {
    h = h * 33 + static_cast<int>(c);
  }
  return h;
}

END_NONAMESPACE

// @brief 識別子に対応するセルを探す．
// @param[in] str 文字列
// @param[in] create 存在しないときに新規生成するなら true
// @return 対応する Iscas89IdCell を返す．
Iscas89IdCell*
Iscas89IdHash::find(const char* str,
		    bool create)
{
  ASSERT_COND(str );
  int pos0 = hash_func(str);
  int pos = pos0 % mTableSize;
  for ( Iscas89IdCell* cell = mTable[pos]; cell; cell = cell->mLink ) {
    if ( strcmp(cell->mStr, str) == 0 ) {
      return cell;
    }
  }

  if ( !create ) {
    return nullptr;
  }

  if ( mCellArray.size() >= mNextLimit ) {
    // テーブルを拡張する．
    Iscas89IdCell** old_table = mTable;
    int old_size = mTableSize;
    alloc_table(old_size * 2);
    for ( int i = 0; i < old_size; ++ i ) {
      for ( Iscas89IdCell* cell = old_table[i]; cell; ) {
	Iscas89IdCell* next = cell->mLink;
	int pos1 = hash_func(cell->mStr) % mTableSize;
	cell->mLink = mTable[pos1];
	mTable[pos1] = cell;
	cell = next;
      }
    }
    delete [] old_table;
  }

  // 新しいセルを確保する．
  int l = strlen(str);
  int reqsize = sizeof(Iscas89IdCell) + l;
  void* p = mAlloc.get_memory(reqsize);
  Iscas89IdCell* cell = new (p) Iscas89IdCell(mCellArray.size(), str);
  mCellArray.push_back(cell);

  // テーブルに追加する．
  pos = pos0 % mTableSize;
  cell->mLink = mTable[pos];
  mTable[pos] = cell;
  return cell;
}

// ハッシュ表を拡大する．
void
Iscas89IdHash::alloc_table(int new_size)
{
  mTable = new Iscas89IdCell*[new_size];
  mTableSize = new_size;
  mNextLimit = static_cast<int>(mTableSize * 1.8);
  for ( int i = 0; i < new_size; ++ i ) {
    mTable[i] = nullptr;
  }
}

END_NAMESPACE_YM_BNET
