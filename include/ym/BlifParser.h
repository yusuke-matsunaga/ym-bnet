﻿#ifndef YM_BLIFPARSER_H
#define YM_BLIFPARSER_H

/// @file ym/BlifParser.h
/// @brief ym/BlifParser のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/ym_cell.h"


BEGIN_NAMESPACE_YM_BNET

class BlifParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class BlifParser BlifParser.h "ym/BlifParser.h"
/// @ingroup BlifGroup
/// @brief blif形式のファイルを読み込むパーサークラス
/// 適切なハンドラクラスを生成して add_handler() で登録して使う．
/// @sa BlifHandler MsgHandler
//////////////////////////////////////////////////////////////////////
class BlifParser
{
public:

  /// @brief コンストラクタ
  BlifParser();

  /// @brief デストラクタ
  ~BlifParser();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 読み込みを行う．
  /// @param[in] filename ファイル名
  /// @param[in] cell_library セルライブラリ
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(const string& filename,
       const CellLibrary* cell_library);

  /// @brief イベントハンドラの登録
  /// @param[in] handler 登録するハンドラ
  /// @note handler はこのインスタンスが破壊される時に同時に破壊される．
  void
  add_handler(BlifHandler* handler);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実体
  BlifParserImpl* mImpl;

};

END_NAMESPACE_YM_BNET

#endif // YM_BLIFPARSER_H