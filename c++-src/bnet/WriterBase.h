#ifndef WRITERBASE_H
#define WRITERBASE_H

/// @file WriterBase.h
/// @brief WriterBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class WriterBase WriterBase.h "WriterBase.h"
/// @brief BnNetwork の内容を出力するクラスの基底クラス
///
/// このクラスの役割は名前を持たないノードに自動生成の名前を
/// 割り当てること
/// そのために init_name_array(prefix, suffix) を呼ぶ必要がある．
/// ここで，prefix, suffix は自動生成名の接頭語，接尾語．
//////////////////////////////////////////////////////////////////////
class WriterBase
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  WriterBase(const BnNetwork& network);

  /// @brief デストラクタ
  ~WriterBase();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード名の解決を行う．
  /// @param[in] prefix 自動生成名の接頭語
  /// @param[in] suffix 自動生成名の接尾語
  void
  init_name_array(const string& prefix,
		  const string& suffix);

  /// @brief 対象のネットワークを返す．
  const BnNetwork&
  network() const;

  /// @brief ノード名を返す．
  /// @param[in] node_id ノード番号
  string
  node_name(int node_id) const;

  /// @brief データ系のノードの時 true を返す．
  /// @param[in] node_id ノード番号
  ///
  /// データ系ではないものはクロックとset/reset
  bool
  is_data(int node_id) const;


  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief TFI のノードに印をつける．
  /// @param[in] node_id ノード番号
  void
  mark_tfi(int node_id);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const BnNetwork& mNetwork;

  // ノード名を入れた配列
  vector<string> mNameArray;

  // ノード番号をキーにしたデータ属性の配列
  vector<bool> mDataArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 対象のネットワークを返す．
inline
const BnNetwork&
WriterBase::network() const
{
  return mNetwork;
}

// @brief ノード名を返す．
// @param[in] node_id ノード番号
inline
string
WriterBase::node_name(int node_id) const
{
  ASSERT_COND( node_id >= 0 && node_id < mNameArray.size() );

  return mNameArray[node_id];
}

// @brief データ系のノードの時 true を返す．
// @param[in] node_id ノード番号
inline
bool
WriterBase::is_data(int node_id) const
{
  ASSERT_COND( node_id >= 0 && node_id < mDataArray.size() );

  return mDataArray[node_id];
}

END_NAMESPACE_YM_BNET

#endif // WRITERBASE_H