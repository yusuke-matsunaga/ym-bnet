
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

END_NAMESPACE_YM_BNET
