#ifndef BNNODEMAP_H
#define BNNODEMAP_H

/// @file BnNodeMap.h
/// @brief BnNodeMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnNodeMap BnNodeMap.h "BnNodeMap.h"
/// @brief 番号をキーにしたノードの辞書
//////////////////////////////////////////////////////////////////////
class BnNodeMap
{
public:

  /// @brief コンストラクタ
  BnNodeMap(
    const BnNetworkImpl* network = nullptr
  ) : mNetwork{network}
  {
  }

  /// @brief デストラクタ
  ~BnNodeMap() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  void
  clear()
  {
    mIdMap.clear();
  }

  /// @brief 要素が登録されているか調べる．
  bool
  is_in(
    SizeType key ///< [in] キー
  ) const
  {
    return mIdMap.count(key) > 0;
  }

  /// @brief ノードを取り出す．
  BnNode
  get(
    SizeType key ///< [in] キー
  ) const
  {
    SizeType id = mIdMap.at(key);
    return BnNode{mNetwork, id};
  }

  /// @brief ノードを登録する．
  void
  put(
    SizeType key, ///< [in] キー
    BnNode node   ///< [in] ノード
  )
  {
    mIdMap.emplace(key, node.id());
  }

  /// @brief ノード番号の辞書を取り出す．
  unordered_map<SizeType, SizeType>&
  _id_map()
  {
    return mIdMap;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  const BnNetworkImpl* mNetwork;

  // ノード番号の辞書
  unordered_map<SizeType, SizeType> mIdMap;

};

END_NAMESPACE_YM_BNET

#endif // BNNODEMAP_H
