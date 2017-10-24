#ifndef BLIFIDCELL_H
#define BLIFIDCELL_H

/// @file BlifIdCell.h
/// @brief BlifIdCell のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"
#include "ym/FileRegion.h"


BEGIN_NAMESPACE_YM_BNET

class BlifIdHash;

//////////////////////////////////////////////////////////////////////
/// @class BlifIdCell BlifIdCell.h "BlifIdCell.h"
/// @brief 識別子に関する情報を表すデータ構造
//////////////////////////////////////////////////////////////////////
class BlifIdCell
{
  friend class BlifIdHash;

private:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  // friend class の BlifIdHash のみがアクセスできる．
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  BlifIdCell(ymuint32 id,
	     const char* str);

  /// @brief デストラクタ
  ~BlifIdCell();


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を得る．
  ymuint32
  id() const;

  /// @brief 文字列を返す．
  const char*
  str() const;

  /// @brief 位置情報を返す．
  const FileRegion&
  loc() const;

  /// @brief この識別子を定義している位置情報を返す．
  const FileRegion&
  def_loc() const;

  /// @brief 定義されているか調べる．
  /// @retval true 定義済み
  /// @retval false 未定義
  bool
  is_defined() const;

  /// @brief 外部入力かどうか調べる．
  /// @return 外部入力なら true を返す．
  bool
  is_input() const;

  /// @brief 外部出力かどうか調べる．
  /// @return 外部出力なら true を返す．
  bool
  is_output() const;

  /// @brief ファンインの識別子の配列を得る．
  const vector<ymuint32>&
  inode_id_array() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を設定する関数
  // BlifParser が用いる．
  //////////////////////////////////////////////////////////////////////

  /// @brief 位置情報を設定する．
  void
  set_loc(const FileRegion& loc);

  /// @brief 定義済みの印をつける．
  void
  set_defined();

  /// @brief 外部入力の印をつける．
  void
  set_input();

  /// @brief 外部出力の印をつける．
  void
  set_output();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  ymuint32 mId;

  // 位置情報
  FileRegion mLoc;

  // 位置情報その2
  FileRegion mLoc2;

  // いくつかのフラグ
  ymuint32 mFlags;

  // ハッシュ表中の次の要素を指すポインタ
  BlifIdCell* mLink;

  // 文字列領域(ダミー)
  // 実際には必要な分だけ領域が確保される．
  char mStr[1];


private:
  //////////////////////////////////////////////////////////////////////
  // 定数
  //////////////////////////////////////////////////////////////////////

  // defined マークのシフト量
  static
  const int kDefSft = 0;

  // input マークのシフト量
  static
  const int kInSft = 1;

  // output マークのシフト量
  static
  const int kOutSft = 2;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ID番号を得る．
inline
ymuint32
BlifIdCell::id() const
{
  return mId;
}

// @brief 位置情報を返す．
inline
const FileRegion&
BlifIdCell::loc() const
{
  return mLoc;
}

// @brief 位置情報その2を返す．
inline
const FileRegion&
BlifIdCell::def_loc() const
{
  return mLoc2;
}

// @brief 文字列を返す．
inline
const char*
BlifIdCell::str() const
{
  return mStr;
}

// @brief 定義されているか調べる．
// @retval true 定義済み
// @retval false 未定義
inline
bool
BlifIdCell::is_defined() const
{
  return static_cast<bool>((mFlags >> kDefSft) & 1U);
}

// @brief 外部入力かどうか調べる．
// @return 外部入力なら true を返す．
inline
bool
BlifIdCell::is_input() const
{
  return static_cast<bool>((mFlags >> kInSft) & 1U);
}

// @brief 外部出力かどうか調べる．
// @return 外部出力なら true を返す．
inline
bool
BlifIdCell::is_output() const
{
  return static_cast<bool>((mFlags >> kOutSft) & 1U);
}

// @brief 位置情報を設定する．
inline
void
BlifIdCell::set_loc(const FileRegion& loc)
{
  mLoc = loc;
}

// @brief 定義済みの印をつける．
inline
void
BlifIdCell::set_defined()
{
  mFlags |= (1U << kDefSft);
  mLoc2 = mLoc;
}

// @brief 外部入力の印をつける．
inline
void
BlifIdCell::set_input()
{
  mFlags |= (1U << kInSft);
}

// @brief 外部出力の印をつける．
inline
void
BlifIdCell::set_output()
{
  mFlags |= (1U << kOutSft);
}

END_NAMESPACE_YM_BNET

#endif // BLIFIDCELL_H
