#ifndef PARSEIMPL_H
#define PARSEIMPL_H

/// @file ParserImpl.h
/// @brief ParserImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/blif_nsdef.h"
#include "ym/ClibCellLibrary.h"
#include "BlifScanner.h"
#include "CoverMgr.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
/// @class ParserImpl ParserImpl.h "ym/ParserImpl.h"
/// @brief blif形式のファイルを読み込むパーサークラス
///
/// 適切なハンドラクラスを生成して add_handler() で登録して使う．
/// @sa BlifHandler MsgHandler
//////////////////////////////////////////////////////////////////////
class ParserImpl
{
public:

  /// @brief コンストラクタ
  ParserImpl() = default;

  /// @brief デストラクタ
  ~ParserImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 読み込みを行う(セルライブラリ付き)．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(
    const string& filename,              ///< [in] ファイル名
    const ClibCellLibrary& cell_library, ///< [in] セルライブラリ
    BlifModel& model                     ///< [out] 結果を格納する変数
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 識別子番号に関連する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号から文字列を得る．
  const string&
  id2str(
    SizeType id ///< [in] ID番号
  )
  {
    ASSERT_COND( 0 <= id && id < mCellArray.size() );
    return mCellArray[id].name();
  }

  /// @brief ID番号からそれに関連した位置情報を得る．
  const FileRegion&
  id2loc(
    SizeType id ///< [in] ID番号
  )
  {
    ASSERT_COND( 0 <= id && id < mCellArray.size() );
    return mCellArray[id].def_loc();
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

    /// @brief .names として定義されている時 true を返す．
    bool
    is_names() const { return mFlags[2]; }

    /// @brief .gate として定義されている時 true を返す．
    bool
    is_gate() const { return mFlags[3]; }

    /// @brief .latch として定義されている時 true を返す．
    bool
    is_latch() const { return mFlags[4]; }

    /// @brief このシンボルを参照している位置を返す．
    const FileRegion&
    ref_loc() const { return mRefLoc; }

    /// @brief このシンボルの定義された位置を返す．
    const FileRegion&
    def_loc() const { return mDefLoc; }

    /// @brief このシンボルの名前を返す．
    const string&
    name() const { return mName; }

    /// @brief 入力数を返す．
    SizeType
    inode_size() const { return mInputList.size(); }

    /// @brief 入力番号のリストを返す．
    const vector<SizeType>&
    inode_id_list() const { return mInputList; }

    /// @brief カバー番号を得る．
    ///
    /// is_names() == true の時のみ有効
    SizeType
    cover_id() const { return mExtId; }

    /// @brief セル番号を得る．
    ///
    /// is_gate() == true の時のみ有効
    SizeType
    cell_id() const { return mExtId; }

    /// @brief 入力ノード番号を得る．
    ///
    /// is_latch() == true の時のみ有効
    SizeType
    inode_id() const { return mInputList[0]; }

    ///
    /// is_latch() == true の時のみ有効
    /// @brief リセット値を得る．
    char
    rval() const { return static_cast<char>(mExtId); }

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

    /// @brief .names として定義されていることをセットする．
    void
    set_names(
      const FileRegion& loc,
      const vector<SizeType>& input_list,
      SizeType cover_id
    )
    {
      set_defined(loc);
      mFlags.set(2);
      mInputList = input_list;
      mExtId = cover_id;
    }

    /// @brief .names として定義されていることをセットする．
    void
    set_gate(
      const FileRegion& loc,
      const vector<SizeType>& input_list,
      SizeType cell_id
    )
    {
      set_defined(loc);
      mFlags.set(3);
      mInputList = input_list;
      mExtId = cell_id;
    }

    /// @brief .latch として定義されていることをセットする．
    void
    set_latch(
      const FileRegion& loc,
      SizeType input_id,
      char rval
    )
    {
      set_defined(loc);
      mFlags.set(4);
      mInputList = {input_id};
      mExtId = static_cast<SizeType>(rval);
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
    // 2: names マーク
    // 3: gate マーク
    // 4: latch マーク
    std::bitset<5> mFlags{0};

    // 名前
    string mName;

    // 入力の識別子番号のリスト
    vector<SizeType> mInputList;

    // カバー番号/セル番号
    SizeType mExtId;

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
  SizeType
  find_id(
    const string& name,   ///< [in] 名前
    const FileRegion& loc ///< [in] name の位置
  );

  /// @brief 対応する識別子がすでに定義済みか調べる．
  /// @retval true 定義済み
  /// @retval false 未定義
  bool
  is_defined(
    SizeType id ///< [in] 識別子番号
  ) const;

  /// @brief 対応する識別子が入力用か調べる．
  /// @retval true 入力
  /// @retval false 入力以外
  bool
  is_input(
    SizeType id ///< [in] 識別子番号
  ) const;

  /// @brief 対応する識別子に入力用の印を付ける．
  void
  set_input(
    SizeType id,               ///< [in] 識別子番号
    const FileRegion& loc ///< [in] 定義している場所．
  )
  {
    ASSERT_COND( 0 <= id && id < mCellArray.size() );
    mCellArray[id].set_input(loc);
  }

  /// @brief .names 文の情報をセットする．
  void
  set_names(
    SizeType id,                        ///< [in] 識別子番号
    const FileRegion& loc,              ///< [in] 定義している場所
    const vector<SizeType>& input_list, ///< [in] 入力の識別子番号のリスト
    SizeType cover_id                   ///< [in] カバー番号
  )
  {
    ASSERT_COND( 0 <= id && id < mCellArray.size() );
    mCellArray[id].set_names(loc, input_list, cover_id);
  }

  /// @brief .gate 文の情報をセットする．
  void
  set_gate(
    SizeType id,                        ///< [in] 識別子番号
    const FileRegion& loc,              ///< [in] 定義している場所
    const vector<SizeType>& input_list, ///< [in] 入力の識別子番号のリスト
    SizeType cell_id                    ///< [in] セル番号
  )
  {
    ASSERT_COND( 0 <= id && id < mCellArray.size() );
    mCellArray[id].set_gate(loc, input_list, cell_id);
  }

  /// @brief .latch 文の情報をセットする．
  void
  set_latch(
    SizeType id,           ///< [in] 識別子番号
    const FileRegion& loc, ///< [in] 定義している場所
    SizeType input_id,     ///< [in] 入力の識別子番号
    char rval              ///< [in] リセット値
  )
  {
    ASSERT_COND( 0 <= id && id < mCellArray.size() );
    mCellArray[id].set_latch(loc, input_id, rval);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // BlifModel を生成する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief BlifModel に内容をセットする．
  void
  gen_model(
    BlifModel& model ///< [in] 結果を格納するオブジェクト
  );

  /// @brief 入力からのトポロジカル順にする．
  void
  order_node(
    SizeType id ///< [in] 識別子番号
  );

  /// @brief ノード番号をつける．
  void
  reg_node(
    SizeType id ///< [in] 識別子番号
  )
  {
    auto num = mIdList.size();
    mIdMap.emplace(id, num);
    mIdList.push_back(id);
  }

  /// @brief ファンインのリストを作る．
  vector<SizeType>
  make_inode_list(
    const IdCell& id_cell
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

  // BlifCover を管理するオブジェクト
  // この変数は read() 内でのみ有効
  CoverMgr* mCoverMgr;

  // 現在のトークン
  BlifToken mCurToken;

  // 現在のトークンの位置
  FileRegion mCurLoc;

  // モデル名
  string mModelName;

  // 名前をキーにした識別子番号のハッシュ表
  unordered_map<string, SizeType> mIdHash;

  // IdCell本体の配列
  vector<IdCell> mCellArray;

  // 入力の ID 番号のリスト
  vector<SizeType> mInputArray;

  // 出力の ID 番号のリスト
  vector<SizeType> mOutputArray;

  // ラッチの ID 番号のリスト
  vector<SizeType> mLatchArray;

  // 識別子番号をキーとしたノード番号の辞書
  unordered_map<SizeType, SizeType> mIdMap;

  // ノード番号順に識別子番号を収める配列
  vector<SizeType> mIdList;

  // ノードのリスト
  vector<BlifNode*> mNodeList;

};

END_NAMESPACE_YM_BLIF

#endif // PARSEIMPL_H
