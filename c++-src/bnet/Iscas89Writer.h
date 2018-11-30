#ifndef ISCAS89WRITER_H
#define ISCAS89WRITER_H

/// @file Iscas89Writer.h
/// @brief Iscas89Writer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "WriterBase.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Writer Iscas89Writer.h "Iscas89Writer.h"
/// @brief BnNetwork を iscas89(.bench) 形式で出力するクラス
//////////////////////////////////////////////////////////////////////
class Iscas89Writer :
  public WriterBase
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] prefix 自動生成名の接頭語
  /// @param[in] suffix 自動生成名の接尾語
  Iscas89Writer(const BnNetwork& network,
		const string& prefix,
		const string& suffix);

  /// @brief デストラクタ
  ~Iscas89Writer();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief iscas89(.bench) 形式で出力する．
  /// @param[in] s 出力先のストリーム
  void
  operator()(ostream& s);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////


};

END_NAMESPACE_YM_BNET

#endif // ISCAS89WRITER_H
