#ifndef OUTPUTSPLIT_H
#define OUTPUTSPLIT_H

/// @file OutputSplit.h
/// @brief OutputSplit のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnModifier.h"
#include "ym/BnNodeMap.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class OutputSplit OutputSplit.h "OutputSplit.h"
/// @brief 1つの出力に関係するノードのみからなるネットワークを作る．
//////////////////////////////////////////////////////////////////////
class OutputSplit :
  public BnModifier
{
public:

  /// @brief コンストラクタ
  OutputSplit() = default;

  /// @brief デストラクタ
  ~OutputSplit() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 処理を行う本体
  void
  split(
    const BnNetwork& network, ///< [in] 元のネットワーク
    SizeType output_pos       ///< [in] 出力番号
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号の対応表
  BnNodeMap mNodeMap;

};

END_NAMESPACE_YM_BNET

#endif // OUTPUTSPLIT_H
