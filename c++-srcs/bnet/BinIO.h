#ifndef BINIO_H
#define BINIO_H

/// @file BinIO.h
/// @brief BinIO のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/BinDec.h"
#include "ym/Bdd.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"


BEGIN_NAMESPACE_YM_BNET

class BnNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class BinIO BinIO.h "BinIO.h"
/// @brief BnNetwork のバイナリダンプ/リストアを行うクラス
//////////////////////////////////////////////////////////////////////
class BinIO
{
public:

  /// @brief コンストラクタ
  BinIO() = default;

  /// @brief デストラクタ
  ~BinIO() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief BnNetwork の内容をダンプする．
  void
  dump(
    BinEnc& s,               ///< [in] 出力ストリーム
    const BnNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief BnNetwork の内容を復元する．
  void
  restore(
    BinDec& s,         ///< [in] 入力ストリーム
    BnNetworkImpl* network_impl ///< [in] 対象のネットワーク
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief DFFの内容をダンプする

  void
  dump_dff(
    BinEnc& s,       ///< [in] 出力ストリーム
    const BnDff& dff ///< [in] 対象のDff
  );

  /// @brief DFFを復元する．
  void
  restore_dff(
    BinDec& s,         ///< [in] 入力ストリーム
    BnNetworkImpl* network_impl ///< [in] 対象のネットワーク
  );

  /// @brief 論理ノードの内容をダンプする．
  void
  dump_logic(
    BinEnc& s,         ///< [in] 出力ストリーム
    const BnNode& node ///< [in] 対象のノード
  );

  /// @brief 論理ノードを復元する．
  void
  restore_logic(
    BinDec& s,         ///< [in] 入力ストリーム
    BnNetworkImpl* network_impl ///< [in] 対象のネットワーク
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 論理式のリスト
  vector<Expr> mExprList;

  // 関数のリスト
  vector<TvFunc> mFuncList;

  // BDDのリスト
  vector<Bdd> mBddList;

  // BDDの対応表
  unordered_map<Bdd, SizeType> mBddMap;

  // ノード番号の対応表
  unordered_map<SizeType, SizeType> mNodeMap;

};

END_NAMESPACE_YM_BNET

#endif // BINIO_H
