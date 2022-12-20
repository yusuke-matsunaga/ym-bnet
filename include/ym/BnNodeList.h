#ifndef BNNODELIST_H
#define BNNODELIST_H

/// @file BnNodeList.h
/// @brief BnNodeList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnNodeListIter BnNodeList.h "BnNodeList.h"
/// @brief BnNodeList の反復子
//////////////////////////////////////////////////////////////////////
class BnNodeListIter
{
public:

  /// @brief コンストラクタ
  BnNodeListIter(
    const BnNetwork& network,             ///< [in] 対象のネットワーク
    vector<SizeType>::const_iterator iter ///< [in] ノード番号のリストの反復子
  ) : mNetwork{network},
      mIter{iter}
  {
  }

  /// @brief デストラクタ
  ~BnNodeListIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を取り出す．
  const BnNode&
  operator*() const;

  /// @brief 一つ進める．
  BnNodeListIter&
  operator++()
  {
    ++ mIter;
    return *this;
  }

  /// @brief 等価比較演算子
  bool
  operator==(
    const BnNodeListIter& right
  ) const
  {
    return &mNetwork == &right.mNetwork && mIter == right.mIter;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const BnNodeListIter& right
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

  // ノード番号のリストの反復子
  vector<SizeType>::const_iterator mIter;

};


//////////////////////////////////////////////////////////////////////
/// @class BnNodeList BnNodeList.h "BnNodeList.h"
/// @brief BnNode のリストを表すクラス
//////////////////////////////////////////////////////////////////////
class BnNodeList
{
public:

  using iterator = BnNodeListIter;

public:

  /// @brief コンストラクタ
  BnNodeList(
    const BnNetwork& network,       ///< [in] 対象のネットワーク
    const vector<SizeType>& id_list ///< [in] ノード番号のリスト
  ) : mNetwork{network},
      mIdList{id_list}
  {
  }

  /// @brief デストラクタ
  ~BnNodeList() = default;


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

  // ID番号のリスト
  const vector<SizeType>& mIdList;

};

//////////////////////////////////////////////////////////////////////
/// @class BnAllNodeListIter BnNodeList.h "BnNodeList.h"
/// @brief BnAllNodeList の反復子
//////////////////////////////////////////////////////////////////////
class BnAllNodeListIter
{
public:

  /// @brief コンストラクタ
  BnAllNodeListIter(
    const BnNetwork& network, ///< [in] 対象のネットワーク
    SizeType pos              ///< [in] 位置
  ) : mNetwork{network},
      mPos{pos}
  {
  }

  /// @brief デストラクタ
  ~BnAllNodeListIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を取り出す．
  const BnNode&
  operator*() const;

  /// @brief 一つ進める．
  BnAllNodeListIter&
  operator++();

  /// @brief 等価比較演算子
  bool
  operator==(
    const BnAllNodeListIter& right
  ) const
  {
    return &mNetwork == &right.mNetwork && mPos == right.mPos;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const BnAllNodeListIter& right
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
/// @class BnAllNodeList BnNodeList.h "BnNodeList.h"
/// @brief BnNode のリストを表すクラス
//////////////////////////////////////////////////////////////////////
class BnAllNodeList
{
public:

  using iterator = BnAllNodeListIter;

public:

  /// @brief コンストラクタ
  BnAllNodeList(
    const BnNetwork& network ///< [in] 対象のネットワーク
  ) : mNetwork{network}
  {
  }

  /// @brief デストラクタ
  ~BnAllNodeList() = default;


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

#endif // BNNODELIST_H
