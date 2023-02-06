#ifndef BLIF2BNET_H
#define BLIF2BNET_H

/// @file Blif2Bnet.h
/// @brief Blif2Bnet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BlifModel.h"
#include "ym/BnModifier.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class Blif2Bnet Blif2Bnet.h
/// @brief BlifModel から BnNetwork を生成するクラス
//////////////////////////////////////////////////////////////////////
class Blif2Bnet
{
public:

  /// @brief コンストラクタ
  Blif2Bnet(
    const BlifModel& model,             ///< [in] blif のパーズ結果
    const string& clock_name = "clock", ///< [in] クロック端子名
    const string& reset_name = "reset"  ///< [in] リセット端子名
  );

  /// @brief デストラクタ
  ~Blif2Bnet() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 結果のネットワークを返す．
  BnNetwork
  get_network();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力を作る．
  void
  make_input(
    SizeType src_id ///< [in] BlifModel のノード番号
  );

  /// @brief 出力を設定する．
  void
  set_output(
    SizeType src_id ///< [in] BlifModel のノード番号
  );

  /// @brief DFFを作る．
  void
  make_dff(
    SizeType src_id ///< [in] BlifModel のノード番号
  );

  /// @brief 論理ノードを生成する．
  void
  make_logic(
    SizeType src_id ///< [in] BlifModel のノード番号
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パーズ結果
  const BlifModel& mModel;

  // ネットワーク
  BnModifier mNetwork;

  // クロック端子名
  string mClockName;

  // リセット端子名
  string mResetName;

  // 名前IDをキーにしてノードを格納するハッシュ表
  unordered_map<SizeType, BnNode> mNodeMap;

  // 出力ノードのノードIDをキーにしてファンインの名前IDを格納するハッシュ表
  unordered_map<SizeType, SizeType> mOutputMap;

  // クロック端子のノード
  BnNode mClock;

  // リセット端子のノード
  BnNode mReset;

};

END_NAMESPACE_YM_BNET

#endif // BNBLIFHANDLER_H
