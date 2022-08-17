#ifndef ISCAS89PARSER_H
#define ISCAS89PARSER_H

/// @file Iscas89Parser.h
/// @brief Iscas89Parser のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/bnet.h"
#include "ym/FileRegion.h"
#include "Iscas89Token.h"


BEGIN_NAMESPACE_YM_BNET

class Iscas89Handler;
class Iscas89Scanner;

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Parser Iscas89Parser.h "Iscas89Parser.h"
/// @ingroup Iscas89Group
/// @brief ISCAS89(.bench) 形式のファイルを読み込むパーサークラス
/// @sa Iscas89Handler
//////////////////////////////////////////////////////////////////////
class Iscas89Parser
{
  friend class Iscas89Handler;

public:

  /// @brief コンストラクタ
  Iscas89Parser() = default;

  /// @brief デストラクタ
  ~Iscas89Parser() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 読み込みを行う．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(
    const string& filename ///< [in] ファイル名
  );

  /// @brief イベントハンドラの登録
  void
  add_handler(
    Iscas89Handler* handler ///< [in] 登録するハンドラ
  );

  /// @brief ID 番号から文字列を得る．
  const string&
  id2str(
    SizeType id ///< [in] ID番号
  ) const
  {
    ASSERT_COND( 0 <= id && id < mIdCellArray.size() );
    return mIdCellArray[id].name();
  }

