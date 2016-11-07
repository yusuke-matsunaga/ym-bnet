
/// @file BnNetwork.cc
/// @brief BnNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNetwork.h"
#include "ym/Cell.h"

#include "ym/BnBuilder.h"

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

// @brief ビルダーを引数にとるコンストラクタ
// @param[in] builder ビルダーオブジェクト
BnNetwork::BnNetwork(const BnBuilder& builder)
{
  copy(builder);
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

  mNetworkName = string();
  mPortList.clear();
  mDffList.clear();
  mLatchList.clear();
  mInputList.clear();
  mOutputList.clear();
  mLogicList.clear();
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

  clear();

  // ネットワーク名の設定
  set_model_name(src.model_name());

  // ノード番号をキーにしてノードを格納するハッシュ表
  HashMap<ymuint, BnNode*> mNodeMap;

  // 外部入力の生成
  ymuint ni = src.input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const BnNode* src_node = src.input(i);
    BnNode* dst_node = new_input(src_node->name());
    mNodeMap.add(src_node->id(), dst_node);
  }

  // 関数情報の生成
  ymuint nfunc = src.func_num();
  set_func_num(nfunc);
  for (ymuint i = 0; i < nfunc; ++ i) {
    set_func(i, src.func(i));
  }

  // 論理式情報の生成
  ymuint nexpr = src.expr_num();
  set_expr_num(nexpr);
  for (ymuint i = 0; i < nexpr; ++ i) {
    set_expr(i, src.expr(i));
  }

  // 論理ノードの生成
  ymuint nl = src.logic_num();
  for (ymuint i = 0; i < nl; ++ i) {
    const BnNode* src_node = src.logic(i);
    ymuint nfi = src_node->fanin_num();
    vector<BnNode*> inode_list(nfi);
    for (ymuint i = 0; i < nfi; ++ i) {
      ymuint iid = src_node->fanin(i)->id();
      BnNode* inode;
      bool stat = mNodeMap.find(iid, inode);
      ASSERT_COND( stat );
      inode_list[i] = inode;
    }
    string name = src_node->name();
    BnLogicType logic_type = src_node->logic_type();
    ymuint expr_id = src_node->expr_id();
    ymuint func_id = src_node->func_id();
    const Cell* cell = src_node->cell();
    BnNode* dst_node = new_logic(name, inode_list, logic_type, expr_id, func_id, cell);
    mNodeMap.add(src_node->id(), dst_node);
  }

  // 外部出力の生成
  ymuint no = src.output_num();
  for (ymuint i = 0; i < no; ++ i) {
    const BnNode* src_node = src.output(i);
    ymuint iid = src_node->input()->id();
    BnNode* inode;
    bool stat = mNodeMap.find(iid, inode);
    ASSERT_COND( stat );

    BnNode* dst_node = new_output(src_node->name(), inode);

    mNodeMap.add(src_node->id(), dst_node);
  }

  // ポートの生成
  ymuint np = src.port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const BnPort* src_port = src.port(i);
    ymuint nb = src_port->bit_width();
    vector<ymuint> bits(nb);
    for (ymuint j = 0; j < nb; ++ j) {
      bits[j] = src_port->bit(j);
    }
    new_port(src_port->name(), bits);
  }

  // DFFの生成
  ymuint nff = src.dff_num();
  for (ymuint i = 0; i < nff; ++ i) {
    const BnDff* src_dff = src.dff(i);

    BnNode* input = nullptr;
    const BnNode* src_input = src_dff->input();
    ASSERT_COND( src_input != nullptr );
    bool stat1 = mNodeMap.find(src_input->id(), input);
    ASSERT_COND( stat1 );

    BnNode* output = nullptr;
    const BnNode* src_output = src_dff->output();
    ASSERT_COND( src_output != nullptr );
    bool stat2 = mNodeMap.find(src_output->id(), output);
    ASSERT_COND( stat2 );

    BnNode* clock = nullptr;
    const BnNode* src_clock = src_dff->clock();
    ASSERT_COND( src_clock != nullptr );
    bool stat3 = mNodeMap.find(src_clock->id(), clock);
    ASSERT_COND( stat3 );

    BnNode* clear = nullptr;
    const BnNode* src_clear = src_dff->clear();
    if ( src_clear != nullptr ) {
      bool stat4 = mNodeMap.find(src_clear->id(), clear);
      ASSERT_COND( stat4 );
    }

    BnNode* preset = nullptr;
    const BnNode* src_preset = src_dff->preset();
    if ( src_preset != nullptr ) {
      bool stat5 = mNodeMap.find(src_preset->id(), preset);
      ASSERT_COND( stat5 );
    }

    new_dff(src_dff->name(), input, output, clock, clear, preset);
  }

  // ラッチの生成
  ymuint nlatch = src.latch_num();
  for (ymuint i = 0; i < nlatch; ++ i) {
    const BnLatch* src_latch = src.latch(i);

    BnNode* input = nullptr;
    const BnNode* src_input = src_latch->input();
    ASSERT_COND( src_input != nullptr );
    bool stat1 = mNodeMap.find(src_input->id(), input);
    ASSERT_COND( stat1 );

    BnNode* output = nullptr;
    const BnNode* src_output = src_latch->output();
    ASSERT_COND( src_output != nullptr );
    bool stat2 = mNodeMap.find(src_output->id(), output);
    ASSERT_COND( stat2 );

    BnNode* enable = nullptr;
    const BnNode* src_enable = src_latch->enable();
    ASSERT_COND( src_enable != nullptr );
    bool stat3 = mNodeMap.find(src_enable->id(), enable);
    ASSERT_COND( stat3 );

    BnNode* clear = nullptr;
    const BnNode* src_clear = src_latch->clear();
    if ( src_clear != nullptr ) {
      bool stat4 = mNodeMap.find(src_clear->id(), clear);
      ASSERT_COND( stat4 );
    }

    BnNode* preset = nullptr;
    const BnNode* src_preset = src_latch->preset();
    if ( src_preset != nullptr ) {
      bool stat5 = mNodeMap.find(src_preset->id(), preset);
      ASSERT_COND( stat5 );
    }

    new_latch(src_latch->name(), input, output, enable, clear, preset);
  }
}

