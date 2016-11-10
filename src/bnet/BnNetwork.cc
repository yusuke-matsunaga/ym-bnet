
/// @file BnNetwork.cc
/// @brief BnNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNetwork.h"
#include "ym/BnBuilder.h"
#include "ym/Cell.h"

#include "BnPortImpl.h"
#include "BnNodeImpl.h"
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
}

// @brief コピーコンストラクタ
// @param[in] src コピー元のオブジェクト
BnNetwork::BnNetwork(const BnNetwork& src)
{
  copy(src);
}

// @brief BnBuilder からのコンストラクタ
// @param[in] src コピー元のBnBuilder
BnNetwork::BnNetwork(const BnBuilder& src)
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

  // ネットワーク名の設定
  set_name(src.name());

  // srcのノード番号をキーにしてノード番号を格納するハッシュ表
  HashMap<ymuint, ymuint> id_map;

  // 外部入力の生成
  ymuint ni = src.input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const BnNode* src_node = src.input(i);
    BnNode* dst_node = new_input(src_node->name());
    id_map.add(src_node->id(), dst_node->id());
  }

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
    vector<ymuint> inode_id_list(nfi);
    for (ymuint i = 0; i < nfi; ++ i) {
      ymuint src_iid = src_node->fanin(i);
      ymuint iid = conv_id(src_iid, id_map);
      inode_id_list[i] = iid;
    }
    string name = src_node->name();
    BnLogicType logic_type = src_node->logic_type();
    ymuint expr_id = src_node->expr_id();
    ymuint func_id = src_node->func_id();
    const Cell* cell = src_node->cell();
    BnNode* dst_node = nullptr;
    if ( logic_type == kBnLt_EXPR ) {
      dst_node = new_expr(name, inode_id_list, src.expr(expr_id), cell);
    }
    else if ( logic_type == kBnLt_TV ) {
      dst_node = new_tv(name, inode_id_list, src.func(func_id), cell);
    }
    else {
      dst_node = new_primitive(name, inode_id_list, logic_type, cell);
    }
    id_map.add(src_node->id(), dst_node->id());
  }

  // 外部出力の生成
  ymuint no = src.output_num();
  for (ymuint i = 0; i < no; ++ i) {
    const BnNode* src_node = src.output(i);
    ymuint src_iid = src_node->input();
    ymuint iid = conv_id(src_iid, id_map);
    BnNode* dst_node = new_output(src_node->name(), iid);
    id_map.add(src_node->id(), dst_node->id());
  }

  // ポートの生成
  ymuint np = src.port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const BnPort* src_port = src.port(i);
    ymuint nb = src_port->bit_width();
    vector<ymuint> bits(nb);
    for (ymuint j = 0; j < nb; ++ j) {
      ymuint src_id = src_port->bit(j);
      bits[j] = conv_id(src_id, id_map);
    }
    new_port(src_port->name(), bits);
  }

  // DFFの生成
  ymuint nff = src.dff_num();
  for (ymuint i = 0; i < nff; ++ i) {
    const BnDff* src_dff = src.dff(i);
    ymuint input = conv_id(src_dff->input(), id_map);
    ymuint output = conv_id(src_dff->output(), id_map);
    ymuint clock = conv_id(src_dff->clock(), id_map);
    ymuint clear = conv_id(src_dff->clear(), id_map);
    ymuint preset = conv_id(src_dff->preset(), id_map);
    new_dff(src_dff->name(), input, output, clock, clear, preset);
  }

  // ラッチの生成
  ymuint nlatch = src.latch_num();
  for (ymuint i = 0; i < nlatch; ++ i) {
    const BnLatch* src_latch = src.latch(i);
    ymuint input = conv_id(src_latch->input(), id_map);
    ymuint output = conv_id(src_latch->output(), id_map);
    ymuint enable = conv_id(src_latch->enable(), id_map);
    ymuint clear = conv_id(src_latch->clear(), id_map);
    ymuint preset = conv_id(src_latch->preset(), id_map);
    new_latch(src_latch->name(), input, output, enable, clear, preset);
  }
}

