
/// @file BlifBnNetworkHandler.cc
/// @brief BlifBnNetworkHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BlifBnNetworkHandler.h"
#include "BnNetworkImpl.h"
#include "ym/BlifCover.h"
#include "ym/BnNode.h"


BEGIN_NAMESPACE_YM_BNET

// @brief コンストラクタ
BlifBnNetworkHandler::BlifBnNetworkHandler(BnNetworkImpl* network) :
  mNetwork(network)
{
}

// @brief デストラクタ
BlifBnNetworkHandler::~BlifBnNetworkHandler()
{
}

// @brief 初期化
bool
BlifBnNetworkHandler::init()
{
  mNetwork->clear();

  return true;
}

// @brief .model 文の読み込み
// @param[in] loc 位置情報
// @param[in] name model名
bool
BlifBnNetworkHandler::model(const FileRegion& loc1,
			    const FileRegion& loc2,
			    const char* name)
{
  mNetwork->set_model(name);

  return true;
}

// @brief .input 文の読み込み
// @param[in] loc 位置情報
// @param[in] name_id 文字列のID番号
// @param[in] name 入力ピン名
bool
BlifBnNetworkHandler::inputs_elem(ymuint name_id,
				  const char* name)
{
  ymuint node_id = mNetwork->new_input(name_id, name);
  mNetwork->new_port(name, vector<ymuint>(1, node_id));

  return true;
}

// @brief .output 文の読み込み
// @param[in] loc 位置情報
// @param[in] name 出力ノード名
// @param[in] name 出力ピン名
bool
BlifBnNetworkHandler::outputs_elem(ymuint name_id,
				   const char* name)
{
  ymuint node_id = mNetwork->new_output(name_id, name);
  mNetwork->new_port(name, vector<ymuint>(1, node_id));

  return true;
}

BEGIN_NONAMESPACE

Expr
cover2expr(const BlifCover* cover)
{
  ymuint ni = cover->input_num();
  ymuint nc = cover->cube_num();
  Expr ans = Expr::make_zero();
  for (ymuint c = 0; c < nc; ++ c) {
    Expr prod = Expr::make_one();
    for (ymuint i = 0; i < ni; ++ i) {
      BlifCover::Pat pat = cover->input_pat(i, c);
      switch ( pat ) {
      case BlifCover::kPat_0:
	prod &= Expr::make_negaliteral(VarId(i));
	break;
      case BlifCover::kPat_1:
	prod &= Expr::make_posiliteral(VarId(i));
	break;
      default:
	break;
      }
    }
    ans |= prod;
  }
  if ( cover->output_pat() == BlifCover::kPat_0 ) {
    ans = ~ans;
  }
  return ans;
}

END_NONAMESPACE

// @brief .names 文の処理
// @param[in] onode_id ノード名のID番号
// @param[in] oname 出力名
// @param[in] inode_id_array ファンイン各のID番号の配列
// @param[in] cover_id カバーID
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::names(ymuint onode_id,
			    const char* oname,
			    const vector<ymuint>& inode_id_array,
			    ymuint cover_id)
{
  while ( mFuncTypeArray.size() <= cover_id ) {
    mFuncTypeArray.push_back(nullptr);
  }
  const BnFuncType* func_type = mFuncTypeArray[cover_id];
  if ( func_type == nullptr ) {
    const BlifCover* cover = id2cover(cover_id);
    Expr expr = cover2expr(cover);
    func_type = mNetwork->new_expr_type(expr, inode_id_array.size());
    mFuncTypeArray[cover_id] = func_type;
  }
  mNetwork->new_logic(onode_id, oname, inode_id_array, func_type);

  return true;
}

// @brief .gate 文の処理
// @param[in] onode_id 出力ノードのID番号
// @param[in] oname 出力名
// @param[in] cell セル
// @param[in] inode_id_array 入力ノードのID番号の配列
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::gate(ymuint onode_id,
			   const char* oname,
			   const vector<ymuint>& inode_id_array,
			   const Cell* cell)
{
  const BnFuncType* func_type = mNetwork->new_cell_type(cell);
  mNetwork->new_logic(onode_id, oname, inode_id_array, func_type);

  return true;
}

// @brief .latch 文の処理
// @param[in] onode_id 出力ノードのID番号
// @param[in] oname 出力名
// @param[in] inode_id 入力ノードのID番号
// @param[in] loc4 リセット値の位置情報
// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
BlifBnNetworkHandler::latch(ymuint onode_id,
			    const char* oname,
			    ymuint inode_id,
			    const FileRegion& loc4,
			    char rval)
{
  mNetwork->new_dff(onode_id, oname, inode_id, rval);

  return true;
}

// @brief 終了処理
// @param[in] loc 位置情報
bool
BlifBnNetworkHandler::end(const FileRegion& loc)
{
  return true;
}

// @brief 通常終了時の処理
void
BlifBnNetworkHandler::normal_exit()
{
}

// @brief エラー終了時の処理
void
BlifBnNetworkHandler::error_exit()
{
  mNetwork->clear();
}

END_NAMESPACE_YM_BNET
