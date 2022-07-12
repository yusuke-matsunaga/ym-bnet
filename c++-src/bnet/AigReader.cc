
/// @file AigReader.cc
/// @brief AigReader の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "AigReader.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

bool debug = false;

END_NONAMESPACE

// @brief 内容を初期化する．
void
AigReader::initialize(
  SizeType I,
  SizeType L,
  SizeType O,
  SizeType A
)
{
  mInputList.clear();
  mInputList.resize(I);

  mLatchList.clear();
  mLatchList.resize(L);

  mOutputList.clear();
  mOutputList.resize(O);

  mAndList.clear();
  mAndList.resize(A);

  mComment = string{};
}

// @brief Ascii AIG フォーマットを読み込む．
bool
AigReader::read_aag(
  const string& filename
)
{
  ifstream s{filename};
  if ( s ) {
    return read_aag(s);
  }
  else {
    cerr << filename << ": No such file" << endl;
    return false;
  }
}

// @brief Ascii AIG フォーマットを読み込む．
bool
AigReader::read_aag(
  istream& s
)
{
  string linebuf;

  // ヘッダ行の読み込み
  if ( !getline(s, linebuf) ) {
    throw AigError{"Unexpected EOF"};
  }
  if ( linebuf.substr(0, 3) != string{"aag"} ) {
    ostringstream buf;
    buf << linebuf << ": Illegal header signature, 'aag' expected.";
    throw AigError{buf.str()};
  }
  istringstream tmp{linebuf.substr(4, string::npos)};
  SizeType M, I, L, O, A;
  tmp >> M >> I >> L >> O >> A;

  if ( debug ) {
    cout << "aag " << M
	 << " " << I
	 << " " << L
	 << " " << O
	 << " " << A << endl;
  }

  // M は捨てる．
  initialize(I, L, O, A);

  // 定義されたリテラルの辞書
  vector<bool> defined(M * 2, false);

  // 入力行の読み込み
  for ( SizeType i = 0; i < I; ++ i ) {
    if ( !getline(s, linebuf) ) {
      throw AigError{"Unexpected EOF"};
    }
    istringstream tmp{linebuf};
    SizeType lit;
    tmp >> lit;
    if ( debug ) {
      cout << "I#" << i << ": " << lit << endl;
    }
    if ( (lit % 2) == 1 ) {
      throw AigError{"Positive Literal(even number) expected"};
    }
    if ( defined[lit] ) {
      ostringstream buf;
      buf << lit << " is already defined.";
      throw AigError{buf.str()};
    }
    defined[lit] = true;
    mInputList[i].mLiteral = lit;
  }

  // ラッチ行の読み込み
  for ( SizeType i = 0; i < L; ++ i ) {
    if ( !getline(s, linebuf) ) {
      throw AigError{"Unexpected EOF"};
    }
    istringstream tmp{linebuf};
    SizeType lit, src;
    tmp >> lit >> src;
    if ( debug ) {
      cout << "L#" << i << ": " << lit << " " << src << endl;
    }
    if ( (lit % 2) == 1 ) {
      throw AigError{"Positive Literal(even number) expected"};
    }
    if ( defined[lit] ) {
      ostringstream buf;
      buf << lit << " is already defined.";
      throw AigError{buf.str()};
    }
    defined[lit] = true;
    mLatchList[i].mLiteral = lit;
    mLatchList[i].mSrc = src;
  }

  // 出力行の読み込み
  for ( SizeType i = 0; i < O; ++ i ) {
    if ( !getline(s, linebuf) ) {
      throw AigError{"Unexpected EOF"};
    }
    istringstream tmp{linebuf};
    SizeType src;
    tmp >> src;
    if ( debug ) {
      cout << "O#" << i << ": " << src << endl;
    }
    mOutputList[i].mSrc = src;
  }

  // AND行の読み込み
  for ( SizeType i = 0; i < A; ++ i ) {
    if ( !getline(s, linebuf) ) {
      throw AigError{"Unexpected EOF"};
    }
    istringstream tmp{linebuf};
    SizeType lit, src0, src1;
    tmp >> lit >> src0 >> src1;
    if ( debug ) {
      cout << "A#" << i << ": " << lit << " " << src0 << " " << src1 << endl;
    }
    if ( (lit % 2) == 1 ) {
      throw AigError{"Positive Literal(even number) expected"};
    }
    if ( defined[lit] ) {
      ostringstream buf;
      buf << lit << " is already defined.";
      throw AigError{buf.str()};
    }
    defined[lit] = true;
    mAndList[i].mLiteral = lit;
    mAndList[i].mSrc1 = src0;
    mAndList[i].mSrc2 = src1;
  }

  // ソースリテラルが定義されているかチェック
  for ( SizeType i = 0; i < L; ++ i ) {
    auto src = latch_src(i);
    if ( !defined[src] && !defined[src ^ 1] ) {
      ostringstream buf;
      buf << src << " is not defined required by Latch#" << i
	  << "(" << latch(i) << ").";
      throw AigError{buf.str()};
    }
  }
  for ( SizeType i = 0; i < O; ++ i ) {
    auto src = output_src(i);
    if ( !defined[src] && !defined[src ^ 1] ) {
      ostringstream buf;
      buf << src << " is not defined required by Output#" << i << ".";
      throw AigError{buf.str()};
    }
  }
  for ( SizeType i = 0; i < A; ++ i ) {
    auto src1 = and_src1(i);
    if ( !defined[src1] && !defined[src1 ^ 1] ) {
      ostringstream buf;
      buf << src1 << " is not defined required by And#" << i
	  << "(" << and_node(i) << ").";
      throw AigError{buf.str()};
    }
    auto src2 = and_src2(i);
    if ( !defined[src2] && !defined[src2 ^ 1] ) {
      ostringstream buf;
      buf << src2 << " is not defined required by And#" << i
	  << "(" << and_node(i) << ").";
      throw AigError{buf.str()};
    }
  }

  // シンボルテーブルとコメントの読み込みを行う．
  read_symbols(s);

  return true;
}