// @brief ビルダーオブジェクトからの生成
// @param[in] builder ビルダーオブジェクト
void
BnNetwork::copy(const BnBuilder& builder)
{
  clear();

  // ネットワーク名の設定
  set_model_name(builder.model_name());

  // ノード番号をキーにしてノードを格納するハッシュ表
  HashMap<ymuint, BnNode*> mNodeMap;

  // 外部入力の生成
  ymuint ni = builder.input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const BnBuilder::NodeInfo& src_info = builder.input(i);
    BnNode* dst_node = new_input(src_info.mName);
    mNodeMap.add(src_info.mId, dst_node);
  }

  // 関数情報の生成
  ymuint nfunc = builder.func_num();
  set_func_num(nfunc);
  for (ymuint i = 0; i < nfunc; ++ i) {
    set_func(i, builder.func(i));
  }

  // 論理式情報の生成
  ymuint nexpr = builder.expr_num();
  set_expr_num(nexpr);
  for (ymuint i = 0; i < nexpr; ++ i) {
    set_expr(i, builder.expr(i));
  }

  // 論理ノードの生成
  ymuint nl = builder.logic_num();
  for (ymuint i = 0; i < nl; ++ i) {
    const BnBuilder::NodeInfo& src_info = builder.logic(i);
    ymuint nfi = src_info.mFaninList.size();
    vector<BnNode*> inode_list(nfi);
    for (ymuint j = 0; j < nfi; ++ j) {
      ymuint iid = src_info.mFaninList[j];
      BnNode* inode;
      bool stat = mNodeMap.find(iid, inode);
      ASSERT_COND( stat );
      inode_list[j] = inode;
    }
    string name = src_info.mName;
    BnLogicType logic_type = src_info.mLogicType;
    ymuint func_id = src_info.mFuncId;
    const Cell* cell = src_info.mCell;
    BnNode* dst_node = new_logic(name, inode_list, logic_type, func_id, func_id, cell);
    mNodeMap.add(src_info.mId, dst_node);
  }

  // 外部出力の生成
  ymuint no = builder.output_num();
  for (ymuint i = 0; i < no; ++ i) {
    const BnBuilder::NodeInfo& src_info = builder.output(i);
    ymuint iid = src_info.mFaninList[0];
    BnNode* inode;
    bool stat = mNodeMap.find(iid, inode);
    ASSERT_COND( stat );

    BnNode* dst_node = new_output(src_info.mName, inode);

    mNodeMap.add(src_info.mId, dst_node);
  }

  // ポートの生成
  ymuint np = builder.port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const BnBuilder::PortInfo& src_info = builder.port(i);
    ymuint nb = src_info.mBits.size();
    vector<ymuint> bits(nb);
    for (ymuint j = 0; j < nb; ++ j) {
      bits[j] = src_info.mBits[j];
    }
    new_port(src_info.mName, bits);
  }

  // DFF の生成
  ymuint nff = builder.dff_num();
  for (ymuint i = 0; i < nff; ++ i) {
    const BnBuilder::DffInfo& src_info = builder.dff(i);

    BnNode* input;
    bool stat1 = mNodeMap.find(src_info.mInput, input);
    ASSERT_COND( stat1 );

    BnNode* output;
    bool stat2 = mNodeMap.find(src_info.mOutput, output);
    ASSERT_COND( stat2 );

    BnNode* clock;
    bool stat3 = mNodeMap.find(src_info.mClock, clock);
    ASSERT_COND( stat3 );

    BnNode* clear = nullptr;
    ymuint clear_id = src_info.mClear;
    if ( clear_id > 0 ) {
      bool stat4 = mNodeMap.find(clear_id, clear);
      ASSERT_COND( stat4 );
    }

    BnNode* preset = nullptr;
    ymuint preset_id = src_info.mPreset;
    if ( preset_id > 0 ) {
      bool stat5 = mNodeMap.find(preset_id, preset);
      ASSERT_COND( stat5 );
    }

    new_dff(src_info.mName, input, output, clock, clear, preset);
  }

  // ラッチの生成
  ymuint nlatch = builder.latch_num();
  for (ymuint i = 0; i < nlatch; ++ i) {
    const BnBuilder::LatchInfo& src_info = builder.latch(i);

    BnNode* input;
    bool stat1 = mNodeMap.find(src_info.mInput, input);
    ASSERT_COND( stat1 );

    BnNode* output;
    bool stat2 = mNodeMap.find(src_info.mOutput, output);
    ASSERT_COND( stat2 );

    BnNode* enable;
    bool stat3 = mNodeMap.find(src_info.mEnable, enable);
    ASSERT_COND( stat3 );

    BnNode* clear = nullptr;
    ymuint clear_id = src_info.mClear;
    if ( clear_id > 0 ) {
      bool stat4 = mNodeMap.find(clear_id, clear);
      ASSERT_COND( stat4 );
    }

    BnNode* preset = nullptr;
    ymuint preset_id = src_info.mPreset;
    if ( preset_id > 0 ) {
      bool stat5 = mNodeMap.find(preset_id, preset);
      ASSERT_COND( stat5 );
    }

    new_latch(src_info.mName, input, output, enable, clear, preset);
  }
}

