
/// @file BnBlifHandler.cc
/// @brief BnBlifHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnBlifHandler.h"
#include "ym/BlifParser.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"
#include "ym/BlifCover.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief blif ファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] clock_name クロック端子名
// @param[in] reset_name リセット端子名
// @return ネットワークを返す．
BnNetwork
BnNetwork::read_blif(const string& filename,
		     const string& clock_name,
		     const string& reset_name)
{
  return read_blif(filename, ClibCellLibrary(), clock_name, reset_name);
}

// @brief blif ファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @param[in] clock_name クロック端子名
// @param[in] reset_name リセット端子名
// @return ネットワークを返す．
BnNetwork
BnNetwork::read_blif(const string& filename,
		     const ClibCellLibrary& cell_library,
		     const string& clock_name,
		     const string& reset_name)
{
  BnNetwork network;

  BlifParser parser;
  BnBlifHandler handler(parser, network, clock_name, reset_name);

  bool stat = parser.read(filename, cell_library);
  if ( !stat ) {
    network.clear();
  }

  return network;
}


//////////////////////////////////////////////////////////////////////
// クラス BnBlifHandler
//////////////////////////////////////////////////////////////////////

// @brief 初期化
bool
BnBlifHandler::init()
{
  mNetwork.clear();

  mIdMap.clear();
  mFaninInfoMap.clear();

  mClockId = kBnNullId;
  mResetId = kBnNullId;

  return true;
}

// @brief セルライブラリの設定
// @param[in] library セルライブラリ
//
// この関数が呼ばれないこともある．
void
BnBlifHandler::set_cell_library(
  const ClibCellLibrary& library
)
{
  mNetwork.set_library(library);
}

// @brief .model 文の読み込み
bool
BnBlifHandler::model(
  const FileRegion& loc1,
  const FileRegion& loc2,
  const string& name
)
{
  mNetwork.set_name(name);

  return true;
}

// @brief .input 文の読み込み
bool
BnBlifHandler::inputs_elem(
  int name_id,
  const string& name
)
{
  auto port_id = mNetwork.new_input_port(name);
  const auto& port = mNetwork.port(port_id);
  int id = port.bit(0);
  mIdMap[name_id] = id;

  return true;
}

// @brief .output 文の読み込み
bool
BnBlifHandler::outputs_elem(
  int name_id,
  const string& name
)
{
  auto port_id = mNetwork.new_output_port(name);
  const auto& port = mNetwork.port(port_id);
  int id = port.bit(0);
  mFaninInfoMap[id] = vector<int>({name_id});

  return true;
}

BEGIN_NONAMESPACE

Expr
cover2expr(
  const BlifCover& cover
)
{
  int input_num = cover.input_num();
  int cube_num = cover.cube_num();
  vector<Expr> prod_list;
  prod_list.reserve(cube_num);
  for ( int c = 0; c < cube_num; ++ c ) {
    vector<Expr> litexpr_list;
    litexpr_list.reserve(input_num);
    for ( int i = 0; i < input_num; ++ i ) {
      VarId var(i);
      switch ( cover.input_pat(c, i) ) {
      case '0':
	litexpr_list.push_back(Expr::make_nega_literal(var));
	break;

      case '1':
	litexpr_list.push_back(Expr::make_posi_literal(var));
	break;

      case '-':
	break;

      default:
	ASSERT_NOT_REACHED;
      }
    }
    prod_list.push_back(Expr::make_and(litexpr_list));
  }
  Expr expr = Expr::make_or(prod_list);
  char opat = cover.output_pat();
  if ( opat == '0' ) {
    expr = ~expr;
  }

  return expr;
}

END_NONAMESPACE

// @brief .names 文の処理
bool
BnBlifHandler::names(
  int oname_id,
  const string& oname,
  const vector<int>& inode_id_array,
  int cover_id
)
{
  auto& cover = id2cover(cover_id);
  auto expr = cover2expr(cover);
  ASSERT_COND( inode_id_array.size() == expr.input_size() );

  int node_id = mNetwork.new_logic(oname, expr);
  mIdMap[oname_id] = node_id;

  mFaninInfoMap[node_id] = inode_id_array;

  return true;
}

