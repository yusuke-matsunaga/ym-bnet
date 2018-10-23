
/// @file BnNetwork.cc
/// @brief BnNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNetwork.h"
#include "ym/BnNode.h"
#include "ym/BnNodeType.h"
#include "ym/Range.h"

#include "BnNetworkImpl.h"

BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
//
// 空の状態で初期化される．
BnNetwork::BnNetwork() :
  mImpl(new BnNetworkImpl())
{
}

// @brief コピーコンストラクタ
// @param[in] src コピー元のオブジェクト
BnNetwork::BnNetwork(const BnNetwork& src) :
  mImpl(new BnNetworkImpl())
{
  mImpl->copy(*src.mImpl);
}

// @brief ムーブコンストラクタ
// @param[in] src ムーブ元のオブジェクト
BnNetwork::BnNetwork(BnNetwork&& src) :
  mImpl(std::move(src.mImpl))
{
}

// @brief コピー代入演算子
// @param[in] src コピー元のオブジェクト
BnNetwork&
BnNetwork::operator=(const BnNetwork& src)
{
  mImpl->copy(*src.mImpl);
  return *this;
}

// @brief ムーブ代入演算子
// @param[in] src ムーブ元のオブジェクト
BnNetwork&
BnNetwork::operator=(BnNetwork&& src)
{
  mImpl = std::move(src.mImpl);
  return *this;
}

// @brief デストラクタ
BnNetwork::~BnNetwork()
{
  clear();
}

// @brief 内容をクリアする．
//
// コンストラクタ直後と同じ状態になる．
void
BnNetwork::clear()
{
  mImpl->clear();
}

// @brief 内容をコピーする．
// @param[in] src コピー元のオブジェクト
void
BnNetwork::copy(const BnNetwork& src)
{
  if ( &src == this ) {
    // 自分自身がソースならなにもしない．
    return;
  }

  mImpl->copy(*src.mImpl);
}

// @brief 内容をムーブする．
// @param[in] src ムーブ元のオブジェクト
//
// src は破壊される．
void
BnNetwork::move(BnNetwork&& src)
{
  mImpl = std::move(src.mImpl);
}

// @brief セルライブラリをセットする．
// @param[in] library ライブラリ
void
BnNetwork::set_library(const ClibCellLibrary& library)
{
  mImpl->set_library(library);
}

// @brief ネットワーク名を設定する．
// @param[in] name ネットワーク名
void
BnNetwork::set_name(const string& name)
{
  mImpl->set_name(name);
}

// @brief 1ビットの入力ポートを作る．
// @param[in] port_name ポート名
// @return 生成したポート番号を返す．
int
BnNetwork::new_input_port(const string& port_name)
{
  return mImpl->new_port(port_name, vector<int>({0}));
}

// @brief 多ビットの入力ポートを作る．
// @param[in] port_name ポート名
// @param[in] bit_width ビット幅
// @return 生成したポート番号を返す．
int
BnNetwork::new_input_port(const string& port_name,
			  int bit_width)
{
  return mImpl->new_port(port_name, vector<int>(bit_width, 0));
}

// @brief 1ビットの出力ポートを作る．
// @param[in] port_name ポート名
// @return 生成したポート番号を返す．
int
BnNetwork::new_output_port(const string& port_name)
{
  return mImpl->new_port(port_name, vector<int>({1}));
}

// @brief 多ビットの出力ポートを作る．
// @param[in] port_name ポート名
// @param[in] bit_width ビット幅
// @return 生成したポート番号を返す．
int
BnNetwork::new_output_port(const string& port_name,
			   int bit_width)
{
  return mImpl->new_port(port_name, vector<int>(bit_width, 1));
}

// @brief 入出力混合のポートを作る．
// @param[in] port_name ポート名
// @param[in] dir_vect 向きを表すベクタ
// @return 生成したポート番号を返す．
//
// dir_vect[i] == 0 の時，入力を表す．
int
BnNetwork::new_port(const string& port_name,
		    const vector<int>& dir_vect)
{
  return mImpl->new_port(port_name, dir_vect);
}

