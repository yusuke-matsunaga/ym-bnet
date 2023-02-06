#ifndef SIMPLEDECOMP_H
#define SIMPLEDECOMP_H

/// @file SimpleDecomp.h
/// @brief SimpleDecomp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnModifier.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class SimpleDecomp SimpleDecomp.h "SimpleDecomp.h"
/// @brief simple decomposition を行うクラス
//////////////////////////////////////////////////////////////////////
class SimpleDecomp :
  public BnModifier
{
public:

  /// @brief コンストラクタ
  SimpleDecomp() = default;

  /// @brief デストラクタ
  ~SimpleDecomp() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 各ノードが simple primitive になるように分解する．
  void
  decomp(
    const BnNetwork& src_network ///< [in] 元のネットワーク
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードを分解する．
  void
  decomp_node(
    const BnNode& node,                       ///< [in] 対象のノード
    unordered_map<SizeType, BnNode>& node_map ///< [in] ID番号の対応表
  );

  /// @brief 論理式を分解する．
  /// @return 生成したノード番号を返す．
  BnNode
  decomp_expr(
    const Expr& expr ///< [in] 論理式
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力ノード
  vector<BnNode> mTermList;

};

END_NAMESPACE_YM_BNET

#endif // SIMPLEDECOMP_H
