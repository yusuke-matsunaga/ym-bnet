
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

// @brief ネットワーク名を設定する．
// @param[in] name ネットワーク名
void
BnNetwork::set_model_name(const string& name)
{
  mNetworkName = name;
}

// @brief ポートを追加する．
// @param[in] port_name ポート名
// @param[in] dir 方向
// @param[in] bits 内容のノード番号のリスト
void
BnNetwork::new_port(const string& port_name,
		    BnPort::Direction dir,
		    const vector<ymuint>& bits)
{
  mPortList.push_back(new BnPortImpl(port_name, dir, bits));

  mSane = false;
}

// @brief 外部入力ノードを追加する．
// @param[in] node_id ノードID番号
// @param[in] node_name ノード名
// @return 追加が成功したら true を返す．
//
// すでに同じノード番号が存在したら失敗する．
// ノード名の重複に関しては感知しない．
bool
BnNetwork::new_input(ymuint node_id,
		     const string& node_name)
{
  if ( mNodeMap.check(node_id) ) {
    return false;
  }

  BnNode* node = new BnInputNode(node_id, node_name);
  mNodeMap.add(node_id, node);
  mInputList.push_back(node);

  mSane = false;

  return true;
}

// @brief DFFノードを追加する．
// @param[in] node_id ノードID番号
// @param[in] node_name ノード名
// @param[in] inode_id 入力のノード番号
// @param[in] reset_val リセット値 ('0', '1', '-' のいずれか)
// @return 追加が成功したら true を返す．
//
// すでに同じノード番号が存在したら失敗する．
// ノード名の重複に関しては感知しない．
bool
BnNetwork::new_dff(ymuint node_id,
		   const string& node_name,
		   ymuint inode_id,
		   char reset_val)
{
  if ( mNodeMap.check(node_id) ) {
    return false;
  }

  BnNode* node = new BnDffNode(node_id, node_name, inode_id, reset_val);
  mNodeMap.add(node_id, node);
  mDffList.push_back(node);

  mSane = false;

  return true;
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
bool
BnNetwork::new_primitive(ymuint node_id,
			 const string& node_name,
			 const vector<ymuint>& inode_id_list,
			 BnLogicType prim_type)
{
  if ( mNodeMap.check(node_id) ) {
    return false;
  }

  BnNode* node = new BnPrimNode(node_id, node_name, inode_id_list, prim_type);
  mNodeMap.add(node_id, node);
  mLogicList.push_back(node);

  mSane = false;

  return true;
}

// @brief セル型の論理ノードを追加する．
// @param[in] node_id ノードID番号
// @param[in] node_name ノード名
// @param[in] inode_id_list ファンインのノード番号のリスト
// @param[in] cell セル
// @return 追加が成功したら true を返す．
//
// すでに同じノード番号が存在したら失敗する．
// ノード名の重複に関しては感知しない．
bool
BnNetwork::new_cell(ymuint node_id,
		    const string& node_name,
		    const vector<ymuint>& inode_id_list,
		    const Cell* cell)
{
  if ( mNodeMap.check(node_id) ) {
    return false;
  }

  BnNode* node = new BnCellNode(node_id, node_name, inode_id_list, cell);
  mNodeMap.add(node_id, node);
  mLogicList.push_back(node);

  mSane = false;

  return true;
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] node_id ノードID番号
// @param[in] node_name ノード名
// @param[in] inode_id_list ファンインのノード番号のリスト
// @param[in] expr 論理式
// @return 追加が成功したら true を返す．
//
// すでに同じノード番号が存在したら失敗する．
// ノード名の重複に関しては感知しない．
bool
BnNetwork::new_expr(ymuint node_id,
		    const string& node_name,
		    const vector<ymuint>& inode_id_list,
		    const Expr& expr)
{
  if ( mNodeMap.check(node_id) ) {
    return false;
  }

  // expr がプリミティブ型かどうかチェックする．
  BnLogicType logic_type = BnNode::expr2logic_type(expr);

  BnNode* node = nullptr;
  if ( logic_type == kBnLt_EXPR ) {
    node = new BnExprNode(node_id, node_name, inode_id_list, expr);
  }
  else {
    node = new BnPrimNode(node_id, node_name, inode_id_list, logic_type);
  }
  mNodeMap.add(node_id, node);
  mLogicList.push_back(node);

  mSane = false;

  return true;
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] node_id ノードID番号
// @param[in] node_name ノード名
// @param[in] inode_id_list ファンインのノード番号のリスト
// @param[in] tv_func 心理値表
// @return 追加が成功したら true を返す．
//
// すでに同じノード番号が存在したら失敗する．
// ノード名の重複に関しては感知しない．
bool
BnNetwork::new_tv(ymuint node_id,
		  const string& node_name,
		  const vector<ymuint>& inode_id_list,
		  const TvFunc& tv)
{
  if ( mNodeMap.check(node_id) ) {
    return false;
  }

  BnNode* node = new BnTvNode(node_id, node_name, inode_id_list, tv);
  mNodeMap.add(node_id, node);
  mLogicList.push_back(node);

  mSane = false;

  return true;
}

// @brief 最終処理と整合性のチェックを行う．
// @retval true 問題なし
// @retval false エラーがあった．
//
// 検査項目は以下の通り．
// - model_name が設定されているか
//   設定されていなければデフォルト値を使う．
//   -> warning
// - 各ポートで参照しているノード番号が存在するか
//   存在しなければ error
// - 各ノードのファンインのノード番号が存在するか
//   存在しなければ error
//
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

  // ポート内で参照されているノード番号が定義されているか調べる．
  for (ymuint i = 0; i < port_num(); ++ i) {
    const BnPort* port = mPortList[i];
    // port の名前は任意
    ymuint bw = port->bit_width();
    for (ymuint j = 0; j < bw; ++ j) {
      ymuint node_id = port->bit(j);
      if ( !mNodeMap.check(node_id) ) {
	// node_id 番のノードが未定義
	cout << "undefined node#" << node_id << endl;
	error = true;
      }
    }
  }

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
  }

  if ( !error ) {
    mSane = true;
  }

  return !error;
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
    if ( port->direction() == BnPort::kIn ) {
      s << " INPUT ";
    }
    else {
      s << " OUTPUT ";
    }
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
    ASSERT_COND( node->type() == BnNode::kInput );
    s << "input#" << i << ": " << node->id()
      << "(" << node->name() << ")" << endl;
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
    case kBnLt_CELL:
      s << "cell: " << node->cell()->name();
      break;
    case kBnLt_EXPR:
      s << "expr: " << node->expr();
      break;
    case kBnLt_TV:
      s << "tv: " << node->tv();
      break;
    }
    s << endl
      << endl;
  }
}

END_NAMESPACE_YM_BNET
