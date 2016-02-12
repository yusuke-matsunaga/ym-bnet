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
class BnNetwork;
class BnNode;
class Iscas89Handler;
class Iscas89Parser;

END_NAMESPACE_YM_BNET

BEGIN_NAMESPACE_YM

using nsBnet::BlifCover;
using nsBnet::BlifHandler;
using nsBnet::BlifParser;
using nsBnet::BnNetwork;
using nsBnet::BnNode;
using nsBnet::Iscas89Parser;
using nsBnet::Iscas89Handler;


//////////////////////////////////////////////////////////////////////
/// @brief ゲートの型
/// @ingroup Iscas89Group
//////////////////////////////////////////////////////////////////////
enum GateType {
  kGt_BUFF,
  kGt_NOT,
  kGt_AND,
  kGt_NAND,
  kGt_OR,
  kGt_NOR,
  kGt_XOR,
  kGt_XNOR,
  kGt_DFF
};

/// @brief GateType のストリーム出力演算子
inline
ostream&
operator<<(ostream& s,
	   GateType gate_type)
{
  switch ( gate_type ) {
  case kGt_BUFF: s << "BUFF"; break;
  case kGt_NOT:  s << "NOT"; break;
  case kGt_AND:  s << "AND"; break;
  case kGt_NAND: s << "NAND"; break;
  case kGt_OR:   s << "OR"; break;
  case kGt_NOR:  s << "NOR"; break;
  case kGt_XOR:  s << "XOR"; break;
  case kGt_XNOR: s << "XNOR"; break;
  case kGt_DFF:  s << "DFF"; break;
  }
  return s;
}

/// @brief カバーのパタンを表す列挙型
enum BlifPat {
  kBlifPat_0,
  kBlifPat_1,
  kBlifPat_d
};

/// @brief BlifPat のストリーム出力演算子
inline
ostream&
operator<<(ostream& s,
	   BlifPat pat)
{
  switch ( pat ) {
  case kBlifPat_0: s << '0'; break;
  case kBlifPat_1: s << '1'; break;
  case kBlifPat_d: s << '-'; break;
  }
  return s;
}

END_NAMESPACE_YM

#endif // YM_YM_BNET_H
