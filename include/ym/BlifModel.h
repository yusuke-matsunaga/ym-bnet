#ifndef BLIFMODEL_H
#define BLIFMODEL_H

/// @file BlifModel.h
/// @brief BlifModel のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/blif_nsdef.h"


BEGIN_NAMESPACE_YM_BLIF

//class ParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class BlifModel BlifModel.h "BlifModel.h"
/// @brief .model 文を表す BlifAst
//////////////////////////////////////////////////////////////////////
class BlifModel
{
  friend class ParserImpl;
  friend class CoverMgr;

public:

  /// @brief コンストラクタ
  BlifModel();

  /// @brief デストラクタ
  ~BlifModel();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を返す．
  const string&
  name() const
  {
    return mName;
  }

  /// @brief 入力数を返す．
  SizeType
  input_num() const
  {
    return mInputNum;
  }

  /// @brief 出力のノード番号のリストを返す．
  const vector<SizeType>&
  output_list() const
  {
    return mOutputList;
  }

  /// @brief ラッチ数を返す．
  SizeType
  latch_num() const
  {
    return mLatchNum;
  }

  /// @brief 全ノード数を返す．
  SizeType
  node_num() const
  {
    return mNodeList.size();
  }

  /// @brief ノードを取り出す．
  const BlifNode*
  node(
    SizeType node_id ///< [in] ノード番号
  ) const
  {
    return mNodeList[node_id];
  }

  /// @brief カバーを取り出す．
  const BlifCover&
  cover(
    SizeType cover_id ///< [in] カバー番号
  ) const;

  /// @brief 内容を出力する．
  void
  print(
    ostream& s ///< [in] 出力先のストリーム
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 名前のノード
  string mName;

  // 入力数
  SizeType mInputNum;

  // 出力のノード番号のリスト
  vector<SizeType> mOutputList;

  // ラッチ数
  SizeType mLatchNum;

  // ノードのリスト
  vector<const BlifNode*> mNodeList;

  // ID 番号をキーにしてカバーを格納する配列
  vector<BlifCover> mCoverArray;

};

END_NAMESPACE_YM_BLIF

#endif // BLIFMODEL_H
