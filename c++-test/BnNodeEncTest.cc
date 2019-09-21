
/// @file BnNodeEncTest.cc
/// @brief BnNodeEncTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/BnNodeEnc.h"
#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/BnPort.h"
#include "ym/Range.h"
#include "ym/SatSolver.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_YM

class BnNodeEncTest :
  public ::testing::TestWithParam<string>
{
public:

  /// @brief コンストラクタ
  BnNodeEncTest();

  /// @brief デストラクタ
  ~BnNodeEncTest();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  void
  SetUp();

  /// @brief 終了処理を行う．
  void
  TearDown();

  /// @brief 入力ノードを用意する．
  /// @param[in] ni 入力数
  void
  make_inputs(int ni);

  /// @brief チェック対象のノードの変数を用意する．
  /// @param[in] id ノード番号
  void
  make_node_variable(int id);

  /// @brief ノードの出力の論理関数が vals[] で示された真理値表と等しいか調べる．
  /// @param[in] node_id 対象のノード番号
  /// @param[in] vals 真理値表の値
  void
  check(int node_id,
	const vector<int>& vals);

  /// @brief ANDのチェックを行う．
  void
  check_and(int ni);

  /// @brief NANDのチェックを行う．
  void
  check_nand(int ni);

  /// @brief ORのチェックを行う．
  void
  check_or(int ni);

  /// @brief NORのチェックを行う．
  void
  check_nor(int ni);

  /// @brief XORのチェックを行う．
  void
  check_xor(int ni);

  /// @brief XNORのチェックを行う．
  void
  check_xnor(int ni);

  /// @brief 論理式のチェックを行う．
  void
  check_expr(const Expr& expr);

  /// @brief 真理値表のチェックを行う．
  void
  check_tvfunc(const TvFunc& func);


public:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // ネットワーク
  BnNetwork mNetwork;

  // 変数の数
  int mVarNum;

  // 変数のマップ
  vector<SatLiteral> mVarMap;

  // エンコーダ
  BnNodeEnc mEnc;

};

BnNodeEncTest::BnNodeEncTest() :
  mSolver(GetParam()),
  mVarNum(10),
  mVarMap(mVarNum),
  mEnc(mSolver, mNetwork, mVarMap)
{
}

BnNodeEncTest::~BnNodeEncTest()
{
}

// @brief 初期化を行う．
void
BnNodeEncTest::SetUp()
{
}

// @brief 終了処理を行う．
void
BnNodeEncTest::TearDown()
{
}

// @brief 入力ノードを用意する．
// @param[in] ni 入力数
void
BnNodeEncTest::make_inputs(int ni)
{
  for ( int i: Range(ni) ) {
    int port_id = mNetwork.new_input_port("");
    auto& port = mNetwork.port(port_id);
    int id = port.bit(0);
    EXPECT_EQ( i, id );
    SatLiteral var = mSolver.new_variable();
    mVarMap[i] = var;
  }
}

// @brief チェック対象のノードの変数を用意する．
// @param[in] id ノード番号
void
BnNodeEncTest::make_node_variable(int id)
{
  SatLiteral var = mSolver.new_variable();
  mVarMap[id] = var;
}

// @brief ノードの出力の論理関数が vals[] で示された真理値表と等しいか調べる．
void
BnNodeEncTest::check(int node_id,
		     const vector<int>& vals)
{
  try {
    auto& node = mNetwork.node(node_id);
    int ni = node.fanin_num();
    for ( int i: Range(ni) ) {
      mNetwork.connect(i, node_id, i);
    }

    make_node_variable(node_id);

    mEnc.make_cnf(node);

    int np = 1 << ni;
    EXPECT_EQ( np, vals.size() );
    SatLiteral olit(mVarMap[ni]);
    for ( int p: Range(np) ) {
      for ( int b: {0, 1} ) {
	vector<SatLiteral> assumptions;
	for ( int i: Range(ni) ) {
	  bool inv = (p & (1U << i)) ? false : true;
	  assumptions.push_back(SatLiteral(mVarMap[i], inv));
	}
	if ( b ) {
	  assumptions.push_back(olit);
	}
	else {
	  assumptions.push_back(~olit);
	}
	SatModel model;
	SatBool3 stat = mSolver.solve(assumptions, model);
	SatBool3 exp_stat = ( vals[p] == b ) ? SatBool3::True : SatBool3::False;
	EXPECT_EQ( exp_stat, stat );
      }
    }
  }
  catch ( AssertError x ) {
    cout << x << endl;
  }
}

// @brief ANDのチェックを行う．
void
BnNodeEncTest::check_and(int ni)
{
  make_inputs(ni);

  int oid = mNetwork.new_logic("", BnNodeType::And, ni);
  ASSERT_COND( oid == ni );

  //  2入力の例
  // lit0  lit1
  //  0     0     0
  //  0     1     0
  //  1     0     0
  //  1     1     1

  int np = 1 << ni;
  vector<int> vals(np);
  for ( int p: Range(np) ) {
    if ( p < (1 << ni) - 1) {
      vals[p] = 0;
    }
    else {
      vals[p] = 1;
    }
  }

  check(oid, vals);
}

