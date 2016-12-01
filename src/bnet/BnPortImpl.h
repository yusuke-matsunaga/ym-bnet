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
  /// @param[in] id ポート番号
  /// @param[in] name 名前
  BnPortImpl(ymuint id,
	     const string& name);

  /// @brief デストラクタ
  virtual
  ~BnPortImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ポート番号を返す．
  virtual
  ymuint
  id() const;

  /// @brief 名前を得る．
  virtual
  string
  name() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ポート番号
  ymuint mId;

  // 名前
  string mName;

};


//////////////////////////////////////////////////////////////////////
/// @class BnPort1 BnPortImpl.h "BnPortImpl.h"
/// @brief 1ビットの BnPort の実装クラス
//////////////////////////////////////////////////////////////////////
class BnPort1 :
  public BnPortImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ポート番号
  /// @param[in] name 名前
  /// @param[in] bit 内容のノード番号
  BnPort1(ymuint id,
	  const string& name,
	  ymuint bit);

  /// @brief デストラクタ
  virtual
  ~BnPort1();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ビット数を得る．
  virtual
  ymuint
  bit_width() const;

  /// @brief pos ビット目のノード番号を得る．
  /// @param[in] pos ビット位置 ( 0 <= pos < bit_width() )
  /// @return 対応するノードのノード番号を返す．
  virtual
  ymuint
  bit(ymuint pos) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号
  ymuint mBit;

};


//////////////////////////////////////////////////////////////////////
/// @class BnPortN BnPortImpl.h "BnPortImpl.h"
/// @brief 多ビットの BnPort の実装クラス
//////////////////////////////////////////////////////////////////////
class BnPortN :
  public BnPortImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ポート番号
  /// @param[in] name 名前
  /// @param[in] bits 内容のノード番号のベクタ
  BnPortN(ymuint id,
	  const string& name,
	  const vector<ymuint>& bits);

  /// @brief デストラクタ
  virtual
  ~BnPortN();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ビット数を得る．
  virtual
  ymuint
  bit_width() const;

  /// @brief pos ビット目のノード番号を得る．
  /// @param[in] pos ビット位置 ( 0 <= pos < bit_width() )
  /// @return 対応するノードのノード番号を返す．
  virtual
  ymuint
  bit(ymuint pos) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ビット数
  ymuint mBitWidth;

  // ノード番号の配列
  ymuint* mBits;

};

END_NAMESPACE_YM_BNET

#endif // BNPORTIMPL_H
