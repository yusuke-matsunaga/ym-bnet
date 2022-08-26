#ifndef BNPORTLIST_H
#define BNPORTLIST_H

/// @file BnPortList.h
/// @brief BnPortList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnPortListIter BnPortList.h "BnPortList.h"
/// @brief BnPortList の反復子
//////////////////////////////////////////////////////////////////////
class BnPortListIter
{
public:

  /// @brief コンストラクタ
  BnPortListIter(
    const BnNetwork& network, ///< [in] 対象のネットワーク
    SizeType pos              ///< [in] 位置番号
  ) : mNetwork{network},
      mPos{pos}
  {
  }

  /// @brief デストラクタ
  ~BnPortListIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を取り出す．
  const BnPort&
  operator*() const;

  /// @brief 一つ進める．
  BnPortListIter&
  operator++()
  {
    ++ mPos;
    return *this;
  }

  /// @brief 等価比較演算
  bool
  operator==(
    const BnPortListIter& right
  ) const
  {
    return &mNetwork == &right.mNetwork && mPos == right.mPos;
  }

  /// @brief 非等価比較演算
  bool
  operator!=(
    const BnPortListIter& right
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
/// @class BnPortList BnPortList.h "BnPortList.h"
/// @brief BnPort のリストを表すクラス
//////////////////////////////////////////////////////////////////////
class BnPortList
{
public:

  using iterator = BnPortListIter;

public:

  /// @brief コンストラクタ
  BnPortList(
    const BnNetwork& network ///< [in] 対象のネットワーク
  ) : mNetwork{network}
  {
  }

  /// @brief デストラクタ
  ~BnPortList() = default;


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

#endif // BNPORTLIST_H
