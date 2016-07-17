
/// @file BnBuilder.cc
/// @brief BnBuilder の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnBuilder.h"
#include "ym/Cell.h"

#include "ym/BlifParser.h"
#include "BlifBnNetworkHandler.h"

#include "ym/Iscas89Parser.h"
#include "Iscas89BnNetworkHandler.h"


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

// @brief blif 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @retval true 正常に読み込めた
// @retval false 読み込み中にエラーが起こった．
bool
BnBuilder::read_blif(const string& filename,
		     const CellLibrary* cell_library)
{
  BlifBnNetworkHandler* handler = new BlifBnNetworkHandler(this);

  BlifParser parser;
  parser.add_handler(handler);

  bool stat = parser.read(filename, cell_library);

  return stat;
}

// @brief iscas89(.bench) 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @retval true 正常に読み込めた
// @retval false 読み込み中にエラーが起こった．
bool
BnBuilder::read_iscas89(const string& filename)
{
  Iscas89BnNetworkHandler* handler = new Iscas89BnNetworkHandler(this);

  Iscas89Parser parser;
  parser.add_handler(handler);

  bool stat = parser.read(filename);

  // handler は parser が解放してくれる．

  return stat;
}

// @brief 内容を書き出す．
// @param[in] s 出力先のストリーム
void
BnBuilder::write(ostream& s) const
{
  s << "model_name: " << name() << endl;

  ymuint np = port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const BnBuilder::PortInfo& port_info = port(i);
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
    const BnBuilder::DffInfo& dff_info = dff(i);
    s << "  Dff#" << i << endl
	 << "    name:   " << dff_info.mName << endl
	 << "    input:  " << dff_info.mInput << endl
	 << "    output: " << dff_info.mOutput << endl
	 << "    clock:  " << dff_info.mClock << endl;
    if ( dff_info.mClear != -1 ) {
      s << "    clear:  " << dff_info.mClear << endl;
    }
    if ( dff_info.mPreset != -1 ) {
      s << "    preset: " << dff_info.mPreset << endl;
    }
    s << endl;
  }

  ymuint nlatch = latch_num();
  for (ymuint i = 0; i < nlatch; ++ i) {
    const BnBuilder::LatchInfo& latch_info = latch(i);
    s << "  Latch#" << i << endl
	 << "    name:   " << latch_info.mName << endl
	 << "    input:  " << latch_info.mInput << endl
	 << "    output: " << latch_info.mOutput << endl
	 << "    enable: " << latch_info.mEnable << endl;
    if ( latch_info.mClear != -1 ) {
      s << "    clear:  " << latch_info.mClear << endl;
    }
    if ( latch_info.mPreset != -1 ) {
      s << "    preset: " << latch_info.mPreset << endl;
    }
    s << endl;
  }

  ymuint nn = node_num();
  for (ymuint i = 0; i < nn; ++ i) {
    const BnBuilder::NodeInfo& node_info = node(i);
    s << "  Node#" << i << endl
	 << "    name:   " << node_info.mName << endl;
    if ( node_info.mType == BnNode::kInput ) {
      s << "    type:   INPUT" << endl;
    }
    else if ( node_info.mType == BnNode::kOutput ) {
      s << "    type:   OUTPUT" << endl
	   << "    input:  " << node_info.mInodeList[0] << endl;
    }
    else if ( node_info.mType == BnNode::kLogic ) {
      s << "    type:   LOGIC" << endl;
      s << "    fanins: ";
      ymuint ni = node_info.mInodeList.size();
      for (ymuint j = 0; j < ni; ++ j) {
	s << " " << node_info.mInodeList[j];
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
      s << endl;
    }
    s << endl;
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
}

// @brief ネットワーク名を設定する．
// @param[in] name ネットワーク名
void
BnBuilder::set_model_name(const string& name)
{
  mName = name;
}

// @brief ポートを追加する．
// @param[in] port_name ポート名
// @param[in] bits 内容のノード番号のリスト
void
BnBuilder::add_port(const string& port_name,
		    const vector<ymuint>& bits)
{
  mPortInfoList.push_back(PortInfo(port_name, bits));
}

// @brief ポートを追加する(1ビット版)．
// @param[in] port_name ポート名
// @param[in] bit 内容のノード番号
void
BnBuilder::add_port(const string& port_name,
		    ymuint bit)
{
  add_port(port_name, vector<ymuint>(1, bit));
}

// @brief DFFを追加する．
// @param[in] name DFF名
ymuint
BnBuilder::add_dff(const string& name)
{
  ymuint id = mDffInfoList.size();
  mDffInfoList.push_back(DffInfo(name));
  return id;
}

// @brief DFFのデータ入力を設定する．
// @param[in] dff_id DFF番号
// @param[in] input 入力のノード番号
void
BnBuilder::set_dff_input(ymuint dff_id,
			 ymuint input)
{
  ASSERT_COND( dff_id < dff_num() );
  mDffInfoList[dff_id].mInput = input;
}

// @brief DFFのクロック端子を設定する．
// @param[in] dff_id DFF番号
// @param[in] clock クロック端子のノード番号
void
BnBuilder::set_dff_clock(ymuint dff_id,
			 ymuint clock)
{
  ASSERT_COND( dff_id < dff_num() );
  mDffInfoList[dff_id].mClock = clock;
}

// @brief DFFのクリア端子を設定する．
// @param[in] dff_id DFF番号
// @param[in] clear クリア端子のノード番号
void
BnBuilder::set_dff_clear(ymuint dff_id,
			 ymuint clear)
{
  ASSERT_COND( dff_id < dff_num() );
  mDffInfoList[dff_id].mClear = clear;
}

// @brief DFFのプリセット端子を設定する．
// @param[in] dff_id DFF番号
// @param[in] preset プリセット端子のノード番号
void
BnBuilder::set_dff_preset(ymuint dff_id,
			  ymuint preset)
{
  ASSERT_COND( dff_id < dff_num() );
  mDffInfoList[dff_id].mPreset = preset;
}

// @brief DFFの出力端子を設定する．
// @param[in] dff_id DFF番号
// @param[in] output 出力のノード番号
void
BnBuilder::set_dff_output(ymuint dff_id,
			  ymuint output)
{
  ASSERT_COND( dff_id < dff_num() );
  mDffInfoList[dff_id].mOutput = output;
}

// @brief ラッチを追加する．
// @param[in] name ラッチ名
// @return ラッチ番号を返す．
ymuint
BnBuilder::add_latch(const string& name)
{
  ymuint id = mLatchInfoList.size();
  mLatchInfoList.push_back(LatchInfo(name));
  return id;
}

// @brief ラッチのデータ入力を設定する．
// @param[in] latch_id ラッチ番号
// @param[in] input 入力端子のノード番号
void
BnBuilder::set_latch_input(ymuint latch_id,
			   ymuint input)
{
  ASSERT_COND( latch_id < latch_num() );
  mLatchInfoList[latch_id].mInput = input;
}

// @brief ラッチのイネーブル端子を設定する．
// @param[in] latch_id ラッチ番号
// @param[in] enable イネーブル端子のノード番号
void
BnBuilder::set_latch_enable(ymuint latch_id,
			    ymuint enable)
{
  ASSERT_COND( latch_id < latch_num() );
  mLatchInfoList[latch_id].mEnable = enable;
}

// @brief ラッチのクリア端子を設定する．
// @param[in] latch_id ラッチ番号
// @param[in] clear クリア端子のノード番号
void
BnBuilder::set_latch_clear(ymuint latch_id,
			   ymuint clear)
{
  ASSERT_COND( latch_id < latch_num() );
  mLatchInfoList[latch_id].mClear = clear;
}

// @brief ラッチのプリセット端子を設定する．
// @param[in] latch_id ラッチ番号
// @param[in] preset プリセット端子のノード番号
void
BnBuilder::set_latch_preset(ymuint latch_id,
			    ymuint preset)
{
  ASSERT_COND( latch_id < latch_num() );
  mLatchInfoList[latch_id].mPreset = preset;
}

// @brief ラッチの出力を設定する．
// @param[in] latch_id ラッチ番号
// @param[in] output 出力端子のノード番号
void
BnBuilder::set_latch_output(ymuint latch_id,
			    ymuint output)
{
  ASSERT_COND( latch_id < latch_num() );
  mLatchInfoList[latch_id].mOutput = output;
}

// @brief 外部入力ノードを追加する．
// @param[in] node_name ノード名
// @return ノード番号を返す．
//
// ノード名の重複に関しては感知しない．
ymuint
BnBuilder::add_input(const string& node_name)
{
  ymuint id = mNodeInfoList.size();
  mNodeInfoList.push_back(NodeInfo(node_name));
  return id;
}

// @brief 外部出力ノードを追加する．
// @param[in] node_name ノード名
// @return ノード番号を返す．
//
// ノード名の重複に関しては感知しない．
ymuint
BnBuilder::add_output(const string& node_name)
{
  ymuint id = mNodeInfoList.size();
  mNodeInfoList.push_back(NodeInfo(node_name, 0));
  return id;
}

// @brief 外部出力ノードの入力を設定する．
// @param[in] node_id ノード番号
// @param[in] input 入力のノード番号
void
BnBuilder::set_output_input(ymuint node_id,
			    ymuint input)
{
  ASSERT_COND( node_id < node_num() );
  mNodeInfoList[node_id].mInodeList[0] = input;
}

// @brief プリミティブ型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni ファンイン数
// @param[in] prim_type プリミティブの型
// @return ノード番号を返す．
//
// ノード名の重複に関しては感知しない．
ymuint
BnBuilder::add_primitive(const string& node_name,
			 ymuint ni,
			 BnLogicType prim_type)
{
  ymuint id = mNodeInfoList.size();
  mNodeInfoList.push_back(NodeInfo(node_name, ni, prim_type));
  return id;
}

// @brief セル型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] cell セル
// @return ノード番号を返す．
//
// ノード名の重複に関しては感知しない．
ymuint
BnBuilder::add_cell(const string& node_name,
		    const Cell* cell)
{
  ymuint id = mNodeInfoList.size();
  ymuint ni = cell->input_num();
  mNodeInfoList.push_back(NodeInfo(node_name, ni, cell));
  return id;
}

// @brief 論理式型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] ni ファンイン数
// @param[in] expr 論理式
// @return ノード番号を返す．
//
// ノード名の重複に関しては感知しない．
ymuint
BnBuilder::add_expr(const string& node_name,
		    ymuint ni,
		    const Expr& expr)
{
  ymuint id = mNodeInfoList.size();
  mNodeInfoList.push_back(NodeInfo(node_name, ni, expr));
  return id;
}

// @brief 真理値表型の論理ノードを追加する．
// @param[in] node_name ノード名
// @param[in] tv_func 真理値表
// @return ノード番号を返す．
//
// ノード名の重複に関しては感知しない．
ymuint
BnBuilder::add_tv(const string& node_name,
		  const TvFunc& tv)
{
  ymuint id = mNodeInfoList.size();
  mNodeInfoList.push_back(NodeInfo(node_name, tv));
  return id;
}

// @brief 論理ノードのファンインを設定する．
// @param[in] node_id ノード番号
// @param[in] pos ファンイン番号
// @param[in] input 入力ノードの番号
void
BnBuilder::set_fanin(ymuint node_id,
		     ymuint pos,
		     ymuint input)
{
  ASSERT_COND( node_id < node_num() );
  NodeInfo& node_info = mNodeInfoList[node_id];
  ASSERT_COND( pos < node_info.mInodeList.size() );
  node_info.mInodeList[pos] = input;
}

// @brief 名前を得る．
string
BnBuilder::name() const
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
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
const BnBuilder::DffInfo&
BnBuilder::dff(ymuint pos) const
{
  ASSERT_COND( pos < dff_num() );
  return mDffInfoList[pos];
}

// @brief ラッチ数を得る．
ymuint
BnBuilder::latch_num() const
{
  return mLatchInfoList.size();
}

// @brief ラッチ情報を得る．
// @param[in] pos 位置番号 ( 0 <= pos < latch_num() )
const BnBuilder::LatchInfo&
BnBuilder::latch(ymuint pos) const
{
  ASSERT_COND( pos < latch_num() );
  return mLatchInfoList[pos];
}

// @brief ノード数を得る．
ymuint
BnBuilder::node_num() const
{
  return mNodeInfoList.size();
}

// @brief ノード情報を得る．
// @param[in] id ノード番号
const BnBuilder::NodeInfo&
BnBuilder::node(ymuint id) const
{
  ASSERT_COND( id < node_num() );
  return mNodeInfoList[id];
}

END_NAMESPACE_YM_BNET
