
/// @file FuncAnalyzerTest.cc
/// @brief FuncAnalyzerTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "FuncAnalyzer.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM_BNET

class FuncAnalyzerTest :
  public ::testing::Test
{
public:

  /// @brief コンストラクタ
  FuncAnalyzerTest() { }

  /// @brief テストを行う．
  /// @param[in] expr 論理式
  /// @param[in] logic_type 期待値
  void
  do_test(const Expr& expr,
	  BnLogicType logic_type);


protected:

};

void
FuncAnalyzerTest::do_test(const Expr& expr,
			  BnLogicType logic_type)
{
  BnLogicType act_logic_type = FuncAnalyzer::analyze(expr);
  EXPECT_EQ( act_logic_type, logic_type );
}

TEST_F(FuncAnalyzerTest, const0)
{
  Expr expr = Expr::make_zero();
  do_test(expr, kBnLt_C0);
}

TEST_F(FuncAnalyzerTest, const1)
{
  Expr expr = Expr::make_one();
  do_test(expr, kBnLt_C1);
}

TEST_F(FuncAnalyzerTest, plit)
{
  Expr expr = Expr::make_posiliteral(VarId(0));
  do_test(expr, kBnLt_BUFF);
}

TEST_F(FuncAnalyzerTest, nlit)
{
  Expr expr = Expr::make_negaliteral(VarId(0));
  do_test(expr, kBnLt_NOT);
}

TEST_F(FuncAnalyzerTest, and2)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr expr = lit0 & lit1;
  do_test(expr, kBnLt_AND);
}

TEST_F(FuncAnalyzerTest, and3)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr expr = lit0 & lit1 & lit2;
  do_test(expr, kBnLt_AND);
}

TEST_F(FuncAnalyzerTest, and4)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr expr = lit0 & lit1 & lit2 & lit3;
  do_test(expr, kBnLt_AND);
}

TEST_F(FuncAnalyzerTest, and5)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr expr = lit0 & lit1 & lit2 & lit3 & lit4;
  do_test(expr, kBnLt_AND);
}

TEST_F(FuncAnalyzerTest, and6)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr expr = lit0 & lit1 & lit2 & lit3 & lit4 & lit5;
  do_test(expr, kBnLt_AND);
}

TEST_F(FuncAnalyzerTest, and7)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr expr = lit0 & lit1 & lit2 & lit3 & lit4 & lit5 & lit6;
  do_test(expr, kBnLt_AND);
}

TEST_F(FuncAnalyzerTest, and8)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr expr = lit0 & lit1 & lit2 & lit3 & lit4 & lit5 & lit6 & lit7;
  do_test(expr, kBnLt_AND);
}

TEST_F(FuncAnalyzerTest, and9)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr expr = lit0 & lit1 & lit2 & lit3 & lit4 & lit5 & lit6 & lit7 & lit8;
  do_test(expr, kBnLt_AND);
}

TEST_F(FuncAnalyzerTest, and10)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr expr = lit0 & lit1 & lit2 & lit3 & lit4 & lit5 & lit6 & lit7 & lit8 & lit9;
  do_test(expr, kBnLt_AND);
}

TEST_F(FuncAnalyzerTest, and11)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr lit10 = Expr::make_posiliteral(VarId(10));
  Expr expr = lit0 & lit1 & lit2 & lit3 & lit4 & lit5 & lit6 & lit7 & lit8 & lit9 & lit10;
  // 11入力は調べずに kBnLt_EXPR を返す．
  do_test(expr, kBnLt_EXPR);
}

TEST_F(FuncAnalyzerTest, nand2)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr expr = ~(lit0 & lit1);
  do_test(expr, kBnLt_NAND);
}

TEST_F(FuncAnalyzerTest, nand3)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr expr = ~(lit0 & lit1 & lit2);
  do_test(expr, kBnLt_NAND);
}

TEST_F(FuncAnalyzerTest, nand4)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr expr = ~(lit0 & lit1 & lit2 & lit3);
  do_test(expr, kBnLt_NAND);
}

TEST_F(FuncAnalyzerTest, nand5)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr expr = ~(lit0 & lit1 & lit2 & lit3 & lit4);
  do_test(expr, kBnLt_NAND);
}

TEST_F(FuncAnalyzerTest, nand6)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr expr = ~(lit0 & lit1 & lit2 & lit3 & lit4 & lit5);
  do_test(expr, kBnLt_NAND);
}

TEST_F(FuncAnalyzerTest, nand7)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr expr = ~(lit0 & lit1 & lit2 & lit3 & lit4 & lit5 & lit6);
  do_test(expr, kBnLt_NAND);
}

TEST_F(FuncAnalyzerTest, nand8)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr expr = ~(lit0 & lit1 & lit2 & lit3 & lit4 & lit5 & lit6 & lit7);
  do_test(expr, kBnLt_NAND);
}

TEST_F(FuncAnalyzerTest, nand9)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr expr = ~(lit0 & lit1 & lit2 & lit3 & lit4 & lit5 & lit6 & lit7 & lit8);
  do_test(expr, kBnLt_NAND);
}

