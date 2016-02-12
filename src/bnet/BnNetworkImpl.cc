
/// @file BnNetworkImpl.cc
/// @brief BnNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BnNetworkImpl.h"
#include "BnNodeImpl.h"
#include "ym/BlifParser.h"
#include "ym/BlifCover.h"
#include "ym/Iscas89Parser.h"
#include "BlifBnNetworkHandler.h"
#include "Iscas89BnNetworkHandler.h"
#include "ym/Cell.h"
#include "ym/CellPin.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BnNetworkImpl::BnNetworkImpl() :
  mAlloc(4096)
{
}

// @brief デストラクタ
BnNetworkImpl::~BnNetworkImpl()
{
}

// @brief 内容をクリアする．
void
BnNetworkImpl::clear()
{
  mName = nullptr;
  mNodeArray.clear();
  mPIArray.clear();
  mPOArray.clear();
  mFFArray.clear();
  mLogicArray.clear();
  mAlloc.destroy();
}

// @brief .model 名の設定
void
BnNetworkImpl::set_model(const char* name)
{
  mName = alloc_string(name);
}

// @brief 外部入力ノードを生成する．
// @param[in] node_id ノードID
// @param[in] node_name ノード名
void
BnNetworkImpl::new_input(ymuint node_id,
			 const char* node_name)
{
  const char* new_node_name = alloc_string(node_name);

  void* p = mAlloc.get_memory(sizeof(BnInputNode));
  BnNodeImpl* node = new (p) BnInputNode(node_id, new_node_name);

  set_node(node_id, node);

  mPIArray.push_back(node);
}

// @brief 外部出力ノードの番号を登録する．
// @param[in] node_id ノードID
void
BnNetworkImpl::new_output(ymuint node_id)
{
  mPOArray.push_back(node_id);
}

// @brief ラッチノードを生成する．
// @param[in] node_id ノードID
// @param[in] node_name ノード名
// @param[in] inode_id ファンインのID番号
// @param[in] rval リセット値 ( '0', '1', ' ' のいずれか )
void
BnNetworkImpl::new_latch(ymuint node_id,
			 const char* node_name,
			 ymuint inode_id,
			 char rval)
{
  const char* new_node_name = alloc_string(node_name);

  void* p = mAlloc.get_memory(sizeof(BnLatchNode));
  BnNodeImpl* node = new (p) BnLatchNode(node_id, new_node_name, inode_id, rval);

  set_node(node_id, node);

  mFFArray.push_back(node);
}

// @brief プリミティブノードを生成する．
// @param[in] node_id ノードID
// @param[in] node_name ノード名
// @param[in] inode_id_array ファンインのID番号の配列
// @param[in] gate_type ゲートの型
void
BnNetworkImpl::new_logic(ymuint node_id,
			 const char* node_name,
			 const vector<ymuint>& inode_id_array,
			 GateType gate_type)
{
  const char* new_node_name = alloc_string(node_name);
  ymuint ni = inode_id_array.size();
  ymuint* fanins = new_fanin_array(inode_id_array);

  void* p = mAlloc.get_memory(sizeof(BnPrimNode));
  BnNodeImpl* node = new (p) BnPrimNode(node_id, new_node_name, ni, fanins, gate_type);

  set_node(node_id, node);

  mLogicArray.push_back(node);
}

// @brief ゲートノードを生成する．
// @param[in] node_id ノードID
// @param[in] node_name ノード名
// @param[in] inode_id_array ファンインのID番号の配列
// @param[in] cell セルへのポインタ
void
BnNetworkImpl::new_logic(ymuint node_id,
			 const char* node_name,
			 const vector<ymuint>& inode_id_array,
			 const Cell* cell)
{
  const char* new_node_name = alloc_string(node_name);
  ymuint ni = inode_id_array.size();
  ymuint* fanins = new_fanin_array(inode_id_array);

  void* p = mAlloc.get_memory(sizeof(BnCellNode));
  BnNodeImpl* node = new (p) BnCellNode(node_id, new_node_name, ni, fanins, cell);

  set_node(node_id, node);

  mLogicArray.push_back(node);
}

// @brief カバーノードを生成する．
// @param[in] node_id ノードID
// @param[in] node_name ノード名
// @param[in] inode_id_array ファンインのID番号の配列
// @param[in] cover カバー
void
BnNetworkImpl::new_logic(ymuint node_id,
			 const char* node_name,
			 const vector<ymuint>& inode_id_array,
			 ymuint cover_id)
{
  const char* new_node_name = alloc_string(node_name);
  ymuint ni = inode_id_array.size();
  ymuint* fanins = new_fanin_array(inode_id_array);
  const BlifCover* cover = mCoverArray[cover_id];

  void* p = mAlloc.get_memory(sizeof(BnCoverNode));
  BnNodeImpl* node = new (p) BnCoverNode(node_id, new_node_name, ni, fanins, cover);

  set_node(node_id, node);

  mLogicArray.push_back(node);
}

