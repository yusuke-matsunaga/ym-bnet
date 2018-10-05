#ifndef YM_ISCAS89PARSER_H
#define YM_ISCAS89PARSER_H

/// @file ym/Iscas89Parser.h
/// @brief Iscas89Parser のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"


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
  friend class Iscas89Handler;

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
  /// @param[in] filename ファイル名
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(const string& filename);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パーサーの実体
  unique_ptr<Iscas89ParserImpl> mRep;

};

END_NAMESPACE_YM_BNET

#endif // YM_ISCAS89PARSER_H
