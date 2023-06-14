#ifndef NVLMODEL_H
#define NVLMODEL_H

/// @file NvlModel.h
/// @brief NvlModel のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/nvl_nsdef.h"


BEGIN_NAMESPACE_YM_NVL

class ModelImpl;

//////////////////////////////////////////////////////////////////////
/// @class NvlModel NvlModel.h "NvlModel.h"
/// @brief netlist 形式の verilog ファイルの読み込み結果を表すクラス
///
/// 以下の情報を持つ．
/// - 入力ノード番号のリスト
/// - 出力ノード番号のリスト
/// - 論理ノード番号のリスト
/// - 各ノードは以下の情報を持つ．
///   * 名前
///   * 種類(Input, Gate)
///   * ファンインのノード番号のリスト
///
/// - Gate タイプは以下の情報を持つ．
///   * ゲートの種類(PrimType)
///
/// 全ての Gate タイプは PrimType で表される．
///
/// 実際には出力ノードという種類はなく，他のいずれかの
/// ノードとなっている．
/// 論理ノードのリストは入力からのトポロジカル順
/// となっている．
//////////////////////////////////////////////////////////////////////
class NvlModel
{
  friend class NvlParser;

public:

  /// @brief コンストラクタ
  NvlModel();

  /// @brief デストラクタ
  ~NvlModel();


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

  /// @brief 論理ノード番号のリストを返す．
  const vector<SizeType>&
  gate_list() const;

  /// @brief ノード名を返す．
  const string&
  node_name(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノードの種類を返す．
  NvlType
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
  /// node_type() が Gate/Complex の時のみ意味を持つ．
  const vector<SizeType>&
  node_fanin_list(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief 論理ノードの種類を返す．
  ///
  /// node_type() が Gate の時のみ意味を持つ．
  PrimType
  node_gate_type(
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

END_NAMESPACE_YM_NVL

#endif // NVLMODEL_H
