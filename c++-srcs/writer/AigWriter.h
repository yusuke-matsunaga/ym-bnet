#ifndef AIGWRITER_H
#define AIGWRITER_H

/// @file AigWriter.h
/// @brief AigWriter のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class AigWriter AigWriter.h "AigWriter.h"
/// @brief aig/aag 形式のファイルを出力するためのクラス
//////////////////////////////////////////////////////////////////////
class AigWriter
{
public:

  /// @brief コンストラクタ
  AigWriter() = default;

  /// @brief デストラクタ
  ~AigWriter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定するための関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  ///
  /// この時点で入力とラッチのリテラル番号は自動的に決まる．
  void
  initialize(
    SizeType I, ///< [in] 入力数
    SizeType L, ///< [in] ラッチ数
    SizeType O  ///< [in] 出力数
  );

  /// @brief ANDノードを追加する．
  /// @return 作成したANDノードのリテラルを返す．
  ///
  /// src1, src2 は定義されている必要がある．
  SizeType
  make_and(
    SizeType src1, ///< [in] ソース1のリテラル
    SizeType src2  ///< [in] ソース2のリテラル
  );

  /// @brief AND ゲートをAIGに変換する．
  /// @return 作成したANDノードのリテラルを返す．
  SizeType
  make_and(
    const vector<SizeType>& fanin_lits ///< [in] ファンインのリテラル
  );

  /// @brief NAND ゲートをAIGに変換する．
  /// @return 作成したANDノードのリテラルを返す．
  SizeType
  make_nand(
    const vector<SizeType>& fanin_lits ///< [in] ファンインのリテラル
  );

  /// @brief OR ゲートをAIGに変換する．
  /// @return 作成したANDノードのリテラルを返す．
  SizeType
  make_or(
    const vector<SizeType>& fanin_lits ///< [in] ファンインのリテラル
  );

  /// @brief NOR ゲートをAIGに変換する．
  /// @return 作成したANDノードのリテラルを返す．
  SizeType
  make_nor(
    const vector<SizeType>& fanin_lits ///< [in] ファンインのリテラル
  );

  /// @brief XOR ゲートをAIGに変換する．
  /// @return 作成したANDノードのリテラルを返す．
  SizeType
  make_xor(
    const vector<SizeType>& fanin_lits ///< [in] ファンインのリテラル
  );

  /// @brief XNOR ゲートをAIGに変換する．
  /// @return 作成したANDノードのリテラルを返す．
  SizeType
  make_xnor(
    const vector<SizeType>& fanin_lits ///< [in] ファンインのリテラル
  );

  // @brief Expr を AIG に変換する．
  SizeType
  make_expr(
    const Expr& expr,                  ///< [in] 論理式
    const vector<SizeType>& fanin_lits ///< [in] ファンインのリテラル
  );

  /// @brief ラッチのソースを追加する．
  ///
  /// src は定義されている必要がある．
  void
  set_latch_src(
    SizeType pos, ///< [in] ラッチ番号 ( 0 <= pos < L )
    SizeType src  ///< [in] ソースのリテラル
  );

  /// @brief 出力のソースを追加する．
  ///
  /// src は定義されている必要がある．
  void
  set_output_src(
    SizeType pos, ///< [in] ラッチ番号 ( 0 <= pos < L )
    SizeType src  ///< [in] ソースのリテラル
  );

  /// @brief 入力のシンボルを設定する．
  void
  set_input_symbol(
    SizeType pos,      ///< [in] 入力番号 ( 0 <= pos < I )
    const string& name ///< [in] 名前
  )
  {
    ASSERT_COND( 0 <= pos && pos < I() );
    mInputSymbols[pos] = name;
  }

  /// @brief ラッチのシンボルを設定する．
  void
  set_latch_symbol(
    SizeType pos,      ///< [in] ラッチ番号 ( 0 <= pos < I )
    const string& name ///< [in] 名前
  )
  {
    ASSERT_COND( 0 <= pos && pos < L() );
    mLatchList[pos].mSymbol = name;
  }

