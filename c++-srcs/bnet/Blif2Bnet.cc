
/// @file Blif2Bnet.cc
/// @brief Blif2Bnet の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Blif2Bnet.h"
#include "ym/BnNetwork.h"
#include "ym/BlifParser.h"
#include "ym/BlifModel.h"
#include "ym/BlifNode.h"
#include "ym/BlifCover.h"
//#include "ym/BnPort.h"
//#include "ym/BnDff.h"
//#include "ym/BnNode.h"
//#include "ym/BnNodeList.h"
#include "ym/ClibCellLibrary.h"
//#include "ym/ClibCell.h"


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
  auto _clock_name = clock_name;
  if ( _clock_name == string{} ) {
    _clock_name = "clock";
  }
  auto _reset_name = reset_name;
  if ( _reset_name == string{} ) {
    _reset_name = "reset";
  }

  BlifModel model;
  bool stat = parser.read(filename, cell_library, model);
  if ( !stat ) {
    ostringstream buf;
    buf << "Error in read_blif(\"" << filename << "\"";
    throw std::invalid_argument{buf.str()};
  }

  Blif2Bnet conv{model, _clock_name, _reset_name};

  return conv.get_network();
}


//////////////////////////////////////////////////////////////////////
// クラス Blif2Bnet
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Blif2Bnet::Blif2Bnet(
  const BlifModel& model,
  const string& clock_name,
  const string& reset_name
) : mModel{model},
    mClockName{clock_name},
    mResetName{reset_name}
{
  mClockId = BNET_NULLID;
  mResetId = BNET_NULLID;

  mNetwork.set_name(model.name());

  SizeType ni = model.input_num();
  for ( SizeType src_id = 0; src_id < ni; ++ src_id ) {
    make_input(src_id);
  }
  SizeType nl = model.latch_num();
  for ( SizeType src_id = ni; src_id < (ni + nl); ++ src_id ) {
    make_latch(src_id);
  }
  SizeType nn = model.node_num();
  for ( SizeType src_id = (ni + nl); src_id < nn; ++ src_id ) {
    make_node(src_id);
  }
  for ( auto src_id: model.output_list() ) {
    make_output(src_id);
  }
}

// @brief 結果のネットワークを返す．
BnNetwork
Blif2Bnet::get_network()
{
  return BnNetwork{std::move(mNetwork)};
}

// @brief 入力を作る．
void
Blif2Bnet::make_input(
  SizeType src_id
)
{
  auto node = mModel.node(src_id);
  ASSERT_COND( node->type() == BlifNode::Input );
  auto oname = node->name();
  auto port_id = mNetwork.new_input_port(oname);
  const auto& port = mNetwork.port(port_id);
  auto id = port.bit(0);
  mIdMap.emplace(src_id, id);
}

// @brief 出力を作る．
void
Blif2Bnet::make_output(
  SizeType src_id
)
{
  auto node = mModel.node(src_id);
  auto name = node->name();
  string name1;
  if ( mNetwork.find_port(name) == static_cast<SizeType>(-1) ) {
    name1 = name;
  }
  auto port_id = mNetwork.new_output_port(name1);
  const auto& port = mNetwork.port(port_id);
  auto id = port.bit(0);
  ASSERT_COND( mIdMap.count(src_id) > 0 );
  auto inode_id = mIdMap.at(src_id);
  mNetwork.set_output_src(id, inode_id);
}

// @brief ラッチを作る．
void
Blif2Bnet::make_latch(
  SizeType src_id
)
{
  auto node = mModel.node(src_id);
  ASSERT_COND( node->type() == BlifNode::Latch );
  auto oname = node->name();
  char rval = node->rval();
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
  mIdMap[src_id] = output_id;

  auto input_id = dff.data_in();
  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  auto inode_id = node->inode();
  mOutputMap.emplace(input_id, inode_id);

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

// @brief name_id のノードを生成する．
void
Blif2Bnet::make_node(
  SizeType src_id
)
{
  auto node = mModel.node(src_id);
  auto oname = node->name();

  // ファンインのノードを作る．
  SizeType ni = node->inode_list().size();
  vector<SizeType> fanin_id_list(ni);
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto iid = node->inode_list()[i];
    ASSERT_COND( mIdMap.count(iid) > 0 );
    fanin_id_list[i] = mIdMap.at(iid);
  }

  auto type = node->type();
  SizeType id;
  if ( type == BlifNode::Names ) {
    auto cover_id = node->cover_id();
    auto& cover = mModel.cover(cover_id);
    auto expr = cover2expr(cover);
    id = mNetwork.new_logic_expr(oname, expr, fanin_id_list);
  }
  else if ( type == BlifNode::Gate ) {
    auto cell_id = node->cell_id();
    id = mNetwork.new_logic_cell(oname, cell_id, fanin_id_list);
  }
  else {
    ASSERT_NOT_REACHED;
  }
  mIdMap.emplace(src_id, id);
}

END_NAMESPACE_YM_BNET
