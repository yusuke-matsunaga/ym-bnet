#ifndef MODELIMPL_H
#define MODELIMPL_H

/// @file ModelImpl.h
/// @brief ModelImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/aig_nsdef.h"


BEGIN_NAMESPACE_YM_AIG

//////////////////////////////////////////////////////////////////////
/// @class ModelImpl ModelImpl.h "ModelImpl.h"
/// @brief AIG 形式のファイルを読むためのクラス
//////////////////////////////////////////////////////////////////////
class ModelImpl
{
public:

  /// @brief コンストラクタ
  ModelImpl() = default;

  /// @brief デストラクタ
  ~ModelImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  /// @name ファイル入出力
  /// @{
  //////////////////////////////////////////////////////////////////////

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
  M() const
  {
    return I() + L() + A();
  }

  /// @brief 入力数を得る．
  SizeType
  I() const
  {
    return mInputList.size();
  }

  /// @brief ラッチ数を得る．
  SizeType
  L() const
  {
    return mLatchList.size();
  }

  /// @brief 出力数を得る．
  SizeType
  O() const
  {
    return mOutputList.size();
  }

  /// @brief ANDノード数を返す．
  SizeType
  A() const
  {
    return mAndList.size();
  }

  /// @brief 入力ノードのリテラルを得る．
  SizeType
  input(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < I() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < I() );
    return mInputList[pos].mLiteral;
  }

  /// @brief ラッチノードのリテラルを得る．
  SizeType
  latch(
    SizeType pos ///< [in] ラッチ番号 ( 0 <= pos < L() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < L() );
    return mLatchList[pos].mLiteral;
  }

  /// @brief ラッチのソースリテラルを得る．
  SizeType
  latch_src(
    SizeType pos ///< [in] ラッチ番号 ( 0 <= pos < L() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < L() );
    return mLatchList[pos].mSrc;
  }

  /// @brief 出力のソースリテラルを得る．
  SizeType
  output_src(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < O() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < O() );
    return mOutputList[pos].mSrc;
  }

  /// @brief ANDノードのリテラルを得る．
  SizeType
  and_node(
    SizeType pos ///< [in] ANDノード番号 ( 0 <= pos < A() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < A() );
    return mAndList[pos].mLiteral;
  }

  /// @brief ANDノードのソース1のリテラルを得る．
  SizeType
  and_src1(
    SizeType pos ///< [in] ANDノード番号 ( 0 <= pos < A() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < A() );
    return mAndList[pos].mSrc1;
  }

  /// @brief ANDノードのソース2のリテラルを得る．
  SizeType
  and_src2(
    SizeType pos ///< [in] ANDノード番号 ( 0 <= pos < A() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < A() );
    return mAndList[pos].mSrc2;
  }

  /// @brief 入力のシンボルを得る．
  const string&
  input_symbol(
    SizeType pos ///< [in] 入力番号 ( 0 <= pos < I() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < I() );
    return mInputList[pos].mSymbol;
  }

  /// @brief ラッチのシンボルを得る．
  const string&
  latch_symbol(
    SizeType pos ///< [in] ラッチ番号 ( 0 <= pos < L() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < L() );
    return mLatchList[pos].mSymbol;
  }

  /// @brief 出力のシンボルを得る．
  const string&
  output_symbol(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < O() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < O() );
    return mOutputList[pos].mSymbol;
  }

  /// @brief コメントを得る．
  string
  comment() const
  {
    return mComment;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  void
  initialize(
    SizeType I, ///< [in] 入力数
    SizeType L, ///< [in] ラッチ数
    SizeType O, ///< [in] 出力数
    SizeType A  ///< [in] ANDノード数
  );

  /// @brief ラッチのソースリテラルを設定する．
  void
  set_latch_src(
    SizeType pos, ///< [in] ラッチ番号 ( 0 <= pos < L() )
    SizeType src ///< [in] ソースのリテラル
  )
  {
    ASSERT_COND( 0 <= pos && pos < L() );
    mLatchList[pos].mSrc = src;
  }

  /// @brief 出力のソースハンドルを設定する．
  void
  set_output_src(
    SizeType pos, ///< [in] 出力番号 ( 0 <= pos < O() )
    SizeType src ///< [in] ソースのリテラル
  )
  {
    ASSERT_COND( 0 <= pos && pos < O() );
    mOutputList[pos].mSrc = src;
  }

  /// @brief ANDノードのソースリテラルを設定する．
  void
  set_and_src(
    SizeType pos,   ///< [in] AND番号 ( 0 <= pos < A() )
    SizeType src1, ///< [in] ソース1のハンドル
    SizeType src2  ///< [in] ソース1のハンドル
  )
  {
    ASSERT_COND( 0 <= pos && pos < A() );
    mAndList[pos].mSrc1 = src1;
    mAndList[pos].mSrc2 = src2;
  }

  /// @brief 入力名を設定する．
  void
  set_input_symbol(
    SizeType pos,      ///< [in] 入力番号 ( 0 <= pos < I() )
    const string& name ///< [in] 名前
  )
  {
    ASSERT_COND( 0 <= pos && pos < I() );
    mInputList[pos].mSymbol = name;
  }

  /// @brief ラッチ名を設定する．
  void
  set_latch_symbol(
    SizeType pos,      ///< [in] ラッチ番号 ( 0 <= pos < L() )
    const string& name ///< [in] 名前
  )
  {
    ASSERT_COND( 0 <= pos && pos < L() );
    mLatchList[pos].mSymbol = name;
  }

  /// @brief 出力名を設定する．
  void
  set_output_symbol(
    SizeType pos,      ///< [in] 出力番号 ( 0 <= pos < O() )
    const string& name ///< [in] 名前
  )
  {
    ASSERT_COND( 0 <= pos && pos < O() );
    mOutputList[pos].mSymbol = name;
  }

  /// @brief シンボルテーブルとコメントを読み込む．
  void
  read_symbols(
    istream& s ///< [in] 入力ストリーム
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノードの情報
  struct InputInfo
  {
    SizeType mLiteral; // リテラル
    string mSymbol;    // シンボル名
  };

  // ラッチノードの情報
  struct LatchInfo
  {
    SizeType mLiteral; // リテラル
    SizeType mSrc;     // ソースリテラル
    string mSymbol;    // シンボル名
  };

  // 出力ノードの情報
  struct OutputInfo
  {
    SizeType mSrc;     // ソースリテラル
    string mSymbol;    // シンボル名
  };

  // ANDノードの情報
  struct AndInfo
  {
    SizeType mLiteral; // リテラル
    SizeType mSrc1;    // ソース1リテラル
    SizeType mSrc2;    // ソース2リテラル
  };

  // 入力ノードのリテラルのリスト
  vector<InputInfo> mInputList;

  // ラッチノードのリスト
  vector<LatchInfo> mLatchList;

  // 出力ノードのリスト
  vector<OutputInfo> mOutputList;

  // ANDノードのソース１のリテラルのリスト
  vector<AndInfo> mAndList;

  // コメント
  string mComment;

};

END_NAMESPACE_YM_AIG

#endif // MODELIMPL_H
