
/// @file CoverMgr.cc
/// @brief CoverMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "CoverMgr.h"


BEGIN_NAMESPACE_YM_BLIF

BEGIN_NONAMESPACE

// キー生成関数その１
string
key_func(
  SizeType input_num,
  const string& ipat_str,
  char opat_char
)
{
  ostringstream buf;
  buf << input_num << ':'
      << opat_char << ':'
      << ipat_str;
  return buf.str();
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス BlifCover
//////////////////////////////////////////////////////////////////////

Expr
BlifCover::expr() const
{
  auto expr = mInputCover.expr();
  if ( output_pat() == '0' ) {
    expr = ~expr;
  }
  return expr;
}

// @brief 内容を出力する．
void
BlifCover::print(
  ostream& s
) const
{
  for ( SizeType c = 0; c < cube_num(); ++ c ) {
    for ( SizeType i = 0; i < input_num(); ++ i ) {
      s << input_pat(c, i);
    }
    if ( input_num() > 0 ) {
      s << ' ';
    }
    s << output_pat() << endl;
  }
}


//////////////////////////////////////////////////////////////////////
// クラス CoverMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
CoverMgr::CoverMgr(
  ModelImpl* model
) : mModel{model}
{
}

// @brief 登録されているカバー数を返す．
SizeType
CoverMgr::cover_num() const
{
  return mModel->mCoverArray.size();
}

// @brief パタン文字列からカバーを返す．
SizeType
CoverMgr::pat2cover(
  SizeType input_num,
  SizeType cube_num,
  const string& ipat_str,
  char opat_char
)
{
  // カバーを表す文字列を作る．
  auto key_str{key_func(input_num, ipat_str, opat_char)};

  // すでに登録されているか調べる．
  if ( mCoverDict.count(key_str) > 0 ) {
    auto id = mCoverDict.at(key_str);
    return id;
  }
  else {
    // 新しいカバーを作る．
    auto id = new_cover(input_num, cube_num, ipat_str, opat_char);

    // そのカバーを登録する．
    mCoverDict.insert({key_str, id});
    return id;
  }
}

// @brief SopCover を作る．
SizeType
CoverMgr::new_cover(
  SizeType input_num,
  SizeType cube_num,
  const string& ipat_str,
  char opat
)
{
  vector<vector<Literal>> cube_list;
  cube_list.reserve(cube_num);
  for ( SizeType c = 0; c < cube_num; ++ c ) {
    string ipat = ipat_str.substr(c * input_num, (c + 1) * input_num);
    vector<Literal> cube;
    cube.reserve(input_num);
    for ( SizeType var = 0; var < input_num; ++ var ) {
      switch ( ipat[var] ) {
      case '0': cube.push_back(Literal{var, true}); break;
      case '1': cube.push_back(Literal{var, false}); break;
      case '-': break;
      default: ASSERT_NOT_REACHED; break;
      }
    }
    cube_list.push_back(cube);
  }

  auto id = cover_num();
  mModel->mCoverArray.push_back(BlifCover{input_num, cube_list, opat});

  return id;
}

END_NAMESPACE_YM_BLIF