// @brief 内容をコピーする．
// @param[in] src コピー元のBnBuilder
void
BnNetwork::copy(const BnBuilder& src)
{
  clear();

  // ネットワーク名の設定
  set_name(src.name());

  // ノード番号をキーにしてノード番号を格納するハッシュ表
  HashMap<ymuint, ymuint> id_map;

  // 外部入力の生成
  ymuint ni = src.input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const BnBuilder::NodeInfo& src_node = src.input(i);
    BnNode* dst_node = new_input(src_node.mName);
    id_map.add(src_node.mId, dst_node->id());
  }

  // 関数情報の生成
  ymuint nfunc = src.func_num();
  for (ymuint i = 0; i < nfunc; ++ i) {
    const TvFunc& func = src.func(i);
    ymuint func_id = _add_tv(func);
    ASSERT_COND( func_id == i );
  }

  // 論理式情報の生成
  ymuint nexpr = src.expr_num();
  mExprList.resize(nexpr);
  for (ymuint i = 0; i < nexpr; ++ i) {
    const Expr& expr = src.expr(i);
    ymuint expr_id = _add_expr(expr);
    ASSERT_COND( expr_id == i );
  }

  // 論理ノードの生成
  ymuint nl = src.logic_num();
  for (ymuint i = 0; i < nl; ++ i) {
    const BnBuilder::NodeInfo& src_node = src.logic(i);
    ymuint nfi = src_node.mFaninList.size();
    vector<ymuint> inode_id_list(nfi);
    for (ymuint i = 0; i < nfi; ++ i) {
      ymuint src_iid = src_node.mFaninList[i];
      inode_id_list[i] = conv_id(src_iid, id_map);
    }
    string name = src_node.mName;
    BnLogicType logic_type = src_node.mLogicType;
    ymuint func_id = src_node.mFuncId;
    const Cell* cell = src_node.mCell;
    BnNode* dst_node = nullptr;
    if ( logic_type == kBnLt_EXPR ) {
      dst_node = new_expr(name, inode_id_list, src.expr(func_id), cell);
    }
    else if ( logic_type == kBnLt_TV ) {
      dst_node = new_tv(name, inode_id_list, src.func(func_id), cell);
    }
    else {
      dst_node = new_primitive(name, inode_id_list, logic_type, cell);
    }
    id_map.add(src_node.mId, dst_node->id());
  }

  // 外部出力の生成
  ymuint no = src.output_num();
  for (ymuint i = 0; i < no; ++ i) {
    const BnBuilder::NodeInfo& src_node = src.output(i);
    ymuint iid = conv_id(src_node.mFaninList[0], id_map);
    BnNode* dst_node = new_output(src_node.mName, iid);
    id_map.add(src_node.mId, dst_node->id());
  }

  // ポートの生成
  ymuint np = src.port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const BnBuilder::PortInfo& src_port = src.port(i);
    ymuint nb = src_port.mBits.size();
    vector<ymuint> bits(nb);
    for (ymuint j = 0; j < nb; ++ j) {
      ymuint src_id = src_port.mBits[j];
      bits[j] = conv_id(src_id, id_map);
    }
    new_port(src_port.mName, bits);
  }

  // DFFの生成
  ymuint nff = src.dff_num();
  for (ymuint i = 0; i < nff; ++ i) {
    const BnBuilder::DffInfo& src_dff = src.dff(i);
    ymuint input = conv_id(src_dff.mInput, id_map);
    ymuint output = conv_id(src_dff.mOutput, id_map);
    ymuint clock = conv_id(src_dff.mClock, id_map);
    ymuint clear = conv_id(src_dff.mClear, id_map);
    ymuint preset = conv_id(src_dff.mPreset, id_map);
    new_dff(src_dff.mName, input, output, clock, clear, preset);
  }

  // ラッチの生成
  ymuint nlatch = src.latch_num();
  for (ymuint i = 0; i < nlatch; ++ i) {
    const BnBuilder::LatchInfo& src_latch = src.latch(i);
    ymuint input = conv_id(src_latch.mInput, id_map);
    ymuint output = conv_id(src_latch.mOutput, id_map);
    ymuint enable = conv_id(src_latch.mEnable, id_map);
    ymuint clear = conv_id(src_latch.mClear, id_map);
    ymuint preset = conv_id(src_latch.mPreset, id_map);
    new_latch(src_latch.mName, input, output, enable, clear, preset);
  }
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
      << "    input: " << node->input() << endl;
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
    ASSERT_COND( node->type() == kBnLogic );
    s << "logic#" << i << ": " << node->id()
      << "(" << node->name() << ")" << endl
      << "    fanins: ";
    ymuint ni = node->fanin_num();
    for (ymuint j = 0; j < ni; ++ j) {
      s << " " << node->fanin(j);
    }
    s << endl;
    s << "    ";
    switch ( node->logic_type() ) {
    case kBnLt_NONE:
      s << "NONE";
      break;
    case kBnLt_C0:
      s << "C0";
      break;
    case kBnLt_C1:
      s << "C1";
      break;
    case kBnLt_BUFF:
      s << "BUFF";
      break;
    case kBnLt_NOT:
      s << "NOT";
      break;
    case kBnLt_AND:
      s << "AND";
      break;
    case kBnLt_NAND:
      s << "NAND";
      break;
    case kBnLt_OR:
      s << "OR";
      break;
    case kBnLt_NOR:
      s << "NOR";
      break;
    case kBnLt_XOR:
      s << "XOR";
      break;
    case kBnLt_XNOR:
      s << "XNOR";
      break;
    case kBnLt_EXPR:
      s << "expr#" << node->expr_id() << ": " << node->expr();
      break;
    case kBnLt_TV:
      s << "func#" << node->func_id() << ": " << node->func();
      break;
    }
    s << endl;
    if ( node->cell() ) {
      s << "    cell: " << node->cell()->name() << endl;
    }
    s << endl;
  }

  s << endl;
}

