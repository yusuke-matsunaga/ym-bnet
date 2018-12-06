
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
SizeType
hash_func(const string& str)
{
  SizeType h = 0;
  for ( int i = 0; i < str.size(); ++ i ) {
    h = h * 33 + str[i];
  }
  return h;
}

// ハッシュ関数その1
SizeType
hash_func(int input_num,
	  int cube_num,
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
SizeType
hash_func(BlifCover* cover)
{
  ostringstream buf;
  buf << cover->input_num() << ':'
      << cover->output_pat() << ':';
  int nc = cover->cube_num();
  int ni = cover->input_num();
  for ( int c = 0; c < nc; ++ c ) {
    for ( int i = 0; i < ni; ++ i ) {
      buf << cover->input_pat(c, i);
    }
  }
  return hash_func(buf.str());
}

// char -> Pat
inline
SopPat
char2pat(char ch)
{
  switch ( ch ) {
  case '0': return SopPat::_0;
  case '1': return SopPat::_1;
  case '-': return SopPat::_X;
  }
  cout << "ERROR: ch = " << ch << endl;
  ASSERT_NOT_REACHED;
  return SopPat::_X;
}

// 等価チェック
bool
check_equal(BlifCover* cover,
	    int input_num,
	    int cube_num,
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
  int j = 0;
  for ( int c = 0; c < cube_num; ++ c ) {
    for ( int i = 0; i < input_num; ++ i, ++ j ) {
      SopPat ipat = cover->input_pat(c, i);
      SopPat ipat2 = char2pat(ipat_str[j]);
      if ( ipat != ipat2 ) {
	return false;
      }
    }
  }
  return true;
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス BlifCover
//////////////////////////////////////////////////////////////////////

// @brief 内容を出力する．
// @param[in] s 出力先のストリーム
void
BlifCover::print(ostream& s) const
{
  for ( int c = 0; c < cube_num(); ++ c ) {
    for ( int i = 0; i < input_num(); ++ i ) {
      s << input_pat(c, i);
    }
    if ( input_num() > 0 ) {
      s << ' ';
    }
    s << output_pat() << endl;
  }
}


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
  for ( int i = 0; i < mHashSize; ++ i ) {
    for ( BlifCover* cover = mHashTable[i]; cover != nullptr; ) {
      BlifCover* cur_cover = cover;
      cover = cur_cover->mLink;
      delete cur_cover;
    }
  }
}

// @brief パタン文字列からカバーを返す．
// @param[in] input_num 入力数
// @param[in] cube_num キューブ数
// @param[in] ipat_str 入力パタン文字列
// @param[in] opat 出力パタン
const BlifCover&
BlifCoverMgr::pat2cover(int input_num,
			int cube_num,
			const string& ipat_str,
			char opat_char)
{
  // すでに登録されているか調べる．
  SizeType h = hash_func(input_num, cube_num, ipat_str, opat_char) % mHashSize;
  for ( BlifCover* cover = mHashTable[h];
	cover != nullptr; cover = cover->mLink ) {
    if ( check_equal(cover, input_num, cube_num, ipat_str, opat_char) ) {
      return *cover;
    }
  }
  // 新しいカバーを登録する．
  BlifCover* cover = new_cover(input_num, cube_num, ipat_str, opat_char);
  return *cover;
}

// @brief BlifCover を作る．
// @param[in] input_num 入力数
// @param[in] cube_num キューブ数
// @param[in] ipat_str 入力パタン文字列
// @param[in] opat 出力パタン
BlifCover*
BlifCoverMgr::new_cover(int input_num,
			int cube_num,
			const string& ipat_str,
			char opat_char)
{
  SopPat opat = char2pat(opat_char);

  int k = 0;
  vector<vector<Literal>> cube_list(cube_num);
  vector<Expr> prod_list(cube_num);
  for ( int c = 0; c < cube_num; ++ c ) {
    int shift = 0;
    vector<Literal> lit_list;
    vector<Expr> litexpr_list;
    lit_list.reserve(input_num);
    litexpr_list.reserve(input_num);
    for ( int i = 0; i < input_num; ++ i, ++ k ) {
      VarId var(i);
      Literal lit(var);
      switch ( ipat_str[k] ) {
      case '0':
	lit_list.push_back(~lit);
	litexpr_list.push_back(Expr::nega_literal(var));
	break;

      case '1':
	lit_list.push_back(lit);
	litexpr_list.push_back(Expr::posi_literal(var));
	break;

      case '-':
	break;

      default:
	ASSERT_NOT_REACHED;
      }
    }
    cube_list[c].swap(lit_list);
    prod_list[c] = Expr::make_and(litexpr_list);
  }
  Expr expr = Expr::make_or(prod_list);
  if ( opat == SopPat::_0 ) {
    expr = ~expr;
  }

  BlifCover* cover = new BlifCover(mCoverNum, input_num, cube_list, opat, expr);
  mCoverArray.push_back(cover);
  ++ mCoverNum;
  if ( mCoverNum >= mNextLimit ) {
    // ハッシュ表のサイズを2倍する．
    alloc_table(mHashSize * 2);
  }
  SizeType h = hash_func(cover) % mHashSize;
  cover->mLink = mHashTable[h];
  mHashTable[h] = cover;

  return cover;
}

// @brief ハッシュ表のメモリを確保する．
// @param[in] req_size 要求サイズ
void
BlifCoverMgr::alloc_table(int req_size)
{
  int old_size = mHashSize;
  BlifCover** old_table = mHashTable;
  mHashSize = req_size;
  mNextLimit = static_cast<int>(mHashSize * 1.8);
  mHashTable = new BlifCover*[mHashSize];
  for ( int i = 0; i < mHashSize; ++ i ) {
    mHashTable[i] = nullptr;
  }
  for ( int i = 0; i < old_size; ++ i ) {
    for ( BlifCover* cover = old_table[i]; cover != nullptr; ) {
      BlifCover* next = cover->mLink;
      SizeType h = hash_func(cover) % mHashSize;
      cover->mLink = mHashTable[h];
      mHashTable[h] = cover;
      cover = next;
    }
  }
  delete [] old_table;
}

END_NAMESPACE_YM_BNET
