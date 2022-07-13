
/// @file AigWriter.cc
/// @brief AigWriter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "AigWriter.h"
#include "ym/BnNetwork.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

// make_and の下請け関数
// 単純に fanin_lits を二等分して各々の結果のANDをとる．
// iinv が true の時には入力を反転する．
SizeType
make_and_sub(
  AigWriter& aig,
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
  SizeType nh = ni / 2;
  SizeType src1 = make_and_sub(aig, fanin_lits, from, from + nh, iinv);
  SizeType src2 = make_and_sub(aig, fanin_lits, from + nh, to, iinv);
  return aig.make_and(src1, src2);
}

// AND ゲートをAIGに変換する．
SizeType
make_and(
  AigWriter& aig,
  const vector<SizeType>& fanin_lits
)
{
  SizeType ni = fanin_lits.size();
  SizeType ans = make_and_sub(aig, fanin_lits, 0, ni, false);
  return ans;
}

// NAND ゲートをAIGに変換する．
SizeType
make_nand(
  AigWriter& aig,
  const vector<SizeType>& fanin_lits
)
{
  return make_and(aig, fanin_lits) ^ 1;
}

// OR ゲートをAIGに変換する．
SizeType
make_or(
  AigWriter& aig,
  const vector<SizeType>& fanin_lits
)
{
  SizeType ni = fanin_lits.size();
  SizeType ans = make_and_sub(aig, fanin_lits, 0, ni, true) ^ 1;
  return ans;
}

// NOR ゲートをAIGに変換する．
SizeType
make_nor(
  AigWriter& aig,
  const vector<SizeType>& fanin_lits
)
{
  return make_or(aig, fanin_lits) ^ 1;
}

// make_xor の下請け関数
// 単純に fanin_lits を二等分して各々の結果の XOR を計算する AIG を作る．
SizeType
make_xor_sub(
  AigWriter& aig,
  const vector<SizeType>& fanin_lits,
  SizeType from,
  SizeType to
)
{
  SizeType ni = to - from;
  ASSERT_COND( ni > 0 );
  if ( ni == 1 ) {
    return fanin_lits[0];
  }
  // ni >= 2
  SizeType nh = ni / 2;
  SizeType src1 = make_xor_sub(aig, fanin_lits, from, from + nh);
  SizeType src2 = make_xor_sub(aig, fanin_lits, from + nh, to);
  SizeType tmp1 = aig.make_and(src1, src2);
  SizeType tmp2 = aig.make_and(src1 ^ 1, src2 ^ 1);
  return aig.make_and(tmp1 ^ 1, tmp2 ^ 1);
}

// XOR ゲートをAIGに変換する．
SizeType
make_xor(
  AigWriter& aig,
  const vector<SizeType>& fanin_lits
)
{
  SizeType ni = fanin_lits.size();
  return make_xor_sub(aig, fanin_lits, 0, ni);
}

// XNOR ゲートをAIGに変換する．
SizeType
make_xnor(
  AigWriter& aig,
  const vector<SizeType>& fanin_lits
)
{
  return make_xor(aig, fanin_lits) ^ 1;
}

// Expr を AIG に変換する．
SizeType
make_expr(
  AigWriter& aig,
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
    child_lits[i] = make_expr(aig, expr.child(i), fanin_lits);
  }
  if ( expr.is_and() ) {
    return make_and(aig, child_lits);
  }
  if ( expr.is_or() ) {
    return make_or(aig, child_lits);
  }
  if ( expr.is_xor() ) {
    return make_xor(aig, child_lits);
  }
  ASSERT_NOT_REACHED;
  return -1;
}

// TvFunc を AIG に変換する．
SizeType
make_tv(
  AigWriter& aig,
  const TvFunc& func,
  const vector<SizeType>& fanin_lits
)
{
  auto expr = func.BCF_expr();
  return make_expr(aig, expr, fanin_lits);
}

// BnNode の内容を AIG に変換する．
SizeType
make_aig(
  AigWriter& aig,
  const BnNetwork& bnet,
  const BnNode& node,
  const vector<SizeType>& fanin_lits
)
{
  switch ( node.type() ) {
  case BnNodeType::C0:   // 定数0
    return 0;
  case BnNodeType::C1:   // 定数1
    return 1;
  case BnNodeType::Buff: // バッファ
    return fanin_lits[0];
  case BnNodeType::Not:  // インバータ
    return fanin_lits[0] ^ 1;
  case BnNodeType::And:  // AND
    return make_and(aig, fanin_lits);
  case BnNodeType::Nand: // NAND
    return make_nand(aig, fanin_lits);
  case BnNodeType::Or:   // OR
    return make_or(aig, fanin_lits);
  case BnNodeType::Nor:  // NOR
    return make_nor(aig, fanin_lits);
  case BnNodeType::Xor:  // XOR
    return make_xor(aig, fanin_lits);
  case BnNodeType::Xnor: // XNOR
    return make_xnor(aig, fanin_lits);
  case BnNodeType::Expr: // 論理式
    return make_expr(aig, bnet.expr(node.expr_id()), fanin_lits);
  case BnNodeType::TvFunc: // 真理値表
    return make_tv(aig, bnet.func(node.func_id()), fanin_lits);
  default:
    ASSERT_NOT_REACHED;
  }
  return static_cast<SizeType>(-1);
}