TEST_F(FuncAnalyzerTest, nand10)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr expr = ~(lit0 & lit1 & lit2 & lit3 & lit4 & lit5 & lit6 & lit7 & lit8 & lit9);
  do_test(expr, kBnLt_NAND);
}

TEST_F(FuncAnalyzerTest, nand11)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr lit10 = Expr::make_posiliteral(VarId(10));
  Expr expr = ~(lit0 & lit1 & lit2 & lit3 & lit4 & lit5 & lit6 & lit7 & lit8 & lit9 & lit10);
  // 11入力は調べずに kBnLt_EXPR を返す．
  do_test(expr, kBnLt_EXPR);
}

TEST_F(FuncAnalyzerTest, or2)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr expr = lit0 | lit1;
  do_test(expr, kBnLt_OR);
}

TEST_F(FuncAnalyzerTest, or3)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr expr = lit0 | lit1 | lit2;
  do_test(expr, kBnLt_OR);
}

TEST_F(FuncAnalyzerTest, or4)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr expr = lit0 | lit1 | lit2 | lit3;
  do_test(expr, kBnLt_OR);
}

TEST_F(FuncAnalyzerTest, or5)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr expr = lit0 | lit1 | lit2 | lit3 | lit4;
  do_test(expr, kBnLt_OR);
}

TEST_F(FuncAnalyzerTest, or6)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr expr = lit0 | lit1 | lit2 | lit3 | lit4 | lit5;
  do_test(expr, kBnLt_OR);
}

TEST_F(FuncAnalyzerTest, or7)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr expr = lit0 | lit1 | lit2 | lit3 | lit4 | lit5 | lit6;
  do_test(expr, kBnLt_OR);
}

TEST_F(FuncAnalyzerTest, or8)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr expr = lit0 | lit1 | lit2 | lit3 | lit4 | lit5 | lit6 | lit7;
  do_test(expr, kBnLt_OR);
}

TEST_F(FuncAnalyzerTest, or9)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr expr = lit0 | lit1 | lit2 | lit3 | lit4 | lit5 | lit6 | lit7 | lit8;
  do_test(expr, kBnLt_OR);
}

TEST_F(FuncAnalyzerTest, or10)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr expr = lit0 | lit1 | lit2 | lit3 | lit4 | lit5 | lit6 | lit7 | lit8 | lit9;
  do_test(expr, kBnLt_OR);
}

TEST_F(FuncAnalyzerTest, or11)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr lit10 = Expr::make_posiliteral(VarId(10));
  Expr expr = lit0 | lit1 | lit2 | lit3 | lit4 | lit5 | lit6 | lit7 | lit8 | lit9 | lit10;
  // 11入力は調べずに kBnLt_EXPR を返す．
  do_test(expr, kBnLt_EXPR);
}

TEST_F(FuncAnalyzerTest, nor2)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr expr = ~(lit0 | lit1);
  do_test(expr, kBnLt_NOR);
}

TEST_F(FuncAnalyzerTest, nor3)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr expr = ~(lit0 | lit1 | lit2);
  do_test(expr, kBnLt_NOR);
}

TEST_F(FuncAnalyzerTest, nor4)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr expr = ~(lit0 | lit1 | lit2 | lit3);
  do_test(expr, kBnLt_NOR);
}

TEST_F(FuncAnalyzerTest, nor5)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr expr = ~(lit0 | lit1 | lit2 | lit3 | lit4);
  do_test(expr, kBnLt_NOR);
}

TEST_F(FuncAnalyzerTest, nor6)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr expr = ~(lit0 | lit1 | lit2 | lit3 | lit4 | lit5);
  do_test(expr, kBnLt_NOR);
}

TEST_F(FuncAnalyzerTest, nor7)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr expr = ~(lit0 | lit1 | lit2 | lit3 | lit4 | lit5 | lit6);
  do_test(expr, kBnLt_NOR);
}

TEST_F(FuncAnalyzerTest, nor8)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr expr = ~(lit0 | lit1 | lit2 | lit3 | lit4 | lit5 | lit6 | lit7);
  do_test(expr, kBnLt_NOR);
}

TEST_F(FuncAnalyzerTest, nor9)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr expr = ~(lit0 | lit1 | lit2 | lit3 | lit4 | lit5 | lit6 | lit7 | lit8);
  do_test(expr, kBnLt_NOR);
}

TEST_F(FuncAnalyzerTest, nor10)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr expr = ~(lit0 | lit1 | lit2 | lit3 | lit4 | lit5 | lit6 | lit7 | lit8 | lit9);
  do_test(expr, kBnLt_NOR);
}

TEST_F(FuncAnalyzerTest, nor11)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr lit10 = Expr::make_posiliteral(VarId(10));
  Expr expr = ~(lit0 | lit1 | lit2 | lit3 | lit4 | lit5 | lit6 | lit7 | lit8 | lit9 | lit10);
  // 11入力は調べずに kBnLt_EXPR を返す．
  do_test(expr, kBnLt_EXPR);
}

