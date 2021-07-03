#ifndef BNPORTIMPL_H
#define BNPORTIMPL_H

/// @file BnPortImpl.h
/// @brief BnPortImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
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
  BnPortImpl(
    int id,            ///< [in] ポート番号
    const string& name ///< [in] 名前
  ) : mId(id),
    mName(name)
  {
  }

  /// @brief デストラクタ
  ~BnPortImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ポート番号を返す．
  int
  id() const override;

  /// @brief 名前を得る．
  string
  name() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ポート番号
  int mId;

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
  BnPort1(
    int id,             ///< [in] ポート番号
    const string& name, ///< [in] 名前
    int bit             ///< [in] 内容のノード番号
  ) : BnPortImpl(id, name),
    mBit(bit)
  {
  }

  /// @brief デストラクタ
  ~BnPort1() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ビット数を得る．
  int
  bit_width() const override;

  /// @brief pos ビット目のノード番号を得る．
  /// @return 対応するノードのノード番号を返す．
  int
  bit(
    int pos ///< [in] ビット位置 ( 0 <= pos < bit_width() )
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号
  int mBit;

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
  BnPortN(
    int id,                 ///< [in] ポート番号
    const string& name,     ///< [in] 名前
    const vector<int>& bits ///< [in] 内容のノード番号のベクタ
  ) : BnPortImpl(id, name),
      mBits{bits}
  {
  }

  /// @brief デストラクタ
  ~BnPortN() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ビット数を得る．
  int
  bit_width() const override;

  /// @brief pos ビット目のノード番号を得る．
  /// @return 対応するノードのノード番号を返す．
  int
  bit(
    int pos ///< [in] ビット位置 ( 0 <= pos < bit_width() )
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ビット数
  int mBitWidth;

  // ノード番号の配列
  vector<int> mBits;

};

END_NAMESPACE_YM_BNET

#endif // BNPORTIMPL_H
