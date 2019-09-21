#ifndef WRITERBASE_H
#define WRITERBASE_H

/// @file WriterBase.h
/// @brief WriterBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/NameMgr.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class WriterBase WriterBase.h "WriterBase.h"
/// @brief BnNetwork の内容を出力するクラスの基底クラス
///
/// このクラスの役割は名前を持たないノードに自動生成の名前を
/// 割り当てること．blif および iscas89(.bench) 形式のファイル出力
/// を対象に考えている．
/// そのために init_name_array(prefix, suffix) を呼ぶ必要がある．
/// ここで，prefix, suffix は自動生成名の接頭語，接尾語．
///
/// 名前が重複している場合も自動生成名に付け替える．
/// この場合，もとの名前を優先順は以下の通り
/// - 外部ポート名
/// - DFF，ラッチの出力名(DFF，ラッチ名)
/// - DFF，ラッチの入力名
/// - 論理ノード名
/// 同じ優先順位の場合は前にあるほうが優先される．
///
/// ここで外部入出力ポート名を外部入出力ノード名と区別している理由は
/// 外部出力ノード名が変わってもインターフェイスとしての外部出力ポート名
/// を変更させないため．
///
/// 厄介なのは多ビットポートの扱い．基本的には <ポート名> '[' <ビット番号> ']'
/// だが，重複した場合は(もともと sss[ddd] という形式のポートが別にあった場合)
/// 自動生成名に置き換えられる．
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

  /// @brief ノード名の登録を行う．
  /// @param[in] node_id ノード番号
  /// @param[in] name 登録する名前
  /// @param[in] name_hash ノード名のハッシュ
  /// @param[in] name_mgr ノード名を管理するクラス
  ///
  /// name が空文字列の場合は登録しない．
  void
  reg_node_name(int node_id,
		const string& name,
		unordered_set<string>& name_hash,
		NameMgr& name_mgr);

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
