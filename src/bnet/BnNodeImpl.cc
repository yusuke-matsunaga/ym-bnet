
/// @file BnNodeImpl.cc
/// @brief BnNodeImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BnNodeImpl.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNode (他に適当な場所がなかったので)
//////////////////////////////////////////////////////////////////////

// @brief 論理式から論理型を得る．
//
// 通常は kBnLt_EXPR だが場合によっては
// プリミティブ型となる．
BnLogicType
BnNode::expr2logic_type(const Expr& expr)
{
  BnLogicType logic_type = kBnLt_EXPR;
  ymuint input_num = expr.input_size();
  if ( input_num < 10 ) {
    // ただし一旦 TvFunc を作るので 10入力以下のときだけ
    TvFunc tv = expr.make_tv(input_num);
    if ( tv == TvFunc::const_zero(0) ) {
      logic_type = kBnLt_C0;
    }
    else if ( tv == TvFunc::const_one(0) ) {
      logic_type = kBnLt_C1;
    }
    else if ( tv == TvFunc::posi_literal(1, VarId(0)) ) {
      logic_type = kBnLt_BUFF;
    }
    else if ( tv == TvFunc::nega_literal(1, VarId(0)) ) {
      logic_type = kBnLt_NOT;
    }
    else {
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
	  logic_type = kBnLt_OR;
	}
	else if ( !has_1 ) {
	  // 11...11 だけ 1 で残りが 0
	  logic_type = kBnLt_AND;
	}
      }
      else if ( val_0 == 1 && val_1 == 0 ) {
	if ( !has_0 ) {
	  // 11...11 だけ 0 で残りが 1
	  logic_type = kBnLt_NAND;
	}
	else if ( !has_1 ) {
	  // 00...00 だけ 1 で残りが 0
	  logic_type = kBnLt_NOR;
	}
      }
      else if ( xor_match ) {
	logic_type = kBnLt_XOR;
      }
      else if ( xnor_match ) {
	logic_type = kBnLt_XNOR;
      }
    }
  }

  return logic_type;
}


//////////////////////////////////////////////////////////////////////
// クラス BnNodeImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
BnNodeImpl::BnNodeImpl(ymuint id,
		       const string& name) :
  mId(id),
  mName(name)
{
}

// @brief デストラクタ
BnNodeImpl::~BnNodeImpl()
{
  // mFanoutList は BnNetworkImpl が管理している．
}

// @brief ID を返す．
ymuint
BnNodeImpl::id() const
{
  return mId;
}

// @brief 名前を返す．
string
BnNodeImpl::name() const
{
  return mName;
}

// @brief 外部入力の時 true を返す．
bool
BnNodeImpl::is_input() const
{
  return false;
}

// @brief 外部出力の時 true を返す．
bool
BnNodeImpl::is_output() const
{
  return false;
}

// @brief D-FF ノードの時 true を返す．
bool
BnNodeImpl::is_dff() const
{
  return false;
}

// @brief 論理ノードの時 true を返す．
bool
BnNodeImpl::is_logic() const
{
  return false;
}

// @brief ファンアウトを追加する．
// @param[in] node ノード
void
BnNodeImpl::add_fanout(const BnNode* node)
{
  mFanoutList.push_back(node);
}

// @brief ファンアウト数を得る．
ymuint
BnNodeImpl::fanout_num() const
{
  return mFanoutList.size();
}

// @brief ファンアウトのノード番号を返す．
// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
const BnNode*
BnNodeImpl::fanout(ymuint pos) const
{
  ASSERT_COND( pos < fanout_num() );
  return mFanoutList[pos];
}

// @brief ファンイン数を得る．
ymuint
BnNodeImpl::fanin_num() const
{
  return 0;
}

// @brief ファンインのノード番号を返す．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
ymuint
BnNodeImpl::fanin(ymuint pos) const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 論理タイプを返す．
//
// is_logic() == false の時の動作は不定
BnLogicType
BnNodeImpl::logic_type() const
{
  ASSERT_NOT_REACHED;
  return kBnLt_NONE;
}

// @brief セルを返す．
//
// is_logic() == false の時の動作は不定
// logic_type() != kBnLt_CELL の時の動作は不定
const Cell*
BnNodeImpl::cell() const
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief 論理式を返す．
//
// is_logic() == false の時の動作は不定
// logic_type() != kBnLt_EXPR の時の動作は不定
Expr
BnNodeImpl::expr() const
{
  ASSERT_NOT_REACHED;
  return Expr();
}

// @brief 真理値表を返す．
//
// is_logic() == false の時の動作は不定
// logic_type() != kBnLt_TV の時の動作は不定
TvFunc
BnNodeImpl::tv() const
{
  ASSERT_NOT_REACHED;
  return TvFunc();
}

// @brief 入力のノード番号を返す．
ymuint
BnNodeImpl::input() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief リセット値を返す．
char
BnNodeImpl::reset_val() const
{
  ASSERT_NOT_REACHED;
  return '-';
}


//////////////////////////////////////////////////////////////////////
// クラス BnInputNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
BnInputNode::BnInputNode(ymuint id,
			 const string& name) :
  BnNodeImpl(id, name)
{
}

// @brief デストラクタ
BnInputNode::~BnInputNode()
{
}

