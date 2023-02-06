#ifndef WRITERBASE_H
#define WRITERBASE_H

/// @file WriterBase.h
/// @brief WriterBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/BnNode.h"


BEGIN_NAMESPACE_YM

class NameMgr;

END_NAMESPACE_YM

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
  WriterBase(
    const BnNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief デストラクタ
  ~WriterBase() = default;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード名の解決を行う．
  void
  init_name_array(
    const string& prefix, ///< [in] 自動生成名の接頭語
    const string& suffix  ///< [in] 自動生成名の接尾語
  );

  /// @brief 対象のネットワークを返す．
  const BnNetwork&
  network() const { return mNetwork; }

  /// @brief ノード名を返す．
  string
  node_name(
    BnNode node ///< [in] ノード
  ) const
  {
    SizeType node_id = node.id();
    ASSERT_COND( 1 <= node_id && node_id <= mNameArray.size() );

    return mNameArray[node_id - 1];
  }

  /// @brief データ系のノードの時 true を返す．
  ///
  /// データ系ではないものはクロックとset/reset
  bool
  is_data(
    BnNode node ///< [in] ノード
  ) const
  {
    SizeType node_id = node.id();
    ASSERT_COND( 1 <= node_id && node_id <= mDataArray.size() );

    return mDataArray[node_id - 1];
  }


  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード名の登録を行う．
  ///
  /// name が空文字列の場合は登録しない．
  void
  reg_node_name(
    BnNode node,                      ///< [in] ノード
    const string& name,               ///< [in] 登録する名前
    unordered_set<string>& name_hash, ///< [in] ノード名のハッシュ
    NameMgr& name_mgr                 ///< [in] ノード名を管理するクラス
  );

  /// @brief ノード名をつける．
  void
  set_node_name(
    BnNode node,        ///< [in] ノード
    const string& name  ///< [in] 名前
  )
  {
    SizeType node_id = node.id();
    ASSERT_COND( 1 <= node_id && node_id <= mNameArray.size() );

    mNameArray[node_id - 1] = name;
  }

  /// @brief TFI のノードに印をつける．
  void
  mark_tfi(
    BnNode node ///< [in] ノード
  );


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

END_NAMESPACE_YM_BNET

#endif // WRITERBASE_H
