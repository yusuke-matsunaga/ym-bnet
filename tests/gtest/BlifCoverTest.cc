
/// @file BlifCoverTest.cc
/// @brief BlifCoverTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "ym/BlifCover.h"
#include "ym/SopCover.h"


BEGIN_NAMESPACE_YM

TEST(BlifCoverTest, null_cover_test1)
{
  auto cov = BlifCover{0, {}};

  EXPECT_EQ( 0, cov.input_num() );
  EXPECT_EQ( 0, cov.cube_num() );
  EXPECT_EQ( '1', cov.output_pat() );

  auto& sop = cov.input_cover();
  EXPECT_EQ( 0, sop.variable_num() );
  EXPECT_EQ( 0, sop.cube_num() );

  auto expr = cov.expr();
  auto rep_str = expr.rep_string();
  EXPECT_EQ( "C0", rep_str );
}

TEST(BlifCoverTest, null_cover_test2)
{
  auto cov = BlifCover{0, {}, '0'};

  EXPECT_EQ( 0, cov.input_num() );
  EXPECT_EQ( 0, cov.cube_num() );
  EXPECT_EQ( '0', cov.output_pat() );

  auto sop = cov.input_cover();
  EXPECT_EQ( 0, sop.variable_num() );
  EXPECT_EQ( 0, sop.cube_num() );

  auto expr = cov.expr();
  auto rep_str = expr.rep_string();
  EXPECT_EQ( "C1", rep_str );
}

TEST(BlifCoverTest, null_cube_test1)
{
  auto cov = BlifCover{0, {{}}};

  EXPECT_EQ( 0, cov.input_num() );
  EXPECT_EQ( 1, cov.cube_num() );
  EXPECT_EQ( '1', cov.output_pat() );

  auto sop = cov.input_cover();
  EXPECT_EQ( 0, sop.variable_num() );
  EXPECT_EQ( 1, sop.cube_num() );

  auto expr = cov.expr();
  auto rep_str = expr.rep_string();
  EXPECT_EQ( "C1", rep_str );
}

TEST(BlifCoverTest, null_cube_test2)
{
  auto cov = BlifCover{0, {{}}, '0'};

  EXPECT_EQ( 0, cov.input_num() );
  EXPECT_EQ( 1, cov.cube_num() );
  EXPECT_EQ( '0', cov.output_pat() );

  auto sop = cov.input_cover();
  EXPECT_EQ( 0, sop.variable_num() );
  EXPECT_EQ( 1, sop.cube_num() );

  auto expr = cov.expr();
  auto rep_str = expr.rep_string();
  EXPECT_EQ( "C0", rep_str );
}

TEST(BlifCoverTest, test3)
{
  Literal lit0{0, false};
  auto cov = BlifCover{1, {{lit0}}, '1'};

  auto sop = cov.input_cover();
  EXPECT_EQ( 1, sop.variable_num() );
  EXPECT_EQ( 1, sop.cube_num() );
  EXPECT_EQ( SopPat::_1, sop.get_pat(0, 0) );

  EXPECT_EQ( '1', cov.output_pat() );

  auto expr = cov.expr();
  auto rep_str = expr.rep_string();
  EXPECT_EQ( "P0", rep_str );
}

TEST(BlifCoverTest, test4)
{
  Literal lit0{0, false};
  Literal lit1{1, false};
  auto cov = BlifCover{2, {{lit0, lit1}}, '0'};

  auto sop = cov.input_cover();
  ASSERT_EQ( 2, sop.variable_num() );
  ASSERT_EQ( 1, sop.cube_num() );
  EXPECT_EQ( SopPat::_1, sop.get_pat(0, 0) );
  EXPECT_EQ( SopPat::_1, sop.get_pat(0, 1) );

  EXPECT_EQ( '0', cov.output_pat() );

  auto expr = cov.expr();
  auto rep_str = expr.rep_string();
  EXPECT_EQ( "O2N0N1", rep_str );
}

TEST(BlifCoverTest, test5)
{
  Literal lit0{0, false};
  Literal lit1{1, false};
  Literal lit2{2, false};
  auto cov = BlifCover{3, {{~lit0}, {~lit1}, {~lit2}}, '1'};

  auto sop = cov.input_cover();
  ASSERT_EQ( 3, sop.variable_num() );
  ASSERT_EQ( 3, sop.cube_num() );
  EXPECT_EQ( SopPat::_0, sop.get_pat(0, 0) );
  EXPECT_EQ( SopPat::_X, sop.get_pat(0, 1) );
  EXPECT_EQ( SopPat::_X, sop.get_pat(0, 2) );
  EXPECT_EQ( SopPat::_X, sop.get_pat(1, 0) );
  EXPECT_EQ( SopPat::_0, sop.get_pat(1, 1) );
  EXPECT_EQ( SopPat::_X, sop.get_pat(1, 2) );
  EXPECT_EQ( SopPat::_X, sop.get_pat(2, 0) );
  EXPECT_EQ( SopPat::_X, sop.get_pat(2, 1) );
  EXPECT_EQ( SopPat::_0, sop.get_pat(2, 2) );

  EXPECT_EQ( '1', cov.output_pat() );

  auto expr = cov.expr();
  auto rep_str = expr.rep_string();
  EXPECT_EQ( "O3N0N1N2", rep_str );
}

END_NAMESPACE_YM
