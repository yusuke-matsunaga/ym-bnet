#ifndef COVERMGR_H
#define COVERMGR_H

/// @file CoverMgr.h
/// @brief CoverMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/blif_nsdef.h"
#include "ym/BlifCover.h"
#include "ModelImpl.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
/// @class CoverMgr CoverMgr.h "CoverMgr.h"
/// @brief BlifCover を管理するクラス
//////////////////////////////////////////////////////////////////////
class CoverMgr
{
public:

  /// @brief コンストラクタ
  CoverMgr(
    ModelImpl* model
  );

  /// @brief デストラクタ
  ~CoverMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 登録されているカバー数を返す．
  SizeType
  cover_num() const;

  /// @brief パタン文字列からカバー番号を返す．
  SizeType
  pat2cover(
    SizeType input_num,     ///< [in] 入力数
    SizeType cube_num,      ///< [in] キューブ数
    const string& ipat_str, ///< [in] 入力パタン文字列
    char opat_char          ///< [in] 出力パタン
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief BlifCover を作る．
  /// @return 作成された BlifCover の ID 番号を返す．
  SizeType
  new_cover(
    SizeType input_num,     ///< [in] 入力数
    SizeType cube_num,      ///< [in] キューブ数
    const string& ipat_str, ///< [in] 入力パタン文字列
    char opat_char          ///< [in] 出力パタン
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // モデル
  ModelImpl* mModel;

  // カバーを表す文字列をキーにしてID番号を納める辞書
  unordered_map<string, SizeType> mCoverDict;

};

END_NAMESPACE_YM_BLIF

#endif // COVERMGR_H