// @brief ネットワーク名を得る．
string
BnNetwork::model_name() const
{
  return mNetworkName;
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
  s << "network name : " << model_name() << endl
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
      << "    input: " << node->input()->id() << endl;
  }
  s << endl;

  ymuint nf = dff_num();
  for (ymuint i = 0; i < nf; ++ i) {
    const BnDff* node = dff(i);
    ASSERT_COND( node != nullptr );
    s << "dff#" << i
      << "(" << node->name() << ")" << endl
      << "    input:  " << node->input()->id() << endl
      << "    output: " << node->output()->id() << endl
      << "    clock:  " << node->clock()->id() << endl;
    if ( node->clear() != nullptr ) {
      s << "    clear:  " << node->clear()->id() << endl;
    }
    if ( node->preset() != nullptr ) {
      s << "    preset: " << node->preset()->id() << endl;
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
      << "    input:  " << node->input()->id() << endl
      << "    output: " << node->output()->id() << endl
      << "    enable: " << node->enable()->id() << endl;
    if ( node->clear() != nullptr ) {
      s << "    clear:  " << node->clear()->id() << endl;
    }
    if ( node->preset() != nullptr ) {
      s << "    preset: " << node->preset()->id() << endl;
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
      s << " " << node->fanin(j)->id();
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
BnNetwork::set_model_name(const string& name)
{
  mNetworkName = name;
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
// @param[in] inode 入力のノード番号
// @return 生成した出力ノードを返す．
//
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_output(const string& node_name,
		      BnNode* inode)
{
  ymuint id = mNodeList.size();
  BnNode* node = new BnOutputNode(id, node_name, inode);
  mNodeList.push_back(node);
  mOutputList.push_back(node);

  return node;
}

// @brief 論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] inode_list ファンインのノードのリスト
// @param[in] logic_type 論理型
// @param[in] expr_id 論理式番号
// @param[in] func_id 関数番号
// @param[in] cell セル
// @return 生成した論理ノードを返す．
//
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_logic(const string& node_name,
		     const vector<BnNode*>& inode_list,
		     BnLogicType logic_type,
		     ymuint expr_id,
		     ymuint func_id,
		     const Cell* cell)
{
  ymuint id = mNodeList.size();
  BnNode* node;
  if ( logic_type == kBnLt_EXPR ) {
    node = new BnExprNode(id, node_name, inode_list, expr(expr_id), expr_id, cell);
  }
  else if ( logic_type == kBnLt_TV ) {
    node = new BnTvNode(id, node_name, inode_list, func(func_id), func_id, cell);
  }
  else {
    node = new BnPrimNode(id, node_name, inode_list, logic_type, cell);
  }
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
// @param[in] input 入力端子のノード
// @param[in] output 出力端子のノード
// @param[in] clock クロック端子のノード
// @param[in] clear クリア端子のノード
// @param[in] preset プリセット端子のノード
// @return 生成したDFFを返す．
//
// 名前の重複に関しては感知しない．
BnDff*
BnNetwork::new_dff(const string& name,
		   BnNode* input,
		   BnNode* output,
		   BnNode* clock,
		   BnNode* clear,
		   BnNode* preset)
{
  ymuint id = mDffList.size();
  BnDff* dff = new BnDffImpl(id, name, input, output, clock, clear, preset);
  mDffList.push_back(dff);

  return dff;
}

// @brief ラッチを追加する．
// @param[in] name ラッチ名
// @param[in] input 入力端子のノード
// @param[in] output 出力端子のノード
// @param[in] enable イネーブル端子のノード
// @param[in] clear クリア端子のノード
// @param[in] preset プリセット端子のノード
// @return 生成したラッチを返す．
//
// 名前の重複に関しては感知しない．
BnLatch*
BnNetwork::new_latch(const string& name,
		     BnNode* input,
		     BnNode* output,
		     BnNode* enable,
		     BnNode* clear,
		     BnNode* preset)
{
  ymuint id = mLatchList.size();
  BnLatch* latch = new BnLatchImpl(id, name, input, output, enable, clear, preset);
  mLatchList.push_back(latch);

  return latch;
}

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

END_NAMESPACE_YM_BNET
