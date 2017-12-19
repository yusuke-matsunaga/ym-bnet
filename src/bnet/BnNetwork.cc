
/// @file BnNetwork.cc
/// @brief BnNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNetwork.h"
#include "ym/ClibCell.h"
#include "ym/ClibFFInfo.h"
#include "ym/ClibLatchInfo.h"
#include "ym/Expr.h"
#include "ym/TvFunc.h"

#include "BnPortImpl.h"
#include "BnInputNode.h"
#include "BnOutputNode.h"
#include "BnLogicNode.h"
#include "BnDffImpl.h"
#include "BnLatchImpl.h"

#include "FuncAnalyzer.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
//
// 空の状態で初期化される．
BnNetwork::BnNetwork()
{
  mSane = false;
}

// @brief コピーコンストラクタ
// @param[in] src コピー元のオブジェクト
BnNetwork::BnNetwork(const BnNetwork& src)
{
  copy(src);
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
  for (ymuint i = 0; i < mPortList.size(); ++ i) {
    BnPort* port = mPortList[i];
    delete port;
  }
  for (ymuint i = 0; i < mDffList.size(); ++ i) {
    BnDff* dff = mDffList[i];
    delete dff;
  }
  for (ymuint i = 0; i < mLatchList.size(); ++ i) {
    BnLatch* latch = mLatchList[i];
    delete latch;
  }
  for (ymuint i = 0; i < mNodeList.size(); ++ i) {
    BnNode* node = mNodeList[i];
    delete node;
  }

  mName = string();
  mPortList.clear();
  mDffList.clear();
  mLatchList.clear();
  mInputList.clear();
  mOutputList.clear();
  mLogicList.clear();
  mNodeList.clear();

  mSane = false;
}

BEGIN_NONAMESPACE

// @brief id_map を使ってID番号を変換する．
// @param[in] id ID番号
// @param[in] id_map ID番号のハッシュ表
// @return 変換先のID番号を返す．
// 登録されていない場合には abort する．
ymuint
conv_id(ymuint id,
	const HashMap<ymuint, ymuint>& id_map)
{
  if ( id == kBnNullId ) {
    return kBnNullId;
  }
  ymuint dst_id;
  bool stat = id_map.find(id, dst_id);
  ASSERT_COND( stat );
  return dst_id;
}

END_NONAMESPACE