  /// @brief 出力のシンボルを設定する．
  void
  set_output_symbol(
    SizeType pos,      ///< [in] 出力番号 ( 0 <= pos < I )
    const string& name ///< [in] 名前
  )
  {
    ASSERT_COND( 0 <= pos && pos < O() );
    mOutputList[pos].mSymbol = name;
  }

  /// @brief コメントを設定する．
  void
  set_comment(
    const string& comment ///< [in] コメント文字列
  )
  {
    mComment = comment;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // BnNetwork からの変換
  //////////////////////////////////////////////////////////////////////

  /// @brief BnNetwork の内容を変換する．
  /// @return 変換できない場合は false を返す．
  bool
  conv_from_bnet(
    const BnNetwork& src_network
  );


public:
  //////////////////////////////////////////////////////////////////////
  // ストリーム出力用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief aig 形式のストリームに出力する．
  void
  write_aig(
    ostream& s,           ///< [in] 出力ストリーム
    const string& comment ///< [in] コメント
  ) const;

  /// @brief aag 形式のストリームに出力する．
  void
  write_aag(
    ostream& s,           ///< [in] 出力ストリーム
    const string& comment ///< [in] コメント
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力数を返す．
  SizeType
  I() const
  {
    return mInputSymbols.size();
  }

  /// @brief ラッチ数を返す．
  SizeType
  L() const
  {
    return mLatchList.size();
  }

  /// @brief 出力数を返す．
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

  /// @brief make_and の下請け関数
  ///
  /// 単純に fanin_lits を二等分して各々の結果のANDをとる．
  /// iinv が true の時には入力を反転する．
  SizeType
  make_and_sub(
    const vector<SizeType>& fanin_lits, ///< [in] ファンインのリテラル
    SizeType from,                      ///< [in] 開始位置
    SizeType to,                        ///< [in] 終了位置
    bool iinv                           ///< [in] 入力の反転フラグ
  );

  /// @brief make_xor の下請け関数
  ///
  /// 単純に fanin_lits を二等分して各々の結果のXORをとる．
  SizeType
  make_xor_sub(
    const vector<SizeType>& fanin_lits, ///< [in] ファンインのリテラル
    SizeType from,                      ///< [in] 開始位置
    SizeType to                         ///< [in] 終了位置
  );

  /// @brief BnNode に対応する AIG を作る．
  /// @return 根のリテラルを返す．
  SizeType
  make_bnnode(
    const BnNode& node,                ///< [in] ノード
    const BnNetwork& network,          ///< [in] ネットワーク
    const vector<SizeType>& fanin_lits ///< [in] ファンインのリテラルのリスト
  );

  /// @brief シンボルテーブルとコメントを出力する．
  void
  write_symbols(
    ostream& s,           ///< [in] 出力ストリーム
    const string& comment ///< [in] コメント
  ) const;

  /// @brief リテラルを定義済みにする．
  void
  reg_literal(
    SizeType lit ///< [in] リテラル
  )
  {
    mDefined.emplace(lit);
    mDefined.emplace(lit ^ 1);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ラッチの情報
  struct LatchInfo
  {
    SizeType mSrc;  // ソースリテラル
    string mSymbol; // シンボル名
  };

  // 出力の情報
  struct OutputInfo
  {
    SizeType mSrc;  // ソースリテラル
    string mSymbol; // シンボル名
  };

  // ANDノードの情報
  struct AndInfo
  {
    SizeType mSrc1;  // ソース1リテラル
    SizeType mSrc2;  // ソース2リテラル
  };

  // 定義済みのリテラルの辞書
  unordered_set<SizeType> mDefined;

  // 入力のシンボル名のリスト
  vector<string> mInputSymbols;

  // ラッチのリスト
  vector<LatchInfo> mLatchList;

  // 出力のリスト
  vector<OutputInfo> mOutputList;

  // ANDノードのリスト
  vector<AndInfo> mAndList;

  // コメント
  string mComment;

};

END_NAMESPACE_YM_BNET

#endif // AIGWRITER_H
