
/// @file BlifIdHash.cc
/// @brief BlifIdHash の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BlifIdHash.h"
#include "BlifIdCell.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BlifIdCell
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifIdCell::BlifIdCell(int id,
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
BlifIdCell::~BlifIdCell()
{
}


//////////////////////////////////////////////////////////////////////
// クラス BlifIdHash
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifIdHash::BlifIdHash() :
  mAlloc(4096),
  mTableSize(0),
  mTable(nullptr)
{
  alloc_table(1024);
}

// @brief デストラクタ
BlifIdHash::~BlifIdHash()
{
  clear();
  delete [] mTable;
}

// @brief 内容をクリアする．
void
BlifIdHash::clear()
{
  for (int i = 0; i < mTableSize; ++ i) {
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
// @return 対応する BlifIdCell を返す．
BlifIdCell*
BlifIdHash::find(const char* str,
		 bool create)
{
  ASSERT_COND( str != nullptr);

  int pos0 = hash_func(str);
  int pos = pos0 % mTableSize;
  for ( BlifIdCell* cell = mTable[pos]; cell; cell = cell->mLink ) {
    if ( strcmp(cell->mStr, str) == 0 ) {
      return cell;
    }
  }

  if ( !create ) {
    return nullptr;
  }

  if ( mCellArray.size() >= mNextLimit ) {
    // テーブルを拡張する．
    alloc_table(mTableSize * 2);
  }

  // 新しいセルを確保する．
  int l = strlen(str);
  int reqsize = sizeof(BlifIdCell) + l;
  void* p = mAlloc.get_memory(reqsize);
  BlifIdCell* cell = new (p) BlifIdCell(mCellArray.size(), str);
  mCellArray.push_back(cell);

  // テーブルに追加する．
  pos = pos0 % mTableSize;
  cell->mLink = mTable[pos];
  mTable[pos] = cell;
  return cell;
}

// ハッシュ表を拡大する．
void
BlifIdHash::alloc_table(int new_size)
{
  BlifIdCell** old_table = mTable;
  int old_size = mTableSize;

  mTable = new BlifIdCell*[new_size];
  mTableSize = new_size;
  mNextLimit = static_cast<int>(mTableSize * 1.8);

  for ( int i = 0; i < new_size; ++ i ) {
    mTable[i] = nullptr;
  }
  for ( int i = 0; i < old_size; ++ i ) {
    for ( BlifIdCell* cell = old_table[i]; cell; ) {
      BlifIdCell* next = cell->mLink;
      int pos1 = hash_func(cell->mStr) % mTableSize;
      cell->mLink = mTable[pos1];
      mTable[pos1] = cell;
      cell = next;
    }
  }
  delete [] old_table;
}

// @brief ID 番号から文字列を得る．
const char*
BlifIdHash::str(int id) const
{
  return cell(id)->str();
}

// @brief ID番号から位置情報を得る．
const FileRegion&
BlifIdHash::loc(int id) const
{
  return cell(id)->loc();
}

// @brief この識別子を定義している位置情報を返す．
const FileRegion&
BlifIdHash::def_loc(int id) const
{
  return cell(id)->def_loc();
}

END_NAMESPACE_YM_BNET
