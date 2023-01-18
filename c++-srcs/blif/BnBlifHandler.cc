
/// @file BnBlifHandler.cc
/// @brief BnBlifHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "BnBlifHandler.h"
#include "BlifParser.h"
#include "BlifCover.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"
#include "ym/BnNodeList.h"
#include "ym/ClibCellLibrary.h"
#include "ym/ClibCell.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief blif ファイルを読み込む．
BnNetwork
BnNetwork::read_blif(
  const string& filename,
  const string& clock_name,
  const string& reset_name
)
{
  return read_blif(filename, ClibCellLibrary{}, clock_name, reset_name);
}

// @brief blif ファイルを読み込む．
BnNetwork
BnNetwork::read_blif(
  const string& filename,
  const ClibCellLibrary& cell_library,
  const string& clock_name,
  const string& reset_name
)
{
  BlifParser parser;
  string _clock_name{clock_name};
  if ( _clock_name == string{} ) {
    _clock_name = "clock";
  }
  string _reset_name{reset_name};
  if ( _reset_name == string{} ) {
    _reset_name = "reset";
  }
  BnBlifHandler handler{parser, _clock_name, _reset_name};

  bool stat = parser.read(filename, cell_library);
  if ( !stat ) {
    ostringstream buf;
    buf << "Error in read_blif(\"" << filename << "\"";
    throw std::invalid_argument{buf.str()};
  }

  return handler.get_network();
}


//////////////////////////////////////////////////////////////////////
// クラス BnBlifHandler
//////////////////////////////////////////////////////////////////////

// @brief 結果のネットワークを返す．
BnNetwork
BnBlifHandler::get_network()
{
  return BnNetwork{std::move(mNetwork)};
}

// @brief 初期化
bool
BnBlifHandler::init()
{
  mNetwork.clear();

  mIdMap.clear();
  mNodeMap.clear();
  mOutputMap.clear();

  mClockId = BNET_NULLID;
  mResetId = BNET_NULLID;

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
  SizeType name_id,
  const string& name
)
{
  auto port_id = mNetwork.new_input_port(name);
  const auto& port = mNetwork.port(port_id);
  auto id = port.bit(0);
  mIdMap.emplace(name_id, id);

  return true;
}

// @brief .output 文の読み込み
bool
BnBlifHandler::outputs_elem(
  SizeType name_id,
  const string& name
)
{
  string name1;
  if ( mNetwork.find_port(name) == static_cast<SizeType>(-1) ) {
    name1 = name;
  }
  auto port_id = mNetwork.new_output_port(name1);
  const auto& port = mNetwork.port(port_id);
  auto id = port.bit(0);
  mOutputMap.emplace(id, name_id);

  return true;
}

BEGIN_NONAMESPACE

Expr
cover2expr(
  const BlifCover& cover
)
{
  auto input_num = cover.input_num();
  auto cube_num = cover.cube_num();
  vector<Expr> prod_list;
  prod_list.reserve(cube_num);
  for ( SizeType c = 0; c < cube_num; ++ c ) {
    vector<Expr> litexpr_list;
    litexpr_list.reserve(input_num);
    for ( SizeType var = 0; var < input_num; ++ var ) {
      switch ( cover.input_pat(c, var) ) {
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
  SizeType oname_id,
  const string& oname,
  const vector<SizeType>& inode_id_array,
  SizeType cover_id
)
{
  auto& cover = id2cover(cover_id);
  auto expr = cover2expr(cover);
  ASSERT_COND( inode_id_array.size() == expr.input_size() );

  mNodeMap.emplace(oname_id, NodeInfo{oname, false, expr, 0, inode_id_array});

  return true;
}

// @brief .gate 文の処理
bool
BnBlifHandler::gate(
  SizeType oname_id,
  const string& oname,
  const vector<SizeType>& inode_id_array,
  SizeType cell_id
)
{
  auto ni = inode_id_array.size();
  const ClibCell& cell = mNetwork.library().cell(cell_id);
  ASSERT_COND( cell.input_num() == ni );
  ASSERT_COND( cell.output_num() == 1 );
  ASSERT_COND( cell.inout_num() == 0 );

  mNodeMap.emplace(oname_id, NodeInfo{oname, true,
				      Expr::make_invalid(), cell_id,
				      inode_id_array});

  return true;
}

// @brief .latch 文の処理
bool
BnBlifHandler::latch(
  SizeType oname_id,
  const string& oname,
  SizeType iname_id,
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
  auto dff_id = mNetwork.new_dff(oname, has_clear, has_preset);
  const auto& dff = mNetwork.dff(dff_id);

  auto output_id = dff.data_out();
  mIdMap[oname_id] = output_id;

  auto input_id = dff.data_in();
  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  mOutputMap.emplace(input_id, iname_id);

  if ( mClockId == BNET_NULLID ) {
    // クロックのポートを作る．
    auto port_id = mNetwork.new_input_port(mClockName);
    const auto& clock_port = mNetwork.port(port_id);
    // クロックの入力ノード番号を記録する．
    mClockId = clock_port.bit(0);
  }

  // クロック入力とdffのクロック端子を結びつける．
  mNetwork.set_output_src(dff.clock(), mClockId);

  if ( has_clear || has_preset ) {
    if ( mResetId == BNET_NULLID ) {
      // リセット端子のポートを作る．
      auto port_id = mNetwork.new_input_port(mResetName);
      const auto& reset_port = mNetwork.port(port_id);
      // リセット端子の入力ノードを記録する．
      mResetId = reset_port.bit(0);
    }
  }
  if ( has_clear ) {
    // リセット入力とクリア端子を結びつける．
    mNetwork.set_output_src(dff.clear(), mResetId);
  }
  else if ( has_preset ) {
    // リセット入力とプリセット端子を結びつける．
    mNetwork.set_output_src(dff.preset(), mResetId);
  }

  return true;
}

// @brief 終了処理
bool
BnBlifHandler::end(
  const FileRegion& loc
)
{
  // 出力ノードのファンインをセットする．
  // 入力側のノードはドミノ式に生成される．
  for ( auto& node: mNetwork.output_list() ) {
    if ( node.is_clock() || node.is_clear() || node.is_preset() ) {
      continue;
    }
    ASSERT_COND ( node.output_src() == BNET_NULLID );
    auto id = node.id();
    ASSERT_COND( mOutputMap.count(id) > 0 );
    auto name_id = mOutputMap.at(id);
    auto inode_id = make_node(name_id);
    mNetwork.set_output_src(id, inode_id);
  }

  return true;
}

// @brief name_id のノードを生成する．
SizeType
BnBlifHandler::make_node(
  SizeType name_id
)
{
  if ( mIdMap.count(name_id) > 0 ) {
    return mIdMap.at(name_id);
  }
  if ( mNodeMap.count(name_id) > 0 ) {
    auto& node_info = mNodeMap.at(name_id);
    // ファンインのノードを作る．
    SizeType ni = node_info.iname_id_array.size();
    vector<SizeType> fanin_id_list(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      fanin_id_list[i] = make_node(node_info.iname_id_array[i]);
    }
    SizeType id;
    if ( node_info.has_cell_id ) {
      id = mNetwork.new_logic_cell(node_info.oname, node_info.cell_id,
				   fanin_id_list);
    }
    else {
      id = mNetwork.new_logic_expr(node_info.oname, node_info.expr,
				   fanin_id_list);
    }
    mIdMap.emplace(name_id, id);
    return id;
  }
  ASSERT_NOT_REACHED;
  return BNET_NULLID;
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