// @brief 内容をコピーする．
// @param[in] src コピー元のオブジェクト
void
BnNetwork::copy(const BnNetwork& src)
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

  // srcのノード番号をキーにしてノード番号を格納するハッシュ表
  HashMap<ymuint, ymuint> id_map;

  // ポートの生成
  ymuint np = src.port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const BnPort* src_port = src.port(i);
    string port_name = src_port->name();

    // 各ビットの方向を求める．
    ymuint nb = src_port->bit_width();
    vector<int> dirs(nb);
    for (ymuint j = 0; j < nb; ++ j) {
      ymuint id = src_port->bit(j);
      const BnNode* node = src.node(id);
      if ( node->is_input() ) {
	dirs[j] = 0;
      }
      else if ( node->is_output() ) {
	dirs[j] = 1;
      }
      else {
	ASSERT_NOT_REACHED;
      }
    }

    // ポートの生成
    BnPort* dst_port = new_port(port_name, dirs);

    ASSERT_COND( src_port->id() == dst_port->id() );

    // 各ビットの対応関係を記録する．
    for (ymuint j = 0; j < nb; ++ j) {
      ymuint src_id = src_port->bit(j);
      ymuint dst_id = dst_port->bit(j);
      id_map.add(src_id, dst_id);
    }
  }

  // DFF の生成
  ymuint ndff = src.dff_num();
  for (ymuint i = 0; i < ndff; ++ i) {
    const BnDff* src_dff = src.dff(i);
    string dff_name = src_dff->name();
    bool has_clear = (src_dff->clear() != kBnNullId);
    bool has_preset = (src_dff->preset() != kBnNullId);
    BnDff* dst_dff = new_dff(dff_name, has_clear, has_preset);

    ASSERT_COND( src_dff->id() == dst_dff->id() );

    // 各端子の対応関係を記録する．
    {
      ymuint src_id = src_dff->input();
      ymuint dst_id = dst_dff->input();
      id_map.add(src_id, dst_id);
    }
    {
      ymuint src_id = src_dff->output();
      ymuint dst_id = dst_dff->output();
      id_map.add(src_id, dst_id);
    }
    {
      ymuint src_id = src_dff->clock();
      ymuint dst_id = dst_dff->clock();
      id_map.add(src_id, dst_id);
    }
    if ( has_clear ) {
      ymuint src_id = src_dff->clear();
      ymuint dst_id = dst_dff->clear();
      id_map.add(src_id, dst_id);
    }
    if ( has_preset ) {
      ymuint src_id = src_dff->preset();
      ymuint dst_id = dst_dff->preset();
      id_map.add(src_id, dst_id);
    }
  }

  // ラッチの生成
  ymuint nlatch = src.latch_num();
  for (ymuint i = 0; i < nlatch; ++ i) {
    const BnLatch* src_latch = src.latch(i);
    string latch_name = src_latch->name();

    bool has_clear = (src_latch->clear() != kBnNullId);
    bool has_preset = (src_latch->preset() != kBnNullId);
    BnLatch* dst_latch = new_latch(latch_name, has_clear, has_preset);

    ASSERT_COND( dst_latch->id() == src_latch->id() );

    // 各端子の対応関係を記録する．
    {
      ymuint src_id = src_latch->input();
      ymuint dst_id = dst_latch->input();
      id_map.add(src_id, dst_id);
    }
    {
      ymuint src_id = src_latch->output();
      ymuint dst_id = dst_latch->output();
      id_map.add(src_id, dst_id);
    }
    {
      ymuint src_id = src_latch->enable();
      ymuint dst_id = dst_latch->enable();
      id_map.add(src_id, dst_id);
    }
    if ( has_clear ) {
      ymuint src_id = src_latch->clear();
      ymuint dst_id = dst_latch->clear();
      id_map.add(src_id, dst_id);
    }
    if ( has_preset ) {
      ymuint src_id = src_latch->preset();
      ymuint dst_id = dst_latch->preset();
      id_map.add(src_id, dst_id);
    }
  }

  ASSERT_COND( src.input_num() == input_num() );
  ASSERT_COND( src.output_num() == output_num() );

  // 関数情報の生成
  ymuint nfunc = src.mFuncList.size();
  for (ymuint i = 0; i < nfunc; ++ i) {
    const TvFunc& func = src.mFuncList[i];
    ymuint func_id = _add_tv(func);
    ASSERT_COND( func_id == i );
  }

  // 論理式情報の生成
  ymuint nexpr = src.mExprList.size();
  mExprList.resize(nexpr);
  for (ymuint i = 0; i < nexpr; ++ i) {
    const Expr& expr = src.mExprList[i];
    ymuint expr_id = _add_expr(expr);
    ASSERT_COND( expr_id == i );
  }

  // 論理ノードの生成
  ymuint nl = src.logic_num();
  for (ymuint i = 0; i < nl; ++ i) {
    const BnNode* src_node = src.logic(i);
    ymuint nfi = src_node->fanin_num();
    string name = src_node->name();
    BnNodeType logic_type = src_node->type();
    ymuint expr_id = src_node->expr_id();
    ymuint func_id = src_node->func_id();
    const ClibCell* cell = src_node->cell();
    ymuint dst_id = kBnNullId;
    if ( logic_type == kBnLogic_EXPR ) {
      dst_id = _new_expr(name, nfi, src.expr(expr_id), cell);
    }
    else if ( logic_type == kBnLogic_TV ) {
      dst_id = _new_tv(name, nfi, src.func(func_id), cell);
    }
    else {
      dst_id = _new_primitive(name, nfi, logic_type, cell);
    }
    id_map.add(src_node->id(), dst_id);
    for (ymuint i = 0; i < nfi; ++ i) {
      ymuint src_iid = src_node->fanin(i);
      ymuint iid = conv_id(src_iid, id_map);
      connect(iid, dst_id, i);
    }
  }

  // 出力端子のファンインの接続
  ymuint npo = output_num();
  for (ymuint i = 0; i < npo; ++ i) {
    const BnNode* src_node = src.output(i);
    ymuint src_id = src_node->id();
    ymuint src_fanin_id = src_node->fanin();

    ymuint dst_id = conv_id(src_id, id_map);
    ymuint dst_fanin_id = conv_id(src_fanin_id, id_map);
    connect(dst_fanin_id, dst_id, 0);
  }

  bool stat = wrap_up();

  ASSERT_COND( stat );
}

// @brief 関連するセルライブラリを得る．
//
// 場合によっては空の場合もある．
const ClibCellLibrary&
BnNetwork::library() const
{
  return mCellLibrary;
}

// @brief ネットワーク名を得る．
string
BnNetwork::name() const
{
  return mName;
}

// @brief ポート数を得る．
ymuint
BnNetwork::port_num() const
{
  return mPortList.size();
}

// @brief ポートの情報を得る．
// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
const BnPort*
BnNetwork::port(ymuint pos) const
{
  ASSERT_COND( pos < port_num() );
  return mPortList[pos];
}

