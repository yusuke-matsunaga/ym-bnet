#ifndef BLIFLATCH_H
#define BLIFLATCH_H

/// @file BlifLatch.h
/// @brief BlifLatch のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BlifNode.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
/// @class BlifLatch BlifLatch.h "BlifLatch.h"
/// @brief .latch 文を表す BlifAst
//////////////////////////////////////////////////////////////////////
class BlifLatch :
  public BlifNode
{
public:

  /// @brief コンストラクタ
  BlifLatch(
    SizeType id,         ///< [in] ID番号
    const string& name,  ///< [in] 出力名
    SizeType inode,      ///< [in] 入力ノード
    char rval            ///< [in] リセット値('0', '1', or 'X'
  );

  /// @brief デストラクタ
  ~BlifLatch();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  Type
  type() const override;

  /// @brief 入力ノードの番号を返す．
  SizeType
  inode() const override;

  /// @brief リセット値を返す．
  char
  rval() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力ノード
  SizeType mInode;

  // リセット値
  char mRval;

};

END_NAMESPACE_YM_BLIF

#endif // BLIFLATCH_H
