
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
	  char opat_char)
{
  ostringstream buf;
  buf << input_num << ':'
      << opat_char << ':'
      << ipat_str;
  return hash_func(buf.str());
}

// ハッシュ関数その2
ymuint
hash_func(BlifCover* cover)
{
  ostringstream buf;
  buf << cover->input_num() << ':'
      << cover->output_pat() << ':';
  for (ymuint c = 0; c < cover->cube_num(); ++ c) {
    for (ymuint i = 0; i < cover->input_num(); ++ i) {
      buf << cover->input_pat(i, c);
    }
  }
  return hash_func(buf.str());
}

// char -> Pat
BlifCover::Pat
char2pat(char ch)
{
  switch ( ch ) {
  case '0': return BlifCover::kPat_0;
  case '1': return BlifCover::kPat_1;
  case '-': return BlifCover::kPat_d;
  }
  cout << "ERROR: ch = " << ch << endl;
  ASSERT_NOT_REACHED;
  return BlifCover::kPat_d;
}

// 等価チェック
bool
check_equal(BlifCover* cover,
	    ymuint input_num,
	    ymuint cube_num,
	    const string& ipat_str,
	    char opat_char)
{
  if ( cover->input_num() != input_num ) {
    return false;
  }
  if ( cover->cube_num() != cube_num ) {
    return false;
  }
  if ( cover->output_pat() != char2pat(opat_char) ) {
    return false;
  }
  ymuint j = 0;
  for (ymuint c = 0; c < cube_num; ++ c) {
    for (ymuint i = 0; i < input_num; ++ i, ++ j) {
      BlifCover::Pat ipat = cover->input_pat(i, c);
      BlifCover::Pat ipat2 = char2pat(ipat_str[j]);
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
  // カバーは領域自体は SimpleAlloc のデストラクタで解放する．
  // ただし BlifCover のデストラクタは呼んでおく必要がある．
  for (ymuint i = 0; i < mHashSize; ++ i) {
    for (BlifCover* cover = mHashTable[i]; cover != nullptr; ) {
      BlifCover* cur_cover = cover;
      cover = cur_cover->mLink;
      // デストラクタの明示的な起動
      cur_cover->~BlifCover();
    }
  }
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
			char opat_char)
{
  // すでに登録されているか調べる．
  ymuint h = hash_func(input_num, cube_num, ipat_str, opat_char) % mHashSize;
  for (BlifCover* cover = mHashTable[h];
       cover != nullptr; cover = cover->mLink) {
    if ( check_equal(cover, input_num, cube_num, ipat_str, opat_char) ) {
      return cover;
    }
  }
  // 新しいカバーを登録する．
  if ( mCoverNum >= mNextLimit ) {
    // ハッシュ表のサイズを2倍する．
    alloc_table(mHashSize * 2);
  }
  BlifCover* cover = new_cover(input_num, cube_num, ipat_str, opat_char);
  reg_cover(cover);
  ++ mCoverNum;
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
			char opat_char)
{
  // キューブ1つ分のブロック数
  ymuint nb1 = ((input_num * 2) + 63) / 64;
  // 全ブロック数
  ymuint nb = nb1 * cube_num;

  void* p = mAlloc.get_memory(sizeof(BlifCover) + sizeof(ymuint64) * (nb - 1));
  BlifCover* cover = new (p) BlifCover;
  cover->mInputNum = input_num;
  if ( opat_char == '0' ) {
    cover->mOutputPat = BlifCover::kPat_0;
  }
  else if ( opat_char == '1' ) {
    cover->mOutputPat = BlifCover::kPat_1;
  }
  else {
    ASSERT_NOT_REACHED;
  }
  cover->mCubeNum = cube_num;
  cover->mId = mCoverArray.size();

  ymuint j = 0;
  ymuint k = 0;
  vector<Expr> prod_list(cube_num);
  for (ymuint c = 0; c < cube_num; ++ c) {
    ymuint64 tmp = 0UL;
    ymuint shift = 0;
    vector<Expr> lit_list;
    lit_list.reserve(input_num);
    for (ymuint i = 0; i < input_num; ++ i, ++ k) {
      ymuint64 pat;
      switch ( ipat_str[k] ) {
      case '0':
	pat = 0UL;
	lit_list.push_back(Expr::nega_literal(VarId(i)));
	break;

      case '1':
	pat = 1UL;
	lit_list.push_back(Expr::posi_literal(VarId(i)));
	break;

      case '-':
	pat = 2UL;
	break;

      default:
	ASSERT_NOT_REACHED;
      }
      tmp |= (pat << shift);
      shift += 2;
      if ( shift == 64 ) {
	cover->mPatArray[j] = tmp;
	tmp = 0UL;
	shift = 0;
	++ j;
      }
    }
    if ( shift > 0 ) {
      cover->mPatArray[j] = tmp;
      ++ j;
    }
    prod_list[c] = Expr::make_and(lit_list);
  }
  Expr expr = Expr::make_or(prod_list);

  if ( cover->mOutputPat == BlifCover::kPat_0 ) {
    expr = ~expr;
  }

  cover->mExpr = expr;

  mCoverArray.push_back(cover);

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