// @brief DFF数を得る．
ymuint
BnNetwork::dff_num() const
{
  return mDffList.size();
}

// @brief DFFを返す．
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
const BnDff*
BnNetwork::dff(ymuint pos) const
{
  ASSERT_COND( pos < dff_num() );
  return mDffList[pos];
}

// @brief ラッチ数を得る．
ymuint
BnNetwork::latch_num() const
{
  return mLatchList.size();
}

// @brief ラッチを得る．
// @param[in] pos 位置番号 ( 0 <= pos < latch_num() )
const BnLatch*
BnNetwork::latch(ymuint pos) const
{
  ASSERT_COND( pos < latch_num() );
  return mLatchList[pos];
}

// @brief ノード数を得る．
ymuint
BnNetwork::node_num() const
{
  return mNodeList.size();
}

// @brief ノードを得る．
// @param[in] id ノード番号 ( 0 <= id < node_num() )
//
// BnNode* node = BnNetwork::node(id);
// node->id() == id が成り立つ．
const BnNode*
BnNetwork::node(ymuint id) const
{
  ASSERT_COND( id < node_num() );
  return mNodeList[id];
}

// @brief 入力数を得る．
ymuint
BnNetwork::input_num() const
{
  return mInputList.size();
}

// @brief 入力ノードのノード番号を得る．
// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
const BnNode*
BnNetwork::input(ymuint pos) const
{
  ASSERT_COND( pos < input_num() );
  return mInputList[pos];
}

// @brief 出力数を得る．
ymuint
BnNetwork::output_num() const
{
  return mOutputList.size();
}

// @brief 出力ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
const BnNode*
BnNetwork::output(ymuint pos) const
{
  ASSERT_COND( pos < output_num() );
  return mOutputList[pos];
}

// @brief 論理ノード数を得る．
ymuint
BnNetwork::logic_num() const
{
  return mLogicList.size();
}

// @brief 論理ノードのノード番号を得る．
// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
const BnNode*
BnNetwork::logic(ymuint pos) const
{
  ASSERT_COND( pos < logic_num() );
  return mLogicList[pos];
}

// @brief 関数の数を得る．
ymuint
BnNetwork::func_num() const
{
  return mFuncList.size();
}

// @brief 関数番号から関数を得る．
// @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
const TvFunc&
BnNetwork::func(ymuint func_id) const
{
  ASSERT_COND( func_id < func_num() );
  return mFuncList[func_id];
}

// @brief 論理式の数を得る．
ymuint
BnNetwork::expr_num() const
{
  return mExprList.size();
}

// @brief 論理式番号から論理式を得る．
// @param[in] expr_id 論理式番号 ( 0 <= expr_id < expr_num() )
Expr
BnNetwork::expr(ymuint expr_id) const
{
  ASSERT_COND( expr_id < expr_num() );
  return mExprList[expr_id];
}

