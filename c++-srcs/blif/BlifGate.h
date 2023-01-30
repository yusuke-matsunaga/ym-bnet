#ifndef BLIFGATE_H
#define BLIFGATE_H

/// @file BlifGate.h
/// @brief BlifGate のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BlifNode.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
/// @class BlifGate BlifGate.h "BlifGate.h"
/// @brief .gate 文を表す BlifAst
//////////////////////////////////////////////////////////////////////
class BlifGate :
  public BlifNode
{
public:

  /// @brief コンストラクタ
  BlifGate(
    SizeType id,                        ///< [in] ID番号
    const string& name,                 ///< [in] 出力名
    const vector<SizeType>& inode_list, ///< [in] 入力ノード番号のリスト
    SizeType cell_id                    ///< [in] セル番号
  );

  /// @brief デストラクタ
  ~BlifGate();


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

  /// @brief セル番号を返す．
  SizeType
  cell_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力ノードのリスト
  vector<SizeType> mInodeList;

  // セル番号
  SizeType mCellId;

};

END_NAMESPACE_YM_BLIF

#endif // BLIFGATE_H
