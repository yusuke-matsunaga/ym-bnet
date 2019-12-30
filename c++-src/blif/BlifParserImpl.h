#ifndef BLIFPARSERIMPL_H
#define BLIFPARSERIMPL_H

/// @file BlifParserImpl.h
/// @brief BlifParserImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"
#include "ym/ClibCellLibrary.h"
#include "BlifScanner.h"
#include "BlifCoverMgr.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class BlifParserImpl BlifParserImpl.h "BlifParserImpl.h"
/// @ingroup BlifGroup
/// @brief blif形式のファイルを読み込むパーサークラス
/// 適切なハンドラクラスを生成して add_handler() で登録して使う．
/// @sa BlifHandler MsgHandler
//////////////////////////////////////////////////////////////////////
class BlifParserImpl
{
public:

  /// @brief コンストラクタ
  BlifParserImpl() = default;

  /// @brief デストラクタ
  ~BlifParserImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 読み込みを行う．
  /// @param[in] filename ファイル名
  /// @param[in] cell_library セルライブラリ
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(const string& filename,
       const ClibCellLibrary& cell_library);

  /// @brief イベントハンドラの登録
  /// @param[in] handler 登録するハンドラ
  void
  add_handler(BlifHandler* handler);


public:
  //////////////////////////////////////////////////////////////////////
  // BlifHandler から用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号から文字列を得る．
  const char*
  id2str(int id);

  /// @brief ID番号からそれに関連した位置情報を得る．
  const FileRegion&
  id2loc(int id);

  /// @brief カバーの数を得る．
  int
  cover_num();

  /// @brief カバーIDから BlifCover を得る．
  const BlifCover&
  id2cover(int id);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief 識別子を表すクラス
  class IdCell
  {
  public:

    /// @brief コンストラクタ
    IdCell(int id,
	   const char* name);

    /// @brief デストラクタ
    ~IdCell();

    /// @brief ID番号を返す．
    int
    id() const;

    /// @brief 定義済みシンボルのとき true を返す．
    bool
    is_defined() const;

    /// @brief 入力として定義されている時 true を返す．
    bool
    is_input() const;

    /// @brief 出力として定義されている時 true を返す．
    bool
    is_output() const;

    /// @brief このシンボルの定義された位置を返す．
    const FileRegion&
    loc() const;

    /// @brief このシンボルの名前を返す．
    const char*
    name() const;

    /// @brief 定義済みフラグをセットする．
    void
    set_defined(const FileRegion& loc);

    /// @brief 入力として定義されたことをセットする．
    void
    set_input(const FileRegion& loc);

    /// @brief 出力として用いられていることをセットする．
    void
    set_output();


  private:
    //////////////////////////////////////////////////////////////////////
    // データメンバ
    //////////////////////////////////////////////////////////////////////

    // ID 番号
    int mId;

    // この識別子を定義している位置情報
    FileRegion mLoc;

    // いくつかのフラグ
    // 0: defined マーク
    // 1: input マーク
    // 2: output マーク
    std::bitset<3> mFlags;

    // 名前
    char* mName;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief トークンを一つ読み出す．
  /// @param[out] loc トークンの位置を格納する変数
  BlifToken
  get_token(FileRegion& loc);

  /// @brief トークンを戻す．
  void
  unget_token(BlifToken token,
	      const FileRegion& loc);

  /// @brief name に対応する IdCell を取り出す．
  /// @param[in] name 名前
  /// @return 対応する IdCell を返す．
  ///
  /// 未登録の場合には新たに作る．
  IdCell*
  find_id(const char* name);

  /// @brief スキャナーを削除する．
  void
  delete_scanner();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // セルライブラリ
  ClibCellLibrary mCellLibrary;

  // 字句解析器
  unique_ptr<BlifScanner> mScanner;

  // 読み戻されたトークン
  BlifToken mUngetToken;

  // mUngetToken に対応する位置情報
  FileRegion mUngetTokenLoc;

  // イベントハンドラのリスト
  vector<BlifHandler*> mHandlerList;

  // 名前をキーにした識別子のハッシュ表
  unordered_map<const char*, IdCell*> mIdHash;

  // 識別本体の配列
  vector<IdCell> mIdArray;

  // 現在処理中のIdCellとFileRegionのペアの配列
  vector<pair<IdCell*, FileRegion>> mCurIdArray;

  // 出力の ID 番号のリスト
  vector<int> mOidArray;

  // BlifCover を管理するオブジェクト
  BlifCoverMgr mCoverMgr;

  // キューブ数
  int mNc;

  // パタンのバッファ
  StrBuff mCoverPat;

  // 出力の極性を表す文字
  char mOpatChar;

  // 現在のセル番号
  int mCellId;

  // 位置情報バッファ
  FileRegion mLoc1;

  // 文字列バッファ1
  StrBuff mName1;

  // 文字列バッファ2
  StrBuff mName2;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ID番号から文字列を得る．
inline
const char*
BlifParserImpl::id2str(int id)
{
  return mIdArray[id].name();
}

// @brief ID番号からそれに関連した位置情報を得る．
inline
const FileRegion&
BlifParserImpl::id2loc(int id)
{
  return mIdArray[id].loc();
}

// @brief カバーの数を得る．
inline
int
BlifParserImpl::cover_num()
{
  return mCoverMgr.cover_num();
}

// @brief カバーIDから BlifCover を得る．
inline
const BlifCover&
BlifParserImpl::id2cover(int id)
{
  return mCoverMgr.cover(id);
}

// @brief トークンを戻す．
inline
void
BlifParserImpl::unget_token(BlifToken token,
			    const FileRegion& loc)
{
  mUngetToken = token;
  mUngetTokenLoc = loc;
}

// @brief コンストラクタ
inline
BlifParserImpl::IdCell::IdCell(int id,
			       const char* name) :
  mId{id},
  mFlags{0}
{
  SizeType n = strlen(name) + 1;
  mName = new char[n];
  memcpy(mName, name, n);
}

// @brief デストラクタ
inline
BlifParserImpl::IdCell::~IdCell()
{
  delete [] mName;
}

// @brief ID番号を返す．
inline
int
BlifParserImpl::IdCell::id() const
{
  return mId;
}

// @brief 定義済みシンボルのとき true を返す．
inline
bool
BlifParserImpl::IdCell::is_defined() const
{
  return mFlags[0];
}

// @brief 入力として定義されている時 true を返す．
inline
bool
BlifParserImpl::IdCell::is_input() const
{
  return mFlags[1];
}

// @brief 出力として定義されている時 true を返す．
inline
bool
BlifParserImpl::IdCell::is_output() const
{
  return mFlags[2];
}

// @brief このシンボルの定義された位置を返す．
inline
const FileRegion&
BlifParserImpl::IdCell::loc() const
{
  return mLoc;
}

// @brief このシンボルの名前を返す．
inline
const char*
BlifParserImpl::IdCell::name() const
{
  return mName;
}

// @brief 定義済みフラグをセットする．
inline
void
BlifParserImpl::IdCell::set_defined(const FileRegion& loc)
{
  mLoc = loc;
  mFlags.set(0);
}

// @brief 入力として定義されたことをセットする．
inline
void
BlifParserImpl::IdCell::set_input(const FileRegion& loc)
{
  set_defined(loc);
  mFlags.set(1);
}

inline
void
BlifParserImpl::IdCell::set_output()
{
  mFlags.set(2);
}

END_NAMESPACE_YM_BNET

#endif // BLIFPARSERIMPL_H