// @brief DFFを追加する．
// @param[in] name DFF名
// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
// @param[in] has_clear クリア端子を持つ時 true にする．
// @param[in] has_preset プリセット端子を持つ時 true にする．
// @return 生成したDFF番号を返す．
//
// 名前の重複に関しては感知しない．
int
BnNetwork::new_dff(const string& name,
		   bool has_xoutput,
		   bool has_clear,
		   bool has_preset)
{
  return mImpl->new_dff(name, has_xoutput, has_clear, has_preset);
}

// @brief セルの情報を持ったDFFを追加する．
// @param[in] name DFF名
// @param[in] cell_name 対応するセル名
// @return 生成したDFF番号を返す．
//
// - 名前の重複に関しては感知しない．
// - セルは FF のセルでなければならない．
int
BnNetwork::new_dff(const string& name,
		   const string& cell_name)
{
  return mImpl->new_dff(name, cell_name);
}

// @brief ラッチを追加する．
// @param[in] name ラッチ名
// @param[in] has_clear クリア端子を持つ時 true にする．
// @param[in] has_preset プリセット端子を持つ時 true にする．
// @return 生成したラッチ番号を返す．
//
// 名前の重複に関しては感知しない．
int
BnNetwork::new_latch(const string& name,
		     bool has_xoutput,
		     bool has_clear,
		     bool has_preset)
{
  return mImpl->new_latch(name, has_xoutput, has_clear, has_preset);
}

