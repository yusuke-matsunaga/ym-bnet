#ifndef BLIFCOVERMGR_H
#define BLIFCOVERMGR_H

/// @file BlifCoverMgr.h
/// @brief BlifCoverMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/SimpleAlloc.h"


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
  BlifCoverMgr();

  /// @brief デストラクタ
  ~BlifCoverMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 登録されているカバー数を返す．
  ymuint
  cover_num() const;

  /// @brief パタン文字列からカバーを返す．
  /// @param[in] input_num 入力数
  /// @param[in] cube_num キューブ数
  /// @param[in] ipat_str 入力パタン文字列
  /// @param[in] opat 出力パタン
  const BlifCover*
  pat2cover(ymuint input_num,
	    ymuint cube_num,
	    const string& ipat_str,
	    char opat_char);

  /// @brief ID番号から BlifCover を返す．
  /// @param[in] id ID番号
  const BlifCover*
  cover(ymuint id) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief BlifCover を作る．
  /// @param[in] input_num 入力数
  /// @param[in] cube_num キューブ数
  /// @param[in] ipat_str 入力パタン文字列
  /// @param[in] opat 出力パタン
  BlifCover*
  new_cover(ymuint input_num,
	    ymuint cube_num,
	    const string& ipat_str,
	    char opat_char);

  /// @brief カバーを登録する．
  void
  reg_cover(BlifCover* cover);

  /// @brief ハッシュ表のメモリを確保する．
  /// @param[in] req_size 要求サイズ
  void
  alloc_table(ymuint req_size);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // BlifCover 用のメモリアロケータ
  SimpleAlloc mAlloc;

  // 登録されているカバー数
  ymuint mCoverNum;

  // ハッシュテーブルのサイズ
  ymuint mHashSize;

  // ハッシュテーブルを拡大する目安
  ymuint mNextLimit;

  // ハッシュテーブル
  BlifCover** mHashTable;

  // ID 番号をキーにしてカバーを格納する配列
  vector<BlifCover*> mCoverArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 登録されているカバー数を返す．
inline
ymuint
BlifCoverMgr::cover_num() const
{
  return mCoverNum;
}

// @brief ID番号から BlifCover を返す．
// @param[in] id ID番号
inline
const BlifCover*
BlifCoverMgr::cover(ymuint id) const
{
  ASSERT_COND( id < cover_num() );
  return mCoverArray[id];
}

END_NAMESPACE_YM_BNET

#endif // BLIFCOVERMGR_H
