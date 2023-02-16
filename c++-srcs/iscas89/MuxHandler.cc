
/// @file MuxHandler.cc
/// @brief MuxHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "MuxHandler.h"
#include "ym/Expr.h"
#include "ym/MsgMgr.h"


BEGIN_NAMESPACE_YM_ISCAS89

// @brief ゲートの読み込みを行う．
bool
MuxHandler::read(
  const FileRegion& first_loc,
  SizeType oname_id
)
{
  vector<SizeType> iname_id_list;
  FileRegion last_loc;
  if ( !parse_name_list(iname_id_list, last_loc) ) {
    return false;
  }
  FileRegion loc{first_loc, last_loc};
  // 入力数をチェックする．
  SizeType ni = iname_id_list.size();
  SizeType nc = 0;
  SizeType nd = 1;
  while ( nc + nd < ni ) {
    ++ nc;
    nd <<= 1;
  }
  if ( nc + nd != ni ) {
    // 引数の数が合わない．
    ostringstream buf;
    auto oname = id2str(oname_id);
    buf << oname << ": Wrong # of inputs for MUX-type.";
    MsgMgr::put_msg(__FILE__, __LINE__, loc,
		    MsgType::Error,
		    "ER_MUX01",
		    buf.str());
    return false;
  }

  vector<Expr> control_inputs(nc);
  for ( SizeType i = 0; i < nc; ++ i ) {
    auto lit = Expr::make_literal(i, false);
    control_inputs[i] = lit;
  }
  vector<Expr> data_inputs(nd);
  for ( SizeType i = 0; i < nd; ++ i ) {
    vector<Expr> fanin_list;
    fanin_list.reserve(nc + 1);
    for ( SizeType j = 0; j < nc; ++ j ) {
      if ( i & (1 << j) ) {
	fanin_list.push_back(control_inputs[j]);
      }
      else {
	fanin_list.push_back(~control_inputs[j]);
      }
    }
    auto lit = Expr::make_literal(i + nc);
    fanin_list.push_back(lit);
    auto and_expr = Expr::make_and(fanin_list);
    data_inputs[i] = and_expr;
  }
  auto expr = Expr::make_or(data_inputs);

  set_complex(oname_id, loc, expr, iname_id_list);

  return true;
}

END_NAMESPACE_YM_ISCAS89
