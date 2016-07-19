#ifndef BNBLIFREADER_H
#define BNBLIFREADER_H

/// @file BnBlifReader.h
/// @brief BnBlifReader のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/ym_cell.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnBlifReader BnBlifReader.h "BnBlifReader.h"
/// @brief blif ファイルを読み込んで BnNetwork に設定するクラス
//////////////////////////////////////////////////////////////////////
class BnBlifReader
{
public:

  /// @brief コンストラクタ
  /// @param[in] clock_name クロック端子名
  /// @param[in] reset_name リセット端子名
  BnBlifReader(const string& clock_name = "clock",
	       const string& reset_name = "reset");

  /// @brief デストラクタ
  ~BnBlifReader();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief blif ファイルを読み込む．
  /// @param[in] network 設定対象のネットワーク
  /// @param[in] filename ファイル名
  /// @param[in] cell_library セルライブラリ
  /// @return 読み込みが成功したら true を返す．
  bool
  read(BnNetwork& network,
       const string& filename,
       const CellLibrary* cell_library = nullptr);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // クロック端子名
  string mClockName;

  // リセット端子名
  string mResetName;

};

END_NAMESPACE_YM_BNET

#endif // BNBLIFREADER_H