// @brief .gate 文の処理
bool
BnBlifHandler::gate(
  int oname_id,
  const string& oname,
  const vector<int>& inode_id_array,
  int cell_id
)
{
  int ni = inode_id_array.size();
  const ClibCell& cell = mNetwork.library().cell(cell_id);
  ASSERT_COND( ni == cell.input_num() );
  int node_id = mNetwork.new_logic(oname, cell_id);
  mIdMap[oname_id] = node_id;

  mFaninInfoMap[node_id] = inode_id_array;

  return true;
}

// @brief .latch 文の処理
bool
BnBlifHandler::latch(
  int oname_id,
  const string& oname,
  int iname_id,
  const FileRegion& loc4,
  char rval
)
{
  bool has_clear = false;
  bool has_preset = false;
  if ( rval == '0' ) {
    has_clear = true;
  }
  else if ( rval == '1' ) {
    has_preset = true;
  }
  bool has_xoutput = false;
  int dff_id = mNetwork.new_dff(oname, has_xoutput, has_clear, has_preset);
  const auto& dff = mNetwork.dff(dff_id);

  int output_id = dff.output();
  mIdMap[oname_id] = output_id;

  int input_id = dff.input();
  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  mFaninInfoMap[input_id] = vector<int>{iname_id};

  if ( mClockId == kBnNullId ) {
    // クロックのポートを作る．
    auto port_id = mNetwork.new_input_port(mClockName);
    const auto& clock_port = mNetwork.port(port_id);
    // クロックの入力ノード番号を記録する．
    mClockId = clock_port.bit(0);
  }

  // クロック入力とdffのクロック端子を結びつける．
  int clock_id = dff.clock();
  mNetwork.connect(mClockId, clock_id, 0);

  if ( has_clear || has_preset ) {
    if ( mResetId == kBnNullId ) {
      // リセット端子のポートを作る．
      auto port_id = mNetwork.new_input_port(mResetName);
      const auto& reset_port = mNetwork.port(port_id);
      // リセット端子の入力ノードを記録する．
      mResetId = reset_port.bit(0);
    }
  }
  if ( has_clear ) {
    // リセット入力とクリア端子を結びつける．
    int clear_id = dff.clear();
    mNetwork.connect(mResetId, clear_id, 0);
  }
  else if ( has_preset ) {
    // リセット入力とプリセット端子を結びつける．
    int preset_id = dff.preset();
    mNetwork.connect(mResetId, preset_id, 0);
  }

  return true;
}

// @brief 終了処理
bool
BnBlifHandler::end(
  const FileRegion& loc
)
{
  // ノードのファンインを設定する．
  for ( int node_id = 1; node_id <= mNetwork.node_num(); ++ node_id ) {
    if ( mFaninInfoMap.count(node_id) == 0 ) {
      continue;
    }
    const auto& fanin_info = mFaninInfoMap.at(node_id);

    const BnNode& node = mNetwork.node(node_id);
    if ( node.is_logic() ) {
      int ni = fanin_info.size();
      for ( int i: Range(ni) ) {
	int iname_id = fanin_info[i];
	ASSERT_COND( mIdMap.count(iname_id) > 0 );
	int inode_id = mIdMap.at(iname_id);
	mNetwork.connect(inode_id, node_id, i);
      }
    }
    else if ( node.is_output() ) {
      int iname_id = fanin_info[0];
      ASSERT_COND( mIdMap.count(iname_id) > 0 );
      int inode_id = mIdMap.at(iname_id);
      mNetwork.connect(inode_id, node_id, 0);
    }
  }

  bool stat = mNetwork.wrap_up();

  return stat;
}

// @brief 通常終了時の処理
void
BnBlifHandler::normal_exit()
{
}

// @brief エラー終了時の処理
void
BnBlifHandler::error_exit()
{
  mNetwork.clear();
}

END_NAMESPACE_YM_BNET