// @brief 論理式ノードを生成する．
// @param[in] node_id ノードID
// @param[in] node_name ノード名
// @param[in] inode_id_array ファンインのID番号の配列
// @param[in] expr 論理式
void
BnNetworkImpl::new_logic(ymuint node_id,
			 const char* node_name,
			 const vector<ymuint>& inode_id_array,
			 Expr expr)
{
  const char* new_node_name = alloc_string(node_name);
  ymuint ni = inode_id_array.size();
  ymuint* fanins = new_fanin_array(inode_id_array);

  void* p = mAlloc.get_memory(sizeof(BnExprNode));
  BnNodeImpl* node = new (p) BnExprNode(node_id, new_node_name, ni, fanins, expr);

  set_node(node_id, node);

  mLogicArray.push_back(node);
}

// @brief 真理値表ノードを生成する．
// @param[in] node_id ノードID
// @param[in] node_name ノード名
// @param[in] inode_id_array ファンインのID番号の配列
// @param[in] tv 真理値表
void
BnNetworkImpl::new_logic(ymuint node_id,
			 const char* node_name,
			 const vector<ymuint>& inode_id_array,
			 TvFunc tv)
{
  const char* new_node_name = alloc_string(node_name);
  ymuint ni = inode_id_array.size();
  ymuint* fanins = new_fanin_array(inode_id_array);

  void* p = mAlloc.get_memory(sizeof(BnTvNode));
  BnNodeImpl* node = new (p) BnTvNode(node_id, new_node_name, ni, fanins, tv);

  set_node(node_id, node);

  mLogicArray.push_back(node);
}

// @brief ファンインのノード番号の配列を作る．
ymuint*
BnNetworkImpl::new_fanin_array(const vector<ymuint>& fanin_array)
{
  ymuint ni = fanin_array.size();
  ymuint* fanins = nullptr;
  if ( ni > 0 ) {
    void* q = mAlloc.get_memory(sizeof(ymuint) * ni);
    fanins = new (q) ymuint[ni];
    for (ymuint i = 0; i < ni; ++ i) {
      fanins[i] = fanin_array[i];
    }
  }
  return fanins;
}

// @brief ノードをセットする．
// @param[in] node_id ノードID
// @param[in] node 対象のノード
void
BnNetworkImpl::set_node(ymuint node_id,
			BnNodeImpl* node)
{
  while ( mNodeArray.size() <= node_id ) {
    mNodeArray.push_back(nullptr);
  }

  mNodeArray[node_id] = node;
}

// @brief カバーを登録する．
void
BnNetworkImpl::new_cover(ymuint cover_id,
			 ymuint input_num,
			 ymuint cube_num,
			 const string& ipat_str,
			 BlifPat opat)
{
  // キューブ1つ分のブロック数
  ymuint nb1 = ((input_num * 2) + 63) / 64;
  // 全ブロック数
  ymuint nb = nb1 * cube_num;

  void* p = mAlloc.get_memory(sizeof(BlifCover) + sizeof(ymuint64) * (nb - 1));
  BlifCover* cover = new (p) BlifCover;
  cover->mInputNum = input_num;
  cover->mOutputPat = opat;
  cover->mCubeNum = cube_num;
  cover->mId = mCoverArray.size();
  ASSERT_COND( cover->mId == cover_id );
  mCoverArray.push_back(cover);
  ymuint j = 0;
  ymuint k = 0;
  for (ymuint c = 0; c < cube_num; ++ c) {
    ymuint64 tmp = 0UL;
    ymuint shift = 0;
    for (ymuint i = 0; i < input_num; ++ i, ++ k) {
      ymuint64 pat;
      switch ( ipat_str[k] ) {
      case '0': pat = 0UL; break;
      case '1': pat = 1UL; break;
      case '-': pat = 2UL; break;
      default: ASSERT_NOT_REACHED;
      }
      tmp |= (pat << shift);
      shift += 2;
      if ( shift == 64 ) {
	cover->mPatArray[j] = tmp;
	tmp = 0UL;
	shift = 0;
	++ j;
      }
    }
    if ( shift > 0 ) {
      cover->mPatArray[j] = tmp;
      ++ j;
    }
  }
}

// @brief 文字列領域を確保する．
const char*
BnNetworkImpl::alloc_string(const char* src_str)
{
  ymuint l = strlen(src_str);
  void* p = mAlloc.get_memory(l + 1);
  char* dst_str = new (p) char[l + 1];
  const char* dst_top = dst_str;
  while ( (*dst_str ++ = *src_str ++) ) ;
  return dst_top;
}

