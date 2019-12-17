#ifndef ISCAS89PARSERIMPL_H
#define ISCAS89PARSERIMPL_H

/// @file Iscas89ParserImpl.h
/// @brief Iscas89ParserImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/bnet.h"
#include "ym/FileRegion.h"
#include "Iscas89Scanner.h"
#include "Iscas89Token.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class Iscas89ParserImpl Iscas89ParserImpl.h "ym/Iscas89ParserImpl.h"
/// @brief Iscas89Parser の実装クラス
//////////////////////////////////////////////////////////////////////
class Iscas89ParserImpl
{
public:

  /// @brief コンストラクタ
  Iscas89ParserImpl() = default;

  /// @brief デストラクタ
  ~Iscas89ParserImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 読み込みを行う．
  /// @param[in] filename ファイル名
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(const string& filename);

  /// @brief イベントハンドラの登録
  /// @param[in] handler 登録するハンドラ
  void
  add_handler(Iscas89Handler* handler);

  /// @brief ID 番号から文字列を得る．
  const char*
  id2str(int id) const;

  /// @brief ID 番号から位置情報を得る．
  const FileRegion&
  id2loc(int id) const;


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
    set_input();

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

  /// @brief ゲート型を読み込む．
  /// @return トークンを返す．
  ///
  /// エラーが起きたら kIscas89_ERROR を返す．
  Iscas89Token
  parse_gate_type();

  /// @brief '(' ')' で囲まれた名前を読み込む．
  /// @param[in] name_id 名前の識別子番号を格納する変数．
  /// @param[in] last_loc 末尾のファイル位置
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  ///
  /// エラーが起きたらエラーメッセージをセットする．
  bool
  parse_name(int& name_id,
	     FileRegion& last_loc);

  /// @brief '(' ')' で囲まれた名前のリストを読み込む．
  /// @param[in] name_id_list 名前の識別子番号を格納するリスト．
  /// @param[in] last_loc 末尾のファイル位置
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  ///
  /// エラーが起きたらエラーメッセージをセットする．
  bool
  parse_name_list(vector<int>& name_id_list,
		  FileRegion& last_loc);

  /// @brief INPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 入力ピン名の ID 番号
  /// @return エラーが起きたら false を返す．
  bool
  read_input(const FileRegion& loc,
	     int name_id);

  /// @brief OUTPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 出力ピン名の ID 番号
  /// @return エラーが起きたら false を返す．
  bool
  read_output(const FileRegion& loc,
	      int name_id);

  /// @brief ゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] type ゲートタイプ
  /// @return エラーが起きたら false を返す．
  bool
  read_gate(const FileRegion& loc,
	    int oname_id,
	    BnNodeType type,
	    const vector<int>& iname_id_list);

  /// @brief ゲート文(MUX)を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] oname_id 出力名の ID 番号
  /// @return エラーが起きたら false を返す．
  bool
  read_mux(const FileRegion& loc,
	   int oname_id,
	   const vector<int>& iname_id_list);

  /// @brief D-FF用のゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] type ゲートタイプ
  /// @return エラーが起きたら false を返す．
  bool
  read_dff(const FileRegion& loc,
	   int oname_id,
	   int iname_id);

  /// @brief 次のトークンが期待されている型か調べる．
  /// @param[in] exp_token トークンの期待値
  /// @return ok/ng, 識別子番号, ファイル位置のタプルを返す．
  ///
  /// トークンが exp_token と同じなら ok/ng は true となる．
  tuple<bool, int, FileRegion>
  expect(Iscas89Token exp_token);

  /// @brief トークンを一つ読みだす．
  /// @return 型，識別子番号，位置のタプルを返す．
  ///
  /// 識別子番号に値が入るのはトークンが Iscas89Token::NAME の時だけ
  tuple<Iscas89Token, int, FileRegion>
  read_token();

  /// @brief ID 番号から IdCell を得る．
  IdCell&
  id2cell(int id);

  /// @brief ID 番号から IdCell を得る．
  const IdCell&
  id2cell(int id) const;

  /// @brief 文字列用の領域を確保する．
  /// @param[in] src_str ソース文字列
  /// @param[in] loc 文字列の位置情報
  /// @return 文字列の ID 番号
  int
  reg_id(const char* src_str,
	 const FileRegion& loc);

  /// @brief スキャナーを削除する．
  void
  delete_scanner();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 字句解析器
  unique_ptr<Iscas89Scanner> mScanner;

  // イベントハンドラのリスト
  vector<Iscas89Handler*> mHandlerList;

  // 名前をキーにした識別子のハッシュ表
  unordered_map<const char*, IdCell*> mIdHash;

  // 識別子の配列
  vector<IdCell> mIdArray;

  // 出力の ID番号とファイル位置のリスト
  vector<pair<int, FileRegion>> mOidArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ID 番号から Iscas89IdCell を得る．
inline
Iscas89ParserImpl::IdCell&
Iscas89ParserImpl::id2cell(int id)
{
  ASSERT_COND( 0 <= id && id < mIdArray.size() );
  return mIdArray[id];
}

// @brief ID 番号から Iscas89IdCell を得る．
inline
const Iscas89ParserImpl::IdCell&
Iscas89ParserImpl::id2cell(int id) const
{
  ASSERT_COND( 0 <= id && id < mIdArray.size() );
  return mIdArray[id];
}

// @brief ID 番号から文字列を得る．
inline
const char*
Iscas89ParserImpl::id2str(int id) const
{
  return id2cell(id).name();
}

// @brief ID 番号から位置情報を得る．
inline
const FileRegion&
Iscas89ParserImpl::id2loc(int id) const
{
  return id2cell(id).loc();
}

// @brief コンストラクタ
inline
Iscas89ParserImpl::IdCell::IdCell(int id,
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
Iscas89ParserImpl::IdCell::~IdCell()
{
  delete [] mName;
}

// @brief ID番号を返す．
inline
int
Iscas89ParserImpl::IdCell::id() const
{
  return mId;
}

// @brief 定義済みシンボルのとき true を返す．
inline
bool
Iscas89ParserImpl::IdCell::is_defined() const
{
  return mFlags[0];
}

// @brief 入力として定義されている時 true を返す．
inline
bool
Iscas89ParserImpl::IdCell::is_input() const
{
  return mFlags[1];
}

// @brief 出力として定義されている時 true を返す．
inline
bool
Iscas89ParserImpl::IdCell::is_output() const
{
  return mFlags[2];
}

// @brief このシンボルの定義された位置を返す．
inline
const FileRegion&
Iscas89ParserImpl::IdCell::loc() const
{
  return mLoc;
}

// @brief このシンボルの名前を返す．
inline
const char*
Iscas89ParserImpl::IdCell::name() const
{
  return mName;
}

// @brief 定義済みフラグをセットする．
inline
void
Iscas89ParserImpl::IdCell::set_defined(const FileRegion& loc)
{
  mLoc = loc;
  mFlags.set(0);
}

// @brief 入力として定義されたことをセットする．
inline
void
Iscas89ParserImpl::IdCell::set_input()
{
  mFlags.set(1);
}

inline
void
Iscas89ParserImpl::IdCell::set_output()
{
  mFlags.set(2);
}

END_NAMESPACE_YM_BNET

#endif // ISCAS89PARSERIMPL_H
