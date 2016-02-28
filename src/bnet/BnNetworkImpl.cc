
/// @file BnNetworkImpl.cc
/// @brief BnNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BnNetworkImpl.h"
#include "BnNodeImpl.h"

#include "ym/BnFuncType.h"
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
// @param[in] node_name ノード名
// @param[in] inode_id ファンインのID番号
void
BnNetworkImpl::new_output(ymuint node_id,
			  const char* node_name)
{
  ymuint onode_id = mNodeArray.size();
  const char* new_node_name = alloc_string(node_name);

  void* p = mAlloc.get_memory(sizeof(BnOutputNode));
  BnNodeImpl* node = new (p) BnOutputNode(onode_id, new_node_name, node_id);

  set_node(onode_id, node);

  mPOArray.push_back(node);
}

// @brief D-FFノードを生成する．
// @param[in] node_id ノードID
// @param[in] node_name ノード名
// @param[in] inode_id ファンインのID番号
// @param[in] rval リセット値 ( '0', '1', ' ' のいずれか )
void
BnNetworkImpl::new_dff(ymuint node_id,
		       const char* node_name,
		       ymuint inode_id,
		       char rval)
{
  const char* new_node_name = alloc_string(node_name);

  void* p = mAlloc.get_memory(sizeof(BnDffNode));
  BnNodeImpl* node = new (p) BnDffNode(node_id, new_node_name, inode_id, rval);

  set_node(node_id, node);

  mFFArray.push_back(node);
}

// @brief 論理ノードを生成する．
// @param[in] node_id ノードID
// @param[in] node_name ノード名
// @param[in] inode_id_array ファンインのID番号の配列
// @param[in] func_type 論理関数の型
void
BnNetworkImpl::new_logic(ymuint node_id,
			 const char* node_name,
			 const vector<ymuint>& inode_id_array,
			 const BnFuncType* func_type)
{
  const char* new_node_name = alloc_string(node_name);
  ymuint ni = inode_id_array.size();
  ASSERT_COND( ni == func_type->input_num() );
  ymuint* fanins = new_fanin_array(inode_id_array);

  void* p = mAlloc.get_memory(sizeof(BnLogicNode));
  BnNodeImpl* node = new (p) BnLogicNode(node_id, new_node_name, fanins, func_type);

  set_node(node_id, node);

  mLogicArray.push_back(node);
}

// @brief プリミティブ型を生成する．
const BnFuncType*
BnNetworkImpl::new_primitive_type(BnFuncType::Type prim_type,
				  ymuint input_num)
{
  return mFuncTypeMgr.primitive_type(prim_type, input_num);
}

// @brief セル型を生成する．
const BnFuncType*
BnNetworkImpl::new_cell_type(const Cell* cell)
{
  return mFuncTypeMgr.cell_type(cell);
}

// @brief 論理式型を生成する．
const BnFuncType*
BnNetworkImpl::new_expr_type(const Expr& expr,
			     ymuint input_num)
{
  return mFuncTypeMgr.expr_type(expr, input_num);
}

// @brief 真理値表型を生成する．
const BnFuncType*
BnNetworkImpl::new_tv_type(const TvFunc& tv)
{
  return mFuncTypeMgr.tv_type(tv);
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
  for (vector<BnNode*>::const_iterator p = mPOArray.begin();
       p != mPOArray.end(); ++ p) {
    const BnNode* node = *p;
    s << ".outputs " << node->name() << endl;
  }
  for (vector<BnNode*>::const_iterator p = mFFArray.begin();
       p != mFFArray.end(); ++ p) {
    const BnNode* node = *p;
    const BnNode* inode = this->node(node->inode_id());
    s << ".latch " << inode->name() << " " << node->name();
    if ( node->reset_val() != ' ' ) {
      s << ' ' << node->reset_val();
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
    {
      const BnFuncType* func_type = node->func_type();
      switch ( func_type->type() ) {
      case BnFuncType::kFt_C0:	 s << "C0" << endl; break;
      case BnFuncType::kFt_C1:   s << "C1" << endl; break;
      case BnFuncType::kFt_BUFF: s << "BUFF" << endl; break;
      case BnFuncType::kFt_NOT:  s << "NOT"  << endl; break;
      case BnFuncType::kFt_AND:  s << "AND"  << endl; break;
      case BnFuncType::kFt_NAND: s << "NAND" << endl; break;
      case BnFuncType::kFt_OR:   s << "OR"   << endl; break;
      case BnFuncType::kFt_NOR:  s << "NOR"  << endl; break;
      case BnFuncType::kFt_XOR:  s << "XOR"  << endl; break;
      case BnFuncType::kFt_XNOR: s << "XNOR" << endl; break;
      case BnFuncType::kFt_CELL:
	{
	  ymuint ni = node->fanin_num();
	  const Cell* cell = func_type->cell();
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
	break;
      case BnFuncType::kFt_EXPR: s << func_type->expr() << endl; break;
      case BnFuncType::kFt_TV:   s << func_type->truth_vector() << endl; break;
      default: ASSERT_NOT_REACHED; break;
      }
    }
  }
  s << ".end" << endl;
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