// @brief NANDのチェックを行う．
void
BnNodeEncTest::check_nand(int ni)
{
  make_inputs(ni);

  int oid = mNetwork.new_logic("", BnNodeType::Nand, ni);
  ASSERT_COND( oid == ni );

  //  2入力の例
  // lit0  lit1
  //  0     0     1
  //  0     1     1
  //  1     0     1
  //  1     1     0
  int np = 1 << ni;
  vector<int> vals(np);
  for ( int p: Range(np) ) {
    if ( p < (1 << ni) - 1 ) {
      vals[p] = 1;
    }
    else {
      vals[p] = 0;
    }
  }

  check(oid, vals);
}

// @brief ORのチェックを行う．
void
BnNodeEncTest::check_or(int ni)
{
  make_inputs(ni);

  int oid = mNetwork.new_logic("", BnNodeType::Or, ni);
  ASSERT_COND( oid == ni );

  //  2入力の例
  // lit0  lit1
  //  0     0     0
  //  0     1     1
  //  1     0     1
  //  1     1     1

  int np = 1 << ni;
  vector<int> vals(np);
  for ( int p: Range(np) ) {
    if ( p == 0 ) {
      vals[p] = 0;
    }
    else {
      vals[p] = 1;
    }
  }

  check(oid, vals);
}

// @brief NORのチェックを行う．
void
BnNodeEncTest::check_nor(int ni)
{
  make_inputs(ni);

  int oid = mNetwork.new_logic("", BnNodeType::Nor, ni);
  ASSERT_COND( oid == ni );

  //  2入力の例
  // lit0  lit1
  //  0     0     1
  //  0     1     0
  //  1     0     0
  //  1     1     0

  int np = 1 << ni;
  vector<int> vals(np);
  for ( int p: Range(np) ) {
    if ( p == 0 ) {
      vals[p] = 1;
    }
    else {
      vals[p] = 0;
    }
  }

  check(oid, vals);
}

// @brief NORのチェックを行う．
void
BnNodeEncTest::check_xor(int ni)
{
  make_inputs(ni);

  int oid = mNetwork.new_logic("", BnNodeType::Xor, ni);
  ASSERT_COND( oid == ni );

  //  2入力の例
  // lit0  lit1
  //  0     0     0
  //  0     1     1
  //  1     0     1
  //  1     1     0

  int np = 1 << ni;
  vector<int> vals(np);
  for ( int p: Range(np) ) {
    int c = 0;
    for ( int i: Range(ni) ) {
      if ( p & (1 << i) ) {
	++ c;
      }
    }
    vals[p] = ((c % 2) == 1);
  }

  check(oid, vals);
}

// @brief NORのチェックを行う．
void
BnNodeEncTest::check_xnor(int ni)
{
  make_inputs(ni);

  int oid = mNetwork.new_logic("", BnNodeType::Xnor, ni);
  ASSERT_COND( oid == ni );

  //  2入力の例
  // lit0  lit1
  //  0     0     1
  //  0     1     0
  //  1     0     0
  //  1     1     1

  int np = 1 << ni;
  vector<int> vals(np);
  for ( int p: Range(np) ) {
    int c = 0;
    for ( int i: Range(ni) ) {
      if ( p & (1 << i) ) {
	++ c;
      }
    }
    vals[p] = ((c % 2) == 0);
  }

  check(oid, vals);
}

// @brief 論理式のチェックを行う．
void
BnNodeEncTest::check_expr(const Expr& expr)
{
  int ni = expr.input_size();
  make_inputs(ni);

  int id = mNetwork.new_logic("", expr);
  for ( int i: Range(ni) ) {
    mNetwork.connect(i, id, i);
  }

  make_node_variable(id);

  auto& node = mNetwork.node(id);

  mEnc.make_cnf(node);

  int np = 1 << ni;
  vector<int> vals(np);
  for ( int p: Range(np) ) {
    vector<Expr::BitVectType> ipat(ni);
    for ( int i: Range(ni) ) {
      if ( p & (1 << i) ) {
	ipat[i] = 1;
      }
      else {
	ipat[i] = 0;
      }
    }
    Expr::BitVectType opat = expr.eval(ipat, 1UL);
    if ( opat ) {
      vals[p] = 1;
    }
    else {
      vals[p] = 0;
    }
  }

  check(id, vals);
}

