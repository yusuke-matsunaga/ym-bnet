#ifndef YM_BNNODEENC_H
#define YM_BNNODEENC_H

/// @file ym/BnNodeEnc.h
/// @brief BnNodeEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/Expr.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class BnNodeEnc BnNodeEnc.h "ym/BnNodeEnc.h"
/// @brief BnNode の入出力の関係を表す CNF 式を作るクラス
//////////////////////////////////////////////////////////////////////
class BnNodeEnc
{
public:

  /// @brief コンストラクタ
  /// @param[in] solver SATソルバ
  /// @param[in] network 対象のネットワーク
  /// @param[in] varmap 変数番号のマップ
  BnNodeEnc(SatSolver& solver,
	    const BnNetwork& network,
	    const vector<SatVarId>& varmap);

  /// @brief デストラクタ
  ~BnNodeEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの入出力の関係を表すCNF式を作る．
  /// @param[in] node 対象のノード
  void
  make_cnf(const BnNode& node);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 定数０のCNFを作る．
  void
  make_zero(SatLiteral olit);

  /// @brief 定数1のCNFを作る．
  void
  make_one(SatLiteral olit);

  /// @brief BUFFのCNFを作る．
  void
  make_buff(SatLiteral olit,
	    SatLiteral ilit);

  /// @brief NOTのCNFを作る．
  void
  make_not(SatLiteral olit,
	   SatLiteral ilit);

  /// @brief AND論理のCNFを作る．
  void
  make_and(SatLiteral olit,
	   const vector<SatLiteral>& ilit_array);

  /// @brief OR論理のCNFを作る．
  void
  make_or(SatLiteral olit,
	  const vector<SatLiteral>& ilit_array);

  /// @brief XOR論理のCNFを作る．
  void
  make_xor(SatLiteral olit,
	   const vector<SatLiteral>& ilit_array);

  /// @brief MUX論理のCNFを作る．
  void
  make_mux(SatLiteral olit,
	   const vector<SatLiteral>& ilit_array);

  /// @brief 論理式のCNFを作る．
  SatLiteral
  make_expr(const Expr& expr,
	    const vector<SatLiteral>& ilit_array);

  /// @brief ノードに対応するリテラルを返す．
  /// @param[in] node_id ノード番号
  SatLiteral
  lit(int node_id);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver& mSolver;

  // 対象のネットワーク
  const BnNetwork& mNetwork;

  // 変数番号のマップ
  const vector<SatVarId>& mVarMap;

};

END_NAMESPACE_YM

#endif // YM_BNNODEENC_H
