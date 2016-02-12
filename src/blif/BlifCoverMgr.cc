
/// @file BlifCoverMgr.cc
/// @brief BlifCoverMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BlifCoverMgr.h"
#include "ym/BlifCover.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

// 文字列からのハッシュ関数
ymuint
hash_func(const string& str)
{
  ymuint h = 0;
  for (ymuint i = 0; i < str.size(); ++ i) {
    h = h * 33 + str[i];
  }
  return h;
}

// ハッシュ関数その1
ymuint
hash_func(ymuint input_num,
	  ymuint cube_num,
	  const string& ipat_str,
	  BlifPat opat)
{
  ostringstream buf;
  buf << input_num << ':'
      << cube_num << ':'
      << opat << ':'
      << ipat_str;
  return hash_func(buf.str());
}

// ハッシュ関数その2
ymuint
hash_func(BlifCover* cover)
{
  ostringstream buf;
  buf << cover->input_num() << ':'
      << cover->cube_num() << ':'
      << cover->output_pat() << ':';
  for (ymuint c = 0; c < cover->cube_num(); ++ c) {
    for (ymuint i = 0; i < cover->input_num(); ++ i) {
      buf << cover->input_pat(i, c);
    }
  }
  return hash_func(buf.str());
}

// 等価チェック
bool
check_equal(BlifCover* cover,
	    ymuint input_num,
	    ymuint cube_num,
	    const string& ipat_str,
	    BlifPat opat)
{
  if ( cover->input_num() != input_num ) {
    return false;
  }
  if ( cover->cube_num() != cube_num ) {
    return false;
  }
  if ( cover->output_pat() != opat ) {
    return false;
  }
  ymuint j = 0;
  for (ymuint c = 0; c < cube_num; ++ c) {
    for (ymuint i = 0; i < input_num; ++ i, ++ j) {
      BlifPat ipat = cover->input_pat(i, c);
      BlifPat ipat2;
      switch ( ipat_str[j] ) {
      case '0': ipat2 = kBlifPat_0; break;
      case '1': ipat2 = kBlifPat_1; break;
      case '2': ipat2 = kBlifPat_d; break;
      default: ASSERT_NOT_REACHED;
      }
      if ( ipat != ipat2 ) {
	return false;
      }
    }
  }
  return true;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス BlifCoverMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BlifCoverMgr::BlifCoverMgr()
{
  mCoverNum = 0;
  mHashSize = 0;
  mNextLimit = 0;
  mHashTable = nullptr;
  alloc_table(1024);
}

// @brief デストラクタ
BlifCoverMgr::~BlifCoverMgr()
{
  // カバーは SimpleAlloc のデストラクタで解放する．
}

// @brief パタン文字列からカバーを返す．
// @param[in] input_num 入力数
// @param[in] cube_num キューブ数
// @param[in] ipat_str 入力パタン文字列
// @param[in] opat 出力パタン
const BlifCover*
BlifCoverMgr::pat2cover(ymuint input_num,
			ymuint cube_num,
			const string& ipat_str,
			BlifPat opat)
{
  // すでに登録されているか調べる．
  ymuint h = hash_func(input_num, cube_num, ipat_str, opat) % mHashSize;
  for (BlifCover* cover = mHashTable[h];
       cover != nullptr; cover = cover->mLink) {
    if ( check_equal(cover, input_num, cube_num, ipat_str, opat) ) {
      return cover;
    }
  }
  // 新しいカバーを登録する．
  if ( mCoverNum >= mNextLimit ) {
    // ハッシュ表のサイズを2倍する．
    alloc_table(mHashSize * 2);
  }
  BlifCover* cover = new_cover(input_num, cube_num, ipat_str, opat);
  reg_cover(cover);
  return cover;
}

// @brief BlifCover を作る．
// @param[in] input_num 入力数
// @param[in] cube_num キューブ数
// @param[in] ipat_str 入力パタン文字列
// @param[in] opat 出力パタン
BlifCover*
BlifCoverMgr::new_cover(ymuint input_num,
			ymuint cube_num,
			const string& ipat_str,
			BlifPat opat)
{
  // キューブ1つ分のブロック数
  ymuint nb1 = ((input_num * 2) + 63) / 64;
  // 全ブロック数
  ymuint nb = nb1 * cube_num;

  void* p = mAlloc.get_memory(sizeof(BlifCover) + sizeof(ymuint64) * (nb - 1));
  BlifCover* cover = new (p) BlifCover;
  cover->mInputNum = input_num;
  cover->mOutputPat = opat;
  cover->mCubeNum = cube_num;
  cover->mId = mCoverArray.size();
  mCoverArray.push_back(cover);
  ymuint j = 0;
  ymuint k = 0;
  for (ymuint c = 0; c < cube_num; ++ c) {
    ymuint64 tmp = 0UL;
    ymuint shift = 0;
    for (ymuint i = 0; i < input_num; ++ i, ++ k) {
      ymuint64 pat;
      switch ( ipat_str[k] ) {
      case '0': pat = 0UL; break;
      case '1': pat = 1UL; break;
      case '-': pat = 2UL; break;
      default: ASSERT_NOT_REACHED;
      }
      tmp |= (pat << shift);
      shift += 2;
      if ( shift == 64 ) {
	cover->mPatArray[j] = tmp;
	shift = 0;
	++ j;
      }
    }
    if ( shift > 0 ) {
      cover->mPatArray[j] = tmp;
      ++ j;
    }
  }

  return cover;
}

// @brief カバーを登録する．
void
BlifCoverMgr::reg_cover(BlifCover* cover)
{
  ymuint h = hash_func(cover) % mHashSize;
  cover->mLink = mHashTable[h];
  mHashTable[h] = cover;
}

// @brief ハッシュ表のメモリを確保する．
// @param[in] req_size 要求サイズ
void
BlifCoverMgr::alloc_table(ymuint req_size)
{
  ymuint old_size = mHashSize;
  BlifCover** old_table = mHashTable;
  mHashSize = req_size;
  mNextLimit = static_cast<ymuint>(mHashSize * 1.8);
  mHashTable = new BlifCover*[mHashSize];
  for (ymuint i = 0; i < mHashSize; ++ i) {
    mHashTable[i] = nullptr;
  }
  for (ymuint i = 0; i < old_size; ++ i) {
    for (BlifCover* cover = old_table[i]; cover != nullptr; ) {
      BlifCover* next = cover->mLink;
      reg_cover(cover);
      cover = next;
    }
  }
  delete [] old_table;
}

END_NAMESPACE_YM_BNET
