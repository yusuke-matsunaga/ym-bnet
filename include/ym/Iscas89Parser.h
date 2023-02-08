#ifndef ISCAS89PARSER_H
#define ISCAS89PARSER_H

/// @file Iscas89Parser.h
/// @brief Iscas89Parser のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2021, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/iscas89_nsdef.h"


BEGIN_NAMESPACE_YM_ISCAS89

class ParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Parser Iscas89Parser.h "Iscas89Parser.h"
/// @brief ISCAS89(.bench) 形式のファイルを読み込むパーサークラス
//////////////////////////////////////////////////////////////////////
class Iscas89Parser
{
public:

  /// @brief コンストラクタ
  Iscas89Parser();

  /// @brief デストラクタ
  ~Iscas89Parser();


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
    Iscas89Model& model     ///< [out] パーズ結果を格納するオブジェクト
  );

  /// @brief 拡張ハンドラを登録する．
  /// @return ハンドラ番号を返す．
  SizeType
  reg_handler(
    Iscas89Handler* handler ///< [in] ハンドラ
  );

  /// @brief 拡張ハンドラを取り出す．
  Iscas89Handler*
  get_handler(
    SizeType id ///< [in] ハンドラ番号
  );

  /// @brief ハンドラとキーワードを結びつける．
  void
  bind_handler(
    const string& keyword, ///< [in] キーワード
    SizeType id            ///< [in] ハンドラ番号
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装オブジェクト
  ParserImpl* mImpl;

};

END_NAMESPACE_YM_ISCAS89

#endif // ISCAS89PARSER_H
