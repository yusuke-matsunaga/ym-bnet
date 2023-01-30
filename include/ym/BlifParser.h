#ifndef BLIFPARSER_H
#define BLIFPARSER_H

/// @file BlifParser.h
/// @brief BlifParser のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/blif_nsdef.h"
#include "ym/clib.h"


BEGIN_NAMESPACE_YM_BLIF

class ParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class BlifParser BlifParser.h "ym/BlifParser.h"
/// @brief blif形式のファイルを読み込むパーサークラス
///
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
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(
    const string& filename, ///< [in] ファイル名
    BlifModel& model        ///< [out] 結果を格納する変数
  );

  /// @brief 読み込みを行う(セルライブラリ付き)．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(
    const string& filename,              ///< [in] ファイル名
    const ClibCellLibrary& cell_library, ///< [in] セルライブラリ
    BlifModel& model                     ///< [out] 結果を格納する変数
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装クラス
  ParserImpl* mImpl{nullptr};

};

END_NAMESPACE_YM_BLIF

#endif // BLIFPARSER_H
