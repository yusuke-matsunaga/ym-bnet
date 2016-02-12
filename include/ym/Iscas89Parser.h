#ifndef YM_ISCAS89PARSER_H
#define YM_ISCAS89PARSER_H

/// @file ym/Iscas89Parser.h
/// @brief Iscas89Parser のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"


BEGIN_NAMESPACE_YM_BNET

class Iscas89ParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Parser Iscas89Parser.h "ym/Iscas89Parser.h"
/// @ingroup Iscas89Group
/// @brief ISCAS89(.bench) 形式のファイルを読み込むパーサークラス
/// @sa Iscas89Handler
//////////////////////////////////////////////////////////////////////
class Iscas89Parser
{
public:

  /// @brief コンストラクタ
  Iscas89Parser();

  /// @brief デストラクタ
  ~Iscas89Parser();


public:

  /// @brief 読み込みを行う．
  /// @param[in] filename ファイル名
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(const string& filename);

  /// @brief イベントハンドラの登録
  /// @param[in] handler 登録する対象のハンドラ
  /// @note handler はこのインスタンスが破壊される時に同時に破壊される．
  void
  add_handler(Iscas89Handler* handler);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パーサーの実体
  Iscas89ParserImpl* mRep;

};

END_NAMESPACE_YM_BNET

#endif // YM_ISCAS89PARSER_H
