
/// @file BnNetwork.cc
/// @brief BnNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNetwork.h"
#include "ym/Cell.h"

#include "BnPortImpl.h"
#include "BnNodeImpl.h"

#include "ym/BlifParser.h"
#include "BlifBnNetworkHandler.h"

#include "ym/Iscas89Parser.h"
#include "Iscas89BnNetworkHandler.h"

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
  mSane = false;
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
  for (HashMapIterator<ymuint, BnNode*> p = mNodeMap.begin();
       p != mNodeMap.end(); ++ p) {
    BnNode* node = p.value();
    delete node;
  }

  mNetworkName = string();
  mPortList.clear();
  mInputList.clear();
  mDffList.clear();
  mLogicList.clear();
  mNodeMap.clear();

  mSane = false;
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

  // ネットワーク名の設定
  set_model_name(src.model_name());

  // 外部入力の生成
  ymuint ni = src.input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const BnNode* src_node = src.input(i);
    new_input(src_node->id(), src_node->name());
  }

  // 外部出力の生成
  ymuint no = src.output_num();
  for (ymuint i = 0; i < no; ++ i) {
    const BnNode* src_node = src.output(i);
    new_output(src_node->name(), src_node->input());
  }

  // DFFの生成
  ymuint nff = src.dff_num();
  for (ymuint i = 0; i < nff; ++ i) {
    const BnNode* src_node = src.dff(i);
    new_dff(src_node->id(), src_node->name(), src_node->input(), src_node->reset_val());
  }

  // 論理ノードの生成
  ymuint nl = src.logic_num();
  for (ymuint i = 0; i < nl; ++ i) {
    const BnNode* src_node = src.logic(i);
    ymuint nfi = src_node->fanin_num();
    vector<ymuint> inode_list(nfi);
    for (ymuint i = 0; i < nfi; ++ i) {
      inode_list[i] = src_node->fanin(i);
    }
    const Cell* cell = src_node->cell();
    if ( cell != nullptr ) {
      new_cell(src_node->id(), src_node->name(), inode_list, cell);
    }
    else {
      BnLogicType logic_type = src_node->logic_type();
      if ( logic_type == kBnLt_EXPR ) {
	new_expr(src_node->id(), src_node->name(), inode_list, src_node->expr());
      }
      else if ( logic_type == kBnLt_TV ) {
	new_tv(src_node->id(), src_node->name(), inode_list, src_node->tv());
      }
      else {
	new_primitive(src_node->id(), src_node->name(), inode_list, logic_type);
      }
    }
  }

  // 最終処理
  bool stat = wrap_up();

  ASSERT_COND( stat );
}

// @brief ネットワーク名を設定する．
// @param[in] name ネットワーク名
void
BnNetwork::set_model_name(const string& name)
{
  mNetworkName = name;
}

// @brief ポートを追加する．
// @param[in] port_name ポート名
// @param[in] bits 内容のノード番号のリスト
void
BnNetwork::new_port(const string& port_name,
		    const vector<BnNode*>& bits)
{
  mPortList.push_back(new BnPortImpl(port_name, bits));

  mSane = false;
}

// @brief 外部入力ノードを追加する．
// @param[in] node_id ノードID番号
// @param[in] node_name ノード名
// @return 生成した入力ノードを返す．
//
// すでに同じノード番号が存在したら失敗する．
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_input(ymuint node_id,
		     const string& node_name)
{
  if ( mNodeMap.check(node_id) ) {
    return nullptr;
  }

  BnNode* node = new BnInputNode(node_id, node_name);
  mNodeMap.add(node_id, node);
  mInputList.push_back(node);

  mSane = false;

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
  BnNode* node = new BnOutputNode(node_name, inode_id);
  mOutputList.push_back(node);

  mSane = false;

  return node;
}

// @brief DFFノードを追加する．
// @param[in] node_id ノードID番号
// @param[in] node_name ノード名
// @param[in] inode_id 入力のノード番号
// @param[in] reset_val リセット値 ('0', '1', '-' のいずれか)
// @return 生成したDFFノードを返す．
//
// すでに同じノード番号が存在したら失敗する．
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_dff(ymuint node_id,
		   const string& node_name,
		   ymuint inode_id,
		   char reset_val)
{
  if ( mNodeMap.check(node_id) ) {
    return nullptr;
  }

  BnNode* node = new BnDffNode(node_id, node_name, inode_id, reset_val);
  mNodeMap.add(node_id, node);
  mDffList.push_back(node);

  mSane = false;

  return node;
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] node_id ノードID番号
// @param[in] node_name ノード名
// @param[in] inode_id_list ファンインのノード番号のリスト
// @param[in] prim_type プリミティブの型
// @return 追加が成功したら true を返す．
//
// すでに同じノード番号が存在したら失敗する．
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_primitive(ymuint node_id,
			 const string& node_name,
			 const vector<ymuint>& inode_id_list,
			 BnLogicType prim_type)
{
  if ( mNodeMap.check(node_id) ) {
    return nullptr;
  }

  BnNode* node = new BnPrimNode(node_id, node_name, inode_id_list, prim_type);
  mNodeMap.add(node_id, node);
  mLogicList.push_back(node);

  mSane = false;

  return node;
}