// @brief blif 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @retval true 正常に読み込めた
// @retval false 読み込み中にエラーが起こった．
bool
BnNetworkImpl::read_blif(const string& filename,
			 const CellLibrary* cell_library)
{
  BlifParser parser;
  BlifBnNetworkHandler* handler = new BlifBnNetworkHandler(this);
  parser.add_handler(handler);

  bool stat = parser.read(filename, cell_library);

  // handler は parser が解放してくれる．

  if ( stat ) {
    wrap_up();
  }

  return stat;
}

// @brief 内容を blif 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnNetworkImpl::write_blif(ostream& s) const
{
  s << ".model " << name() << endl;
  for (vector<BnNode*>::const_iterator p = mPIArray.begin();
       p != mPIArray.end(); ++ p) {
    const BnNode* node = *p;
    s << ".inputs " << node->name() << endl;
  }
  for (vector<ymuint>::const_iterator p = mPOArray.begin();
       p != mPOArray.end(); ++ p) {
    const BnNode* node = this->node(*p);
    s << ".outputs " << node->name() << endl;
  }
  for (vector<BnNode*>::const_iterator p = mFFArray.begin();
       p != mFFArray.end(); ++ p) {
    const BnNode* node = *p;
    const BnNode* inode = this->node(node->inode_id());
    s << ".latch " << inode->name() << " " << node->name();
    if ( node->reset_val() != ' ' ) {
      s << node->reset_val();
    }
    s << endl;
  }
  for (vector<BnNode*>::const_iterator p = mLogicArray.begin();
       p != mLogicArray.end(); ++ p) {
    const BnNode* node = *p;
    s << ".names";
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const BnNode* inode = this->node(node->fanin_id(i));
      s << " " << inode->name();
    }
    s << " " << node->name() << endl;
    const BlifCover* cover = node->cover();
    cover->print(s);
  }
#if 0
  for (vector<BnNode*>::const_iterator p = mGateArray.begin();
       p != mGateArray.end(); ++ p) {
    const BnNode* node = *p;
    ymuint ni = node->fanin_num();
    const Cell* cell = node->cell();
    ASSERT_COND( ni == cell->input_num() );
    ASSERT_COND( cell->output_num() == 1 );
    ASSERT_COND( cell->inout_num() == 0 );
    s << ".gate " << cell->name();
    for (ymuint i = 0; i < ni; ++ i) {
      const CellPin* ipin = cell->input(i);
      const BnNode* inode = this->node(node->fanin_id(i));
      s << " " << ipin->name() << "=" << inode->name();
    }
    const CellPin* opin = cell->output(0);
    s << " " << opin->name() << "=" << node->name() << endl;
  }
#endif
  s << ".end" << endl;
}

// @brief iscas89(.bench) 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @retval true 正常に読み込めた
// @retval false 読み込み中にエラーが起こった．
bool
BnNetworkImpl::read_iscas89(const string& filename)
{
  Iscas89Parser parser;
  Iscas89BnNetworkHandler* handler = new Iscas89BnNetworkHandler(this);
  parser.add_handler(handler);

  bool stat = parser.read(filename);

  // handler は parser が解放してくれる．

  if ( stat ) {
    wrap_up();
  }

  return stat;
}

// @brief 内容を iscas89 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnNetworkImpl::write_iscas89(ostream& s) const
{
}

// @brief 各ノードのファンアウト情報を設定する．
void
BnNetworkImpl::wrap_up()
{
  // ファンアウト数を数える．
  for (vector<BnNodeImpl*>::iterator p = mNodeArray.begin();
       p != mNodeArray.end(); ++ p) {
    BnNode* node = *p;
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      ymuint iid = node->fanin_id(i);
      BnNodeImpl* inode = mNodeArray[iid];
      ++ inode->mFanoutNum;
    }
  }

  // ファンアウトリストの配列を確保する．
  for (vector<BnNodeImpl*>::iterator p = mNodeArray.begin();
       p != mNodeArray.end(); ++ p) {
    BnNodeImpl* node = *p;
    ymuint no = node->mFanoutNum;
    if ( no == 0 ) {
      continue;
    }
    void* q = mAlloc.get_memory(sizeof(ymuint) * no);
    node->mFanoutList = new (q) ymuint[no];
    node->mFanoutNum = 0;
  }

  // ファンアウトリストの設定を行う．
  for (vector<BnNodeImpl*>::iterator p = mNodeArray.begin();
       p != mNodeArray.end(); ++ p) {
    BnNode* node = *p;
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      ymuint iid = node->fanin_id(i);
      BnNodeImpl* inode = mNodeArray[iid];
      inode->mFanoutList[inode->mFanoutNum] = node->id();
      ++ inode->mFanoutNum;
    }
  }
}

END_NAMESPACE_YM_BNET
