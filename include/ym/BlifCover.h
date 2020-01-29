#ifndef YM_BLIFCOVER_H
#define YM_BLIFCOVER_H

/// @file ym/BlifCover.h
/// @brief BlifCover のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"
#include "ym/Sop.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BlifCover BlifCover.h "ym/BlifCover.h"
/// @brief blif 形式の .names 本体のカバーを表すクラス
//////////////////////////////////////////////////////////////////////
class BlifCover
{
public:

  /// @brief コンストラクタ
  /// @param[in] ni 入力数
  /// @param[in] ipat_list 入力のパタンベクタ
  /// @param[in] opat 出力のパタン
  BlifCover(int ni,
	    const vector<vector<SopPat>>& ipat_list,
	    SopPat opat);

  /// @brief デストラクタ
  ~BlifCover() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力数を返す．
  int
  input_num() const;

  /// @brief キューブ数を返す．
  int
  cube_num() const;

  /// @brief 入力パタンを返す．
  /// @param[in] cpos キューブ番号 ( 0 <= cpos < cube_num() )
  /// @param[in] ipos 入力番号 ( 0 <= ipos < input_num() )
  /// @return パタンを返す．
  SopPat
  input_pat(int cpos,
	    int ipos) const;

  /// @brief 出力パタンを返す．
  ///
  /// - すべてのキューブに対して同一のパタンとなる．
  /// - ドントケアはない．
  SopPat
  output_pat() const;

  /// @brief 内容を出力する．
  /// @param[in] s 出力先のストリーム
  void
  print(ostream& s) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力数
  int mInputNum;

  // 入力パタンのリスト
  vector<vector<SopPat>> mInputPatList;

  // 出力パタン
  SopPat mOutputPat;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] ni 入力数
// @param[in] ipat_list 入力のパタンベクタ
// @param[in] opat 出力のパタン
inline
BlifCover::BlifCover(int ni,
		     const vector<vector<SopPat>>& ipat_list,
		     SopPat opat) :
  mInputNum{ni},
  mInputPatList{ipat_list},
  mOutputPat{opat}
{
}

// @brief 入力数を返す．
inline
int
BlifCover::input_num() const
{
  return mInputNum;
}

// @brief キューブ数を返す．
inline
int
BlifCover::cube_num() const
{
  return mInputPatList.size();
}

// @brief 入力パタンを返す．
// @param[in] cpos キューブ番号 ( 0 <= cpos < cube_num() )
// @param[in] ipos 入力番号 ( 0 <= ipos < input_num() )
// @return パタンを返す．
inline
SopPat
BlifCover::input_pat(int cpos,
		     int ipos) const
{
  ASSERT_COND( 0 <= cpos && cpos < cube_num() );
  ASSERT_COND( 0 <= ipos && ipos < input_num() );
  return mInputPatList[cpos][ipos];
}

// @brief 出力パタンを返す．
//
// - すべてのキューブに対して同一のパタンとなる．
// - ドントケアはない．
inline
SopPat
BlifCover::output_pat() const
{
  return mOutputPat;
}

END_NAMESPACE_YM_BNET

#endif // YM_BLIFCOVER_H
