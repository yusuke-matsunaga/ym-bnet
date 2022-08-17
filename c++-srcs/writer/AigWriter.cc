
/// @file AigWriter.cc
/// @brief AigWriter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "AigWriter.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス AigWriter
//////////////////////////////////////////////////////////////////////

// @brief 初期化する．
void
AigWriter::initialize(
  SizeType I,
  SizeType L,
  SizeType O
)
{
  mDefined.clear();

  mInputSymbols.clear();
  mInputSymbols.resize(I);

  mLatchList.clear();
  mLatchList.resize(L);

  mOutputList.clear();
  mOutputList.resize(O);

  mAndList.clear();
}

// @brief ANDノードを追加する．
SizeType
AigWriter::make_and(
  SizeType src1,
  SizeType src2
)
{
  if ( mDefined.count(src1) == 0) {
    // src1 が未定義
  }
  if ( mDefined.count(src2) == 0 ) {
    // src2 が未定義
  }
  if ( src1 < src2 ) {
    std::swap(src1, src2);
  }
  SizeType id = mAndList.size();
  mAndList.push_back(AndInfo{src1, src2});
  SizeType ans = (id + I() + L() + 1) * 2;
  return ans;
}

// AND ゲートをAIGに変換する．
SizeType
AigWriter::make_and(
  const vector<SizeType>& fanin_lits
)
{
  SizeType ni = fanin_lits.size();
  return make_and_sub(fanin_lits, 0, ni, false);
}

// NAND ゲートをAIGに変換する．
SizeType
AigWriter::make_nand(
  const vector<SizeType>& fanin_lits
)
{
  return make_and(fanin_lits) ^ 1;
}

// OR ゲートをAIGに変換する．
SizeType
AigWriter::make_or(
  const vector<SizeType>& fanin_lits
)
{
  SizeType ni = fanin_lits.size();
  return make_and_sub(fanin_lits, 0, ni, true) ^ 1;
}

// NOR ゲートをAIGに変換する．
SizeType
AigWriter::make_nor(
  const vector<SizeType>& fanin_lits
)
{
  return make_or(fanin_lits) ^ 1;
}

// XOR ゲートをAIGに変換する．
SizeType
AigWriter::make_xor(
  const vector<SizeType>& fanin_lits
)
{
  SizeType ni = fanin_lits.size();
  return make_xor_sub(fanin_lits, 0, ni);
}

// XNOR ゲートをAIGに変換する．
SizeType
AigWriter::make_xnor(
  const vector<SizeType>& fanin_lits
)
{
  return make_xor(fanin_lits) ^ 1;
}

// Expr を AIG に変換する．
SizeType
AigWriter::make_expr(
  const Expr& expr,
  const vector<SizeType>& fanin_lits
)
{
  if ( expr.is_zero() ) {
    return 0;
  }
  if ( expr.is_one() ) {
    return 1;
  }
  if ( expr.is_posi_literal() ) {
    return fanin_lits[expr.varid().val()];
  }
  if ( expr.is_nega_literal() ) {
    return fanin_lits[expr.varid().val()] ^ 1;
  }
  SizeType nc = expr.child_num();
  vector<SizeType> child_lits(nc);
  for ( SizeType i = 0; i < nc; ++ i ) {
    child_lits[i] = make_expr(expr.child(i), fanin_lits);
  }
  if ( expr.is_and() ) {
    return make_and(child_lits);
  }
  if ( expr.is_or() ) {
    return make_or(child_lits);
  }
  if ( expr.is_xor() ) {
    return make_xor(child_lits);
  }
  ASSERT_NOT_REACHED;
  return -1;
}

// @brief ラッチのソースを追加する．
void
AigWriter::set_latch_src(
  SizeType pos,
  SizeType src
)
{
  if ( mDefined.count(src) == 0 ) {
    // src が未定義
  }
  ASSERT_COND( 0 <= pos && pos < L() );
  mLatchList[pos].mSrc = src;
}

// @brief 出力のソースを追加する．
void
AigWriter::set_output_src(
  SizeType pos,
  SizeType src
)
{
  if ( mDefined.count(src) == 0 ) {
    // src が未定義
  }
  ASSERT_COND( 0 <= pos && pos < O() );
  mOutputList[pos].mSrc = src;
}

BEGIN_NONAMESPACE

void
put_number(
  ostream& s,
  SizeType num
)
{
  if ( num <= 127 ) {
    s.put(static_cast<char>(num));
  }
  else {
    SizeType num1 = (num & 127) | 128;
    s.put(static_cast<char>(num1));
    put_number(s, (num >> 7));
  }
}

END_NONAMESPACE

