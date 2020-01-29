#ifndef BLIFCOVERMGR_H
#define BLIFCOVERMGR_H

/// @file BlifCoverMgr.h
/// @brief BlifCoverMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

class BlifCover;

//////////////////////////////////////////////////////////////////////
/// @class BlifCoverMgr BlifCoverMgr.h "BlifCoverMgr.h"
/// @brief BlifCover を管理するクラス
//////////////////////////////////////////////////////////////////////
class BlifCoverMgr
{
public:

  /// @brief コンストラクタ
  BlifCoverMgr() = default;

  /// @brief デストラクタ
  ~BlifCoverMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 登録されているカバー数を返す．
  int
  cover_num() const;

  /// @brief パタン文字列からカバー番号を返す．
  /// @param[in] input_num 入力数
  /// @param[in] cube_num キューブ数
  /// @param[in] ipat_str 入力パタン文字列
  /// @param[in] opat 出力パタン
  int
  pat2cover(int input_num,
	    int cube_num,
	    const string& ipat_str,
	    char opat_char);

  /// @brief ID番号から BlifCover を返す．
  /// @param[in] id ID番号
  const BlifCover&
  cover(int id) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief BlifCover を作る．
  /// @param[in] input_num 入力数
  /// @param[in] cube_num キューブ数
  /// @param[in] ipat_str 入力パタン文字列
  /// @param[in] opat 出力パタン
  /// @return 作成された BlifCover の ID 番号を返す．
  int
  new_cover(int input_num,
	    int cube_num,
	    const string& ipat_str,
	    char opat_char);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // カバーを表す文字列をキーにしてID番号を納める辞書
  unordered_map<string, int> mCoverDict;

  // ID 番号をキーにしてカバーを格納する配列
  vector<BlifCover> mCoverArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_BNET

#endif // BLIFCOVERMGR_H
