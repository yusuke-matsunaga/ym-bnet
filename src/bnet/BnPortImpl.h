#ifndef BNPORTIMPL_H
#define BNPORTIMPL_H

/// @file BnPortImpl.h
/// @brief BnPortImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnPort.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnPortImpl BnPortImpl.h "BnPortImpl.h"
/// @brief BnPort の実装クラス
//////////////////////////////////////////////////////////////////////
class BnPortImpl :
  public BnPort
{
public:

  /// @brief コンストラクタ
  /// @param[in] name 名前
  /// @param[in] bits 内容のベクタ
  BnPortImpl(const string& name,
	     const vector<BnNode*>& bits);

  /// @brief デストラクタ
  virtual
  ~BnPortImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を得る．
  virtual
  string
  name() const;

  /// @brief ビット数を得る．
  virtual
  ymuint
  bit_width() const;

  /// @brief pos ビット目の内容を得る．
  /// @param[in] pos ビット位置 ( 0 <= pos < bit_width() )
  /// @return 対応するノードを返す．
  virtual
  const BnNode*
  bit(ymuint pos) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 名前
  string mName;

  // 個々のビットに対応するノード番号のリスト
  vector<BnNode*> mBits;

};

END_NAMESPACE_YM_BNET

#endif // BNPORTIMPL_H
