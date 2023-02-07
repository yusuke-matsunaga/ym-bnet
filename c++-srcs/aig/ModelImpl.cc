
/// @file ModelImpl.cc
/// @brief ModelImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ModelImpl.h"


BEGIN_NAMESPACE_YM_AIG

BEGIN_NONAMESPACE

bool debug = false;

END_NONAMESPACE

// @brief 内容を初期化する．
void
ModelImpl::initialize(
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
ModelImpl::read_aag(
  istream& s
)
{
  string linebuf;

  // ヘッダ行の読み込み
  if ( !getline(s, linebuf) ) {
    throw std::invalid_argument{"Unexpected EOF"};
  }
  if ( linebuf.substr(0, 3) != string{"aag"} ) {
    ostringstream buf;
    buf << linebuf << ": Illegal header signature, 'aag' expected.";
    throw std::invalid_argument{buf.str()};
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
      throw std::invalid_argument{"Unexpected EOF"};
    }
    istringstream tmp{linebuf};
    SizeType lit;
    tmp >> lit;
    if ( debug ) {
      cout << "I#" << i << ": " << lit << endl;
    }
    if ( (lit % 2) == 1 ) {
      throw std::invalid_argument{"Positive Literal(even number) expected"};
    }
    if ( defined[lit] ) {
      ostringstream buf;
      buf << lit << " is already defined.";
      throw std::invalid_argument{buf.str()};
    }
    defined[lit] = true;
    mInputList[i].mLiteral = lit;
  }

  // ラッチ行の読み込み
  for ( SizeType i = 0; i < L; ++ i ) {
    if ( !getline(s, linebuf) ) {
      throw std::invalid_argument{"Unexpected EOF"};
    }
    istringstream tmp{linebuf};
    SizeType lit, src;
    tmp >> lit >> src;
    if ( debug ) {
      cout << "L#" << i << ": " << lit << " " << src << endl;
    }
    if ( (lit % 2) == 1 ) {
      throw std::invalid_argument{"Positive Literal(even number) expected"};
    }
    if ( defined[lit] ) {
      ostringstream buf;
      buf << lit << " is already defined.";
      throw std::invalid_argument{buf.str()};
    }
    defined[lit] = true;
    mLatchList[i].mLiteral = lit;
    mLatchList[i].mSrc = src;
  }

  // 出力行の読み込み
  for ( SizeType i = 0; i < O; ++ i ) {
    if ( !getline(s, linebuf) ) {
      throw std::invalid_argument{"Unexpected EOF"};
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
      throw std::invalid_argument{"Unexpected EOF"};
    }
    istringstream tmp{linebuf};
    SizeType lit, src0, src1;
    tmp >> lit >> src0 >> src1;
    if ( debug ) {
      cout << "A#" << i << ": " << lit << " " << src0 << " " << src1 << endl;
    }
    if ( (lit % 2) == 1 ) {
      throw std::invalid_argument{"Positive Literal(even number) expected"};
    }
    if ( defined[lit] ) {
      ostringstream buf;
      buf << lit << " is already defined.";
      throw std::invalid_argument{buf.str()};
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
      throw std::invalid_argument{buf.str()};
    }
  }
  for ( SizeType i = 0; i < O; ++ i ) {
    auto src = output_src(i);
    if ( !defined[src] && !defined[src ^ 1] ) {
      ostringstream buf;
      buf << src << " is not defined required by Output#" << i << ".";
      throw std::invalid_argument{buf.str()};
    }
  }
  for ( SizeType i = 0; i < A; ++ i ) {
    auto src1 = and_src1(i);
    if ( !defined[src1] && !defined[src1 ^ 1] ) {
      ostringstream buf;
      buf << src1 << " is not defined required by And#" << i
	  << "(" << and_node(i) << ").";
      throw std::invalid_argument{buf.str()};
    }
    auto src2 = and_src2(i);
    if ( !defined[src2] && !defined[src2 ^ 1] ) {
      ostringstream buf;
      buf << src2 << " is not defined required by And#" << i
	  << "(" << and_node(i) << ").";
      throw std::invalid_argument{buf.str()};
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
ModelImpl::read_aig(
  istream& s
)
{
  string linebuf;

  // ヘッダ行の読み込み
  if ( !getline(s, linebuf) ) {
    throw std::invalid_argument{"Unexpected EOF"};
  }
  if ( linebuf.substr(0, 3) != string{"aig"} ) {
    ostringstream buf;
    buf << linebuf << ": Illegal header signature, 'aig' expected.";
    throw std::invalid_argument{buf.str()};
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
      throw std::invalid_argument{"Unexpected EOF"};
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
      throw std::invalid_argument{"Unexpected EOF"};
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
ModelImpl::read_symbols(
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

END_NAMESPACE_YM_AIG
