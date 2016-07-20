
/// @file BnBuilder.cc
/// @brief BnBuilder の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnBuilder.h"
#include "ym/Cell.h"

#include "FuncAnalyzer.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnBuilder
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BnBuilder::BnBuilder()
{
}

// @brief デストラクタ
BnBuilder::~BnBuilder()
{
}

// @brief 名前を得る．
string
BnBuilder::model_name() const
{
  return mName;
}

// @brief ポート数を得る．
ymuint
BnBuilder::port_num() const
{
  return mPortInfoList.size();
}

// @brief ポート情報を得る．
// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
const BnBuilder::PortInfo&
BnBuilder::port(ymuint pos) const
{
  ASSERT_COND( pos < port_num() );
  return mPortInfoList[pos];
}

// @brief DFF数を得る．
ymuint
BnBuilder::dff_num() const
{
  return mDffInfoList.size();
}

// @brief DFF情報を得る．
// @param[in] id DFF番号 ( 0 <= id < dff_num() )
const BnBuilder::DffInfo&
BnBuilder::dff(ymuint id) const
{
  ASSERT_COND( id < dff_num() );
  return mDffInfoList[id];
}

// @brief ラッチ数を得る．
ymuint
BnBuilder::latch_num() const
{
  return mLatchInfoList.size();
}

// @brief ラッチ情報を得る．
// @param[in] id ラッチ番号 ( 0 <= id < latch_num() )
const BnBuilder::LatchInfo&
BnBuilder::latch(ymuint id) const
{
  ASSERT_COND( id < latch_num() );
  return mLatchInfoList[id];
}

// @brief ノード数を得る．
ymuint
BnBuilder::node_num() const
{
  return mNodeInfoList.size();
}

// @brief ノード情報を得る．
// @param[in] id ノード番号 ( 0 < id <= node_num() )
//
// ノード番号 0 は不正な値として予約されている．
const BnBuilder::NodeInfo&
BnBuilder::node(ymuint id) const
{
  ASSERT_COND( id > 0 && id <= node_num() );
  return mNodeInfoList[id - 1];
}

// @brief ノード情報を得る．
// @param[in] id ノード番号 ( 0 < id <= node_num() )
//
// ノード番号 0 は不正な値として予約されている．
BnBuilder::NodeInfo&
BnBuilder::_node(ymuint id)
{
  ASSERT_COND( id > 0 && id <= node_num() );
  return mNodeInfoList[id - 1];
}

// @brief 入力ノード数を得る．
ymuint
BnBuilder::input_num() const
{
  return mInputList.size();
}

// @brief 入力ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
const BnBuilder::NodeInfo&
BnBuilder::input(ymuint pos) const
{
  ASSERT_COND( pos < input_num() );
  return node(mInputList[pos]);
}

// @brief 出力ノード数を得る．
ymuint
BnBuilder::output_num() const
{
  return mOutputList.size();
}

// @brief 出力ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
const BnBuilder::NodeInfo&
BnBuilder::output(ymuint pos) const
{
  ASSERT_COND( pos < output_num() );
  return node(mOutputList[pos]);
}

// @brief 論理ノード数を得る．
ymuint
BnBuilder::logic_num() const
{
  return mLogicList.size();
}

// @brief 論理ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
//
// 入力からのトポロジカル順に整列している．
const BnBuilder::NodeInfo&
BnBuilder::logic(ymuint pos) const
{
  ASSERT_COND( pos < logic_num() );
  return node(mLogicList[pos]);
}