// @brief ネットワーク名を設定する．
// @param[in] name ネットワーク名
void
BnNetwork::set_name(const string& name)
{
  mName = name;
}

// @brief 外部入力ノードを追加する．
// @param[in] node_name ノード名
// @return 生成した入力ノードを返す．
//
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_input(const string& node_name)
{
  ymuint id = mNodeList.size();
  BnNode* node = new BnInputNode(id, node_name);
  mNodeList.push_back(node);
  mInputList.push_back(node);

  return node;
}

// @brief 外部出力ノードを追加する．
// @param[in] node_name ノード名
// @param[in] inode_id 入力のノード番号
// @return 生成した出力ノードを返す．
//
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_output(const string& node_name,
		      ymuint inode_id)
{
  ymuint id = mNodeList.size();
  BnNode* node = new BnOutputNode(id, node_name, inode_id);
  mNodeList.push_back(node);
  mOutputList.push_back(node);

  return node;
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] inode_id_list ファンインのノード番号のリスト
// @param[in] logic_type 論理型
// @param[in] cell セル
// @return 生成した論理ノードを返す．
//
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_primitive(const string& node_name,
			 const vector<ymuint>& inode_id_list,
			 BnLogicType logic_type,
			 const Cell* cell)
{
  ymuint id = mNodeList.size();
  BnNode* node = new BnPrimNode(id, node_name, inode_id_list, logic_type, cell);
  mNodeList.push_back(node);
  mLogicList.push_back(node);

  return node;
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] inode_id_list ファンインのノード番号のリスト
// @param[in] expr 論理式
// @param[in] cell セル
// @return 生成した論理ノードを返す．
//
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_expr(const string& node_name,
		    const vector<ymuint>& inode_id_list,
		    const Expr& expr,
		    const Cell* cell)
{
  BnLogicType logic_type = FuncAnalyzer::analyze(expr);
  if ( logic_type != kBnLt_EXPR ) {
    // 組み込み型だった．
    return new_primitive(node_name, inode_id_list, logic_type, cell);
  }

  ymuint expr_id = _add_expr(expr);
  ymuint id = mNodeList.size();
  BnNode* node = new BnExprNode(id, node_name, inode_id_list, expr, expr_id, cell);
  mNodeList.push_back(node);
  mLogicList.push_back(node);

  return node;
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] inode_id_list ファンインのノード番号のリスト
// @param[in] tv 真理値表
// @param[in] cell セル
// @return 生成した論理ノードを返す．
//
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_tv(const string& node_name,
		  const vector<ymuint>& inode_id_list,
		  const TvFunc& tv,
		  const Cell* cell)
{
  BnLogicType logic_type = FuncAnalyzer::analyze(tv);
  if ( logic_type != kBnLt_TV ) {
    // 組み込み型だった．
    return new_primitive(node_name, inode_id_list, logic_type, cell);
  }

  ymuint func_id = _add_tv(tv);
  ymuint id = mNodeList.size();
  BnNode* node = new BnTvNode(id, node_name, inode_id_list, tv, func_id, cell);
  mNodeList.push_back(node);
  mLogicList.push_back(node);

  return node;
}

