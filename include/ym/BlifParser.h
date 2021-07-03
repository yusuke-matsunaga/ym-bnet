#ifndef YM_BLIFPARSER_H
#define YM_BLIFPARSER_H

/// @file ym/BlifParser.h
/// @brief ym/BlifParser のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/clib.h"


BEGIN_NAMESPACE_YM_BNET

class BlifParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class BlifParser BlifParser.h "ym/BlifParser.h"
/// @ingroup BlifGroup
/// @brief blif形式のファイルを読み込むパーサークラス
///
/// 適切なハンドラクラスを生成して add_handler() で登録して使う．
/// @sa BlifHandler MsgHandler
//////////////////////////////////////////////////////////////////////
class BlifParser
{
  friend class BlifHandler;

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
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(
    const string& filename ///< [in] ファイル名
  );

  /// @brief 読み込みを行う(セルライブラリ付き)．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(
    const string& filename,             ///< [in] ファイル名
    const ClibCellLibrary& cell_library ///< [in] セルライブラリ
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パーサーの実体
  unique_ptr<BlifParserImpl> mImpl;

};

END_NAMESPACE_YM_BNET

#endif // YM_BLIFPARSER_H