// @brief aag 形式のストリームに出力する．
void
AigWriter::write_aig(
  ostream& s
) const
{
  // ヘッダ行の出力
  s << "aig " << (I() + L() + A())
    << " " << I()
    << " " << L()
    << " " << O()
    << " " << A() << endl;

  // ラッチ行の出力
  for ( const auto& src: mLatchList ) {
    s << src.mSrc << endl;
  }

  // 出力行の出力
  for ( const auto& src: mOutputList ) {
    s << src.mSrc << endl;
  }

  // AND行の出力
  SizeType id = I() + L() + 1;
  for ( const auto& node: mAndList ) {
    SizeType src0 = node.mSrc1;
    SizeType src1 = node.mSrc2;
    SizeType lhs = id * 2;
    SizeType d0 = lhs - src0;
    SizeType d1 = src0 - src1;
    put_number(s, d0);
    put_number(s, d1);
    ++ id;
  }

  // シンボルテーブルとコメントの出力
  write_symbols(s);
}

// @brief aag 形式のストリームに出力する．
void
AigWriter::write_aag(
  ostream& s
) const
{
  // ヘッダ行の出力
  s << "aag " << (I() + L() + A())
    << " " << I()
    << " " << L()
    << " " << O()
    << " " << A() << endl;

  // 入力行の出力
  for ( SizeType i = 0; i < I(); ++ i ) {
    s << (i + 1) * 2 << endl;
  }

  // ラッチ行の出力
  for ( SizeType i = 0; i < L(); ++ i ) {
    SizeType node_id = i + I() + 1;
    auto src = mLatchList[i].mSrc;
    s << (node_id * 2) << " " << src << endl;
  }

  // 出力行の出力
  for ( SizeType i = 0; i < O(); ++ i ) {
    auto src = mOutputList[i].mSrc;
    s << src << endl;
  }

  // AND行の出力
  for ( SizeType i = 0; i < A(); ++ i ) {
    auto lit = (i + I() + L() + 1) * 2;
    auto src1 = mAndList[i].mSrc1;
    auto src2 = mAndList[i].mSrc2;
    s << lit << " "
      << src1 << " "
      << src2 << endl;
  }

  // シンボルテーブルとコメントの出力
  write_symbols(s);
}

// make_and の下請け関数
SizeType
AigWriter::make_and_sub(
  const vector<SizeType>& fanin_lits,
  SizeType from,
  SizeType to,
  bool iinv
)
{
  SizeType imask = iinv ? 1 : 0;
  SizeType ni = to - from;
  ASSERT_COND( ni > 0 );
  if ( ni == 1 ) {
    return fanin_lits[from] ^ imask;
  }
  // ni >= 2
  SizeType half = from + ni / 2;
  SizeType src1 = make_and_sub(fanin_lits, from, half, iinv);
  SizeType src2 = make_and_sub(fanin_lits, half, to, iinv);
  return make_and(src1, src2);
}

// make_xor の下請け関数
SizeType
AigWriter::make_xor_sub(
  const vector<SizeType>& fanin_lits,
  SizeType from,
  SizeType to
)
{
  SizeType ni = to - from;
  ASSERT_COND( ni > 0 );
  if ( ni == 1 ) {
    return fanin_lits[from];
  }
  // ni >= 2
  SizeType half = from + ni / 2;
  SizeType src1 = make_xor_sub(fanin_lits, from, half);
  SizeType src2 = make_xor_sub(fanin_lits, half, to);
  SizeType tmp1 = make_and(src1, src2);
  SizeType tmp2 = make_and(src1 ^ 1, src2 ^ 1);
  return make_and(tmp1 ^ 1, tmp2 ^ 1);
}

// @brief シンボルテーブルとコメントを出力する．
void
AigWriter::write_symbols(
  ostream& s ///< [in] 出力ストリーム
) const
{
  // 入力のシンボルテーブルの出力
  for ( SizeType i = 0; i < I(); ++ i ) {
    const auto& name = mInputSymbols[i];
    if ( name != string{} ) {
      s << "i" << i << " " << name << endl;
    }
  }

  // ラッチのシンボルテーブルの出力
  for ( SizeType i = 0; i < L(); ++ i ) {
    const auto& name = mLatchList[i].mSymbol;
    if ( name != string{} ) {
      s << "l" << i << " " << name << endl;
    }
  }

  // 出力のシンボルテーブルの出力
  for ( SizeType i = 0; i < O(); ++ i ) {
    const auto& name = mOutputList[i].mSymbol;
    if ( name != string{} ) {
      s << "o" << i << " " << name << endl;
    }
  }

  // コメントの出力
  if ( mComment != string{} ) {
    s << "c" << endl;
    s << mComment;
  }
}

END_NAMESPACE_YM_BNET