// @brief セル型の論理ノードを追加する．
// @param[in] node_id ノードID番号
// @param[in] node_name ノード名
// @param[in] inode_id_list ファンインのノード番号のリスト
// @param[in] cell セル
// @return 生成した論理ノードを返す．
//
// すでに同じノード番号が存在したら失敗する．
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_cell(ymuint node_id,
		    const string& node_name,
		    const vector<ymuint>& inode_id_list,
		    const Cell* cell)
{
  if ( mNodeMap.check(node_id) ) {
    return nullptr;
  }

  if ( !cell->has_logic() || cell->output_num() != 1 ) {
    // 1出力の論理セルでなければエラー
    return nullptr;
  }

  // expr がプリミティブ型かどうかチェックする．
  Expr expr = cell->logic_expr(0);
  ymuint func_id;
  BnLogicType logic_type = analyze_expr(expr, func_id);

  BnNode* node = nullptr;
  if ( logic_type == kBnLt_EXPR ) {
    node = new BnExprNode(node_id, node_name, inode_id_list, expr, func_id, cell);
  }
  else {
    node = new BnPrimNode(node_id, node_name, inode_id_list, logic_type, cell);
  }
  mNodeMap.add(node_id, node);
  mLogicList.push_back(node);

  mSane = false;

  return node;
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] node_id ノードID番号
// @param[in] node_name ノード名
// @param[in] inode_id_list ファンインのノード番号のリスト
// @param[in] expr 論理式
// @return 生成した論理ノードを返す．
//
// すでに同じノード番号が存在したら失敗する．
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_expr(ymuint node_id,
		    const string& node_name,
		    const vector<ymuint>& inode_id_list,
		    const Expr& expr)
{
  if ( mNodeMap.check(node_id) ) {
    return nullptr;
  }

  // expr がプリミティブ型かどうかチェックする．
  ymuint func_id;
  BnLogicType logic_type = analyze_expr(expr, func_id);

  BnNode* node = nullptr;
  if ( logic_type == kBnLt_EXPR ) {
    node = new BnExprNode(node_id, node_name, inode_id_list, expr, func_id);
  }
  else {
    node = new BnPrimNode(node_id, node_name, inode_id_list, logic_type);
  }
  mNodeMap.add(node_id, node);
  mLogicList.push_back(node);

  mSane = false;

  return node;
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] node_id ノードID番号
// @param[in] node_name ノード名
// @param[in] inode_id_list ファンインのノード番号のリスト
// @param[in] tv_func 心理値表
// @return 生成した論理ノードを返す．
//
// すでに同じノード番号が存在したら失敗する．
// ノード名の重複に関しては感知しない．
BnNode*
BnNetwork::new_tv(ymuint node_id,
		  const string& node_name,
		  const vector<ymuint>& inode_id_list,
		  const TvFunc& tv)
{
  if ( mNodeMap.check(node_id) ) {
    return nullptr;
  }

  // tv がプリミティブ型かどうかチェックする．
  ymuint func_id;
  BnLogicType logic_type = analyze_func(tv, func_id);

  BnNode* node = nullptr;
  if ( logic_type == kBnLt_TV ) {
    node = new BnTvNode(node_id, node_name, inode_id_list, tv, func_id);
  }
  else {
    node = new BnPrimNode(node_id, node_name, inode_id_list, logic_type);
  }
  mNodeMap.add(node_id, node);
  mLogicList.push_back(node);

  mSane = false;

  return node;
}

