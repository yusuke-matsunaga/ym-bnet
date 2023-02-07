#ifndef BENCH2BNET_H
#define BENCH2BNET_H

/// @file Bench2Bnet.h
/// @brief Bench2Bnet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Model.h"
#include "ym/BnModifier.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class Bench2Bnet Bench2Bnet.h "Bench2Bnet.h"
/// @brief Iscas89Model から BnNetwork を生成するクラス
//////////////////////////////////////////////////////////////////////
class Bench2Bnet
{
public:

  /// @brief コンストラクタ
  Bench2Bnet(
    const Iscas89Model& model, ///< [in] パース結果
    const string& clock_name   ///< [in] クロック名
  );

  /// @brief デストラクタ
  ~Bench2Bnet() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 結果のネットワークを返す．
  BnNetwork
  get_network();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力ノードを作る．
  void
  make_input(
    SizeType src_id ///< [in] 識別子番号
  );

  /// @brief 出力の設定を行う．
  void
  set_output(
    SizeType src_id ///< [in] 識別子番号
  );

  /// @brief DFF を作る．
  void
  make_dff(
    SizeType src_id ///< [in] 識別子番号
  );

  /// @brief ゲートを作る．
  void
  make_gate(
    SizeType src_id ///< [in] 識別子番号
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パーズ結果
  const Iscas89Model& mModel;

  // ネットワーク
  BnModifier mNetwork;

  // クロック端子名
  string mClockName;

  // 名前IDをキーにしてノード番号を格納するハッシュ表
  unordered_map<SizeType, BnNode> mNodeMap;

  // 出力のノードIDをキーにしてファンインの名前IDを格納するハッシュ表
  unordered_map<SizeType, SizeType> mOutputMap;

  // クロック端子のノード
  BnNode mClock;

};

END_NAMESPACE_YM_BNET

#endif // BENCH2BNET_H
