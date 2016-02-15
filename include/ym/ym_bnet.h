#ifndef YM_YM_BNET_H
#define YM_YM_BNET_H

/// @file ym/ym_bnet.h
/// @brief ym-bnet の定義
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.

/// @defgroup BlifGroup BLIF パーサ
///
/// UCB で開発された mis, sis の標準入出力フォーマット
/// BLIF(Berkeley Logic Interchange Format) 用のパーサ


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
class BlifCover;
class BlifHandler;
class BlifParser;
class BnFuncType;
class BnNetwork;
class BnNode;
class Iscas89Handler;
class Iscas89Parser;

END_NAMESPACE_YM_BNET

BEGIN_NAMESPACE_YM

using nsBnet::BlifCover;
using nsBnet::BlifHandler;
using nsBnet::BlifParser;
using nsBnet::BnFuncType;
using nsBnet::BnNetwork;
using nsBnet::BnNode;
using nsBnet::Iscas89Parser;
using nsBnet::Iscas89Handler;

END_NAMESPACE_YM

#endif // YM_YM_BNET_H