// @brief 内容を出力する．
// @param[in] s 出力先のストリーム
//
// 形式は独自フォーマット
void
BnNetwork::write(ostream& s) const
{
  s << "network name : " << name() << endl
    << endl;

  ymuint np = port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const BnPort* port = this->port(i);
    s << "port#" << i << ": ";
    s << "(" << port->name() << ") : ";
    ymuint bw = port->bit_width();
    for (ymuint j = 0; j < bw; ++ j) {
      s << " " << port->bit(j);
    }
    s << endl;
  }
  s << endl;

  ymuint ni = input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const BnNode* node = input(i);
    ASSERT_COND( node != nullptr );
    ASSERT_COND( node->type() == kBnInput );
    s << "input#" << i << ": " << node->id()
      << "(" << node->name() << ")" << endl;
  }
  s << endl;

  ymuint no = output_num();
  for (ymuint i = 0; i < no; ++ i) {
    const BnNode* node = output(i);
    ASSERT_COND( node != nullptr );
    ASSERT_COND( node->type() == kBnOutput );
    s << "output#" << i << ": " << node->id()
      << "(" << node->name() << ")" << endl
      << "    input: " << node->fanin() << endl;
  }
  s << endl;

  ymuint nf = dff_num();
  for (ymuint i = 0; i < nf; ++ i) {
    const BnDff* node = dff(i);
    ASSERT_COND( node != nullptr );
    s << "dff#" << i
      << "(" << node->name() << ")" << endl
      << "    input:  " << node->input() << endl
      << "    output: " << node->output() << endl
      << "    clock:  " << node->clock() << endl;
    if ( node->clear() != kBnNullId ) {
      s << "    clear:  " << node->clear() << endl;
    }
    if ( node->preset() != kBnNullId ) {
      s << "    preset: " << node->preset() << endl;
    }
    s << endl;
  }
  s << endl;

  ymuint nlatch = latch_num();
  for (ymuint i = 0; i < nlatch; ++ i) {
    const BnLatch* node = latch(i);
    ASSERT_COND( node != nullptr );
    s << "latch#" << i
      << "(" << node->name() << ")" << endl
      << "    input:  " << node->input() << endl
      << "    output: " << node->output() << endl
      << "    enable: " << node->enable() << endl;
    if ( node->clear() != kBnNullId ) {
      s << "    clear:  " << node->clear() << endl;
    }
    if ( node->preset() != kBnNullId ) {
      s << "    preset: " << node->preset() << endl;
    }
  }
  s << endl;

  ymuint nl = logic_num();
  for (ymuint i = 0; i < nl; ++ i) {
    const BnNode* node = logic(i);
    ASSERT_COND( node != nullptr );
    ASSERT_COND( node->is_logic() );
    s << "logic#" << i << ": " << node->id()
      << "(" << node->name() << ")" << endl
      << "    fanins: ";
    ymuint ni = node->fanin_num();
    for (ymuint j = 0; j < ni; ++ j) {
      s << " " << node->fanin(j);
    }
    s << endl;
    s << "    ";
    switch ( node->type() ) {
    case kBnLogic_NONE:
      s << "NONE";
      break;
    case kBnLogic_C0:
      s << "C0";
      break;
    case kBnLogic_C1:
      s << "C1";
      break;
    case kBnLogic_BUFF:
      s << "BUFF";
      break;
    case kBnLogic_NOT:
      s << "NOT";
      break;
    case kBnLogic_AND:
      s << "AND";
      break;
    case kBnLogic_NAND:
      s << "NAND";
      break;
    case kBnLogic_OR:
      s << "OR";
      break;
    case kBnLogic_NOR:
      s << "NOR";
      break;
    case kBnLogic_XOR:
      s << "XOR";
      break;
    case kBnLogic_XNOR:
      s << "XNOR";
      break;
    case kBnLogic_EXPR:
      s << "expr#" << node->expr_id() << ": " << node->expr();
      break;
    case kBnLogic_TV:
      s << "func#" << node->func_id() << ": " << node->func();
      break;
    default:
      ASSERT_NOT_REACHED;
    }
    s << endl;
    if ( node->cell() ) {
      s << "    cell: " << node->cell()->name() << endl;
    }
    s << endl;
  }

  s << endl;
}

// @brief セルライブラリをセットする．
// @param[in] library ライブラリ
void
BnNetwork::set_library(const ClibCellLibrary& library)
{
  mCellLibrary = library;
}

// @brief ネットワーク名を設定する．
// @param[in] name ネットワーク名
void
BnNetwork::set_name(const string& name)
{
  mName = name;
}

// @brief 1ビットの入力ポートを作る．
// @param[in] port_name ポート名
// @return 生成したポートを返す．
BnPort*
BnNetwork::new_input_port(const string& port_name)
{
  return new_port(port_name, vector<int>(1, 0));
}

// @brief 多ビットの入力ポートを作る．
// @param[in] port_name ポート名
// @param[in] bit_width ビット幅
// @return 生成したポートを返す．
BnPort*
BnNetwork::new_input_port(const string& port_name,
			  ymuint bit_width)
{
  return new_port(port_name, vector<int>(bit_width, 0));
}

// @brief 1ビットの出力ポートを作る．
// @param[in] port_name ポート名
// @return 生成したポートを返す．
BnPort*
BnNetwork::new_output_port(const string& port_name)
{
  return new_port(port_name, vector<int>(1, 1));
}

// @brief 多ビットの出力ポートを作る．
// @param[in] port_name ポート名
// @param[in] bit_width ビット幅
// @return 生成したポートを返す．
BnPort*
BnNetwork::new_output_port(const string& port_name,
			   ymuint bit_width)
{
  return new_port(port_name, vector<int>(bit_width, 1));
}

