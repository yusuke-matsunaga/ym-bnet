#ifndef BLIFNODE_H
#define BLIFNODE_H

/// @file BlifNode.h
/// @brief BlifNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/blif_nsdef.h"
#include "ym/clib.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
/// @class BlifNode BlifNode.h "BlifNode.h"
/// @brief .inputs/.latch/.names/.gate 文を表す BlifAst
//////////////////////////////////////////////////////////////////////
class BlifNode
{
public:

  /// @brief 種類を表す列挙型
  enum Type
  {
    Input, ///< .inputs
    Latch, ///< .latch
    Names, ///< .names
    Gate   ///< .gate
  };


public:

  /// @brief コンストラクタ
  BlifNode(
    SizeType id,       ///< [in] ID番号
    const string& name ///< [in] 名前
  ) : mId{id},
      mName{name}
  {
  }

  /// @brief デストラクタ
  virtual
  ~BlifNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 名前を返す．
  const string&
  name() const
  {
    return mName;
  }

  /// @brief 種類を返す．
  virtual
  Type
  type() const = 0;

  /// @brief 入力のノード番号を返す．
  ///
  /// - Latch 用
  virtual
  SizeType
  inode() const;

  /// @brief リセット値を返す．
  ///
  /// - Latch 用
  virtual
  char
  rval() const;

  /// @brief 入力ノード番号のリストを返す．
  ///
  /// - Names, Gate 用
  virtual
  const vector<SizeType>&
  inode_list() const;

  /// @brief カバー番号を返す．
  ///
  /// - Names 用
  virtual
  SizeType
  cover_id() const;

  /// @brief セル番号を返す．
  ///
  /// - Gate 用
  virtual
  SizeType
  cell_id() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // 出力名
  string mName;

};

END_NAMESPACE_YM_BLIF

#endif // BLIFNODE_H