// @brief 内容を書き出す．
// @param[in] s 出力先のストリーム
void
BnBuilder::write(ostream& s) const
{
  s << "model_name: " << model_name() << endl;

  ymuint np = port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const PortInfo& port_info = port(i);
    s << "  Port#" << i << endl
	 << "    name: " << port_info.mName << endl;
    ymuint nb = port_info.mBits.size();
    s << "    bits: {";
    for (ymuint j = 0; j < nb; ++ j) {
      s << " " << port_info.mBits[j];
    }
    s << "}" << endl
	 << endl;
  }

  ymuint nff = dff_num();
  for (ymuint i = 0; i < nff; ++ i) {
    const DffInfo& dff_info = dff(i);
    s << "  Dff#" << i << endl
	 << "    name:   " << dff_info.mName << endl
	 << "    input:  " << dff_info.mInput << endl
	 << "    output: " << dff_info.mOutput << endl
	 << "    clock:  " << dff_info.mClock << endl;
    if ( dff_info.mClear != 0 ) {
      s << "    clear:  " << dff_info.mClear << endl;
    }
    if ( dff_info.mPreset != 0 ) {
      s << "    preset: " << dff_info.mPreset << endl;
    }
    s << endl;
  }

  ymuint nlatch = latch_num();
  for (ymuint i = 0; i < nlatch; ++ i) {
    const LatchInfo& latch_info = latch(i);
    s << "  Latch#" << i << endl
	 << "    name:   " << latch_info.mName << endl
	 << "    input:  " << latch_info.mInput << endl
	 << "    output: " << latch_info.mOutput << endl
	 << "    enable: " << latch_info.mEnable << endl;
    if ( latch_info.mClear != 0 ) {
      s << "    clear:  " << latch_info.mClear << endl;
    }
    if ( latch_info.mPreset != 0 ) {
      s << "    preset: " << latch_info.mPreset << endl;
    }
    s << endl;
  }

  ymuint npi = input_num();
  for (ymuint i = 0; i < npi; ++ i) {
    const NodeInfo& node_info = input(i);
    s << "  INPUT#" << i << endl
      << "    node#" << node_info.mId << endl
      << "    name:   " << node_info.mName << endl
      << endl;
  }

  ymuint nlogic = logic_num();
  for (ymuint i = 0; i < nlogic; ++ i) {
    const NodeInfo& node_info = logic(i);
    s << "  LOGIC#" << i << endl
      << "    node#" << node_info.mId << endl
      << "    name:   " << node_info.mName << endl
      << "    fanins: ";
    ymuint ni = node_info.mFaninList.size();
    for (ymuint j = 0; j < ni; ++ j) {
      s << " " << node_info.mFaninList[j];
    }
    s << endl;
    s << "    ";
    switch ( node_info.mLogicType ) {
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
      s << "EXPR: " << node_info.mExpr;
      break;
    case kBnLt_TV:
      s << "TV: " << node_info.mTv;
      break;
    case kBnLt_NONE:
      s << "cell: " << node_info.mCell;
      break;
    }
    s << endl
      << endl;
  }

  ymuint npo = output_num();
  for (ymuint i = 0; i < npo; ++ i) {
    const NodeInfo& node_info = output(i);
    s << "  OUTPUT#" << i << endl
      << "    Node#" << node_info.mId << endl
      << "    name:   " << node_info.mName << endl
      << "    input:  " << node_info.mFaninList[0] << endl
      << endl;
  }
}

// @brief 内容をクリアする．
//
// コンストラクタ直後と同じ状態になる．
void
BnBuilder::clear()
{
  mName = string();
  mPortInfoList.clear();
  mDffInfoList.clear();
  mLatchInfoList.clear();
  mNodeInfoList.clear();
  mInputList.clear();
  mOutputList.clear();
  mLogicList.clear();
  mSane = false;
}

// @brief ネットワーク名を設定する．
// @param[in] name ネットワーク名
void
BnBuilder::set_model_name(const string& name)
{
  mName = name;
}

// @brief ポート情報を追加する．
// @param[in] name ポート名
// @param[in] bits ビットの内容(ノード番号)
void
BnBuilder::add_port(const string& name,
		     const vector<ymuint>& bits)
{
  mPortInfoList.push_back(PortInfo(name, bits));
  mSane = false;
}

// @brief ポート情報を追加する(1ビット版)．
// @param[in] name ポート名
// @param[in] bit ビットの内容(ノード番号)
void
BnBuilder::add_port(const string& name,
		     ymuint bit)
{
  add_port(name, vector<ymuint>(1, bit));
  mSane = false;
}

// @brief DFF情報を追加する．
// @param[in] name DFF名
// @return DFF情報を返す．
BnBuilder::DffInfo&
BnBuilder::add_dff(const string& name)
{
  mDffInfoList.push_back(DffInfo(name));

  mSane = false;

  return mDffInfoList.back();
}

// @brief ラッチ情報を追加する．
// @param[in] name ラッチ名
// @return ラッチ情報を返す．
BnBuilder::LatchInfo&
BnBuilder::add_latch(const string& name)
{
  mLatchInfoList.push_back(LatchInfo(name));

  mSane = false;

  return mLatchInfoList.back();
}

// @brief 入力用のノード情報を追加する．
// @param[in] name ノード名
// @return ノード番号を返す．
ymuint
BnBuilder::add_input(const string& name)
{
  ymuint id = _add_node(NodeInfo(name));
  mInputList.push_back(id);

  return id;
}

