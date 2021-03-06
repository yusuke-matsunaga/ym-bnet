
/// @file BlifCoverMgr.cc
/// @brief BlifCoverMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BlifCoverMgr.h"
#include "ym/BlifCover.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

// キー生成関数その１
string
key_func(int input_num,
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
const BlifCover&
BlifCoverMgr::cover(
  int id
) const
{
  ASSERT_COND( 0 <= id && id < cover_num() );

  return mCoverArray[id];
}

// @brief パタン文字列からカバーを返す．
int
BlifCoverMgr::pat2cover(
  int input_num,
  int cube_num,
  const string& ipat_str,
  char opat_char
)
{
  // カバーを表す文字列を作る．
  auto key_str{key_func(input_num, ipat_str, opat_char)};

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
int
BlifCoverMgr::new_cover(
  int input_num,
  int cube_num,
  const string& ipat_str,
  char opat
)
{
  vector<string> ipat_list;
  ipat_list.reserve(cube_num);
  for ( int c = 0; c < cube_num; ++ c ) {
    string ipat = ipat_str.substr(c * input_num, (c + 1) * input_num);
    ipat_list.push_back(ipat);
  }

  int id = cover_num();
  mCoverArray.push_back({input_num, ipat_list, opat});

  return id;
}

END_NAMESPACE_YM_BNET
