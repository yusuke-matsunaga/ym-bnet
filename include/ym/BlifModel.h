#ifndef BLIFMODEL_H
#define BLIFMODEL_H

/// @file BlifModel.h
/// @brief BlifModel のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/blif_nsdef.h"
#include "ym/clib.h"
#include "ym/sop_nsdef.h"


BEGIN_NAMESPACE_YM_BLIF

class ModelImpl;

//////////////////////////////////////////////////////////////////////
/// @class BlifModel BlifModel.h "BlifModel.h"
/// @brief blif ファイルの読み込み結果を表すクラス
///
/// 以下の情報を持つ．
/// - 名前(.model)
/// - 入力ノード番号のリスト
/// - 出力ノード番号のリスト
/// - DFFノード番号のリスト
/// - 論理ノード番号のリスト
/// - 各ノードごとに以下の情報を持つ．
///   * 名前
///   * 種類(Input, Dff, Cover, Cell)
///   * ファンインのノード番号のリスト
/// - カバー(BlifCover)のリスト
///
/// - Dffタイプは以下の情報を持つ．
///   * リセット値 ('0' or '1' or 'X')
/// - Coverタイプは以下の情報を持つ．
///   * カバー番号
/// - Cellタイプは以下の情報を持つ．
///   * セル番号
///
/// 実際には出力ノードという種類はなく，他のいずれかの
/// ノードとなっている．
/// 論理ノードのリストは入力からのトポロジカル順
/// となっている．
//////////////////////////////////////////////////////////////////////
class BlifModel
{
  friend class BlifParser;

public:

  /// @brief コンストラクタ
  BlifModel();

  /// @brief デストラクタ
  ~BlifModel();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 読み込みを行う．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read_blif(
    const string& filename ///< [in] ファイル名
  );

  /// @brief 読み込みを行う(セルライブラリ付き)．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read_blif(
    const string& filename,             ///< [in] ファイル名
    const ClibCellLibrary& cell_library ///< [in] セルライブラリ
  );

  /// @brief 名前を返す．
  const string&
  name() const;

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
  logic_list() const;

  /// @brief ノード名を返す．
  const string&
  node_name(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノードの種類を返す．
  BlifType
  node_type(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノードのファンインのノード番号のリストを返す．
  ///
  /// node_type が Cover か Cell の時のみ意味を持つ．
  const vector<SizeType>&
  node_fanin_list(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノードのカバー番号を返す．
  ///
  /// node_type が Cover の時のみ意味を持つ．
  SizeType
  node_cover_id(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノードのセル番号を返す．
  ///
  /// node_type が Cell の時のみ意味を持つ．
  SizeType
  node_cell_id(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノードの入力ノード番号を返す．
  ///
  /// node_type が DFF の時のみ意味を持つ．
  SizeType
  node_input(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノードのリセット値を返す．
  ///
  /// node_type が DFF の時のみ意味を持つ．
  char
  node_rval(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief カバーの種類の数を返す．
  SizeType
  cover_num() const;

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

  // 内部情報
  ModelImpl* mImpl;

};

END_NAMESPACE_YM_BLIF

#endif // BLIFMODEL_H