// BnNetwork の情報を AigWriter に変換する．
void
bnet2aig(
  AigWriter& aig,
  const BnNetwork& bnet,
  const string& comment
)
{
  SizeType I = bnet.input_num();
  SizeType L = bnet.dff_num();
  SizeType O = bnet.output_num();
  aig.initialize(I, L, O);

  // ノード番号をキーにしてリテラルを格納する辞書
  unordered_map<SizeType, SizeType> lit_map;

  // 入力ノードを登録する．
  for ( SizeType i = 0; i < I; ++ i ) {
    lit_map.emplace(bnet.input_id(i), (i + 1) * 2);
  }
  // ラッチノード(DFFの出力ノード)を登録する．
  for ( SizeType i = 0; i < L; ++ i ) {
    const auto& dff = bnet.dff(i);
    lit_map.emplace(dff.output(), (i + I + 1) * 2);
  }
  // AND ノードを生成する．
  for ( auto id: bnet.logic_id_list() ) {
    const auto& node = bnet.node(id);
    SizeType ni = node.fanin_num();
    // ファンインのリテラルのリスト
    vector<SizeType> fanin_list(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto fid = node.fanin_id(i);
      ASSERT_COND( lit_map.count(fid) > 0 );
      auto ilit = lit_map.at(fid);
      fanin_list[i] = ilit;
    }
    auto olit = make_aig(aig, bnet, node, fanin_list);
    lit_map.emplace(id, olit);
  }
  // ラッチのソースを設定する．
  for ( SizeType i = 0; i < L; ++ i ) {
    const auto& dff = bnet.dff(i);
    auto src_id = dff.input();
    ASSERT_COND( lit_map.count(src_id) > 0 );
    auto src = lit_map.at(src_id);
    aig.set_latch_src(i, src);
  }
  // 出力のソースを設定する．
  for ( SizeType i = 0; i < O; ++ i ) {
    auto src_id = bnet.output_src_id(i);
    ASSERT_COND( lit_map.count(src_id) > 0 );
    auto src = lit_map.at(src_id);
    aig.set_output_src(i, src);
  }
  // 入力のシンボル名を設定する．
  for ( SizeType i = 0; i < I; ++ i ) {
    const auto& node = bnet.node(bnet.input_id(i));
    auto name = node.name();
    if ( name != string{} ) {
      aig.set_input_symbol(i, name);
    }
  }
  // ラッチのシンボル名を設定する．
  for ( SizeType i = 0; i < L; ++ i ) {
    const auto& dff = bnet.dff(i);
    auto name = dff.name();
    if ( name != string{} ) {
      aig.set_latch_symbol(i, name);
    }
  }
  // 出力のシンボル名を設定する．
  for ( SizeType i = 0; i < O; ++ i ) {
    const auto& node = bnet.node(bnet.output_id(i));
    auto name = node.name();
    if ( name != string{} ) {
      aig.set_output_symbol(i, name);
    }
  }
  // コメントを設定する．
  if ( comment != string{} ) {
    aig.set_comment(comment);
  }
}

END_NONAMESPACE

// @brief 内容を aig 形式で出力する．
void
BnNetwork::write_aig(
  const string& filename,
  const string& comment
) const
{
  ofstream s{filename};
  if ( s ) {
    write_aig(s, comment);
  }
}

// @brief 内容を aag (ascii aig) 形式で出力する．
void
BnNetwork::write_aag(
  const string& filename,
  const string& comment
) const
{
  ofstream s{filename};
  if ( s ) {
    write_aag(s, comment);
  }
}

// @brief 内容を aig 形式で出力する．
void
BnNetwork::write_aig(
  ostream& s,
  const string& comment
) const
{
  AigWriter aig;
  bnet2aig(aig, *this, comment);
  aig.write_aig(s);
}

// @brief 内容を aag (ascii aig) 形式で出力する．
void
BnNetwork::write_aag(
  ostream& s,
  const string& comment
) const
{
  AigWriter aig;
  bnet2aig(aig, *this, comment);
  aig.write_aag(s);
}

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