BEGIN_NONAMESPACE

// @brief 数字を一つ読み出す．
SizeType
get_number(
  istream& s
)
{
  SizeType num = 0;
  for ( SizeType weight = 1; ; weight *= 128 ) {
    SizeType c = s.get();
    num += (c & 127) * weight;
    if ( (c & 128) == 0 ) {
      break;
    }
  }
  return num;
}

END_NONAMESPACE

// @brief AIG フォーマットを読み込む．
bool
AigReader::read_aig(
  const string& filename
)
{
  ifstream s{filename};
  if ( s ) {
    return read_aig(s);
  }
  else {
    cerr << filename << ": No such file" << endl;
    return false;
  }
}

// @brief AIG フォーマットを読み込む．
bool
AigReader::read_aig(
  istream& s
)
{
  string linebuf;

  // ヘッダ行の読み込み
  if ( !getline(s, linebuf) ) {
    throw AigError{"Unexpected EOF"};
  }
  if ( linebuf.substr(0, 3) != string{"aig"} ) {
    ostringstream buf;
    buf << linebuf << ": Illegal header signature, 'aig' expected.";
    throw AigError{buf.str()};
  }
  istringstream tmp{linebuf.substr(4, string::npos)};
  SizeType M, I, L, O, A;
  tmp >> M >> I >> L >> O >> A;

  ASSERT_COND( M == I + L + A );

  if ( debug ) {
    cout << "aig " << M
	 << " " << I
	 << " " << L
	 << " " << O
	 << " " << A << endl;
  }

  // M は捨てる．
  initialize(I, L, O, A);

  // 入力の情報を作る．
  for ( SizeType i = 0; i < I; ++ i ) {
    mInputList[i].mLiteral = (i + 1) * 2;
  }

  // ラッチ行の読み込み
  for ( SizeType i = 0; i < L; ++ i ) {
    if ( !getline(s, linebuf) ) {
      throw AigError{"Unexpected EOF"};
    }
    istringstream tmp{linebuf};
    SizeType src;
    tmp >> src;
    if ( debug ) {
      cout << "L#" << i << ": " << src << endl;
    }
    mLatchList[i] = LatchInfo{(i + I + 1) * 2, src};
  }

  // 出力行の読み込み
  for ( SizeType i = 0; i < O; ++ i ) {
    if ( !getline(s, linebuf) ) {
      throw AigError{"Unexpected EOF"};
    }
    istringstream tmp{linebuf};
    SizeType src;
    tmp >> src;
    if ( debug ) {
      cout << "O#" << i << ": " << src << endl;
    }
    mOutputList[i] = OutputInfo{src};
  }

  // AND行の読み込み
  for ( SizeType i = 0; i < A; ++ i ) {
    SizeType d0 = get_number(s);
    SizeType d1 = get_number(s);
    SizeType pos = i + I + L + 1;
    SizeType lhs = pos * 2;
    SizeType rhs0 = lhs - d0;
    SizeType rhs1 = rhs0 - d1;
    if ( debug ) {
      cout << "A#" << i << ": "
	   << d0 << " " << d1
	   << " -> " << rhs0 << " " << rhs1 << endl;
    }
    mAndList[i] = AndInfo{(i + I + L + 1) * 2, rhs0, rhs1};
  }

  // シンボルの読み込み
  read_symbols(s);

  return true;
}

