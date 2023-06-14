#ifndef NVLPARSER_H
#define NVLPARSER_H

/// @file NvlParser.h
/// @brief NvlParser のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/nvl_nsdef.h"


BEGIN_NAMESPACE_YM_NVL

class ParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class NvlParser NvlParser.h "NvlParser.h"
/// @brief netlist 形式の verilog ファイルを読み込むパーサークラス
//////////////////////////////////////////////////////////////////////
class NvlParser
{
public:

  /// @brief コンストラクタ
  NvlParser();

  /// @brief デストラクタ
  ~NvlParser();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 読み込みを行う．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(
    const string& filename, ///< [in] ファイル名
    NvlModel& model         ///< [out] パーズ結果を格納するオブジェクト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装オブジェクト
  ParserImpl* mImpl;

};

END_NAMESPACE_YM_NVL

#endif // NVLPARSER_H
