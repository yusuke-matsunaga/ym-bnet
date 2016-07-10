#ifndef BLIFCOVER_H
#define BLIFCOVER_H

/// @file BlifCover.h
/// @brief BlifCover のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym_bnet.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BlifCover BlifCover.h "ym/BlifCover.h"
/// @brief blif 形式の .names 本体のカバーを表すクラス
//////////////////////////////////////////////////////////////////////
class BlifCover
{
  friend class BlifCoverMgr;

public:
  //////////////////////////////////////////////////////////////////////
  // パタンを表す列挙型
  //////////////////////////////////////////////////////////////////////
  enum Pat {
    kPat_0,
    kPat_1,
    kPat_d
  };

private:

  /// @brief コンストラクタ
  BlifCover();

  /// @brief デストラクタ
  ~BlifCover();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  ymuint
  id() const;

  /// @brief 入力数を返す．
  ymuint
  input_num() const;

  /// @brief キューブ数を返す．
  ymuint
  cube_num() const;

  /// @brief 入力パタンを返す．
  /// @param[in] ipos 入力番号 ( 0 <= ipos < input_num() )
  /// @param[in] cpos キューブ番号 ( 0 <= cpos < cube_num() )
  /// @return パタンを返す．
  Pat
  input_pat(ymuint ipos,
	    ymuint cpos) const;

  /// @brief 出力パタンを返す．
  ///
  /// - すべてのキューブに対して同一のパタンとなる．
  /// - ドントケアはない．
  Pat
  output_pat() const;

  /// @brief 対応する論理式を返す．
  Expr
  expr() const;

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
  ymuint8 mInputNum;

  // 出力パタン
  ymuint8 mOutputPat;

  // キューブ数
  ymuint16 mCubeNum;

  // ID番号
  ymuint32 mId;

  // 論理式
  Expr mExpr;

  // 次の要素を指すリンクポインタ
  BlifCover* mLink;

  // パタンを表すビット配列
  // 一つのパタンを2ビットで表す．
  ymuint64 mPatArray[1];

};

/// @brief BlifCover::Pat のストリーム出力
ostream&
operator<<(ostream& s,
	   BlifCover::Pat pat);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ID番号を返す．
inline
ymuint
BlifCover::id() const
{
  return mId;
}

// @brief 入力数を返す．
inline
ymuint
BlifCover::input_num() const
{
  return mInputNum;
}

// @brief キューブ数を返す．
inline
ymuint
BlifCover::cube_num() const
{
  return mCubeNum;
}

// @brief 出力パタンを返す．
//
// - すべてのキューブに対して同一のパタンとなる．
// - ドントケアはない．
inline
BlifCover::Pat
BlifCover::output_pat() const
{
  switch ( mOutputPat ) {
  case 0: return kPat_0;
  case 1: return kPat_1;
  }
  ASSERT_NOT_REACHED;
  return kPat_d;
}

// @brief 対応する論理式を返す．
inline
Expr
BlifCover::expr() const
{
  return mExpr;
}

// @brief BlifCover::Pat のストリーム出力
inline
ostream&
operator<<(ostream& s,
	   BlifCover::Pat pat)
{
  switch ( pat ) {
  case BlifCover::kPat_0: s << '0'; break;
  case BlifCover::kPat_1: s << '1'; break;
  case BlifCover::kPat_d: s << '-'; break;
  default: ASSERT_NOT_REACHED;
  }
  return s;
}

END_NAMESPACE_YM_BNET

#endif // BLIFCOVER_H
