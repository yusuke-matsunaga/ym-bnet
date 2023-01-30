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
    SizeType src_id ///< [in] BlifMode のノード番号
  );

  /// @brief 出力を作る．
  void
  make_output(
    SizeType src_id ///< [in] BlifMode のノード番号
  );

  /// @brief ラッチを作る．
  void
  make_latch(
    SizeType src_id ///< [in] BlifMode のノード番号
  );

  /// @brief name_id のノードを生成する．
  void
  make_node(
    SizeType src_id ///< [in] BlifMode のノード番号
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  struct NodeInfo
  {
    string oname;     ///< [in] 出力名
    bool has_cell_id; ///< [in] セル番号を持つ時 true
    Expr expr;        ///< [in] 論理式
    SizeType cell_id; ///< [in] セル番号
    vector<SizeType> iname_id_array; ///< [in] 入力の名前IDのリスト
  };


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

  // 名前IDをキーにしてノード番号を格納するハッシュ表
  unordered_map<SizeType, SizeType> mIdMap;

  // 名前IDをキーにしてノード情報を格納するハッシュ表
  unordered_map<SizeType, NodeInfo> mNodeMap;

  // 出力ノードのノードIDをキーにしてファンインの名前IDを格納するハッシュ表
  unordered_map<SizeType, SizeType> mOutputMap;

  // クロック端子のノード番号
  SizeType mClockId;

  // リセット端子のノード番号
  SizeType mResetId;

};

END_NAMESPACE_YM_BNET

#endif // BNBLIFHANDLER_H
