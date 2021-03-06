﻿#ifndef BLIFPARSERIMPL_H
#define BLIFPARSERIMPL_H

/// @file BlifParserImpl.h
/// @brief BlifParserImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016, 2019, 2021 Yusuke Matsunaga
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
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(
    const string& filename,             ///< [in] ファイル名
    const ClibCellLibrary& cell_library ///< [in] セルライブラリ
  );

  /// @brief イベントハンドラの登録
  void
  add_handler(
    BlifHandler* handler ///< [in] 登録するハンドラ
  );


public:
  //////////////////////////////////////////////////////////////////////
  // BlifHandler から用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号から文字列を得る．
  const string&
  id2str(
    int id ///< [in] ID番号
  )
  {
    ASSERT_COND( 0 <= id && id < mCellArray.size() );
    return mCellArray[id].name();
  }

  /// @brief ID番号からそれに関連した位置情報を得る．
  const FileRegion&
  id2loc(
    int id ///< [in] ID番号
  )
  {
    ASSERT_COND( 0 <= id && id < mCellArray.size() );
    return mCellArray[id].def_loc();
  }

  /// @brief カバーの数を得る．
  int
  cover_num()
  {
    return mCoverMgr.cover_num();
  }

  /// @brief カバーIDから BlifCover を得る．
  const BlifCover&
  id2cover(
    int id ///< [in] ID番号
  )
  {
    return mCoverMgr.cover(id);
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
      const string& name,
      const FileRegion& loc
    ) : mName{name},
	mRefLoc{loc}
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

    /// @brief このシンボルを参照している位置を返す．
    const FileRegion&
    ref_loc() const { return mRefLoc; }

    /// @brief このシンボルの定義された位置を返す．
    const FileRegion&
    def_loc() const { return mDefLoc; }

    /// @brief このシンボルの名前を返す．
    const string&
    name() const { return mName; }

    /// @brief 定義済みフラグをセットする．
    void
    set_defined(
      const FileRegion& loc
    )
    {
      mDefLoc = loc;
      mFlags.set(0);
    }

    /// @brief 入力として定義されたことをセットする．
    void
    set_input(
      const FileRegion& loc
    )
    {
      set_defined(loc);
      mFlags.set(1);
    }

    /// @brief 出力として用いられていることをセットする．
    void
    set_output()
    {
      mFlags.set(2);
    }


  private:
    //////////////////////////////////////////////////////////////////////
    // データメンバ
    //////////////////////////////////////////////////////////////////////

    // この識別子を参照している位置情報
    FileRegion mRefLoc;

    // この識別子を定義している位置情報
    FileRegion mDefLoc;

    // いくつかのフラグ
    // 0: defined マーク
    // 1: input マーク
    // 2: output マーク
    std::bitset<3> mFlags{0};

    // 名前
    string mName;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief .model 文の読み込みを行う．
  /// @retval true 正しく読み込んだ．
  /// @retval false エラーが起こった．
  bool
  read_model();

  /// @brief .inputs 文の読み込みを行う．
  /// @retval true 正しく読み込んだ．
  /// @retval false エラーが起こった．
  bool
  read_inputs();

  /// @brief .outputs 文の読み込みを行う．
  /// @retval true 正しく読み込んだ．
  /// @retval false エラーが起こった．
  bool
  read_outputs();

  /// @brief .names 文の読み込みを行う．
  /// @retval true 正しく読み込んだ．
  /// @retval false エラーが起こった．
  bool
  read_names();

  /// @brief .gate 文の読み込みを行う．
  /// @retval true 正しく読み込んだ．
  /// @retval false エラーが起こった．
  bool
  read_gate();

  /// @brief .latch 文の読み込みを行う．
  /// @retval true 正しく読み込んだ．
  /// @retval false エラーが起こった．
  bool
  read_latch();

  /// @brief .exdc 文の読み込みを行う．
  /// @retval true 正しく読み込んだ．
  /// @retval false エラーが起こった．
  bool
  read_exdc();

  /// @brief ダミーの１行読み込みを行う．
  /// @retval true 正しく読み込んだ．
  /// @retval false エラーが起こった．
  bool
  read_dummy1();

  /// @brief 次のトークンを読み出す．
  void
  next_token();

  /// @brief 直前に読み出したトークンを返す．
  BlifToken
  cur_token() const;

  /// @brief 直前に読み出したトークンが文字列の場合にその文字列を返す．
  string
  cur_string() const;

  /// @brief 直前に読み出したトークンの位置を返す．
  FileRegion
  cur_loc() const;

  /// @brief name に対応する IdCell を取り出す．
  /// @return 対応するID番号を返す．
  ///
  /// 未登録の場合には新たに作る．
  int
  find_id(
    const string& name,   ///< [in] 名前
    const FileRegion& loc ///< [in] name の位置
  );

  /// @brief 対応する識別子がすでに定義済みか調べる．
  /// @retval true 定義済み
  /// @retval false 未定義
  bool
  is_defined(
    int id ///< [in] 識別子番号
  ) const;

  /// @brief 対応する識別子が入力用か調べる．
  /// @retval true 入力
  /// @retval false 入力以外
  bool
  is_input(
    int id ///< [in] 識別子番号
  ) const;

  /// @brief 対応する識別子が出力用か調べる．
  /// @retval true 出力
  /// @retval false 出力以外
  bool
  is_output(
    int id ///< [in] 識別子番号
  ) const;

  /// @brief 対応する識別子に定義済みの印をつける．
  void
  set_defined(
    int id,               ///< [in] 識別子番号
    const FileRegion& loc ///< [in] 定義している場所．
  );

  /// @brief 対応する識別子に入力用の印を付ける．
  void
  set_input(
    int id,               ///< [in] 識別子番号
    const FileRegion& loc ///< [in] 定義している場所．
  );

  /// @brief 対応する識別子に出力用の印を付ける．
  void
  set_output(
    int id ///< [in] 識別子番号
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // セルライブラリ
  ClibCellLibrary mCellLibrary;

  // 字句解析器
  // この変数は read() 内でのみ有効
  BlifScanner* mScanner;

  // イベントハンドラのリスト
  vector<BlifHandler*> mHandlerList;

  // 名前をキーにした識別子番号のハッシュ表
  unordered_map<string, int> mIdHash;

  // IdCell本体の配列
  vector<IdCell> mCellArray;

  // 出力の ID 番号のリスト
  vector<int> mOidArray;

  // BlifCover を管理するオブジェクト
  BlifCoverMgr mCoverMgr;

  // 現在のトークン
  BlifToken mCurToken;

  // 現在のトークンの位置
  FileRegion mCurLoc;

};

END_NAMESPACE_YM_BNET

#endif // BLIFPARSERIMPL_H
