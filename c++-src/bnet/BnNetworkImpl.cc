
/// @file BnNetworkImpl.cc
/// @brief BnNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "BnNetworkImpl.h"
#include "ym/ClibCell.h"
#include "ym/ClibFFInfo.h"
#include "ym/ClibLatchInfo.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"
#include "ym/Range.h"

#include "BnPortImpl.h"
#include "BnInputNode.h"
#include "BnOutputNode.h"
#include "BnLogicNode.h"
#include "BnDffImpl.h"
#include "BnLatchImpl.h"

#include "FuncAnalyzer.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
//
// 空の状態で初期化される．
BnNetworkImpl::BnNetworkImpl()
{
  mSane = false;
}

// @brief デストラクタ
BnNetworkImpl::~BnNetworkImpl()
{
  clear();
}

// @brief 内容をクリアする．
//
// コンストラクタ直後と同じ状態になる．
void
BnNetworkImpl::clear()
{
  for ( auto port_p: mPortList ) {
    delete port_p;
  }
  for ( auto dff_p: mDffList ) {
    delete dff_p;
  }
  for ( auto latch_p: mLatchList ) {
    delete latch_p;
  }
  for ( auto node_p: mNodeList ) {
    delete node_p;
  }

  mName = string();
  mPortList.clear();
  mDffList.clear();
  mLatchList.clear();
  mInputList.clear();
  mOutputList.clear();
  mOutputSrcList.clear();
  mLogicList.clear();
  mNodeList.clear();

  mSane = false;
}


// @brief 内容をコピーする．
// @param[in] src コピー元のオブジェクト
void
BnNetworkImpl::copy(const BnNetworkImpl& src)
{
  if ( &src == this ) {
    // 自分自身がソースならなにもしない．
    return;
  }

  clear();

  // セルライブラリの設定
  set_library(src.library());

  // ネットワーク名の設定
  set_name(src.name());

  // srcのノード番号をキーにしてノード番号を格納する配列
  vector<int> id_map(src.node_num());

  // ポートの生成
  int np = src.port_num();
  for ( int i: Range<>(np) ) {
    auto& src_port = src.port(i);
    string port_name = src_port.name();

    // 各ビットの方向を求める．
    int nb = src_port.bit_width();
    vector<int> dirs(nb);
    for ( int i: Range<>(nb) ) {
      int id = src_port.bit(i);
      auto& node = src.node(id);
      if ( node.is_input() ) {
	dirs[i] = 0;
      }
      else if ( node.is_output() ) {
	dirs[i] = 1;
      }
      else {
	ASSERT_NOT_REACHED;
      }
    }

    // ポートの生成
    int dst_port_id = new_port(port_name, dirs);
    ASSERT_COND( src_port.id() == dst_port_id );

    // 各ビットの対応関係を記録する．
    auto& dst_port = port(dst_port_id);
    for ( int i: Range<>(nb) ) {
      int src_id = src_port.bit(i);
      int dst_id = dst_port.bit(i);
      id_map[src_id] = dst_id;
    }
  }

  // src の入力の対応するノードを input_list に入れる．
  int input_num = src.input_num();
  vector<int> input_list(input_num);
  for ( int i: Range<>(input_num) ) {
    int src_id = src.input_id(i);
    int dst_id = id_map[src_id];
    input_list[i] = dst_id;
  }

  // src 本体をインポートする．
  vector<int> output_list;
  import_subnetwork(src, input_list, output_list);

  // 出力端子のファンインの接続
  int output_num = src.output_num();
  for ( int i: Range<>(src.output_num()) ) {
    int src_id = src.output_id(i);
    int src_fanin_id = src.output_src_id(i);

    int dst_id = id_map[src_id];
    int dst_fanin_id = output_list[i];
    connect(dst_fanin_id, dst_id, 0);
  }

  bool stat = wrap_up();

  ASSERT_COND( stat );
}

// @brief ノードを得る．
// @param[in] id ノード番号 ( 0 <= id < node_num() )
//
// BnNode* node = BnNetworkImpl::node(id);
// node->id() == id が成り立つ．
const BnNode&
BnNetworkImpl::node(int id) const
{
  ASSERT_COND( id >= 0 && id < node_num() );
  return *mNodeList[id];
}

// @brief セルライブラリをセットする．
// @param[in] library ライブラリ
void
BnNetworkImpl::set_library(const ClibCellLibrary& library)
{
  mCellLibrary = library;
}