// @brief 最終処理と整合性のチェックを行う．
// @retval true 問題なし
// @retval false エラーがあった．
//
// 検査項目は以下の通り．
// - model_name が設定されているか
//   設定されていなければデフォルト値を使う．
//   -> warning
// - 各ノードのファンインのノード番号が存在するか
//   存在しなければ error
//
// 検査後に論理ノードのリストをトポロジカル順にソートする．
// BnNodeInfo のファンアウト情報はここで設定される．
bool
BnNetwork::wrap_up()
{
  if ( mSane ) {
    return true;
  }

  if ( model_name() == string() ) {
    // model_name が設定されていない．
    set_model_name("network");
  }

  bool error = false;

  // 各ノードのファンインとして参照されているノード番号が定義されているか調べる．
  // 同時にファンアウト情報を設定する．
  for (HashMapIterator<ymuint, BnNode*> p = mNodeMap.begin();
       p != mNodeMap.end(); ++ p) {
    ymuint node_id = p.key();
    BnNode* node = p.value();
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      ymuint inode_id = node->fanin(i);
      BnNode* inode = nullptr;
      if ( mNodeMap.find(inode_id, inode) ) {
	inode->add_fanout(node_id);
      }
      else {
	// inode_id が未定義
	cout << "undefined node#" << node_id << endl;
	error = true;
      }
    }
    if ( node->is_output() || node->is_dff() ) {
      ymuint inode_id = node->input();
      BnNode* inode = nullptr;
      if ( mNodeMap.find(inode_id, inode) ) {
	inode->add_fanout(node_id);
      }
      else {
	// inode_id が未定義
	cout << "undefined node#" << node_id << endl;
	error = true;
      }
    }
  }

  if ( error ) {
    return false;
  }

  // 論理ノードをトポロジカル順にソートする．

  // キュー
  Queue queue(input_num() + dff_num() + logic_num());

  // 外部入力をキューに積む．
  for (ymuint i = 0; i < input_num(); ++ i) {
    BnNode* node = mInputList[i];
    queue.put(node);
  }
  // DFFをキューに積む．
  for (ymuint i = 0; i < dff_num(); ++ i) {
    BnNode* node = mDffList[i];
    queue.put(node);
  }

  vector<BnNode*> node_list;
  node_list.reserve(logic_num());

  // キューが空になるまで処理を繰り返す．
  for ( ; ; ) {
    BnNode* node = queue.get();
    if ( node == nullptr ) {
      break;
    }
    if ( node->is_logic() ) {
      node_list.push_back(node);
    }
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      BnNode* onode = find_node(node->fanout(i));
      ymuint ni = onode->fanin_num();
      bool ready = true;
      for (ymuint j = 0; j < ni; ++ j) {
	BnNode* inode = find_node(onode->fanin(j));
	if ( !queue.check(inode)) {
	  ready = false;
	  break;
	}
      }
      if ( ready ) {
	queue.put(onode);
      }
    }
  }

  ASSERT_COND( node_list.size() == logic_num() );

  mLogicList = node_list;

  mSane = true;

  return true;
}

// @brief blif 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @retval true 正常に読み込めた
// @retval false 読み込み中にエラーが起こった．
bool
BnNetwork::read_blif(const string& filename,
		     const CellLibrary* cell_library)
{
  BlifBnNetworkHandler* handler = new BlifBnNetworkHandler(this);

  BlifParser parser;
  parser.add_handler(handler);

  bool stat = parser.read(filename, cell_library);

  // handler は parser が解放してくれる．

  return stat;
}

// @brief iscas89(.bench) 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @retval true 正常に読み込めた
// @retval false 読み込み中にエラーが起こった．
bool
BnNetwork::read_iscas89(const string& filename)
{
  Iscas89BnNetworkHandler* handler = new Iscas89BnNetworkHandler(this);

  Iscas89Parser parser;
  parser.add_handler(handler);

  bool stat = parser.read(filename);

  // handler は parser が解放してくれる．

  return stat;
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

// @brief DFF数を得る．
ymuint
BnNetwork::dff_num() const
{
  return mDffList.size();
}

// @brief DFFノードのノード番号を得る．
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
const BnNode*
BnNetwork::dff(ymuint pos) const
{
  ASSERT_COND( pos < dff_num() );
  return mDffList[pos];
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

// @brief ノード情報を得る．
// @param[in] node_id ノード番号
//
// 該当するノードがない場合には nullptr を返す．
const BnNode*
BnNetwork::find_node(ymuint node_id) const
{
  BnNode* node = nullptr;
  if ( mNodeMap.find(node_id, node) ) {
    return node;
  }
  return nullptr;
}

// @brief ノードを得る．
// param[in] node_id ノード番号
//
// 該当するノードがない場合には nullptr を返す．
BnNode*
BnNetwork::find_node(ymuint node_id)
{
  BnNode* node = nullptr;
  if ( mNodeMap.find(node_id, node) ) {
    return node;
  }
  return nullptr;
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
  if ( !mSane ) {
    // 正しい状態でなければ何もしない．
    return;
  }

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
    s << "output#" << i << ": (" << node->name() << ")" << endl
      << "    input: " << node->input() << endl;
  }
  s << endl;

  ymuint nf = dff_num();
  for (ymuint i = 0; i < nf; ++ i) {
    const BnNode* node = dff(i);
    ASSERT_COND( node != nullptr );
    ASSERT_COND( node->type() == BnNode::kDFF );
    s << "dff#" << i << ": " << node->id()
      << "(" << node->name() << ")" << endl
      << "    input: " << node->input() << endl
      << "    reset: " << node->reset_val() << endl;
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
