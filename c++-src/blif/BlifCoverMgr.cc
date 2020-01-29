
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

// キー生成関数その１
string
key_func(int input_num,
	 int cube_num,
	 const string& ipat_str,
	 char opat_char)
{
  ostringstream buf;
  buf << input_num << ':'
      << opat_char << ':'
      << ipat_str;
  return buf.str();
}

// キー生成関数その2
string
key_func(const BlifCover* cover)
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
  return buf.str();
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

// @brief 登録されているカバー数を返す．
int
BlifCoverMgr::cover_num() const
{
  return mCoverArray.size();
}

// @brief ID番号から BlifCover を返す．
// @param[in] id ID番号
const BlifCover&
BlifCoverMgr::cover(int id) const
{
  ASSERT_COND( 0 <= id && id < cover_num() );

  return mCoverArray[id];
}

// @brief パタン文字列からカバーを返す．
// @param[in] input_num 入力数
// @param[in] cube_num キューブ数
// @param[in] ipat_str 入力パタン文字列
// @param[in] opat 出力パタン
int
BlifCoverMgr::pat2cover(int input_num,
			int cube_num,
			const string& ipat_str,
			char opat_char)
{
  // カバーを表す文字列を作る．
  auto key_str{key_func(input_num, cube_num, ipat_str, opat_char)};

  // すでに登録されているか調べる．
  if ( mCoverDict.count(key_str) > 0 ) {
    int id = mCoverDict.at(key_str);
    return id;
  }
  else {
    // 新しいカバーを作る．
    int id = new_cover(input_num, cube_num, ipat_str, opat_char);

    // そのカバーを登録する．
    mCoverDict.insert({key_str, id});
    return id;
  }
}

// @brief BlifCover を作る．
// @param[in] input_num 入力数
// @param[in] cube_num キューブ数
// @param[in] ipat_str 入力パタン文字列
// @param[in] opat 出力パタン
int
BlifCoverMgr::new_cover(int input_num,
			int cube_num,
			const string& ipat_str,
			char opat_char)
{
  vector<vector<SopPat>> ipat_list;
  ipat_list.reserve(cube_num);
  for ( int c = 0; c < cube_num; ++ c ) {
    vector<SopPat> ipat(input_num);
    for ( int i = 0; i < input_num; ++ i ) {
      switch ( ipat_str[c * input_num + i] ) {
      case '0':
	ipat[i] = SopPat::_0;
	break;

      case '1':
	ipat[i] = SopPat::_1;
	break;

      case '-':
	ipat[i] = SopPat::_X;
	break;

      default:
	ASSERT_NOT_REACHED;
      }
    }
    ipat_list.push_back(ipat);
  }
  SopPat opat = char2pat(opat_char);

  int id = cover_num();
  mCoverArray.push_back({input_num, ipat_list, opat});

  return id;
}

END_NAMESPACE_YM_BNET
