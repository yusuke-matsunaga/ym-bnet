#ifndef QUEUE_H
#define QUEUE_H

/// @file Queue.h
/// @brief Queue のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/HashSet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class Queue Queue.h "Queue.h"
/// @brief BnNetwork の論理ノードをソートするためのキュー
//////////////////////////////////////////////////////////////////////
class Queue
{
public:

  /// @brief コンストラクタ
  /// @param[in] size キューの最大サイズ
  Queue(ymuint size);

  /// @brief デストラクタ
  ~Queue();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief キューに積む．
  /// @param[in] node ノード
  void
  put(BnNode* node);

  /// @brief キューから取り出す．
  /// @return 取り出されたノードを返す．
  ///
  /// 空の時には nullptr を返す．
  BnNode*
  get();

  /// @brief キューに含まれているか調べる．
  /// @param[in] node ノード
  bool
  check(BnNode* node) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // キューの本体
  vector<BnNode*> mNodeArray;

  // キューに積まれているノード番号を入れるハッシュ表
  HashSet<ymuint> mMark;

  // キューの書き込み位置
  ymuint mWpos;

  // キューの読み出し位置
  ymuint mRpos;

};

END_NAMESPACE_YM_BNET

#endif // QUEUE_H
