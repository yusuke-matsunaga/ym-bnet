#ifndef AIG2BNET_H
#define AIG2BNET_H

/// @file Aig2Bnet.h
/// @brief Aig2Bnet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnModifier.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class Aig2Bnet Aig2Bnet.h "Aig2Bnet.h"
/// @brief Aig から BnNetwork を作るクラス
//////////////////////////////////////////////////////////////////////
class Aig2Bnet :
  public BnModifier
{
public:

  /// @brief コンストラクタ
  Aig2Bnet() = default;

  /// @brief デストラクタ
  ~Aig2Bnet() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief AIG から変換する．
  void
  conv(
    const AigReader& aig,     ///< [in] AIG の情報
    const string& clock_name, ///< [in] クロック端子の名前
    const string& reset_name  ///< [in] リセット端子の名前
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // AIG のリテラルと BnNetwork のノードの対応表
  unordered_map<SizeType, BnNode> mLitMap;

};

END_NAMESPACE_YM_BNET

#endif // AIG2BNET_H
