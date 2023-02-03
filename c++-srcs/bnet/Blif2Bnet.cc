
/// @file Blif2Bnet.cc
/// @brief Blif2Bnet の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Blif2Bnet.h"
#include "ym/BlifModel.h"
#include "ym/BlifCover.h"
#include "ym/BnNetwork.h"
#include "ym/ClibCellLibrary.h"


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
  BlifModel model;
  bool stat = model.read_blif(filename, cell_library);
  if ( !stat ) {
    ostringstream buf;
    buf << "Error in read_blif(\"" << filename << "\"";
    throw std::invalid_argument{buf.str()};
  }

  auto _clock_name = clock_name;
  if ( _clock_name == string{} ) {
    _clock_name = "clock";
  }
  auto _reset_name = reset_name;
  if ( _reset_name == string{} ) {
    _reset_name = "reset";
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

  for ( SizeType src_id: model.input_list() ) {
    make_input(src_id);
  }
  for ( SizeType src_id: model.dff_list() ) {
    make_dff(src_id);
  }
  for ( SizeType src_id: model.logic_list() ) {
    make_logic(src_id);
  }
  for ( auto src_id: model.output_list() ) {
    set_output(src_id);
  }
  for ( auto& p: mOutputMap ) {
    auto id = p.first;
    auto src_id = p.second;
    ASSERT_COND( mIdMap.count(src_id) > 0 );
    auto inode_id = mIdMap.at(src_id);
    mNetwork.set_output_src(id, inode_id);
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
  auto oname = mModel.node_name(src_id);
  auto port_id = mNetwork.new_input_port(oname);
  const auto& port = mNetwork.port(port_id);
  auto id = port.bit(0);
  mIdMap.emplace(src_id, id);
}

// @brief 出力を設定する．
void
Blif2Bnet::set_output(
  SizeType src_id
)
{
  auto name = mModel.node_name(src_id);
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

// @brief DFFを作る．
void
Blif2Bnet::make_dff(
  SizeType src_id
)
{
  auto oname = mModel.node_name(src_id);
  char rval = mModel.node_rval(src_id);
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
  mIdMap.emplace(src_id, output_id);

  auto input_id = dff.data_in();
  // 本当の入力ノードはできていないのでファンイン情報を記録しておく．
  auto inode_id = mModel.node_input(src_id);
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

// @brief 論理ノードを生成する．
void
Blif2Bnet::make_logic(
  SizeType src_id
)
{
  auto oname = mModel.node_name(src_id);

  // ファンインのノードを作る．
  SizeType ni = mModel.node_fanin_list(src_id).size();
  vector<SizeType> fanin_id_list;
  fanin_id_list.reserve(ni);
  for ( auto iid: mModel.node_fanin_list(src_id) ) {
    ASSERT_COND( mIdMap.count(iid) > 0 );
    fanin_id_list.push_back(mIdMap.at(iid));
  }

  auto type = mModel.node_type(src_id);
  SizeType id;
  if ( type == BlifType::Cover ) {
    auto cover_id = mModel.node_cover_id(src_id);
    auto& cover = mModel.cover(cover_id);
    auto expr = cover.expr();
    id = mNetwork.new_logic_expr(oname, expr, fanin_id_list);
  }
  else if ( type == BlifType::Cell ) {
    auto cell_id = mModel.node_cell_id(src_id);
    id = mNetwork.new_logic_cell(oname, cell_id, fanin_id_list);
  }
  else {
    ASSERT_NOT_REACHED;
  }
  mIdMap.emplace(src_id, id);
}

END_NAMESPACE_YM_BNET
