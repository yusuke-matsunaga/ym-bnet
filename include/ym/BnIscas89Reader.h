#ifndef BNISCAS89READER_H
#define BNISCAS89READER_H

/// @file BnIscas89Reader.h
/// @brief BnIscas89Reader のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "ym/ym_bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnIscas89Reader BnIscas89Reader.h "BnIscas89Reader.h"
/// @brief iscas89(.bench) ファイルを読み込んで BnBuilder に設定するクラス
///
/// このクラスは実体を持たない．
//////////////////////////////////////////////////////////////////////
class BnIscas89Reader
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief iscas89 ファイルを読み込む．
  /// @param[in] network 設定対象のネットワーク
  /// @param[in] filename ファイル名
  /// @param[in] clock_name クロック端子名
  /// @return 読み込みが成功したら true を返す．
  static
  bool
  read(BnNetwork& network,
       const string& filename,
       const string& clock_name = "clock");

};

END_NAMESPACE_YM_BNET

#endif // BNISCAS89READER_H