TEST_F(FuncAnalyzerTest, xor2)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr expr = lit0 ^ lit1;
  do_test(expr, kBnLt_XOR);
}

TEST_F(FuncAnalyzerTest, xor3)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr expr = lit0 ^ lit1 ^ lit2;
  do_test(expr, kBnLt_XOR);
}

TEST_F(FuncAnalyzerTest, xor4)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr expr = lit0 ^ lit1 ^ lit2 ^ lit3;
  do_test(expr, kBnLt_XOR);
}

TEST_F(FuncAnalyzerTest, xor5)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr expr = lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4;
  do_test(expr, kBnLt_XOR);
}

TEST_F(FuncAnalyzerTest, xor6)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr expr = lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5;
  do_test(expr, kBnLt_XOR);
}

TEST_F(FuncAnalyzerTest, xor7)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr expr = lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5 ^ lit6;
  do_test(expr, kBnLt_XOR);
}

TEST_F(FuncAnalyzerTest, xor8)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr expr = lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5 ^ lit6 ^ lit7;
  do_test(expr, kBnLt_XOR);
}

TEST_F(FuncAnalyzerTest, xor9)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr expr = lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5 ^ lit6 ^ lit7 ^ lit8;
  do_test(expr, kBnLt_XOR);
}

TEST_F(FuncAnalyzerTest, xor10)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr expr = lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5 ^ lit6 ^ lit7 ^ lit8 ^ lit9;
  do_test(expr, kBnLt_XOR);
}

TEST_F(FuncAnalyzerTest, xor11)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr lit10 = Expr::make_posiliteral(VarId(10));
  Expr expr = lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5 ^ lit6 ^ lit7 ^ lit8 ^ lit9 ^ lit10;
  // 11入力は調べずに kBnLt_EXPR を返す．
  do_test(expr, kBnLt_EXPR);
}

TEST_F(FuncAnalyzerTest, xnor2)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr expr = ~(lit0 ^ lit1);
  do_test(expr, kBnLt_XNOR);
}

TEST_F(FuncAnalyzerTest, xnor3)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr expr = ~(lit0 ^ lit1 ^ lit2);
  do_test(expr, kBnLt_XNOR);
}

TEST_F(FuncAnalyzerTest, xnor4)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr expr = ~(lit0 ^ lit1 ^ lit2 ^ lit3);
  do_test(expr, kBnLt_XNOR);
}

TEST_F(FuncAnalyzerTest, xnor5)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr expr = ~(lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4);
  do_test(expr, kBnLt_XNOR);
}

TEST_F(FuncAnalyzerTest, xnor6)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr expr = ~(lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5);
  do_test(expr, kBnLt_XNOR);
}

TEST_F(FuncAnalyzerTest, xnor7)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr expr = ~(lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5 ^ lit6);
  do_test(expr, kBnLt_XNOR);
}

TEST_F(FuncAnalyzerTest, xnor8)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr expr = ~(lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5 ^ lit6 ^ lit7);
  do_test(expr, kBnLt_XNOR);
}

TEST_F(FuncAnalyzerTest, xnor9)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr expr = ~(lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5 ^ lit6 ^ lit7 ^ lit8);
  do_test(expr, kBnLt_XNOR);
}

TEST_F(FuncAnalyzerTest, xnor10)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr expr = ~(lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5 ^ lit6 ^ lit7 ^ lit8 ^ lit9);
  do_test(expr, kBnLt_XNOR);
}

TEST_F(FuncAnalyzerTest, xnor11)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr lit4 = Expr::make_posiliteral(VarId(4));
  Expr lit5 = Expr::make_posiliteral(VarId(5));
  Expr lit6 = Expr::make_posiliteral(VarId(6));
  Expr lit7 = Expr::make_posiliteral(VarId(7));
  Expr lit8 = Expr::make_posiliteral(VarId(8));
  Expr lit9 = Expr::make_posiliteral(VarId(9));
  Expr lit10 = Expr::make_posiliteral(VarId(10));
  Expr expr = ~(lit0 ^ lit1 ^ lit2 ^ lit3 ^ lit4 ^ lit5 ^ lit6 ^ lit7 ^ lit8 ^ lit9 ^ lit10);
  // 11入力は調べずに kBnLt_EXPR を返す．
  do_test(expr, kBnLt_EXPR);
}

TEST_F(FuncAnalyzerTest, expr1)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr expr = (~lit0 & ~lit1 & ~lit2) | (lit0 & lit1 & lit2);
  do_test(expr, kBnLt_EXPR);
}

TEST_F(FuncAnalyzerTest, expr2)
{
  Expr lit0 = Expr::make_posiliteral(VarId(0));
  Expr lit1 = Expr::make_posiliteral(VarId(1));
  Expr lit2 = Expr::make_posiliteral(VarId(2));
  Expr lit3 = Expr::make_posiliteral(VarId(3));
  Expr expr = (lit0 & lit1) | (lit2 & lit3);
  do_test(expr, kBnLt_EXPR);
}

END_NAMESPACE_YM_BNET
