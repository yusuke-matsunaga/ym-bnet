#ifndef BLIFNAMES_H
#define BLIFNAMES_H

/// @file BlifNames.h
/// @brief BlifNames のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BlifNode.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
/// @class BlifNames BlifNames.h "BlifNames.h"
/// @brief .names 文を表す BlifAst
//////////////////////////////////////////////////////////////////////
class BlifNames :
  public BlifNode
{
public:

  /// @brief コンストラクタ
  BlifNames(
    SizeType id,                        ///< [in] ID番号
    const string& name,                 ///< [in] 名前
    const vector<SizeType>& inode_list, ///< [in] 入力ノードのリスト
    SizeType cover_id                   ///< [in] カバー番号
  );

  /// @brief デストラクタ
  ~BlifNames();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  Type
  type() const override;

  /// @brief 入力ノード番号のリストを返す．
  const vector<SizeType>&
  inode_list() const override;

  /// @brief カバー番号を返す．
  SizeType
  cover_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力ノードのリスト
  vector<SizeType> mInodeList;

  // カバー番号
  SizeType mCoverId;

};

END_NAMESPACE_YM_BLIF

#endif // BLIFNAMES_H