// @brief セルの情報を持ったラッチを追加する．
// @param[in] name ラッチ名
// @param[in] cell_name 対応するセル名．
// @return 生成したラッチ番号を返す．
//
// - 名前の重複に関しては感知しない．
// - セルはラッチのセルでなければならない．
int
BnNetwork::new_latch(const string& name,
		     const string& cell_name)
{
  return mImpl->new_latch(name, cell_name);
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] logic_type 論理型
// @param[in] ni 入力数
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
int
BnNetwork::new_logic(const string& node_name,
		     BnNodeType logic_type,
		     int ni)
{
  int id = mImpl->new_primitive(node_name, ni, logic_type);
  return id;
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] logic_type 論理型
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
int
BnNetwork::new_logic(const string& node_name,
		     BnNodeType logic_type,
		     const vector<int>& fanin_id_list)
{
  int ni = fanin_id_list.size();
  int id = mImpl->new_primitive(node_name, ni, logic_type);
  if ( id != kBnNullId ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief C0型(定数０)の論理ノードを追加する．
// @param[in] node_name ノード名
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_c0(const string& node_name)
{
  int id = mImpl->new_primitive(node_name, 1, BnNodeType::C0);
  return id;
}

// @brief C1型(定数1)の論理ノードを追加する．
// @param[in] node_name ノード名
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_c1(const string& node_name)
{
  int id = mImpl->new_primitive(node_name, 1, BnNodeType::C1);
  return id;
}

// @brief Buff型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] fanin_id ファンインのノード番号
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_buff(const string& node_name,
		    int fanin_id)
{
  int id = mImpl->new_primitive(node_name, 1, BnNodeType::Buff);
  if ( fanin_id != kBnNullId ) {
    connect(fanin_id, id, 0);
  }
  return id;
}

// @brief Not型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] fanin_id ファンインのノード番号
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_not(const string& node_name,
		   int fanin_id)
{
  int id = mImpl->new_primitive(node_name, 1, BnNodeType::Not);
  if ( fanin_id != kBnNullId ) {
    connect(fanin_id, id, 0);
  }
  return id;
}

// @brief And型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_and(const string& node_name,
		   int ni)
{
  return mImpl->new_primitive(node_name, ni, BnNodeType::And);
}

// @brief AND型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_and(const string& node_name,
		   const vector<int>& fanin_id_list)
{
  int ni = fanin_id_list.size();
  int id = mImpl->new_primitive(node_name, ni, BnNodeType::And);
  if ( id != kBnNullId ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief Nand型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_nand(const string& node_name,
		    int ni)
{
  return mImpl->new_primitive(node_name, ni, BnNodeType::Nand);
}

// @brief NAND型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_nand(const string& node_name,
		    const vector<int>& fanin_id_list)
{
  int ni = fanin_id_list.size();
  int id = mImpl->new_primitive(node_name, ni, BnNodeType::Nand);
  if ( id != kBnNullId ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief Or型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_or(const string& node_name,
		  int ni)
{
  return mImpl->new_primitive(node_name, ni, BnNodeType::Or);
}

// @brief OR型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_or(const string& node_name,
		  const vector<int>& fanin_id_list)
{
  int ni = fanin_id_list.size();
  int id = mImpl->new_primitive(node_name, ni, BnNodeType::Or);
  if ( id != kBnNullId ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief Nor型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_nor(const string& node_name,
		   int ni)
{
  return mImpl->new_primitive(node_name, ni, BnNodeType::Nor);
}

// @brief NOR型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_nor(const string& node_name,
		   const vector<int>& fanin_id_list)
{
  int ni = fanin_id_list.size();
  int id = mImpl->new_primitive(node_name, ni, BnNodeType::Nor);
  if ( id != kBnNullId ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief Xor型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_xor(const string& node_name,
		   int ni)
{
  return mImpl->new_primitive(node_name, ni, BnNodeType::Xor);
}

// @brief XOR型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_xor(const string& node_name,
		   const vector<int>& fanin_id_list)
{
  int ni = fanin_id_list.size();
  int id = mImpl->new_primitive(node_name, ni, BnNodeType::Xor);
  if ( id != kBnNullId ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief Xnor型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_xnor(const string& node_name,
		    int ni)
{
  return mImpl->new_primitive(node_name, ni, BnNodeType::Xnor);
}

// @brief XNOR型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_xnor(const string& node_name,
		    const vector<int>& fanin_id_list)
{
  int ni = fanin_id_list.size();
  int id = mImpl->new_primitive(node_name, ni, BnNodeType::Xnor);
  if ( id != kBnNullId ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] expr 論理式
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetwork::new_logic(const string& node_name,
		     const Expr& expr,
		     const vector<int>& fanin_id_list)
{
  int id = mImpl->new_expr(node_name, expr);
  if ( id != kBnNullId && !fanin_id_list.empty() ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] tv 真理値表
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードを返す．
//
// ノード名の重複に関しては感知しない．
int
BnNetwork::new_logic(const string& node_name,
		     const TvFunc& tv,
		     const vector<int>& fanin_id_list)
{
  int id = mImpl->new_tv(node_name, tv);
  if ( id != kBnNullId && !fanin_id_list.empty() ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief 論理セルを追加する．
// @param[in] node_name ノード名
// @param[in] cell_name セル名
// @param[in] fanin_id_list ファンインのノード番号のリスト
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
// - セル名に合致するセルがない場合と論理セルでない場合には kBnNullId を返す．
int
BnNetwork::new_logic(const string& node_name,
		     const string& cell_name,
		     const vector<int>& fanin_id_list)
{
  int id = mImpl->new_cell(node_name, cell_name);
  if ( id != kBnNullId && !fanin_id_list.empty() ) {
    connect_fanins(id, fanin_id_list);
  }
  return id;
}

// @brief 部分回路を追加する．
// @param[in] src_network 部分回路
// @param[in] input_list インポートした部分回路の入力に接続するノード番号のリスト
// @param[out] output_list インポートした部分回路の出力ノード番号のリスト
//
// * src_network は wrap_up() されている必要がある．
// * src_network のポートの情報は失われる．
// * 矛盾しない限りセルライブラリの情報も引く継がれる．
void
BnNetwork::import_subnetwork(const BnNetwork& src_network,
			     const vector<int>& input_list,
			     vector<int>& output_list)
{
  mImpl->import_subnetwork(*src_network.mImpl, input_list, output_list);
}

// @brief ノード間を接続する．
// @param[in] src_id ファンアウト元のノード番号
// @param[in] dst_id ファンイン先のノード番号
// @param[in] ipos ファンインの位置
void
BnNetwork::connect(int src_id,
		   int dst_id,
		   int ipos)
{
  mImpl->connect(src_id, dst_id, ipos);
}

// @brief 整合性のチェックを行う．
// @return チェック結果を返す．
//
// チェック項目は以下の通り
// - model_name() が設定されているか？
//   設定されていない場合にはデフォルト値を設定する．
//   エラーとはならない．
// - 各ポートの各ビットが設定されているか？
// - 各DFFの入力，出力およびクロックが設定されているか？
// - 各ラッチの入力，出力およびイネーブルが設定されているか？
// - 各ノードのファンインが設定されているか？
bool
BnNetwork::wrap_up()
{
  return mImpl->wrap_up();
}

// @brief ファンインの接続を行う．
// @param[in] id 対象のノード番号
// @param[in] fanin_id_list ファンインのノード番号のリスト
void
BnNetwork::connect_fanins(int id,
			  const vector<int>& fanin_id_list)
{
  int ni = fanin_id_list.size();
  ASSERT_COND( mImpl->node(id).fanin_num() == ni );
  for ( int i: Range(ni) ) {
    int iid = fanin_id_list[i];
    mImpl->connect(iid, id, i);
  }
}

// @brief ネットワーク名を得る．
string
BnNetwork::name() const
{
  return mImpl->name();
}

// @brief 関連するセルライブラリを得る．
//
// 場合によっては空の場合もある．
const ClibCellLibrary&
BnNetwork::library() const
{
  return mImpl->library();
}

// @brief ポート数を得る．
int
BnNetwork::port_num() const
{
  return mImpl->port_num();
}

// @brief ポートの情報を得る．
// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
const BnPort&
BnNetwork::port(int pos) const
{
  return mImpl->port(pos);
}

// @brief DFF数を得る．
int
BnNetwork::dff_num() const
{
  return mImpl->dff_num();
}

// @brief DFFを得る．
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
const BnDff&
BnNetwork::dff(int pos) const
{
  return mImpl->dff(pos);
}

// @brief ラッチ数を得る．
int
BnNetwork::latch_num() const
{
  return mImpl->latch_num();
}

// @brief ラッチを得る．
// @param[in] pos 位置番号 ( 0 <= pos < latch_num() )
const BnLatch&
BnNetwork::latch(int pos) const
{
  return mImpl->latch(pos);
}

// @brief ノード数を得る．
int
BnNetwork::node_num() const
{
  return mImpl->node_num();
}

// @brief ノードを得る．
// @param[in] id ノード番号 ( 0 <= id < node_num() )
//
// BnNode* node = BnNetwork::node(id);
// node->id() == id が成り立つ．
const BnNode&
BnNetwork::node(int id) const
{
  return mImpl->node(id);
}

// @brief 入力数を得る．
int
BnNetwork::input_num() const
{
  return mImpl->input_num();
}

// @brief 入力ノードのノード番号を得る．
// @param[in] pos 入力番号 ( 0 <= pos < input_num() )
int
BnNetwork::input_id(int pos) const
{
  return mImpl->input_num();
}

// @brief 入力ノードのノード番号のリストを得る．
const vector<int>&
BnNetwork::input_id_list() const
{
  return mImpl->input_id_list();
}

// @brief 出力数を得る．
int
BnNetwork::output_num() const
{
  return mImpl->output_num();
}

// @brief 出力ノードのノード番号を得る．
// @param[in] pos 出力番号 ( 0 <= pos < output_num() )
int
BnNetwork::output_id(int pos) const
{
  return mImpl->output_id(pos);
}

// @brief 出力ノードのノード番号のリストを得る．
const vector<int>&
BnNetwork::output_id_list() const
{
  return mImpl->output_id_list();
}

// @brief 出力ノードのソースノード番号を得る．
// @param[in] pos 出力番号 ( 0 <= pos < output_num() )
//
// ソースノードとは出力ノードのファンインのノード
int
BnNetwork::output_src_id(int pos) const
{
  return mImpl->output_src_id(pos);
}

// @brief 出力ノードのソースノード番号のリストを得る．
//
// ソースノードとは出力ノードのファンインのノード
const vector<int>&
BnNetwork::output_src_id_list() const
{
  return mImpl->output_src_id_list();
}

// @brief 論理ノード数を得る．
int
BnNetwork::logic_num() const
{
  return mImpl->logic_num();
}

// @brief 論理ノードのノード番号を得る．
// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
int
BnNetwork::logic_id(int pos) const
{
  return mImpl->logic_id(pos);
}

// @brief 論理ノードのノード番号のリストを得る．
const vector<int>&
BnNetwork::logic_id_list() const
{
  return mImpl->logic_id_list();
}

// @brief 関数の数を得る．
int
BnNetwork::func_num() const
{
  return mImpl->func_num();
}

// @brief 関数番号から関数を得る．
// @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
const TvFunc&
BnNetwork::func(int func_id) const
{
  return mImpl->func(func_id);
}

// @brief 論理式の数を得る．
int
BnNetwork::expr_num() const
{
  return mImpl->expr_num();
}

// @brief 論理式番号から論理式を得る．
// @param[in] expr_id 論理式番号 ( 0 <= expr_id < expr_num() )
Expr
BnNetwork::expr(int expr_id) const
{
  return mImpl->expr(expr_id);
}

// @brief 内容を出力する．
// @param[in] s 出力先のストリーム
//
// 形式は独自フォーマット
void
BnNetwork::write(ostream& s) const
{
  mImpl->write(s);
}


//////////////////////////////////////////////////////////////////////
// BnNodeType
//////////////////////////////////////////////////////////////////////

// @relates BnNodeType
// @brief BnNodeType の内容をストリームに出力する．
ostream&
operator<<(ostream& s,
	   BnNodeType type)
{
  switch ( type ) {
  case BnNodeType::None:   s << "None"; break;
  case BnNodeType::Input:  s << "Input"; break;
  case BnNodeType::Output: s << "Output"; break;
  case BnNodeType::C0:     s << "C0"; break;
  case BnNodeType::C1:     s << "C1"; break;
  case BnNodeType::Buff:   s << "Buff"; break;
  case BnNodeType::Not:    s << "Not"; break;
  case BnNodeType::And:    s << "And"; break;
  case BnNodeType::Nand:   s << "Nand"; break;
  case BnNodeType::Or:     s << "Or"; break;
  case BnNodeType::Nor:    s << "Nor"; break;
  case BnNodeType::Xor:    s << "Xor"; break;
  case BnNodeType::Xnor:   s << "Xnor"; break;
  case BnNodeType::Expr:   s << "Expr"; break;
  case BnNodeType::TvFunc: s << "TvFunc"; break;
  defult: ASSERT_NOT_REACHED; break;
  }
  return s;
}

// cython 用の変換関数
int
__bnnodetype_to_int(BnNodeType type)
{
  switch ( type ) {
  case BnNodeType::None:   return 0;
  case BnNodeType::Input:  return 1;
  case BnNodeType::Output: return 2;
  case BnNodeType::C0:     return 3;
  case BnNodeType::C1:     return 4;
  case BnNodeType::Buff:   return 5;
  case BnNodeType::Not:    return 6;
  case BnNodeType::And:    return 7;
  case BnNodeType::Nand:   return 8;
  case BnNodeType::Or:     return 9;
  case BnNodeType::Nor:    return 10;
  case BnNodeType::Xor:    return 11;
  case BnNodeType::Xnor:   return 12;
  case BnNodeType::Expr:   return 13;
  case BnNodeType::TvFunc: return 14;
  defult: ASSERT_NOT_REACHED; break;
  }
  return 0;
}

BnNodeType
__int_to_bnnodetype(int val)
{
  static BnNodeType tbl[] {
    BnNodeType::None,
    BnNodeType::Input,
    BnNodeType::Output,
    BnNodeType::C0,
    BnNodeType::C1,
    BnNodeType::Buff,
    BnNodeType::Not,
    BnNodeType::And,
    BnNodeType::Nand,
    BnNodeType::Or,
    BnNodeType::Nor,
    BnNodeType::Xor,
    BnNodeType::Xnor,
    BnNodeType::Expr,
    BnNodeType::TvFunc
  };

  ASSERT_COND( val >= 0 && val < 15 );
  return tbl[val];
}


END_NAMESPACE_YM_BNET