// @brief 真理値表のチェックを行う．
void
BnNodeEncTest::check_tvfunc(const TvFunc& func)
{
  int ni = func.input_num();
  make_inputs(ni);

  int id = mNetwork.new_logic("", func);
  for ( int i: Range(ni) ) {
    mNetwork.connect(i, id, i);
  }

  make_node_variable(id);

  auto& node = mNetwork.node(id);

  mEnc.make_cnf(node);

  int np = 1 << ni;
  vector<int> vals(np);
  for ( int p: Range(np) ) {
    vals[p] = func.value(p);
  }

  check(id, vals);
}

TEST_P(BnNodeEncTest, zero)
{
  int id = mNetwork.new_logic("", BnNodeType::C0, 0);
  ASSERT_COND( id == 0 );

  //  0
  vector<int> vals({ 0 });

  check(id, vals);
}

TEST_P(BnNodeEncTest, one)
{
  int id = mNetwork.new_logic("", BnNodeType::C1, 0);
  ASSERT_COND( id == 0 );

  //  1
  vector<int> vals({ 1 });

  check(id, vals);
}

TEST_P(BnNodeEncTest, buff)
{
  make_inputs(1);

  int oid = mNetwork.new_logic("", BnNodeType::Buff, 1);
  ASSERT_COND( oid == 1 );

  // lit0
  //  0    0
  //  1    1
  vector<int> vals({ 0, 1 });

  check(oid, vals);
}

TEST_P(BnNodeEncTest, not)
{
  make_inputs(1);

  int oid = mNetwork.new_logic("", BnNodeType::Not, 1);
  ASSERT_COND( oid == 1 );

  // lit0
  //  0     1
  //  1     0
  vector<int> vals({ 1, 0 });

  check(oid, vals);
}

TEST_P(BnNodeEncTest, and2)
{
  check_and(2);
}

TEST_P(BnNodeEncTest, and3)
{
  check_and(3);
}

TEST_P(BnNodeEncTest, and4)
{
  check_and(4);
}

TEST_P(BnNodeEncTest, and5)
{
  check_and(5);
}

TEST_P(BnNodeEncTest, nand2)
{
  check_nand(2);
}

TEST_P(BnNodeEncTest, nand3)
{
  check_nand(3);
}

TEST_P(BnNodeEncTest, nand4)
{
  check_nand(4);
}

TEST_P(BnNodeEncTest, nand5)
{
  check_nand(5);
}

TEST_P(BnNodeEncTest, or2)
{
  check_or(2);
}

TEST_P(BnNodeEncTest, or3)
{
  check_or(3);
}

TEST_P(BnNodeEncTest, or4)
{
  check_or(4);
}

TEST_P(BnNodeEncTest, or5)
{
  check_or(5);
}

TEST_P(BnNodeEncTest, nor2)
{
  check_nor(2);
}

TEST_P(BnNodeEncTest, nor3)
{
  check_nor(3);
}

TEST_P(BnNodeEncTest, nor4)
{
  check_nor(4);
}

TEST_P(BnNodeEncTest, nor5)
{
  check_nor(5);
}

TEST_P(BnNodeEncTest, xor2)
{
  check_xor(2);
}

TEST_P(BnNodeEncTest, xor3)
{
  check_xor(3);
}

TEST_P(BnNodeEncTest, xor4)
{
  check_xor(4);
}

TEST_P(BnNodeEncTest, xor5)
{
  check_xor(5);
}

TEST_P(BnNodeEncTest, xnor2)
{
  check_xnor(2);
}

TEST_P(BnNodeEncTest, xnor3)
{
  check_xnor(3);
}

TEST_P(BnNodeEncTest, xnor4)
{
  check_xnor(4);
}

TEST_P(BnNodeEncTest, xnor5)
{
  check_xnor(5);
}

TEST_P(BnNodeEncTest, expr1)
{
  string err_msg;
  Expr expr = Expr::from_string("0 + 1", err_msg);

  check_expr(expr);
}

TEST_P(BnNodeEncTest, expr2)
{
  string err_msg;
  Expr expr = Expr::from_string("0 + (1 & 2)", err_msg);

  check_expr(expr);
}

TEST_P(BnNodeEncTest, expr3)
{
  string err_msg;
  Expr expr = Expr::from_string("~0 + (1 & 2)", err_msg);

  check_expr(expr);
}

TEST_P(BnNodeEncTest, tvfunc1)
{
  vector<int> values({0, 0, 0, 1, 0, 1, 1, 1});
  TvFunc func(3, values);

  check_tvfunc(func);
}

TEST_P(BnNodeEncTest, tvfunc2)
{
  vector<int> values({0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1});
  TvFunc func(4, values);

  check_tvfunc(func);
}

#if 0
INSTANTIATE_TEST_CASE_P(SatSolverTest,
			BnNodeEncTest,
			::testing::Values("glueminisat2", "minisat2", "ymsat2" ));
#endif

INSTANTIATE_TEST_CASE_P(SatSolverTest,
			BnNodeEncTest,
			::testing::Values( "ymsat2", "ymsat2" ));

END_NAMESPACE_YM