// @brief ポートを追加する．
// @param[in] port_name ポート名
// @param[in] bits 内容のノード番号のリスト
void
BnNetwork::new_port(const string& port_name,
		    const vector<ymuint>& bits)
{
  if ( bits.size() == 1 ) {
    mPortList.push_back(new BnPort1(port_name, bits[0]));
  }
  else {
    mPortList.push_back(new BnPortN(port_name, bits));
  }
}

// @brief ポートを追加する(1ビット版)．
// @param[in] port_name ポート名
// @param[in] bit 内容のノード番号
void
BnNetwork::new_port(const string& port_name,
		    ymuint bit)
{
  mPortList.push_back(new BnPort1(port_name, bit));
}

// @brief DFFを追加する．
// @param[in] name DFF名
// @param[in] input 入力端子のノード番号
// @param[in] output 出力端子のノード番号
// @param[in] clock クロック端子のノード番号
// @param[in] clear クリア端子のノード番号
// @param[in] preset プリセット端子のノード番号
// @return 生成したDFFを返す．
//
// 名前の重複に関しては感知しない．
BnDff*
BnNetwork::new_dff(const string& name,
		   ymuint input,
		   ymuint output,
		   ymuint clock,
		   ymuint clear,
		   ymuint preset)
{
  ymuint id = mDffList.size();
  BnDff* dff = new BnDffImpl(id, name, input, output, clock, clear, preset);
  mDffList.push_back(dff);

  return dff;
}

// @brief ラッチを追加する．
// @param[in] name ラッチ名
// @param[in] input 入力端子のノード番号
// @param[in] output 出力端子のノード番号
// @param[in] enable イネーブル端子のノード番号
// @param[in] clear クリア端子のノード番号
// @param[in] preset プリセット端子のノード番号
// @return 生成したラッチを返す．
//
// 名前の重複に関しては感知しない．
BnLatch*
BnNetwork::new_latch(const string& name,
		     ymuint input,
		     ymuint output,
		     ymuint enable,
		     ymuint clear,
		     ymuint preset)
{
  ymuint id = mLatchList.size();
  BnLatch* latch = new BnLatchImpl(id, name, input, output, enable, clear, preset);
  mLatchList.push_back(latch);

  return latch;
}

#if 0
// @brief 関数の数を設定する．
// @param[in] func_num 関数の数
void
BnNetwork::set_func_num(ymuint func_num)
{
  mFuncList.clear();
  mFuncList.resize(func_num);
}

// @brief 関数を設定する．
// @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
// @param[in] tv 設定する関数
void
BnNetwork::set_func(ymuint func_id,
		    const TvFunc& tv)
{
  ASSERT_COND( func_id < func_num() );
  mFuncList[func_id] = tv;
}

// @brief 論理式の数を設定する．
// @param[in] expr_num 論理式の数
void
BnNetwork::set_expr_num(ymuint expr_num)
{
  mExprList.clear();
  mExprList.resize(expr_num);
}

// @brief 論理式を設定する．
// @param[in] expr_id 論理式番号 ( 0 <= expr_id < expr_num() )
// @param[in] expr 設定する論理式
void
BnNetwork::set_expr(ymuint expr_id,
		    const Expr& expr)
{
  ASSERT_COND( expr_id < expr_num() );
  mExprList[expr_id] = expr;
}
#endif

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
