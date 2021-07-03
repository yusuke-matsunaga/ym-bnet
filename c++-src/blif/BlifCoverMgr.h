#ifndef BLIFCOVERMGR_H
#define BLIFCOVERMGR_H

/// @file BlifCoverMgr.h
/// @brief BlifCoverMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
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
  int
  pat2cover(
    int input_num,          ///< [in] 入力数
    int cube_num,           ///< [in] キューブ数
    const string& ipat_str, ///< [in] 入力パタン文字列
    char opat_char          ///< [in] 出力パタン
  );

  /// @brief ID番号から BlifCover を返す．
  const BlifCover&
  cover(
    int id ///< [in] ID番号
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief BlifCover を作る．
  /// @return 作成された BlifCover の ID 番号を返す．
  int
  new_cover(
    int input_num,          ///< [in] 入力数
    int cube_num,           ///< [in] キューブ数
    const string& ipat_str, ///< [in] 入力パタン文字列
    char opat_char          ///< [in] 出力パタン
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // カバーを表す文字列をキーにしてID番号を納める辞書
  unordered_map<string, int> mCoverDict;

  // ID 番号をキーにしてカバーを格納する配列
  vector<BlifCover> mCoverArray;

};

END_NAMESPACE_YM_BNET

#endif // BLIFCOVERMGR_H
