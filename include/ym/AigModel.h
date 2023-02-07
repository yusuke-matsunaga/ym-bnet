#ifndef AIGMODEL_H
#define AIGMODEL_H

/// @file AigModel.h
/// @brief AigModel のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/aig_nsdef.h"


BEGIN_NAMESPACE_YM_AIG

class ModelImpl;

//////////////////////////////////////////////////////////////////////
/// @class AigModel AigModel.h "ym/AigModel.h"
/// @brief AIG 形式のファイルを読み込んだ結果を表すクラス
//////////////////////////////////////////////////////////////////////
class AigModel
{
public:

  /// @brief コンストラクタ
  AigModel();

  /// @brief デストラクタ
  ~AigModel();


public:
  //////////////////////////////////////////////////////////////////////
  /// @name ファイル入出力
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief Ascii AIG フォーマットを読み込む．
  /// @return 読み込みが成功したら true を返す．
  bool
  read_aag(
    const string& filename ///< [in] ファイル名
  );

  /// @brief Ascii AIG フォーマットを読み込む．
  /// @return 読み込みが成功したら true を返す．
  bool
  read_aag(
    istream& s ///< [in] 入力ストリーム
  );

  /// @brief AIG フォーマットを読み込む．
  /// @return 読み込みが成功したら true を返す．
  bool
  read_aig(
    const string& filename ///< [in] ファイル名
  );

  /// @brief AIG フォーマットを読み込む．
  /// @return 読み込みが成功したら true を返す．
  bool
  read_aig(
    istream& s ///< [in] 入力ストリーム
  );

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数番号の最大値を返す．
  SizeType
  M() const;

  /// @brief 入力数を得る．
  SizeType
  I() const;

  /// @brief ラッチ数を得る．
  SizeType
  L() const;

  /// @brief 出力数を得る．
  SizeType
  O() const;

  /// @brief ANDノード数を返す．
  SizeType
  A() const;

  /// @brief 入力ノードのリテラルを得る．
  SizeType
  input(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < I() )
  ) const;

  /// @brief ラッチノードのリテラルを得る．
  SizeType
  latch(
    SizeType pos ///< [in] ラッチ番号 ( 0 <= pos < L() )
  ) const;

  /// @brief ラッチのソースリテラルを得る．
  SizeType
  latch_src(
    SizeType pos ///< [in] ラッチ番号 ( 0 <= pos < L() )
  ) const;

  /// @brief 出力のソースリテラルを得る．
  SizeType
  output_src(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < O() )
  ) const;

  /// @brief ANDノードのリテラルを得る．
  SizeType
  and_node(
    SizeType pos ///< [in] ANDノード番号 ( 0 <= pos < A() )
  ) const;

  /// @brief ANDノードのソース1のリテラルを得る．
  SizeType
  and_src1(
    SizeType pos ///< [in] ANDノード番号 ( 0 <= pos < A() )
  ) const;

  /// @brief ANDノードのソース2のリテラルを得る．
  SizeType
  and_src2(
    SizeType pos ///< [in] ANDノード番号 ( 0 <= pos < A() )
  ) const;

  /// @brief 入力のシンボルを得る．
  const string&
  input_symbol(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < I() )
  ) const;

  /// @brief ラッチのシンボルを得る．
  const string&
  latch_symbol(
    SizeType pos ///< [in] ラッチ番号 ( 0 <= pos < L() )
  ) const;

  /// @brief 出力のシンボルを得る．
  const string&
  output_symbol(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < O() )
  ) const;

  /// @brief コメントを得る．
  string
  comment() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装クラス
  ModelImpl* mImpl;

};

END_NAMESPACE_YM_AIG

#endif // AIGMODEL_H