// @brief 入出力混合のポートを作る．
// @param[in] port_name ポート名
// @param[in] dir_vect 向きを表すベクタ
// @return 生成したポートを返す．
//
// dir_vect[i] == 0 の時，入力を表す．
BnPort*
BnNetwork::new_port(const string& port_name,
		    const vector<int>& dir_vect)
{
  ymuint port_id = mPortList.size();
  ymuint bit_width = dir_vect.size();
  vector<ymuint> bits(bit_width);
  for (ymuint i = 0; i < bit_width; ++ i) {
    ymuint node_id = mNodeList.size();
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
      ymuint input_id = mInputList.size();
      BnNodeImpl* node = new BnPortInput(node_id, node_name, input_id, port_id, i);
      mNodeList.push_back(node);
      mInputList.push_back(node);
    }
    else {
      ymuint output_id = mOutputList.size();
      BnNodeImpl* node = new BnPortOutput(node_id, node_name, output_id, port_id, i);
      mNodeList.push_back(node);
      mOutputList.push_back(node);
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

  return port;
}

// @brief DFFを追加する．
// @param[in] name DFF名
// @param[in] has_xoutput 反転出力端子を持つ時 true にする．
// @param[in] has_clear クリア端子を持つ時 true にする．
// @param[in] has_preset プリセット端子を持つ時 true にする．
// @return 生成したDFFを返す．
//
// 名前の重複に関しては感知しない．
BnDff*
BnNetwork::new_dff(const string& name,
		   bool has_xoutput,
		   bool has_clear,
		   bool has_preset)
{
  return _new_dff(name, has_xoutput, has_clear, has_preset, nullptr);
}

// @brief セルの情報を持ったDFFを追加する．
// @param[in] name DFF名
// @param[in] cell_name 対応するセル名
// @return 生成したDFFを返す．
//
// - 名前の重複に関しては感知しない．
// - セルは FF のセルでなければならない．
BnDff*
BnNetwork::new_dff_cell(const string& name,
			const string& cell_name)
{
  const ClibCell* cell = mCellLibrary.cell(cell_name);
  if ( cell == nullptr || !cell->is_ff() ) {
    return nullptr;
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
// @return 生成したDFFを返す．
//
// 名前の重複に関しては感知しない．
BnDff*
BnNetwork::_new_dff(const string& name,
		    bool has_xoutput,
		    bool has_clear,
		    bool has_preset,
		    const ClibCell* cell)
{
  ymuint dff_id = mDffList.size();

  ymuint input_id = mNodeList.size();
  {
    ymuint oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".input";
    string name = buf.str();
    BnNodeImpl* node = new BnDffInput(input_id, name, oid, dff_id);
    mNodeList.push_back(node);
    mOutputList.push_back(node);
  }

  ymuint output_id = mNodeList.size();
  {
    ymuint iid = mInputList.size();
    ostringstream buf;
    buf << name << ".output";
    string name = buf.str();
    BnNodeImpl* node = new BnDffOutput(output_id, name, iid, dff_id);
    mNodeList.push_back(node);
    mInputList.push_back(node);
  }

  ymuint xoutput_id = kBnNullId;
  if ( has_xoutput ) {
    ymuint xoutput_id = mNodeList.size();
    ymuint iid = mInputList.size();
    ostringstream buf;
    buf << name << ".xoutput";
    string name = buf.str();
    BnNodeImpl* node = new BnDffXOutput(output_id, name, iid, dff_id);
    mNodeList.push_back(node);
    mInputList.push_back(node);
  }

  ymuint clock_id = mNodeList.size();
  {
    ymuint oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".clock";
    string name = buf.str();
    BnNodeImpl* node = new BnDffClock(clock_id, name, oid, dff_id);
    mNodeList.push_back(node);
    mOutputList.push_back(node);
  }

  ymuint clear_id = kBnNullId;
  if ( has_clear ) {
    clear_id = mNodeList.size();
    ymuint oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".clear";
    string name = buf.str();
    BnNodeImpl* node = new BnDffClear(clear_id, name, oid, dff_id);
    mNodeList.push_back(node);
    mOutputList.push_back(node);
  }

  ymuint preset_id = kBnNullId;
  if ( has_preset ) {
    preset_id = mNodeList.size();
    ymuint oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".preset";
    BnNodeImpl* node = new BnDffPreset(preset_id, name, oid, dff_id);
    mNodeList.push_back(node);
    mOutputList.push_back(node);
  }

  BnDff* dff = new BnDffImpl(dff_id, name, input_id, output_id, xoutput_id,
			     clock_id, clear_id, preset_id, cell);
  mDffList.push_back(dff);

  return dff;
}

// @brief ラッチを追加する．
// @param[in] name ラッチ名
// @param[in] has_clear クリア端子を持つ時 true にする．
// @param[in] has_preset プリセット端子を持つ時 true にする．
// @return 生成したラッチを返す．
//
// 名前の重複に関しては感知しない．
BnLatch*
BnNetwork::new_latch(const string& name,
		     bool has_clear,
		     bool has_preset)
{
  return _new_latch(name, has_clear, has_preset, nullptr);
}

// @brief セルの情報を持ったラッチを追加する．
// @param[in] name ラッチ名
// @param[in] cell_name 対応するセル名．
// @return 生成したラッチを返す．
//
// - 名前の重複に関しては感知しない．
// - セルはラッチのセルでなければならない．
BnLatch*
BnNetwork::new_latch_cell(const string& name,
			  const string& cell_name)
{
  const ClibCell* cell = mCellLibrary.cell(cell_name);
  if ( cell == nullptr || !cell->is_latch() ) {
    return nullptr;
  }

  ClibLatchInfo latchinfo = cell->latch_info();

  bool has_clear = latchinfo.has_clear();
  bool has_preset = latchinfo.has_preset();
  return _new_latch(name, has_clear, has_preset, cell);
}

// @brief ラッチを追加する共通の処理を行う関数
// @param[in] name ラッチ名
// @param[in] has_clear クリア端子を持つ時 true にする．
// @param[in] has_preset プリセット端子を持つ時 true にする．
// @param[in] cell 対応するセル．
// @return 生成したラッチを返す．
//
// 名前の重複に関しては感知しない．
// cell はラッチのセルでなければならない．
BnLatch*
BnNetwork::_new_latch(const string& name,
		      bool has_clear,
		      bool has_preset,
		      const ClibCell* cell)
{
  ymuint latch_id = mLatchList.size();

  ymuint input_id = mNodeList.size();
  {
    ymuint oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".input";
    string name = buf.str();
    BnNodeImpl* node = new BnLatchInput(input_id, name, oid, latch_id);
    mNodeList.push_back(node);
    mOutputList.push_back(node);
  }

  ymuint output_id = mNodeList.size();
  {
    ymuint iid = mInputList.size();
    ostringstream buf;
    buf << name << ".output";
    string name = buf.str();
    BnNodeImpl* node = new BnLatchOutput(output_id, name, iid, latch_id);
    mNodeList.push_back(node);
    mInputList.push_back(node);
  }

  ymuint enable_id = mNodeList.size();
  {
    ymuint oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".enable";
    string name = buf.str();
    BnNodeImpl* node = new BnLatchEnable(enable_id, name, oid, latch_id);
    mNodeList.push_back(node);
    mOutputList.push_back(node);
  }

  ymuint clear_id = kBnNullId;
  if ( has_clear ) {
    clear_id = mNodeList.size();
    ymuint oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".clear";
    string name = buf.str();
    BnNodeImpl* node = new BnLatchClear(clear_id, name, oid, latch_id);
    mNodeList.push_back(node);
    mOutputList.push_back(node);
  }

  ymuint preset_id = kBnNullId;
  if ( has_preset ) {
    preset_id = mNodeList.size();
    ymuint oid = mOutputList.size();
    ostringstream buf;
    buf << name << ".preset";
    BnNodeImpl* node = new BnLatchPreset(preset_id, name, oid, latch_id);
    mNodeList.push_back(node);
    mOutputList.push_back(node);
  }

  BnLatch* latch = new BnLatchImpl(latch_id, name, input_id, output_id,
				   enable_id, clear_id, preset_id, cell);
  mLatchList.push_back(latch);

  return latch;
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @param[in] logic_type 論理型
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
// - logic_type は BnNodeType のうち論理プリミティブを表すもののみ
ymuint
BnNetwork::new_primitive(const string& node_name,
			 ymuint ni,
			 BnNodeType logic_type)
{
  return _new_primitive(node_name, ni, logic_type, nullptr);
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @param[in] expr 論理式
// @return 生成した論理ノードの番号を返す．
//
// - ノード名の重複に関しては感知しない．
ymuint
BnNetwork::new_expr(const string& node_name,
		    ymuint ni,
		    const Expr& expr)
{
  BnNodeType logic_type = FuncAnalyzer::analyze(expr);
  if ( logic_type != kBnLogic_EXPR ) {
    // 組み込み型だった．
    return _new_primitive(node_name, ni, logic_type, nullptr);
  }
  return _new_expr(node_name, ni, expr, nullptr);
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @param[in] tv 真理値表
// @return 生成した論理ノードを返す．
//
// ノード名の重複に関しては感知しない．
ymuint
BnNetwork::new_tv(const string& node_name,
		  ymuint ni,
		  const TvFunc& tv)
{
  BnNodeType logic_type = FuncAnalyzer::analyze(tv);
  if ( logic_type != kBnLogic_TV ) {
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
ymuint
BnNetwork::new_logic_cell(const string& node_name,
			  const string& cell_name)
{
  const ClibCell* cell = mCellLibrary.cell(cell_name);
  if ( cell == nullptr ||
       cell->is_logic() ||
       cell->output_num() != 1 ) {
    return kBnNullId;
  }

  ymuint ni = cell->input_num();
  Expr expr = cell->logic_expr(0);
  BnNodeType logic_type = FuncAnalyzer::analyze(expr);
  if ( logic_type != kBnLogic_EXPR ) {
    // 組み込み型だった．
    return _new_primitive(node_name, ni, logic_type, cell);
  }
  else {
    return _new_expr(node_name, ni, expr, cell);
  }
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @param[in] logic_type 論理型
// @param[in] cell 対応するセル
// @return 生成した論理ノードの番号を返す．
ymuint
BnNetwork::_new_primitive(const string& node_name,
			  ymuint ni,
			  BnNodeType logic_type,
			  const ClibCell* cell)
{
  ymuint id = mNodeList.size();
  BnNodeImpl* node = new BnPrimNode(id, node_name, ni, logic_type, cell);
  mNodeList.push_back(node);
  mLogicList.push_back(node);

  return node->id();
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @param[in] expr 論理式
// @param[in] cell 対応するセル
// @return 生成した論理ノードの番号を返す．
ymuint
BnNetwork::_new_expr(const string& node_name,
		     ymuint ni,
		     const Expr& expr,
		     const ClibCell* cell)
{
  ymuint expr_id = _add_expr(expr);
  ymuint id = mNodeList.size();
  BnNodeImpl* node = new BnExprNode(id, node_name, ni, expr, expr_id, cell);
  mNodeList.push_back(node);
  mLogicList.push_back(node);

  return node->id();
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni 入力数
// @param[in] tv 真理値表
// @param[in] cell 対応するセル
// @return 生成した論理ノードの番号を返す．
ymuint
BnNetwork::_new_tv(const string& node_name,
		   ymuint ni,
		   const TvFunc& tv,
		   const ClibCell* cell)
{
  ymuint func_id = _add_tv(tv);
  ymuint id = mNodeList.size();
  BnNodeImpl* node = new BnTvNode(id, node_name, ni, tv, func_id, cell);
  mNodeList.push_back(node);
  mLogicList.push_back(node);

  return node->id();
}

// @brief ノード間を接続する．
// @param[in] src_id ファンアウト元のノード番号
// @param[in] dst_id ファンイン先のノード番号
// @param[in] ipos ファンインの位置
void
BnNetwork::connect(ymuint src_id,
		   ymuint dst_id,
		   ymuint ipos)
{
  ASSERT_COND( src_id != kBnNullId );
  ASSERT_COND( dst_id != kBnNullId );
  BnNodeImpl* src_node = mNodeList[src_id];
  BnNodeImpl* dst_node = mNodeList[dst_id];
  dst_node->set_fanin(ipos, src_id);
  src_node->add_fanout(dst_id);

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
BnNetwork::wrap_up()
{
  if ( mSane ) {
    return true;
  }

  bool error = false;

  // ポートのチェック
  for (ymuint i = 0; i < port_num(); ++ i) {
    const BnPort* port = mPortList[i];
    ymuint nb = port->bit_width();
    for (ymuint j = 0; j < nb; ++ j) {
      ymuint id = port->bit(j);
      if ( id == kBnNullId || id >= node_num() ) {
	cerr << "Port#" << i << "(" << port->name() << ").bit["
	     << j << "] is not set" << endl;
	error = true;
      }
    }
  }

  // DFF のチェック
  for (ymuint i = 0; i < dff_num(); ++ i) {
    const BnDff* dff = mDffList[i];
    ymuint id1 = dff->input();
    if ( id1 == kBnNullId ) {
      cerr << "DFF#" << i << "(" << dff->name() << ").input is not set" << endl;
      error = true;
    }
    else if ( id1 >= node_num() ) {
      cerr << "DFF#" << i << "(" << dff->name() << ").input is not valid" << endl;
      error = true;
    }
    ymuint id2 = dff->output();
    if ( id2 == kBnNullId ) {
      cerr << "DFF#" << i << "(" << dff->name() << ").output is not set" << endl;
      error = true;
    }
    else if ( id2 >= node_num() ) {
      cerr << "DFF#" << i << "(" << dff->name() << ").output is not valid" << endl;
      error = true;
    }
    ymuint id3 = dff->clock();
    if ( id3 == kBnNullId ) {
      cerr << "DFF#" << i << "(" << dff->name() << ").clock is not set" << endl;
      error = true;
    }
    else if ( id3 >= node_num() ) {
      cerr << "DFF#" << i << "(" << dff->name() << ").clock is not valid" << endl;
      error = true;
    }
    ymuint id4 = dff->clear();
    if ( id4 != kBnNullId && id4 >= node_num() ) {
      cerr << "DFF#" << i << "(" << dff->name() << ").clear is not valid" << endl;
      error = true;
    }
    ymuint id5 = dff->preset();
    if ( id5 != kBnNullId && id5 >= node_num() ) {
      cerr << "DFF#" << i << "(" << dff->name() << ").preset is not valid" << endl;
      error = true;
    }
  }

  // ラッチのチェック
  for (ymuint i = 0; i < latch_num(); ++ i) {
    const BnLatch* latch = mLatchList[i];
    ymuint id1 = latch->input();
    if ( id1 == kBnNullId ) {
      cerr << "LATCH#" << i << "(" << latch->name() << ").input is not set" << endl;
      error = true;
    }
    else if ( id1 >= node_num() ) {
      cerr << "LATCH#" << i << "(" << latch->name() << ").input is not valid" << endl;
      error = true;
    }
    ymuint id2 = latch->output();
    if ( id2 == kBnNullId ) {
      cerr << "LATCH#" << i << "(" << latch->name() << ").output is not set" << endl;
      error = true;
    }
    else if ( id2 >= node_num() ) {
      cerr << "LATCH#" << i << "(" << latch->name() << ").output is not valid" << endl;
      error = true;
    }
    ymuint id3 = latch->enable();
    if ( id3 == kBnNullId ) {
      cerr << "LATCH#" << i << "(" << latch->name() << ").enable is not set" << endl;
      error = true;
    }
    else if ( id3 >= node_num() ) {
      cerr << "LATCH#" << i << "(" << latch->name() << ").enable is not valid" << endl;
      error = true;
    }
    ymuint id4 = latch->clear();
    if ( id4 != kBnNullId && id4 >= node_num() ) {
      cerr << "LATCH#" << i << "(" << latch->name() << ").clear is not valid" << endl;
      error = true;
    }
    ymuint id5 = latch->preset();
    if ( id5 != kBnNullId && id5 >= node_num() ) {
      cerr << "LATCH#" << i << "(" << latch->name() << ").preset is not valid" << endl;
      error = true;
    }
  }

  // ノードのチェック
  for (ymuint i = 0; i < node_num(); ++ i) {
    const BnNode* node = mNodeList[i];
    ASSERT_COND( node->id() == i );
    ymuint ni = node->fanin_num();
    for (ymuint j = 0; j < ni; ++ j) {
      ymuint id = node->fanin(j);
      if ( id == kBnNullId ) {
	cerr << "NODE#" << i << "(" << node->name() << ").fanin["
	     << j << "] is not set" << endl;
	error = true;
      }
      else if ( id >= node_num() ) {
	cerr << "NODE#" << i << "(" << node->name() << ").fanin["
	     << j << "] is not valid" << endl;
	error = true;
      }
    }
  }

  if ( error ) {
    return false;
  }

  // 論理ノードをトポロジカル順にソートする．

  // ノード番号を入れるキュー
  vector<ymuint> queue;
  queue.reserve(node_num());

  // キューの印を表すマーク配列
  vector<bool> mark(node_num() + 1, false);

  // 入力ノードをキューに積む．
  for (ymuint i = 0; i < input_num(); ++ i) {
    const BnNode* node = input(i);
    ymuint id = node->id();
    queue.push_back(id);
    mark[id] = true;
  }

  mLogicList.clear();
  mLogicList.reserve(node_num() - input_num() - output_num());

  // キューからノードを取り出してファンアウト先のノードをキューに積む．
  for (ymuint rpos = 0; rpos < queue.size(); ++ rpos) {
    ymuint id = queue[rpos];
    BnNode* node = mNodeList[id];
    if ( node->is_logic() ) {
      mLogicList.push_back(node);
    }
    ymuint fo = node->fanout_num();
    for (ymuint i = 0; i < fo; ++ i) {
      ymuint oid = node->fanout(i);
      if ( mark[oid] ) {
	continue;
      }
      const BnNode* onode = mNodeList[oid];
      ymuint ni = onode->fanin_num();
      bool ready = true;
      for (ymuint j = 0; j < ni; ++ j) {
	if ( !mark[onode->fanin(j)] ) {
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

  mSane = true;

  return true;
}

// @brief 論理式を登録する．
// @param[in] expr 論理式
// @return 関数番号を返す．
ymuint
BnNetwork::_add_expr(const Expr& expr)
{
  ymuint ni = expr.input_size();
  ymuint expr_id = 0;
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
ymuint
BnNetwork::_add_tv(const TvFunc& tv)
{
  // 同じ関数が登録されていないか調べる．
  ymuint func_id;
  if ( !mFuncMap.find(tv, func_id) ) {
    // 新たに登録する．
    func_id = mFuncList.size();
    mFuncList.push_back(tv);
    mFuncMap.add(tv, func_id);
  }
  return func_id;
}

END_NAMESPACE_YM_BNET