  /// @brief ID 番号から位置情報を得る．
  const FileRegion&
  id2loc(
    SizeType id ///< [in] ID番号
  ) const
  {
    ASSERT_COND( 0 <= id && id < mIdCellArray.size() );
    return mIdCellArray[id].loc();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief 識別子を表すクラス
  class IdCell
  {
  public:

    /// @brief コンストラクタ
    IdCell(
      const string& name ///< [in] 名前
    ) : mFlags{0},
	mName{name}
    {
    }

    /// @brief デストラクタ
    ~IdCell() = default;

    /// @brief 定義済みシンボルのとき true を返す．
    bool
    is_defined() const { return mFlags[0]; }

    /// @brief 入力として定義されている時 true を返す．
    bool
    is_input() const { return mFlags[1]; }

    /// @brief 出力として定義されている時 true を返す．
    bool
    is_output() const { return mFlags[2]; }

    /// @brief このシンボルの定義された位置を返す．
    const FileRegion&
    loc() const { return mLoc; }

    /// @brief このシンボルの名前を返す．
    const string&
    name() const { return mName; }

    /// @brief 定義済みフラグをセットする．
    void
    set_defined(
      const FileRegion& loc ///< [in] ファイル上の位置
    )
    {
      mLoc = loc;
      mFlags.set(0);
    }

    /// @brief 入力として定義されたことをセットする．
    void
    set_input()
    {
      mFlags.set(1);
    }

    void
    set_output()
    {
      mFlags.set(2);
    }


  private:
    //////////////////////////////////////////////////////////////////////
    // データメンバ
    //////////////////////////////////////////////////////////////////////

    // この識別子を定義している位置情報
    FileRegion mLoc;

    // いくつかのフラグ
    // 0: defined マーク
    // 1: input マーク
    // 2: output マーク
    std::bitset<3> mFlags;

    // 名前
    string mName;

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
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  ///
  /// エラーが起きたらエラーメッセージをセットする．
  bool
  parse_name(
    SizeType& name_id,   ///< [out] 名前の識別子番号を格納する変数．
    FileRegion& last_loc ///< [in] 末尾のファイル位置
  );

  /// @brief '(' ')' で囲まれた名前のリストを読み込む．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  ///
  /// エラーが起きたらエラーメッセージをセットする．
  bool
  parse_name_list(
    vector<SizeType>& name_id_list, ///< [out] 名前の識別子番号を格納するリスト．
    FileRegion& last_loc            ///< [in] 末尾のファイル位置
  );

  /// @brief INPUT 文を読み込む．
  /// @return エラーが起きたら false を返す．
  bool
  read_input(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType name_id       ///< [in] 入力ピン名の ID 番号
  );

  /// @brief OUTPUT 文を読み込む．
  /// @return エラーが起きたら false を返す．
  bool
  read_output(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType name_id       ///< [in] 出力ピン名の ID 番号
  );

  /// @brief ゲート文を読み込む．
  /// @return エラーが起きたら false を返す．
  bool
  read_gate(
    const FileRegion& loc,                ///< [in] ファイル位置
    SizeType oname_id,                    ///< [in] 出力名の ID 番号
    BnNodeType type,                      ///< [in] ゲートタイプ
    const vector<SizeType>& iname_id_list ///< [in] 入力名のID番号のリスト
  );

  /// @brief ゲート文(MUX)を読み込む．
  /// @return エラーが起きたら false を返す．
  bool
  read_mux(
    const FileRegion& loc,                ///< [in] ファイル位置
    SizeType oname_id,                    ///< [in] 出力名の ID 番号
    const vector<SizeType>& iname_id_list ///< [in] 入力名のID番号のリスト
  );

  /// @brief D-FF用のゲート文を読み込む．
  /// @return エラーが起きたら false を返す．
  bool
  read_dff(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType oname_id,     ///< [in] 出力名の ID 番号
    SizeType iname_id      ///< [in] 入力名のID番号
  );

  /// @brief 次のトークンが期待されている型か調べる．
  /// @return ok/ng, 識別子番号, ファイル位置のタプルを返す．
  ///
  /// トークンが exp_token と同じなら ok/ng は true となる．
  tuple<bool, SizeType, FileRegion>
  expect(
    Iscas89Token exp_token ///< [in] トークンの期待値
  );

  /// @brief トークンを一つ読みだす．
  /// @return 型，識別子番号，位置のタプルを返す．
  ///
  /// 識別子番号に値が入るのはトークンが Iscas89Token::NAME の時だけ
  tuple<Iscas89Token, SizeType, FileRegion>
  read_token();

  /// @brief 文字列用の領域を確保する．
  /// @return 文字列の ID 番号
  SizeType
  reg_id(
    const string& src_str, ///< [in] ソース文字列
    const FileRegion& loc  ///< [in] 文字列の位置情報
  );

  /// @brief 該当の識別子が定義済みか調べる．
  bool
  is_defined(
    SizeType id ///< [in] ID番号
  ) const
  {
    ASSERT_COND( 0 <= id && id < mIdCellArray.size() );
    return mIdCellArray[id].is_defined();
  }

  /// @brief 該当の識別子が入力か調べる．
  bool
  is_input(
    SizeType id ///< [in] ID番号
  ) const
  {
    ASSERT_COND( 0 <= id && id < mIdCellArray.size() );
    return mIdCellArray[id].is_input();
  }

  /// @brief 該当の識別子が出力か調べる．
  bool
  is_output(
    SizeType id ///< [in] ID番号
  ) const
  {
    ASSERT_COND( 0 <= id && id < mIdCellArray.size() );
    return mIdCellArray[id].is_output();
  }

  /// @brief 識別子に定義済みの印を付ける．
  void
  set_defined(
    SizeType id,          ///< [in] ID番号
    const FileRegion& loc ///< [in] 定義している場所
  )
  {
    ASSERT_COND( 0 <= id && id < mIdCellArray.size() );
    mIdCellArray[id].set_defined(loc);
  }

  /// @brief 識別子に入力の印を付ける．
  ///
  /// 同時に定義済みになる．
  void
  set_input(
    SizeType id,          ///< [in] ID番号
    const FileRegion& loc ///< [in] 定義している場所
  )
  {
    ASSERT_COND( 0 <= id && id < mIdCellArray.size() );
    mIdCellArray[id].set_input();
    mIdCellArray[id].set_defined(loc);
  }

  /// @brief 識別子に出力の印を付ける．
  void
  set_output(
    SizeType id ///< [in] ID番号
  )
  {
    ASSERT_COND( 0 <= id && id < mIdCellArray.size() );
    mIdCellArray[id].set_output();
  }

  /// @brief スキャナーを削除する．
  void
  delete_scanner();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 字句解析器
  // この変数の値は read() 内のみで意味を持つ．
  Iscas89Scanner* mScanner;

  // イベントハンドラのリスト
  vector<Iscas89Handler*> mHandlerList;

  // 名前をキーにした識別子のハッシュ表
  unordered_map<string, SizeType> mIdHash;

  // 識別子の配列
  vector<IdCell> mIdCellArray;

  // 出力の ID番号とファイル位置のリスト
  vector<pair<SizeType, FileRegion>> mOidArray;

};

END_NAMESPACE_YM_BNET

#endif // ISCAS89PARSER_H
