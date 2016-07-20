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
//////////////////////////////////////////////////////////////////////
class BnIscas89Reader
{
public:

  /// @brief コンストラクタ
  /// @param[in] clock_name クロック端子名
  BnIscas89Reader(const string& clock_name = "clock");

  /// @brief デストラクタ
  ~BnIscas89Reader();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief blif ファイルを読み込む．
  /// @param[in] builder 設定対象のビルダーオブジェクト
  /// @param[in] filename ファイル名
  /// @return 読み込みが成功したら true を返す．
  bool
  read(BnBuilder& builder,
       const string& filename);


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

};

END_NAMESPACE_YM_BNET

#endif // BNISCAS89READER_H
