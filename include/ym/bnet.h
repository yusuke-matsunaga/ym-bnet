#ifndef YM_BNET_H
#define YM_BNET_H

/// @file ym/bnet.h
/// @brief ym-bnet の定義
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2017, 2021 Yusuke Matsunaga
/// All rights reserved.

/// @defgroup ym-bnet Boolean Network を扱うモジュール
///
///
///

#include "ym_config.h"


/// @brief bnet 用の名前空間の開始
#define BEGIN_NAMESPACE_YM_BNET \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsBnet)

/// @brief bnet 用の名前空間の終了
#define END_NAMESPACE_YM_BNET \
END_NAMESPACE(nsBnet) \
END_NAMESPACE_YM


BEGIN_NAMESPACE_YM_BNET

// クラスの前方宣言

class BnBlifReader;
class BnIscas89Reader;

class BnNetwork;
class BnPort;
class BnDff;
class BnLatch;
class BnNode;
enum class BnNodeType;

class BlifCover;
class BlifHandler;
class BlifParser;

class Iscas89Handler;
class Iscas89Parser;

END_NAMESPACE_YM_BNET

BEGIN_NAMESPACE_YM

/// @brief ポートの向きを表す列挙型(サイズは8ビット)
enum class BnDir : ymuint8 {
  INPUT  = 0, ///< 入力
  OUTPUT = 1  ///< 出力
};

/// @brief 不正なノード番号を表す定数
const SizeType BNET_NULLID = -1;


//////////////////////////////////////////////////////////////////////
/// @class BnetError BnetError.h "BnetError.h"
/// @brief bnet 関係のエラーを表す例外クラス
//////////////////////////////////////////////////////////////////////
class BnetError :
  public exception
{
public:

  /// @brief コンストラクタ
  BnetError(
    const string& msg ///< [in] エラーメッセージ
  ) : mMsg{msg}
  {
  }

  /// @brief デストラクタ
  ~BnetError() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を表す文字列を返す．
  const string&
  str() const
  {
    return mMsg;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // メッセージ
  string mMsg;

};

using nsBnet::BnBlifReader;
using nsBnet::BnIscas89Reader;
using nsBnet::BnNetwork;
using nsBnet::BnPort;
using nsBnet::BnDff;
using nsBnet::BnLatch;
using nsBnet::BnNode;
using nsBnet::BnNodeType;

END_NAMESPACE_YM

#endif // YM_BNET_H
