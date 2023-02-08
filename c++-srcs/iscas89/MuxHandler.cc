
/// @file MuxHandler.cc
/// @brief MuxHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "MuxHandler.h"
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

  vector<SizeType> control_list;
  vector<SizeType> control_inv_list;
  control_list.reserve(nc);
  for ( SizeType i = 0; i < nc; ++ i ) {
    SizeType id = iname_id_list[i];
    control_list.push_back(id);
    SizeType inv_id = new_gate(loc, PrimType::Not, {id});
    control_inv_list.push_back(inv_id);
  }
  vector<SizeType> tmp_list;
  tmp_list.reserve(nd);
  for ( SizeType i = 0; i < nd; ++ i ) {
    vector<SizeType> fanin_list;
    fanin_list.reserve(nc + 1);
    SizeType id = iname_id_list[i + nc];
    fanin_list.push_back(id);
    for ( SizeType j = 0; j < nc; ++ j ) {
      if ( i & (1 << j) ) {
	fanin_list.push_back(control_list[j]);
      }
      else {
	fanin_list.push_back(control_inv_list[j]);
      }
    }
    SizeType and_id = new_gate(loc, PrimType::And, fanin_list);
    tmp_list.push_back(and_id);
  }

  set_gate(oname_id, loc, PrimType::Or, tmp_list);

  return true;
}

END_NAMESPACE_YM_ISCAS89
