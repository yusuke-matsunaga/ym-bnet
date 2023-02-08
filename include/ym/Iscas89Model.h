#ifndef ISCAS89MODEL_H
#define ISCAS89MODEL_H

/// @file Iscas89Model.h
/// @brief Iscas89Model のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/iscas89_nsdef.h"
#include "ym/logic.h"


BEGIN_NAMESPACE_YM_ISCAS89

class ModelImpl;

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Model Iscas89Model.h "Iscas89Model.h"
/// @brief iscas89(.bench) 形式の読み込み結果を表すクラス
///
/// 以下の情報を持つ．
/// - 入力ノード番号のリスト
/// - 出力ノード番号のリスト
/// - DFFノード番号のリスト
/// - 論理ノード番号のリスト
/// - 各ノードごとに以下の情報を持つ．
///   * 名前
///   * 種類(Input, Dff, Gate)
///   * ファンインのノード番号のリスト
///
/// - Gate タイプは以下の情報を持つ．
///   * ゲートの種類(PrimType)
///
/// 実際には出力ノードという種類はなく，他のいずれかの
/// ノードとなっている．
/// 論理ノードのリストは入力からのトポロジカル順
/// となっている．
//////////////////////////////////////////////////////////////////////
class Iscas89Model
{
  friend class Iscas89Parser;

public:

  /// @brief コンストラクタ
  Iscas89Model();

  /// @brief デストラクタ
  ~Iscas89Model();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力のノード番号のリストを返す．
  const vector<SizeType>&
  input_list() const;

  /// @brief 出力のノード番号のリストを返す．
  const vector<SizeType>&
  output_list() const;

  /// @brief DFFのノード番号のリストを返す．
  const vector<SizeType>&
  dff_list() const;

  /// @brief 論理ノード番号のリストを返す．
  const vector<SizeType>&
  gate_list() const;

  /// @brief ノード名を返す．
  const string&
  node_name(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノードの種類を返す．
  Iscas89Type
  node_type(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノードのファンイン数を返す．
  ///
  /// node_type が Gate の時のみ意味を持つ．
  SizeType
  node_fanin_num(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノードのファンインのノード番号のリストを返す．
  ///
  /// node_type が Gate の時のみ意味を持つ．
  const vector<SizeType>&
  node_fanin_list(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief 論理ノードの種類を返す．
  ///
  /// node_type が Gate の時のみ意味を持つ．
  PrimType
  node_gate_type(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノードの入力ノード番号を返す．
  ///
  /// node_type が DFF の時のみ意味を持つ．
  SizeType
  node_input(
    SizeType node_id ///< [in] ノード番号
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

  // 実装オブジェクト
  ModelImpl* mImpl;

};

END_NAMESPACE_YM_ISCAS89

#endif // ISCAS89MODEL_H
