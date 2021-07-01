#ifndef YM_BLIFCOVER_H
#define YM_BLIFCOVER_H

/// @file ym/BlifCover.h
/// @brief BlifCover のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BlifCover BlifCover.h "ym/BlifCover.h"
/// @brief blif 形式の .names 本体のカバーを表すクラス
//////////////////////////////////////////////////////////////////////
class BlifCover
{
public:

  /// @brief コンストラクタ
  ///
  /// - ipat_list の要素は '0', '1', '-' のいずれか
  /// - opat は '0', '1' のいずれか
  BlifCover(int ni,                          ///< [in] 入力数
	    const vector<string>& ipat_list, ///< [in] 入力のパタンベクタ
	    char opat)                       ///< [in] 出力のパタン
    : mInputNum{ni},
      mInputPatList{ipat_list},
      mOutputPat{opat}
  {
  }

  /// @brief デストラクタ
  ~BlifCover() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力数を返す．
  int
  input_num() const { return mInputNum; }

  /// @brief キューブ数を返す．
  int
  cube_num() const { return mInputPatList.size(); }

  /// @brief 入力パタンを返す．
  /// @return パタンを返す．('0', '1', '-')
  char
  input_pat(int cpos,       ///< [in] キューブ番号 ( 0 <= cpos < cube_num() )
	    int ipos) const ///< [in] 入力番号 ( 0 <= ipos < input_num() )
  {
    ASSERT_COND( 0 <= cpos && cpos < cube_num() );
    ASSERT_COND( 0 <= ipos && ipos < input_num() );
    return mInputPatList[cpos][ipos];
  }

  /// @brief 出力パタンを返す．
  ///
  /// - すべてのキューブに対して同一のパタンとなる．
  /// - ドントケアはない．
  char
  output_pat() const { return mOutputPat; }

  /// @brief 内容を出力する．
  void
  print(ostream& s) const; ///< [in] 出力先のストリーム


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力数
  int mInputNum;

  // 入力パタンのリスト
  vector<string> mInputPatList;

  // 出力パタン
  char mOutputPat;

};

END_NAMESPACE_YM_BNET

#endif // YM_BLIFCOVER_H