// @brief 出力用のノード情報を追加する．
// @param[in] name ノード名
// @param[in] input 入力のノード番号
// @return ノード番号を返す．
ymuint
BnBuilder::add_output(const string& name,
		      ymuint input)
{
  ymuint id = _add_node(NodeInfo(name, input));
  mOutputList.push_back(id);

  if ( input != 0 ) {
    connect(input, id, 0);
  }

  return id;
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] name ノード名
// @param[in] logic_type 論理型
// @praam[in] ni ファンイン数
// @return ノード番号を返す．
ymuint
BnBuilder::add_primitive(const string& name,
			 BnLogicType logic_type,
			 ymuint ni)
{
  ymuint id = _add_node(NodeInfo(name, logic_type, ni));

  return id;
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] name ノード名
// @param[in] expr 論理式
// @param[in] ni ファンイン数
// @return ノード番号を返す．
ymuint
BnBuilder::add_expr(const string& name,
		    const Expr& expr,
		    ymuint ni)
{
  BnLogicType logic_type = FuncAnalyzer::analyze(expr);
  ymuint id = 0;
  if ( logic_type == kBnLt_EXPR ) {
    id = _add_node(NodeInfo(name, expr, ni));
  }
  else {
    id = _add_node(NodeInfo(name, logic_type, ni));
  }

  return id;
}

// @brief セル型の論理ノードを追加する．
// @param[in] name ノード名
// @param[in] cell セル
// @return ノード番号を返す．
ymuint
BnBuilder::add_cell(const string& name,
		    const Cell* cell)
{
  ymuint id = _add_node(NodeInfo(name, cell, cell->input_num()));

  return id;
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] name ノード名
// @param[in] tv 真理値表
// @return ノード番号を返す．
ymuint
BnBuilder::add_tv(const string& name,
		  const TvFunc& tv)
{
  BnLogicType logic_type = FuncAnalyzer::analyze(tv);
  ymuint id = 0;
  if ( logic_type == kBnLt_TV ) {
    id = _add_node(NodeInfo(name, tv));
  }
  else {
    id = _add_node(NodeInfo(name, logic_type, tv.input_num()));
  }

  return id;
}

// @brief ノード情報を追加する．
// @param[in] node_info ノード情報
// @return ノード番号を返す．
ymuint
BnBuilder::_add_node(const NodeInfo& node_info)
{
  mNodeInfoList.push_back(node_info);
  ymuint id = mNodeInfoList.size();
  mNodeInfoList.back().mId = id;

  mSane = false;

  return id;
}

