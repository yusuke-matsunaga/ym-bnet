#ifndef BNBLIFREADER_H
#define BNBLIFREADER_H

/// @file BnBlifReader.h
/// @brief BnBlifReader のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"
#include "ym/ClibCellLibrary.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnBlifReader BnBlifReader.h "BnBlifReader.h"
/// @brief blif ファイルを読み込んで BnNetwork に設定するクラス
///
/// このクラスは実体を持たない．
//////////////////////////////////////////////////////////////////////
class BnBlifReader
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief blif ファイルを読み込む．
  /// @param[in] network 設定対象のネットワーク
  /// @param[in] filename ファイル名
  /// @param[in] clock_name クロック端子名
  /// @param[in] reset_name リセット端子名
  /// @param[in] cell_library セルライブラリ
  /// @return 読み込みが成功したら true を返す．
  static
  bool
  read(BnNetwork& network,
       const string& filename,
       const string& clock_name = "clock",
       const string& reset_name = "reset");

  /// @brief blif ファイルを読み込む(セルライブラリ付き)．
  /// @param[in] network 設定対象のネットワーク
  /// @param[in] filename ファイル名
  /// @param[in] cell_library セルライブラリ
  /// @param[in] clock_name クロック端子名
  /// @param[in] reset_name リセット端子名
  /// @return 読み込みが成功したら true を返す．
  static
  bool
  read(BnNetwork& network,
       const string& filename,
       const ClibCellLibrary& cell_library,
       const string& clock_name = "clock",
       const string& reset_name = "reset");

};

END_NAMESPACE_YM_BNET

#endif // BNBLIFREADER_H
