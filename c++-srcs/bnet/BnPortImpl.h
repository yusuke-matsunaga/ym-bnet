#ifndef BNPORTIMPL_H
#define BNPORTIMPL_H

/// @file BnPortImpl.h
/// @brief BnPortImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BnPortImpl BnPortImpl.h "BnPortImpl.h"
/// @brief BnPort の実装クラス
//////////////////////////////////////////////////////////////////////
class BnPortImpl
{
public:

  /// @brief コンストラクタ
  BnPortImpl(
    SizeType id,       ///< [in] ポート番号
    const string& name ///< [in] 名前
  ) : mId{id},
      mName{name}
  {
  }

  /// @brief デストラクタ
  virtual
  ~BnPortImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ポート番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 名前を得る．
  string
  name() const
  {
    return mName;
  }

  /// @brief ビット数を得る．
  virtual
  SizeType
  bit_width() const = 0;

  /// @brief pos ビット目のノード番号を得る．
  /// @return 対応するノードのノード番号を返す．
  virtual
  SizeType
  bit(
    SizeType pos ///< [in] ビット位置 ( 0 <= pos < bit_width() )
  ) const = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ポート番号
  SizeType mId;

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
    SizeType id,        ///< [in] ポート番号
    const string& name, ///< [in] 名前
    SizeType bit        ///< [in] 内容のノード番号
  ) : BnPortImpl(id, name),
      mBit{bit}
  {
  }

  /// @brief デストラクタ
  ~BnPort1() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ビット数を得る．
  SizeType
  bit_width() const override;

  /// @brief pos ビット目のノード番号を得る．
  /// @return 対応するノードのノード番号を返す．
  SizeType
  bit(
    SizeType pos ///< [in] ビット位置 ( 0 <= pos < bit_width() )
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号
  SizeType mBit;

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
    SizeType id,                 ///< [in] ポート番号
    const string& name,          ///< [in] 名前
    const vector<SizeType>& bits ///< [in] 内容のノード番号のベクタ
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
  SizeType
  bit_width() const override;

  /// @brief pos ビット目のノード番号を得る．
  /// @return 対応するノードのノード番号を返す．
  SizeType
  bit(
    SizeType pos ///< [in] ビット位置 ( 0 <= pos < bit_width() )
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号の配列
  vector<SizeType> mBits;

};

END_NAMESPACE_YM_BNET

#endif // BNPORTIMPL_H