// @brief ネットワーク名を設定する．
// @param[in] name ネットワーク名
void
BnNetworkImpl::set_name(const string& name)
{
  mName = name;
}

// @brief 入出力混合のポートを作る．
// @param[in] port_name ポート名
// @param[in] dir_vect 向きを表すベクタ
// @return 生成したポート番号を返す．
//
// dir_vect[i] == 0 の時，入力を表す．
int
BnNetworkImpl::new_port(const string& port_name,
			const vector<int>& dir_vect)
{
  int port_id = mPortList.size();
  int bit_width = dir_vect.size();
  vector<int> bits(bit_width);
  for ( int i: Range<>(bit_width) ) {
    int node_id = mNodeList.size();
    bits[i] = node_id;
    string node_name;
    if ( bit_width > 1 ) {
      ostringstream buf;
      buf << port_name << "[" << i << "]";
      node_name = buf.str();
    }
    else {
      node_name = port_name;
    }
    if ( dir_vect[i] == 0 ) {
      int input_id = mInputList.size();
      BnNodeImpl* node = new BnPortInput(node_id, node_name, input_id, port_id, i);
      mNodeList.push_back(node);
      mInputList.push_back(node_id);
    }
    else {
      int output_id = mOutputList.size();
      BnNodeImpl* node = new BnPortOutput(node_id, node_name, output_id, port_id, i);
      mNodeList.push_back(node);
      mOutputList.push_back(node_id);
    }
  }

  BnPort* port;
  if ( bit_width == 1 ) {
    port = new BnPort1(port_id, port_name, bits[0]);
  }
  else {
    port = new BnPortN(port_id, port_name, bits);
  }
  mPortList.push_back(port);

  return port_id;
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
BnNetworkImpl::new_dff(const string& name,
		       bool has_xoutput,
		       bool has_clear,
		       bool has_preset)
{
  return _new_dff(name, has_xoutput, has_clear, has_preset, nullptr);
}

// @brief セルの情報を持ったDFFを追加する．
// @param[in] name DFF名
// @param[in] cell_name 対応するセル名
// @return 生成したDFF番号を返す．
//
// - 名前の重複に関しては感知しない．
// - セルは FF のセルでなければならない．
int
BnNetworkImpl::new_dff(const string& name,
		       const string& cell_name)
{
  const ClibCell* cell = mCellLibrary.cell(cell_name);
  if ( cell == nullptr || !cell->is_ff() ) {
    return -1;
  }

  ClibFFInfo ffinfo = cell->ff_info();
  bool has_xoutput = ffinfo.has_xq();
  bool has_clear = ffinfo.has_clear();
  bool has_preset = ffinfo.has_preset();
  return _new_dff(name, has_xoutput, has_clear, has_preset, cell);
}

// @brief DFFを追加する共通の関数
// @param[in] name DFF名
// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
// @param[in] has_clear クリア端子を持つ時 true にする．
// @param[in] has_preset プリセット端子を持つ時 true にする．
// @param[in] cell 対応するセル．
// @return 生成したDFF番号を返す．
//
// 名前の重複に関しては感知しない．
int
BnNetworkImpl::_new_dff(const string& name,
			bool has_xoutput,
			bool has_clear,
			bool has_preset,
			const ClibCell* cell)
{
  int dff_id = mDffList.size();

  int input_id = mNodeList.size();
  {
    int oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".input";
    string name = buf.str();
    BnNodeImpl* node = new BnDffInput(input_id, name, oid, dff_id);
    mNodeList.push_back(node);
    mOutputList.push_back(input_id);
  }

  int output_id = mNodeList.size();
  {
    int iid = mInputList.size();
    ostringstream buf;
    buf << name << ".output";
    string name = buf.str();
    BnNodeImpl* node = new BnDffOutput(output_id, name, iid, dff_id);
    mNodeList.push_back(node);
    mInputList.push_back(output_id);
  }

  int xoutput_id = kBnNullId;
  if ( has_xoutput ) {
    int xoutput_id = mNodeList.size();
    int iid = mInputList.size();
    ostringstream buf;
    buf << name << ".xoutput";
    string name = buf.str();
    BnNodeImpl* node = new BnDffXOutput(xoutput_id, name, iid, dff_id);
    mNodeList.push_back(node);
    mInputList.push_back(xoutput_id);
  }

  int clock_id = mNodeList.size();
  {
    int oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".clock";
    string name = buf.str();
    BnNodeImpl* node = new BnDffClock(clock_id, name, oid, dff_id);
    mNodeList.push_back(node);
    mOutputList.push_back(clock_id);
  }

  int clear_id = kBnNullId;
  if ( has_clear ) {
    clear_id = mNodeList.size();
    int oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".clear";
    string name = buf.str();
    BnNodeImpl* node = new BnDffClear(clear_id, name, oid, dff_id);
    mNodeList.push_back(node);
    mOutputList.push_back(clear_id);
  }

  int preset_id = kBnNullId;
  if ( has_preset ) {
    preset_id = mNodeList.size();
    int oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".preset";
    BnNodeImpl* node = new BnDffPreset(preset_id, name, oid, dff_id);
    mNodeList.push_back(node);
    mOutputList.push_back(preset_id);
  }

  BnDff* dff = new BnDffImpl(dff_id, name, input_id, output_id, xoutput_id,
			     clock_id, clear_id, preset_id, cell);
  mDffList.push_back(dff);

  return dff_id;
}

// @brief ラッチを追加する．
// @param[in] name ラッチ名
// @param[in] has_clear クリア端子を持つ時 true にする．
// @param[in] has_preset プリセット端子を持つ時 true にする．
// @return 生成したラッチ番号を返す．
//
// 名前の重複に関しては感知しない．
int
BnNetworkImpl::new_latch(const string& name,
			 bool has_xoutput,
			 bool has_clear,
			 bool has_preset)
{
  return _new_latch(name, has_xoutput, has_clear, has_preset, nullptr);
}

// @brief セルの情報を持ったラッチを追加する．
// @param[in] name ラッチ名
// @param[in] cell_name 対応するセル名．
// @return 生成したラッチ番号を返す．
//
// - 名前の重複に関しては感知しない．
// - セルはラッチのセルでなければならない．
int
BnNetworkImpl::new_latch(const string& name,
			 const string& cell_name)
{
  const ClibCell* cell = mCellLibrary.cell(cell_name);
  if ( cell == nullptr || !cell->is_latch() ) {
    return -1;
  }

  ClibLatchInfo latchinfo = cell->latch_info();
  bool has_xoutput = latchinfo.has_xq();
  bool has_clear = latchinfo.has_clear();
  bool has_preset = latchinfo.has_preset();
  return _new_latch(name, has_xoutput, has_clear, has_preset, cell);
}

// @brief ラッチを追加する共通の処理を行う関数
// @param[in] name ラッチ名
// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
// @param[in] has_clear クリア端子を持つ時 true にする．
// @param[in] has_preset プリセット端子を持つ時 true にする．
// @param[in] cell 対応するセル．
// @return 生成したラッチ番号を返す．
//
// 名前の重複に関しては感知しない．
// cell はラッチのセルでなければならない．
int
BnNetworkImpl::_new_latch(const string& name,
			  bool has_xoutput,
			  bool has_clear,
			  bool has_preset,
			  const ClibCell* cell)
{
  int latch_id = mLatchList.size();

  int input_id = mNodeList.size();
  {
    int oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".input";
    string name = buf.str();
    BnNodeImpl* node = new BnLatchInput(input_id, name, oid, latch_id);
    mNodeList.push_back(node);
    mOutputList.push_back(input_id);
  }

  int output_id = mNodeList.size();
  {
    int iid = mInputList.size();
    ostringstream buf;
    buf << name << ".output";
    string name = buf.str();
    BnNodeImpl* node = new BnLatchOutput(output_id, name, iid, latch_id);
    mNodeList.push_back(node);
    mInputList.push_back(output_id);
  }

  int xoutput_id = kBnNullId;
  if ( has_xoutput ) {
    int iid = mInputList.size();
    ostringstream buf;
    buf << name << ".xoutput";
    string name = buf.str();
    BnNodeImpl* node = new BnLatchXOutput(output_id, name, iid, latch_id);
    mNodeList.push_back(node);
    mInputList.push_back(xoutput_id);
  }

  int enable_id = mNodeList.size();
  {
    int oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".enable";
    string name = buf.str();
    BnNodeImpl* node = new BnLatchEnable(enable_id, name, oid, latch_id);
    mNodeList.push_back(node);
    mOutputList.push_back(enable_id);
  }

  int clear_id = kBnNullId;
  if ( has_clear ) {
    clear_id = mNodeList.size();
    int oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".clear";
    string name = buf.str();
    BnNodeImpl* node = new BnLatchClear(clear_id, name, oid, latch_id);
    mNodeList.push_back(node);
    mOutputList.push_back(clear_id);
  }

  int preset_id = kBnNullId;
  if ( has_preset ) {
    preset_id = mNodeList.size();
    int oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".preset";
    BnNodeImpl* node = new BnLatchPreset(preset_id, name, oid, latch_id);
    mNodeList.push_back(node);
    mOutputList.push_back(preset_id);
  }

  BnLatch* latch = new BnLatchImpl(latch_id, name, input_id, output_id, xoutput_id,
				   enable_id, clear_id, preset_id, cell);
  mLatchList.push_back(latch);

  return latch_id;
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @param[in] logic_type 論理型
// @return 生成した論理ノードの番号を返す．
int
BnNetworkImpl::new_primitive(const string& node_name,
			     int ni,
			     BnNodeType logic_type)
{
  return _new_primitive(node_name, ni, logic_type, nullptr);
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] expr 論理式
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
int
BnNetworkImpl::new_expr(const string& node_name,
			const Expr& expr)
{
  BnNodeType logic_type = FuncAnalyzer::analyze(expr);
  int ni = expr.input_size();
  if ( logic_type != BnNodeType::Expr ) {
    // 組み込み型だった．
    return _new_primitive(node_name, ni, logic_type, nullptr);
  }
  return _new_expr(node_name, ni, expr, nullptr);
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] tv 真理値表
// @return 生成した論理ノードを返す．
//
// ノード名の重複に関しては感知しない．
int
BnNetworkImpl::new_tv(const string& node_name,
		      const TvFunc& tv)
{
  BnNodeType logic_type = FuncAnalyzer::analyze(tv);
  int ni = tv.input_num();
  if ( logic_type != BnNodeType::TvFunc ) {
    // 組み込み型だった．
    return _new_primitive(node_name, ni, logic_type, nullptr);
  }
  return _new_tv(node_name, ni, tv, nullptr);
}

// @brief 論理セルを追加する．
// @param[in] node_name ノード名
// @param[in] cell_name セル名
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
// - セル名に合致するセルがない場合と論理セルでない場合には kBnNullId を返す．
int
BnNetworkImpl::new_cell(const string& node_name,
			const string& cell_name)
{
  const ClibCell* cell = mCellLibrary.cell(cell_name);
  if ( cell == nullptr ||
       cell->is_logic() ||
       cell->output_num() != 1 ) {
    return kBnNullId;
  }

  Expr expr = cell->logic_expr(0);
  BnNodeType logic_type = FuncAnalyzer::analyze(expr);
  int ni = cell->input_num();
  if ( logic_type != BnNodeType::Expr ) {
    // 組み込み型だった．
    return _new_primitive(node_name, ni, logic_type, cell);
  }
  return _new_expr(node_name, ni, expr, cell);
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @param[in] logic_type 論理型
// @param[in] cell 対応するセル
// @return 生成した論理ノードの番号を返す．
int
BnNetworkImpl::_new_primitive(const string& node_name,
			      int ni,
			      BnNodeType logic_type,
			      const ClibCell* cell)
{
  int id = mNodeList.size();
  BnNodeImpl* node = new BnPrimNode(id, node_name, ni, logic_type, cell);
  mNodeList.push_back(node);
  mLogicList.push_back(id);

  return id;
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @param[in] expr 論理式
// @param[in] cell 対応するセル
// @return 生成した論理ノードの番号を返す．
int
BnNetworkImpl::_new_expr(const string& node_name,
			 int ni,
			 const Expr& expr,
			 const ClibCell* cell)
{
  int id = mNodeList.size();
  int expr_id = _add_expr(expr);
  BnNodeImpl* node = new BnExprNode(id, node_name, ni, expr_id, cell);
  mNodeList.push_back(node);
  mLogicList.push_back(id);

  return id;
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @param[in] tv 真理値表
// @param[in] cell 対応するセル
// @return 生成した論理ノードの番号を返す．
int
BnNetworkImpl::_new_tv(const string& node_name,
		       int ni,
		       const TvFunc& tv,
		       const ClibCell* cell)
{
  int id = mNodeList.size();
  int func_id = _add_tv(tv);
  BnNodeImpl* node = new BnTvNode(id, node_name, ni, func_id, cell);
  mNodeList.push_back(node);
  mLogicList.push_back(id);

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
BnNetworkImpl::import_subnetwork(const BnNetworkImpl& src_network,
				 const vector<int>& input_list,
				 vector<int>& output_list)
{
  ASSERT_COND( src_network.mSane );

  int input_num = src_network.input_num();
  ASSERT_COND( input_list.size() == input_num );

  int output_num = src_network.output_num();
  output_list.clear();
  output_list.reserve(output_num);

  // src_network のノード番号をキーにして生成したノード番号を入れる配列
  vector<int> id_map(src_network.node_num());

  // src_network の入力と input_list の対応関係を id_map に入れる．
  for ( int i: Range<>(input_num) ) {
    int src_id = src_network.input_id(i);
    int dst_id = input_list[i];
    id_map[src_id] = dst_id;
  }

  // DFFを作る．
  for ( auto src_dff_p: src_network.mDffList ) {
    dup_dff(*src_dff_p, id_map);
  }

  // ラッチを作る．
  for ( auto src_latch_p: src_network.mLatchList ) {
    dup_latch(*src_latch_p, id_map);
  }

  // 関数情報の生成
  for ( auto& func: src_network.mFuncList ) {
    int func_id = _add_tv(func);
  }

  // 論理式情報の生成
  for ( auto& expr: src_network.mExprList ) {
    int expr_id = _add_expr(expr);
  }

  // 論理ノードの生成
  for ( int src_id: src_network.logic_id_list() ) {
    auto& src_node = src_network.node(src_id);
    int dst_id = dup_logic(src_node, src_network, id_map);
  }

  // src_network の外部出力のファンインに対応するノード番号を
  // output_list に入れる．
  for ( int src_id: src_network.output_src_id_list() ) {
    int dst_id = id_map[src_id];
    output_list.push_back(dst_id);
  }
}

// @brief DFFを複製する．
// @param[in] src_dff 元のDFF
// @param[out] id_map 生成したノードの対応関係を記録する配列
// @return 生成した DFF を返す．
int
BnNetworkImpl::dup_dff(const BnDff& src_dff,
		       vector<int>& id_map)
{
  string dff_name = src_dff.name();
  bool has_clear = (src_dff.clear() != kBnNullId);
  bool has_preset = (src_dff.preset() != kBnNullId);
  int dst_id = new_dff(dff_name, has_clear, has_preset);
  auto& dst_dff = dff(dst_id);

  // 各端子の対応関係を記録する．
  {
    int src_id = src_dff.input();
    int dst_id = dst_dff.input();
    id_map[src_id] = dst_id;
  }
  {
    int src_id = src_dff.output();
    int dst_id = dst_dff.output();
    id_map[src_id] = dst_id;
  }
  {
    int src_id = src_dff.clock();
    int dst_id = dst_dff.clock();
    id_map[src_id] = dst_id;
  }
  if ( has_clear ) {
    int src_id = src_dff.clear();
    int dst_id = dst_dff.clear();
    id_map[src_id] = dst_id;
  }
  if ( has_preset ) {
    int src_id = src_dff.preset();
    int dst_id = dst_dff.preset();
    id_map[src_id] = dst_id;
  }

  return dst_id;
}

// @brief ラッチを複製する．
// @param[in] src_latch 元のラッチ
// @param[out] id_map 生成したノードの対応関係を記録する配列
// @return 生成したラッチを返す．
int
BnNetworkImpl::dup_latch(const BnLatch& src_latch,
			 vector<int>& id_map)
{
  string latch_name = src_latch.name();
  bool has_clear = (src_latch.clear() != kBnNullId);
  bool has_preset = (src_latch.preset() != kBnNullId);
  int dst_id = new_latch(latch_name, has_clear, has_preset);
  auto& dst_latch = latch(dst_id);

  // 各端子の対応関係を記録する．
  {
    int src_id = src_latch.input();
    int dst_id = dst_latch.input();
    id_map[src_id] = dst_id;
  }
  {
    int src_id = src_latch.output();
    int dst_id = dst_latch.output();
    id_map[src_id] = dst_id;
  }
  {
    int src_id = src_latch.enable();
    int dst_id = dst_latch.enable();
    id_map[src_id] = dst_id;
  }
  if ( has_clear ) {
    int src_id = src_latch.clear();
    int dst_id = dst_latch.clear();
    id_map[src_id] = dst_id;
  }
  if ( has_preset ) {
    int src_id = src_latch.preset();
    int dst_id = dst_latch.preset();
    id_map[src_id] = dst_id;
  }

  return dst_id;
}

// @brief 論理ノードを複製する．
// @param[in] src_node 元のノード
// @param[out] id_map 生成したノードの対応関係を記録する配列
// @return 生成したノードのノード番号を返す．
//
// ノード間の接続は行わない．
int
BnNetworkImpl::dup_logic(const BnNode& src_node,
			 const BnNetworkImpl& src_network,
			 vector<int>& id_map)
{
  ASSERT_COND( src_node.is_logic() );

  int nfi = src_node.fanin_num();
  string name = src_node.name();
  BnNodeType logic_type = src_node.type();
  const ClibCell* cell = src_node.cell();
  int dst_id = kBnNullId;
  if ( logic_type == BnNodeType::Expr ) {
    dst_id = _new_expr(name, nfi, src_network.expr(src_node.expr_id()), cell);
  }
  else if ( logic_type == BnNodeType::TvFunc ) {
    dst_id = _new_tv(name, nfi, src_network.func(src_node.func_id()), cell);
  }
  else {
    dst_id = _new_primitive(name, nfi, logic_type, cell);
  }
  id_map[src_node.id()] = dst_id;

  for ( int i: Range<>(nfi) ) {
    int src_iid = src_node.fanin_id(i);
    int iid = id_map[src_iid];
    connect(iid, dst_id, i);
  }

  return dst_id;
}

// @brief ノード間を接続する．
// @param[in] src_id ファンアウト元のノード番号
// @param[in] dst_id ファンイン先のノード番号
// @param[in] ipos ファンインの位置
void
BnNetworkImpl::connect(int src_id,
		       int dst_id,
		       int ipos)
{
  ASSERT_COND( src_id >= 0 && src_id < mNodeList.size() );
  ASSERT_COND( dst_id >= 0 && dst_id < mNodeList.size() );

  BnNodeImpl* dst_node = mNodeList[dst_id];
  dst_node->set_fanin(ipos, src_id);

  mSane = false;
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
BnNetworkImpl::wrap_up()
{
  if ( mSane ) {
    return true;
  }

  bool error = false;

  // ポートのチェック
  for ( auto port_p: mPortList ) {
    for ( int i: Range<>(port_p->bit_width()) ) {
      int id = port_p->bit(i);
      if ( id == kBnNullId || id >= node_num() ) {
	cerr << "Port#" << port_p->id() << "(" << port_p->name() << ").bit["
	     << i << "] is not set" << endl;
	error = true;
      }
    }
  }

  // DFF のチェック
  for ( auto dff_p: mDffList ) {
    int id1 = dff_p->input();
    if ( id1 == kBnNullId ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").input is not set" << endl;
      error = true;
    }
    else if ( id1 >= node_num() ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").input is not valid" << endl;
      error = true;
    }
    int id2 = dff_p->output();
    if ( id2 == kBnNullId ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").output is not set" << endl;
      error = true;
    }
    else if ( id2 >= node_num() ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").output is not valid" << endl;
      error = true;
    }
    int id3 = dff_p->clock();
    if ( id3 == kBnNullId ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").clock is not set" << endl;
      error = true;
    }
    else if ( id3 >= node_num() ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").clock is not valid" << endl;
      error = true;
    }
    int id4 = dff_p->clear();
    if ( id4 != kBnNullId && id4 >= node_num() ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").clear is not valid" << endl;
      error = true;
    }
    int id5 = dff_p->preset();
    if ( id5 != kBnNullId && id5 >= node_num() ) {
      cerr << "DFF#" << dff_p->id() << "(" << dff_p->name() << ").preset is not valid" << endl;
      error = true;
    }
  }

  // ラッチのチェック
  for ( auto latch_p: mLatchList ) {
    int id1 = latch_p->input();
    if ( id1 == kBnNullId ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").input is not set" << endl;
      error = true;
    }
    else if ( id1 >= node_num() ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").input is not valid" << endl;
      error = true;
    }
    int id2 = latch_p->output();
    if ( id2 == kBnNullId ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").output is not set" << endl;
      error = true;
    }
    else if ( id2 >= node_num() ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").output is not valid" << endl;
      error = true;
    }
    int id3 = latch_p->enable();
    if ( id3 == kBnNullId ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").enable is not set" << endl;
      error = true;
    }
    else if ( id3 >= node_num() ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").enable is not valid" << endl;
      error = true;
    }
    int id4 = latch_p->clear();
    if ( id4 != kBnNullId && id4 >= node_num() ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").clear is not valid" << endl;
      error = true;
    }
    int id5 = latch_p->preset();
    if ( id5 != kBnNullId && id5 >= node_num() ) {
      cerr << "LATCH#" << latch_p->id()
	   << "(" << latch_p->name() << ").preset is not valid" << endl;
      error = true;
    }
  }

  // ノードのチェック
  for ( auto node_p: mNodeList ) {
    for ( int i: Range<>(node_p->fanin_num()) ) {
      int id = node_p->fanin_id(i);
      if ( id == kBnNullId ) {
	cerr << "NODE#" << node_p->id() << "(" << node_p->name() << ").fanin["
	     << i << "] is not set" << endl;
	error = true;
      }
      else if ( id < 0 || id >= node_num() ) {
	cerr << "NODE#" << node_p->id() << "(" << node_p->name() << ").fanin["
	     << i << "] is not valid" << endl;
	error = true;
      }
    }
  }

  if ( error ) {
    return false;
  }

  // 各ノードのファンアウトリストの作成
  for ( auto node_p: mNodeList ) {
    node_p->clear_fanout();
  }
  for ( auto node_p: mNodeList ) {
    for ( int id: node_p->fanin_id_list() ) {
      auto src_node_p = mNodeList[id];
      src_node_p->add_fanout(node_p->id());
    }
  }

  // 論理ノードをトポロジカル順にソートする．

  // ノード番号を入れるキュー
  vector<int> queue;
  queue.reserve(node_num());

  // キューの印を表すマーク配列
  vector<bool> mark(node_num(), false);

  // 入力ノードをキューに積む．
  for ( int id: input_id_list() ) {
    queue.push_back(id);
    mark[id] = true;
  }

  mLogicList.clear();
  mLogicList.reserve(node_num() - input_num() - output_num());

  // キューからノードを取り出してファンアウト先のノードをキューに積む．
  for ( int rpos = 0; rpos < queue.size(); ++ rpos ) {
    int id = queue[rpos];
    auto node_p = mNodeList[id];
    if ( node_p->is_logic() ) {
      mLogicList.push_back(id);
    }
    for ( int oid: node_p->fanout_id_list() ) {
      if ( mark[oid] ) {
	continue;
      }
      auto onode_p = mNodeList[oid];
      bool ready = true;
      for ( int iid: onode_p->fanin_id_list() ) {
	if ( !mark[iid] ) {
	  ready = false;
	  break;
	}
      }
      if ( ready ) {
	queue.push_back(oid);
	mark[oid] = true;
      }
    }
  }

  // mOutputSrcList を作る．
  mOutputSrcList.clear();
  mOutputSrcList.resize(mOutputList.size());
  for ( int i: Range<>(mOutputList.size()) ) {
    int oid = mOutputList[i];
    auto node_p = mNodeList[oid];
    int iid = node_p->fanin_id(0);
    mOutputSrcList[i] = iid;
  }

  mSane = true;

  return true;
}

// @brief 論理式を登録する．
// @param[in] expr 論理式
// @return 関数番号を返す．
int
BnNetworkImpl::_add_expr(const Expr& expr)
{
  int ni = expr.input_size();
  int expr_id = 0;
  if ( ni <= 10 ) {
    // 10入力以下の場合は一旦 TvFunc に変換する．
    TvFunc tv = expr.make_tv(ni);
    if ( !mExprMap.find(tv, expr_id) ) {
      // 新たに登録する．
      expr_id = mExprList.size();
      mExprList.push_back(expr);
      mExprMap.add(tv, expr_id);
    }
    return expr_id;
  }
  else {
    // 11入力以上は常に新規に登録する．
    expr_id = mExprList.size();
    mExprList.push_back(expr);
  }
  return expr_id;
}

// @brief 真理値表を登録する．
// @param[in] tv 真理値表
// @return 関数番号を返す．
int
BnNetworkImpl::_add_tv(const TvFunc& tv)
{
  // 同じ関数が登録されていないか調べる．
  int func_id;
  if ( !mFuncMap.find(tv, func_id) ) {
    // 新たに登録する．
    func_id = mFuncList.size();
    mFuncList.push_back(tv);
    mFuncMap.add(tv, func_id);
  }
  return func_id;
}

// @brief 内容を出力する．
// @param[in] s 出力先のストリーム
//
// 形式は独自フォーマット
void
BnNetworkImpl::write(ostream& s) const
{
  s << "network name : " << name() << endl
    << endl;

  for ( auto port_p: mPortList ) {
    s << "port#" << port_p->id() << ": ";
    s << "(" << port_p->name() << ") : ";
    for ( int i: Range<>(port_p->bit_width()) ) {
      s << " " << port_p->bit(i);
    }
    s << endl;
  }
  s << endl;

  for ( int id: input_id_list() ) {
    auto& node = this->node(id);
    ASSERT_COND( node.type() == BnNodeType::Input );
    s << "input: " << node.id()
      << "(" << node.name() << ")" << endl;
  }
  s << endl;

  for ( int id: output_id_list() ) {
    auto& node_p = mNodeList[id];
    ASSERT_COND( node_p != nullptr );
    ASSERT_COND( node_p->type() == BnNodeType::Output );
    s << "output: " << node_p->id()
      << "(" << node_p->name() << ")" << endl
      << "    input: " << node_p->fanin_id(0) << endl;
  }
  s << endl;

  for ( auto dff_p: mDffList ) {
    ASSERT_COND( dff_p != nullptr );
    s << "dff#" << dff_p->id()
      << "(" << dff_p->name() << ")" << endl
      << "    input:  " << dff_p->input() << endl
      << "    output: " << dff_p->output() << endl
      << "    clock:  " << dff_p->clock() << endl;
    if ( dff_p->clear() != kBnNullId ) {
      s << "    clear:  " << dff_p->clear() << endl;
    }
    if ( dff_p->preset() != kBnNullId ) {
      s << "    preset: " << dff_p->preset() << endl;
    }
    s << endl;
  }
  s << endl;

  for ( auto latch_p: mLatchList ) {
    ASSERT_COND( latch_p != nullptr );
    s << "latch#" << latch_p->id()
      << "(" << latch_p->name() << ")" << endl
      << "    input:  " << latch_p->input() << endl
      << "    output: " << latch_p->output() << endl
      << "    enable: " << latch_p->enable() << endl;
    if ( latch_p->clear() != kBnNullId ) {
      s << "    clear:  " << latch_p->clear() << endl;
    }
    if ( latch_p->preset() != kBnNullId ) {
      s << "    preset: " << latch_p->preset() << endl;
    }
  }
  s << endl;

  for ( int id: logic_id_list() ) {
    auto node_p = mNodeList[id];
    ASSERT_COND( node_p != nullptr );
    ASSERT_COND( node_p->is_logic() );
    s << "logic: " << id
      << "(" << node_p->name() << ")" << endl
      << "    fanins: ";
    for ( int fanin_id: node_p->fanin_id_list() ) {
    s << " " << fanin_id;
    }
    s << endl;
    s << "    ";
    switch ( node_p->type() ) {
    case BnNodeType::None:
      s << "NONE";
      break;
    case BnNodeType::C0:
      s << "C0";
      break;
    case BnNodeType::C1:
      s << "C1";
      break;
    case BnNodeType::Buff:
      s << "BUFF";
      break;
    case BnNodeType::Not:
      s << "NOT";
      break;
    case BnNodeType::And:
      s << "AND";
      break;
    case BnNodeType::Nand:
      s << "NAND";
      break;
    case BnNodeType::Or:
      s << "OR";
      break;
    case BnNodeType::Nor:
      s << "NOR";
      break;
    case BnNodeType::Xor:
      s << "XOR";
      break;
    case BnNodeType::Xnor:
      s << "XNOR";
      break;
    case BnNodeType::Expr:
      s << "expr#" << node_p->expr_id() << ": " << expr(node_p->expr_id());
      break;
    case BnNodeType::TvFunc:
      s << "func#" << node_p->func_id() << ": " << func(node_p->func_id());
      break;
    default:
      ASSERT_NOT_REACHED;
    }
    s << endl;
    if ( node_p->cell() ) {
      s << "    cell: " << node_p->cell()->name() << endl;
    }
    s << endl;
  }

  s << endl;
}

END_NAMESPACE_YM_BNET