// @brief タイプを返す．
BnNode::Type
BnInputNode::type() const
{
  return kInput;
}

// @brief 外部入力の時 true を返す．
bool
BnInputNode::is_input() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BnDffNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID 番号
// @param[in] name ノード名
// @param[in] input_id 入力ノードのID番号
// @param[in] rval リセット値
BnDffNode::BnDffNode(ymuint id,
		     const string& name,
		     ymuint input_id,
		     char rval) :
  BnNodeImpl(id, name),
  mInput(input_id),
  mResetVal(rval)
{
}

// @brief デストラクタ
BnDffNode::~BnDffNode()
{
}

// @brief タイプを返す．
BnNode::Type
BnDffNode::type() const
{
  return kDFF;
}

// @brief D-FF ノードの時 true を返す．
bool
BnDffNode::is_dff() const
{
  return true;
}

// @brief 入力のノード番号を返す．
//
// is_dff() == false の時の動作は不定
ymuint
BnDffNode::input() const
{
  return mInput;
}

// @brief リセット値を返す．
char
BnDffNode::reset_val() const
{
  return mResetVal;
}


//////////////////////////////////////////////////////////////////////
// クラス BnLogicNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] fanins ファンインのID番号の配列
BnLogicNode::BnLogicNode(ymuint id,
			 const string& name,
			 const vector<ymuint>& fanins) :
  BnNodeImpl(id, name),
  mFanins(fanins)
{
}

// @brief デストラクタ
BnLogicNode::~BnLogicNode()
{
}

// @brief タイプを返す．
BnNode::Type
BnLogicNode::type() const
{
  return kLogic;
}

// @brief 外部入力の時 true を返す．
bool
BnLogicNode::is_logic() const
{
  return true;
}

// @brief ファンイン数を得る．
ymuint
BnLogicNode::fanin_num() const
{
  return mFanins.size();
}

// @brief ファンインを求める．
// @param[in] pos 入力位置 ( 0 <= pos < fanin_num() )
ymuint
BnLogicNode::fanin(ymuint pos) const
{
  ASSERT_COND( pos < fanin_num() );
  return mFanins[pos];
}


//////////////////////////////////////////////////////////////////////
// クラス BnPrimNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] fanins ファンインのID番号の配列
// @param[in] logic_type 論理タイプ
BnPrimNode::BnPrimNode(ymuint id,
		       const string& name,
		       const vector<ymuint>& fanins,
		       BnLogicType logic_type) :
  BnLogicNode(id, name, fanins),
  mLogicType(logic_type)
{
}

// @brief デストラクタ
BnPrimNode::~BnPrimNode()
{
}

// @brief 論理タイプを返す．
//
// is_logic() == false の時の動作は不定
BnLogicType
BnPrimNode::logic_type() const
{
  return mLogicType;
}


//////////////////////////////////////////////////////////////////////
// クラス BnCellNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] fanins ファンインのID番号の配列
// @param[in] cell セル
BnCellNode::BnCellNode(ymuint id,
		       const string& name,
		       const vector<ymuint>& fanins,
		       const Cell* cell) :
  BnLogicNode(id, name, fanins),
  mCell(cell)
{
}

// @brief デストラクタ
BnCellNode::~BnCellNode()
{
}

// @brief 論理タイプを返す．
//
// is_logic() == false の時の動作は不定
BnLogicType
BnCellNode::logic_type() const
{
  return kBnLt_CELL;
}

// @brief セルを返す．
//
// is_logic() == false の時の動作は不定
// logic_type() != kBnLt_CELL の時の動作は不定
const Cell*
BnCellNode::cell() const
{
  return mCell;
}


//////////////////////////////////////////////////////////////////////
// クラス BnExprNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] fanins ファンインのID番号の配列
// @param[in] expr 論理式
BnExprNode::BnExprNode(ymuint id,
		       const string& name,
		       const vector<ymuint>& fanins,
		       const Expr& expr) :
  BnLogicNode(id, name, fanins),
  mExpr(expr)
{
}

// @brief デストラクタ
BnExprNode::~BnExprNode()
{
}

// @brief 論理タイプを返す．
//
// is_logic() == false の時の動作は不定
BnLogicType
BnExprNode::logic_type() const
{
  return kBnLt_EXPR;
}

// @brief 論理式を返す．
//
// is_logic() == false の時の動作は不定
// logic_type() != kBnLt_EXPR の時の動作は不定
Expr
BnExprNode::expr() const
{
  return mExpr;
}


//////////////////////////////////////////////////////////////////////
// クラス BnTvNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] fanins ファンインのID番号の配列
// @param[in] tv 真理値表
BnTvNode::BnTvNode(ymuint id,
		   const string& name,
		   const vector<ymuint>& fanins,
		   const TvFunc& tv) :
  BnLogicNode(id, name, fanins),
  mTv(tv)
{
}

// @brief デストラクタ
BnTvNode::~BnTvNode()
{
}

// @brief 論理タイプを返す．
//
// is_logic() == false の時の動作は不定
BnLogicType
BnTvNode::logic_type() const
{
  return kBnLt_TV;
}

// @brief 真理値表を返す．
//
// is_logic() == false の時の動作は不定
// logic_type() != kBnLt_TV の時の動作は不定
TvFunc
BnTvNode::tv() const
{
  return mTv;
}

END_NAMESPACE_YM_BNET
