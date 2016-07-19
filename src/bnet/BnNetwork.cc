
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

#include "Queue.h"


BEGIN_NAMESPACE_YM_BNET

BEGIN_NONAMESPACE

// @brief 真理値表から論理型を得る．
//
// 通常は kBnLt_TV だが場合によっては
// プリミティブ型となる．
BnLogicType
tv2logic_type(const TvFunc& tv)
{
  if ( tv == TvFunc::const_zero(0) ) {
    return kBnLt_C0;
  }
  else if ( tv == TvFunc::const_one(0) ) {
    return kBnLt_C1;
  }
  else if ( tv == TvFunc::posi_literal(1, VarId(0)) ) {
    return kBnLt_BUFF;
  }
  else if ( tv == TvFunc::nega_literal(1, VarId(0)) ) {
    return kBnLt_NOT;
  }
  else {
    ymuint input_num = tv.input_num();
    ymuint np = 1UL << input_num;
    int val_0;
    int val_1;
    bool has_0 = false;
    bool has_1 = false;
    bool xor_match = true;
    bool xnor_match = true;
    for (ymuint p = 0; p < np; ++ p) {
      int val = tv.value(p);
      if ( p == 0UL ) {
	val_0 = val;
      }
      else if ( p == (np - 1) ) {
	val_1 = val;
      }
      else {
	if ( val == 0 ) {
	  has_0 = true;
	}
	else {
	  has_1 = true;
	}
      }
      bool parity = false;
      for (ymuint i = 0; i < input_num; ++ i) {
	if ( (1UL << i) & p ) {
	  parity = !parity;
	}
      }
      if ( parity ) {
	if ( val ) {
	  xnor_match = false;
	}
      }
      else {
	if ( val ) {
	  xor_match = false;
	}
      }
    }
    if ( val_0 == 0 && val_1 == 1 ) {
      if ( !has_0 ) {
	// 00...00 だけ 0 で残りが 1
	return kBnLt_OR;
      }
      else if ( !has_1 ) {
	// 11...11 だけ 1 で残りが 0
	return kBnLt_AND;
      }
    }
    else if ( val_0 == 1 && val_1 == 0 ) {
      if ( !has_0 ) {
	// 11...11 だけ 0 で残りが 1
	return kBnLt_NAND;
      }
      else if ( !has_1 ) {
	// 00...00 だけ 1 で残りが 0
	return kBnLt_NOR;
      }
    }
    else if ( xor_match ) {
      return kBnLt_XOR;
    }
    else if ( xnor_match ) {
      return kBnLt_XNOR;
    }
  }

  return kBnLt_TV;
}

END_NONAMESPACE


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
    const Cell* cell = src_node->cell();
    BnNode* dst_node = nullptr;
    if ( cell != nullptr ) {
      dst_node = new_cell(src_node->name(), inode_list, cell);
    }
    else {
      BnLogicType logic_type = src_node->logic_type();
      if ( logic_type == kBnLt_EXPR ) {
	dst_node = new_expr(src_node->name(), inode_list, src_node->expr());
      }
      else if ( logic_type == kBnLt_TV ) {
	dst_node = new_tv(src_node->name(), inode_list, src_node->tv());
      }
      else {
	dst_node = new_primitive(src_node->name(), inode_list, logic_type);
      }
    }
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
    vector<BnNode*> bits(nb);
    for (ymuint j = 0; j < nb; ++ j) {
      ymuint id = src_port->bit(j)->id();
      BnNode* inode;
      bool stat = mNodeMap.find(id, inode);
      ASSERT_COND( stat );
      bits[j] = inode;
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

    const Cell* cell = src_info.mCell;
    BnNode* dst_node = nullptr;
    if ( cell != nullptr ) {
      dst_node = new_cell(src_info.mName, inode_list, cell);
    }
    else {
      BnLogicType logic_type = src_info.mLogicType;
      if ( logic_type == kBnLt_EXPR ) {
	dst_node = new_expr(src_info.mName, inode_list, src_info.mExpr);
      }
      else if ( logic_type == kBnLt_TV ) {
	dst_node = new_tv(src_info.mName, inode_list, src_info.mTv);
      }
      else {
	dst_node = new_primitive(src_info.mName, inode_list, logic_type);
      }
    }
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
    vector<BnNode*> bits(nb);
    for (ymuint j = 0; j < nb; ++ j) {
      ymuint id = src_info.mBits[j];
      BnNode* inode;
      bool stat = mNodeMap.find(id, inode);
      ASSERT_COND( stat );
      bits[j] = inode;
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

// @brief 関数番号から論理式を得る．
// @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
const Expr&
BnNetwork::expr(ymuint func_id) const
{
  ASSERT_COND( func_id < func_num() );
  return mExprList[func_id];
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
      s << " " << port->bit(j)->id();
    }
    s << endl;
  }
  s << endl;

  ymuint ni = input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const BnNode* node = input(i);
    ASSERT_COND( node != nullptr );
    ASSERT_COND( node->type() == BnNode::kInput );
    s << "input#" << i << ": " << node->id()
      << "(" << node->name() << ")" << endl;
  }
  s << endl;

  ymuint no = output_num();
  for (ymuint i = 0; i < no; ++ i) {
    const BnNode* node = output(i);
    ASSERT_COND( node != nullptr );
    ASSERT_COND( node->type() == BnNode::kOutput );
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
    ASSERT_COND( node->type() == BnNode::kLogic );
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
      s << "expr: [" << node->func_id() << "] " << node->expr();
      break;
    case kBnLt_TV:
      s << "tv: [" << node->func_id() << "] " << node->tv();
      break;
    }
    s << endl;
    if ( node->cell() ) {
      s << "    cell: " << node->cell()->name() << endl;
    }
    s << endl;
  }

  s << endl;
  for (ymuint func_id = 0; func_id < func_num(); ++ func_id) {
    s << "Func#" << func_id << ": " << func(func_id) << endl
      << "          " << expr(func_id) << endl;
  }

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

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] inode_list ファンインのノードのリスト
// @param[in] prim_type プリミティブの型
// @return 追加が成功したら true を返す．
//
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_primitive(const string& node_name,
			 const vector<BnNode*>& inode_list,
			 BnLogicType prim_type)
{
  ymuint id = mNodeList.size();
  BnNode* node = new BnPrimNode(id, node_name, inode_list, prim_type);
  mNodeList.push_back(node);
  mLogicList.push_back(node);

  return node;
}