// @brief シンボルテーブルとコメントを読み込む．
void
AigReader::read_symbols(
  istream& s
)
{
  string linebuf;
  bool symbol_mode = true;
  while ( getline(s, linebuf) ) {
    if ( symbol_mode ) {
      if ( linebuf == "c" ) {
	symbol_mode = false;
      }
      else {
	auto p = linebuf.find_first_of(" ");
	auto pos_str = linebuf.substr(0, p);
	auto name = linebuf.substr(p + 1, string::npos);
	SizeType pos = atoi(pos_str.substr(1, string::npos).c_str());
	char prefix = pos_str[0];
	if ( prefix == 'i' ) {
	  set_input_symbol(pos, name);
	}
	else if ( prefix == 'l' ) {
	  set_latch_symbol(pos, name);
	}
	else if ( prefix == 'o' ) {
	  set_output_symbol(pos, name);
	}
	else {
	  ASSERT_NOT_REACHED;
	}
      }
    }
    else {
      mComment += linebuf + '\n';
    }
  }
}

#if 0

// @brief 内容を Ascii AIG フォーマットで書き出す．
void
Aig::write_aag(
  const string& filename
) const
{
  ofstream s{filename};
  if ( s ) {
    write_aag(s);
  }
  else {
    cerr << filename << ": Could not open" << endl;
  }
}

// @brief 内容を Ascii AIG フォーマットで書き出す．
void
Aig::write_aag(
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
    auto src = latch_src(i);
    s << (node_id * 2) << " " << src << endl;
  }

  // 出力行の出力
  for ( SizeType i = 0; i < O(); ++ i ) {
    auto src = output_src(i);
    s << src << endl;
  }

  // AND行の出力
  for ( SizeType i = 0; i < A(); ++ i ) {
    auto lit = and_node(i);
    auto src1 = and_src1(i);
    auto src2 = and_src2(i);
    s << lit << " "
      << src1 << " "
      << src2 << endl;
  }

  // シンボルテーブルとコメントの出力
  write_symbols(s);
}