// @brief ノード間を接続する．
// @param[in] src_node ファンアウト元のノード番号
// @param[in] dst_node ファンイン先のノード番号
// @param[in] ipos ファンインの位置
void
BnBuilder::connect(ymuint src_node,
		   ymuint dst_node,
		   ymuint ipos)
{
  NodeInfo& src_info = mNodeInfoList[src_node - 1];
  NodeInfo& dst_info = mNodeInfoList[dst_node - 1];
  src_info.mFanoutList.push_back(dst_node);
  dst_info.mFaninList[ipos] = src_node;

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
BnBuilder::wrap_up()
{
  if ( mSane ) {
    return true;
  }

  bool error = false;

  // ポートのチェック
  for (ymuint i = 0; i < port_num(); ++ i) {
    const PortInfo& port_info = port(i);
    ymuint nb = port_info.mBits.size();
    for (ymuint j = 0; j < nb; ++ j) {
      ymuint id = port_info.mBits[j];
      if ( id == 0 || id > node_num() ) {
	cerr << "Port#" << i << "(" << port_info.mName << ").bit["
	     << j << "] is not set" << endl;
	error = true;
      }
    }
  }

  // DFF のチェック
  for (ymuint i = 0; i < dff_num(); ++ i) {
    const DffInfo& dff_info = dff(i);
    ymuint id1 = dff_info.mInput;
    if ( id1 == 0 ) {
      cerr << "DFF#" << i << "(" << dff_info.mName << ").input is not set" << endl;
      error = true;
    }
    else if ( id1 > node_num() ) {
      cerr << "DFF#" << i << "(" << dff_info.mName << ").input is not valid" << endl;
      error = true;
    }
    ymuint id2 = dff_info.mOutput;
    if ( id2 == 0 ) {
      cerr << "DFF#" << i << "(" << dff_info.mName << ").output is not set" << endl;
      error = true;
    }
    else if ( id2 > node_num() ) {
      cerr << "DFF#" << i << "(" << dff_info.mName << ").output is not valid" << endl;
      error = true;
    }
    ymuint id3 = dff_info.mClock;
    if ( id3 == 0 ) {
      cerr << "DFF#" << i << "(" << dff_info.mName << ").clock is not set" << endl;
      error = true;
    }
    else if ( id3 > node_num() ) {
      cerr << "DFF#" << i << "(" << dff_info.mName << ").clock is not valid" << endl;
      error = true;
    }
    ymuint id4 = dff_info.mClear;
    if ( id4 > node_num() ) {
      cerr << "DFF#" << i << "(" << dff_info.mName << ").clear is not valid" << endl;
      error = true;
    }
    ymuint id5 = dff_info.mPreset;
    if ( id5 > node_num() ) {
      cerr << "DFF#" << i << "(" << dff_info.mName << ").preset is not valid" << endl;
      error = true;
    }
  }

  // ラッチのチェック
  for (ymuint i = 0; i < latch_num(); ++ i) {
    const LatchInfo& latch_info = latch(i);
    ymuint id1 = latch_info.mInput;
    if ( id1 == 0 ) {
      cerr << "LATCH#" << i << "(" << latch_info.mName << ").input is not set" << endl;
      error = true;
    }
    else if ( id1 > node_num() ) {
      cerr << "LATCH#" << i << "(" << latch_info.mName << ").input is not valid" << endl;
      error = true;
    }
    ymuint id2 = latch_info.mOutput;
    if ( id2 == 0 ) {
      cerr << "LATCH#" << i << "(" << latch_info.mName << ").output is not set" << endl;
      error = true;
    }
    else if ( id2 > node_num() ) {
      cerr << "LATCH#" << i << "(" << latch_info.mName << ").output is not valid" << endl;
      error = true;
    }
    ymuint id3 = latch_info.mEnable;
    if ( id3 == 0 ) {
      cerr << "LATCH#" << i << "(" << latch_info.mName << ").enable is not set" << endl;
      error = true;
    }
    else if ( id3 > node_num() ) {
      cerr << "LATCH#" << i << "(" << latch_info.mName << ").enable is not valid" << endl;
      error = true;
    }
    ymuint id4 = latch_info.mClear;
    if ( id4 > node_num() ) {
      cerr << "LATCH#" << i << "(" << latch_info.mName << ").clear is not valid" << endl;
      error = true;
    }
    ymuint id5 = latch_info.mPreset;
    if ( id5 > node_num() ) {
      cerr << "LATCH#" << i << "(" << latch_info.mName << ").preset is not valid" << endl;
      error = true;
    }
  }

  // ノードのチェック
  for (ymuint i = 1; i <= node_num(); ++ i) {
    const NodeInfo& node_info = node(i);
    ymuint ni = node_info.mFaninList.size();
    for (ymuint j = 0; j < ni; ++ j) {
      ymuint id = node_info.mFaninList[j];
      if ( id == 0 ) {
	cerr << "NODE#" << i << "(" << node_info.mName << ").fanin["
	     << j << "] is not set" << endl;
	error = true;
      }
      else if ( id > node_num() ) {
	cerr << "NODE#" << i << "(" << node_info.mName << ").fanin["
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
    const NodeInfo& node_info = input(i);
    ymuint id = node_info.mId;
    queue.push_back(id);
    mark[id] = true;
  }

  mLogicList.clear();
  mLogicList.reserve(node_num() - input_num() - output_num());

  // キューからノードを取り出してファンアウト先のノードをキューに積む．
  for (ymuint rpos = 0; rpos < queue.size(); ++ rpos) {
    ymuint id = queue[rpos];
    const NodeInfo& node_info = node(id);
    if ( node_info.mType == BnNode::kLogic ) {
      mLogicList.push_back(id);
    }
    ymuint fo = node_info.mFanoutList.size();
    for (ymuint i = 0; i < fo; ++ i) {
      ymuint oid = node_info.mFanoutList[i];
      if ( mark[oid] ) {
	continue;
      }
      const NodeInfo& onode_info = node(oid);
      ymuint ni = onode_info.mFaninList.size();
      bool ready = true;
      for (ymuint j = 0; j < ni; ++ j) {
	if ( !mark[onode_info.mFaninList[j]] ) {
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

END_NAMESPACE_YM_BNET