// @brief セル型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] inode_list ファンインのノードのリスト
// @param[in] cell セル
// @return 生成した論理ノードを返す．
//
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_cell(const string& node_name,
		    const vector<BnNode*>& inode_list,
		    const Cell* cell)
{

  if ( !cell->has_logic() || cell->output_num() != 1 ) {
    // 1出力の論理セルでなければエラー
    return nullptr;
  }

  // expr がプリミティブ型かどうかチェックする．
  Expr expr = cell->logic_expr(0);
  ymuint func_id;
  BnLogicType logic_type = analyze_expr(expr, func_id);

  ymuint id = mNodeList.size();
  BnNode* node = nullptr;
  if ( logic_type == kBnLt_EXPR ) {
    node = new BnExprNode(id, node_name, inode_list, expr, func_id, cell);
  }
  else {
    node = new BnPrimNode(id, node_name, inode_list, logic_type, cell);
  }
  mNodeList.push_back(node);
  mLogicList.push_back(node);

  return node;
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] inode_list ファンインのノードのリスト
// @param[in] expr 論理式
// @return 生成した論理ノードを返す．
//
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_expr(const string& node_name,
		    const vector<BnNode*>& inode_list,
		    const Expr& expr)
{
  // expr がプリミティブ型かどうかチェックする．
  ymuint func_id;
  BnLogicType logic_type = analyze_expr(expr, func_id);

  ymuint id = mNodeList.size();
  BnNode* node = nullptr;
  if ( logic_type == kBnLt_EXPR ) {
    node = new BnExprNode(id, node_name, inode_list, expr, func_id);
  }
  else {
    node = new BnPrimNode(id, node_name, inode_list, logic_type);
  }
  mNodeList.push_back(node);
  mLogicList.push_back(node);

  return node;
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] inode_list ファンインのノードのリスト
// @param[in] tv_func 心理値表
// @return 生成した論理ノードを返す．
//
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_tv(const string& node_name,
		  const vector<BnNode*>& inode_list,
		  const TvFunc& tv)
{
  // tv がプリミティブ型かどうかチェックする．
  ymuint func_id;
  BnLogicType logic_type = analyze_func(tv, func_id);

  ymuint id = mNodeList.size();
  BnNode* node = nullptr;
  if ( logic_type == kBnLt_TV ) {
    node = new BnTvNode(id, node_name, inode_list, tv, func_id);
  }
  else {
    node = new BnPrimNode(id, node_name, inode_list, logic_type);
  }
  mNodeList.push_back(node);
  mLogicList.push_back(node);

  return node;
}

// @brief ポートを追加する．
// @param[in] port_name ポート名
// @param[in] bits 内容のノードのリスト
void
BnNetwork::new_port(const string& port_name,
		    const vector<BnNode*>& bits)
{
  mPortList.push_back(new BnPortImpl(port_name, bits));
}

// @brief ポートを追加する(1ビット版)．
// @param[in] port_name ポート名
// @param[in] bit 内容のノード
void
BnNetwork::new_port(const string& port_name,
		    BnNode* bit)
{
  new_port(port_name, vector<BnNode*>(1, bit));
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

// @brief 論理式を解析する．
// @param[in] expr 対象の論理式
// @param[out] func_id 関数番号
// @return 論理タイプ
BnLogicType
BnNetwork::analyze_expr(const Expr& expr,
			ymuint& func_id)
{
  ymuint input_num = expr.input_size();
  if ( input_num <= 10 ) {
    // 10入力以下の場合は一旦 TvFunc に変換する．
    TvFunc tv = expr.make_tv(input_num);
    BnLogicType logic_type = analyze_func(tv, func_id);
    if ( logic_type == kBnLt_TV ) {
      logic_type = kBnLt_EXPR;
      if ( func_id == mExprList.size() ) {
	mExprList.push_back(expr);
      }
    }
    return logic_type;
  }
  // 11入力以上の場合は常に新しい関数だと思う．
  func_id = mFuncList.size();
  mFuncList.push_back(TvFunc()); // ダミー
  mExprList.push_back(expr);
  return kBnLt_EXPR;
}

// @brief 関数を解析する．
// @param[in] func 対象の関数
// @param[out] func_id 関数番号
// @return 論理タイプ
//
// func がプリミティブ型の場合，
// プリミティブ型の値を返す．
// それ以外の場合，既に同じ関数が登録されていないか調べ，
// 関数番号を func_id に設定する．
// この場合は kBnLt_TV を返す．
BnLogicType
BnNetwork::analyze_func(const TvFunc& func,
			ymuint& func_id)
{
  ymuint input_num = func.input_num();
  BnLogicType logic_type = tv2logic_type(func);
  if ( logic_type != kBnLt_TV ) {
    return logic_type;
  }

  if ( !mFuncMap.find(func, func_id) ) {
    func_id = mFuncList.size();
    mFuncList.push_back(func);
    mFuncMap.add(func, func_id);
  }
  return kBnLt_TV;
}

END_NAMESPACE_YM_BNET
