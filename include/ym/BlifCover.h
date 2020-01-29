#ifndef YM_BLIFCOVER_H
#define YM_BLIFCOVER_H

/// @file ym/BlifCover.h
/// @brief BlifCover のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"
#include "ym/Expr.h"
#include "ym/SopCover.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BlifCover BlifCover.h "ym/BlifCover.h"
/// @brief blif 形式の .names 本体のカバーを表すクラス
///
/// 実際にはキューブ数に応じてアロケートするサイズが異なるので
/// 通常のコンストラクタは使用できない．
/// 必ず BlifCoverMgr を用いること．
//////////////////////////////////////////////////////////////////////
class BlifCover
{
  friend class BlifCoverMgr;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] ni 入力数
  /// @param[in] cube_list キューブを表すリテラルのリストのリスト
  /// @param[in] opat 出力のパタン
  /// @param[in] expr 論理式
  BlifCover(int id,
	    int ni,
	    const vector<vector<Literal>>& cube_list,
	    SopPat opat,
	    const Expr& expr);

public:

  /// @brief デストラクタ
  ~BlifCover() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  int
  id() const;

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

  // ID番号
  int mId;

  // 入力カバー
  SopCover mInputCover;

  // 出力パタン
  SopPat mOutputPat;

  // 論理式
  Expr mExpr;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] ni 入力数
// @param[in] cube_list キューブを表すリテラルのリストのリスト
// @param[in] opat 出力のパタン
// @param[in] expr 論理式
inline
BlifCover::BlifCover(int id,
		     int ni,
		     const vector<vector<Literal>>& cube_list,
		     SopPat opat,
		     const Expr& expr) :
  mId(id),
  mInputCover(ni, cube_list),
  mOutputPat(opat),
  mExpr(expr)
{
}

// @brief ID番号を返す．
inline
int
BlifCover::id() const
{
  return mId;
}

// @brief 入力数を返す．
inline
int
BlifCover::input_num() const
{
  return mInputCover.variable_num();
}

// @brief キューブ数を返す．
inline
int
BlifCover::cube_num() const
{
  return mInputCover.cube_num();
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
  return mInputCover.get_pat(cpos, VarId(ipos));
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

// @brief 対応する論理式を返す．
inline
Expr
BlifCover::expr() const
{
  return mExpr;
}

END_NAMESPACE_YM_BNET

#endif // YM_BLIFCOVER_H
