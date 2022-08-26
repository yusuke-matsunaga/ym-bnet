#ifndef BNDFFLIST_H
#define BNDFFLIST_H

/// @file BnDffList.h
/// @brief BnDffList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnDffListIter BnDffList.h "BnDffList.h"
/// @brief BnDffList の反復子
//////////////////////////////////////////////////////////////////////
class BnDffListIter
{
public:

  /// @brief コンストラクタ
  BnDffListIter(
    const BnNetwork& network, ///< [in] 対象のネットワーク
    SizeType pos              ///< [in] 位置番号
  ) : mNetwork{network},
      mPos{pos}
  {
  }

  /// @brief デストラクタ
  ~BnDffListIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を取り出す．
  const BnDff&
  operator*() const;

  /// @brief 一つ進める．
  BnDffListIter&
  operator++()
  {
    ++ mPos;
    return *this;
  }

  /// @brief 等価比較演算
  bool
  operator==(
    const BnDffListIter& right
  ) const
  {
    return &mNetwork == &right.mNetwork && mPos == right.mPos;
  }

  /// @brief 非等価比較演算
  bool
  operator!=(
    const BnDffListIter& right
  ) const
  {
    return !operator==(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const BnNetwork& mNetwork;

  // 位置
  SizeType mPos;

};


//////////////////////////////////////////////////////////////////////
/// @class BnDffList BnDffList.h "BnDffList.h"
/// @brief BnDff のリストを表すクラス
//////////////////////////////////////////////////////////////////////
class BnDffList
{
public:

  using iterator = BnDffListIter;

public:

  /// @brief コンストラクタ
  BnDffList(
    const BnNetwork& network ///< [in] 対象のネットワーク
  ) : mNetwork{network}
  {
  }

  /// @brief デストラクタ
  ~BnDffList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 先頭の反復子を返す．
  iterator
  begin() const;

  /// @brief 末尾の反復子を返す．
  iterator
  end() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const BnNetwork& mNetwork;

};

END_NAMESPACE_YM_BNET

#endif // BNDFFLIST_H