// @brief 内容を AIG フォーマットで書き出す．
void
Aig::write_aig(
  const string& filename
) const
{
  ofstream s{filename};
  if ( s ) {
    write_aig(s);
  }
  else {
    cerr << filename << ": Could not open" << endl;
  }
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

// @brief 内容を AIG フォーマットで書き出す．
void
Aig::write_aig(
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
  for ( auto src: mLatchSrcList ) {
    s << src.lit() << endl;
  }

  // 出力行の出力
  for ( auto src: mOutputList ) {
    s << src.lit() << endl;
  }

  // AND行の出力
  for ( auto node: mAndList ) {
    SizeType src0 = node->src1().lit();
    SizeType src1 = node->src2().lit();
    SizeType lhs = node->lit();
    SizeType d0 = lhs - src0;
    SizeType d1 = src0 - src1;
    put_number(s, d0);
    put_number(s, d1);
  }

  // シンボルテーブルとコメントの出力
  write_symbols(s);
}

// @brief シンボルテーブルとコメントを出力する．
void
Aig::write_symbols(
  ostream& s ///< [in] 出力ストリーム
) const
{

  // 入力のシンボルテーブルの出力
  for ( SizeType i = 0; i < I(); ++ i ) {
    const auto& name = input_symbol(i);
    if ( name != string{} ) {
      s << "i" << i << " " << name << endl;
    }
  }

  // ラッチのシンボルテーブルの出力
  for ( SizeType i = 0; i < L(); ++ i ) {
    const auto& name = latch_symbol(i);
    if ( name != string{} ) {
      s << "l" << i << " " << name << endl;
    }
  }

  // 出力のシンボルテーブルの出力
  for ( SizeType i = 0; i < O(); ++ i ) {
    const auto& name = output_symbol(i);
    if ( name != string{} ) {
      s << "o" << i << " " << name << endl;
    }
  }

  // コメントの出力
  if ( comment() != string{} ) {
    s << "c" << endl;
    s << comment();
  }
}

// @brief ノードのID番号を振り直す．
void
Aig::reordering()
{
  // ノード番号をキーにしてファンアウトリストを保持するリスト
  vector<vector<AigNode*>> fanout_list_array(I() + L() + A() + 1);
  // ANDノードのファンインのファンアウトリストを作る．
  for ( auto node: mAndList ) {
    auto src1 = node->src1();
    auto src1_node = src1.node();
    fanout_list_array[src1_node->id()].push_back(node);
    auto src2 = node->src2();
    auto src2_node = src2.node();
    fanout_list_array[src2_node->id()].push_back(node);
  }
  // 新しいID番号のマップ
  vector<SizeType> id_map(I() + L() + A() + 1, 0);
  // 有効なID番号
  SizeType last_id = 1;
  // ファンインが処理済みとなったノードを入れるキュー
  vector<AigNode*> queue;
  // キューに入っていることを示すフラグ
  vector<bool> in_queue(I() + L() + A() + 1, false);
  for ( auto node: mInputList ) {
    id_map[node->id()] = last_id;
    ++ last_id;
    bool ok = true;
    for ( auto node1: fanout_list_array[node->id()] ) {
      if ( in_queue[node1->id()] ) {
	continue;
      }
      if ( id_map[node1->src1().node()->id()] > 0 &&
	   id_map[node1->src2().node()->id()] > 0 ) {
	// 2つのファンインが処理済み
	queue.push_back(node1);
	in_queue[node1->id()] = true;
      }
    }
  }
  for ( auto node: mLatchList ) {
    id_map[node->id()] = last_id;
    ++ last_id;
    bool ok = true;
    for ( auto node1: fanout_list_array[node->id()] ) {
      if ( in_queue[node1->id()] ) {
	continue;
      }
      if ( id_map[node1->src1().node()->id()] > 0 &&
	   id_map[node1->src2().node()->id()] > 0 ) {
	// 2つのファンインが処理済み
	queue.push_back(node1);
	in_queue[node1->id()] = true;
      }
    }
  }
  for ( SizeType rpos = 0; rpos < queue.size(); ++ rpos ) {
    auto node = queue[rpos];
    id_map[node->id()] = last_id;
    ++ last_id;
    bool ok = true;
    for ( auto node1: fanout_list_array[node->id()] ) {
      if ( in_queue[node1->id()] ) {
	continue;
      }
      if ( id_map[node1->src1().node()->id()] > 0 &&
	   id_map[node1->src2().node()->id()] > 0 ) {
	// 2つのファンインが処理済み
	queue.push_back(node1);
	in_queue[node1->id()] = true;
      }
    }
  }
  // ID番号を付け替える．
  for ( auto node: mInputList ) {
    node->set_id(id_map[node->id()]);
  }
  for ( auto node: mLatchList ) {
    node->set_id(id_map[node->id()]);
  }
  for ( auto node: mAndList ) {
    node->set_id(id_map[node->id()]);
  }
}
#endif

END_NAMESPACE_YM_BNET
