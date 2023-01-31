#ifndef ISCAS89PARSER_H
#define ISCAS89PARSER_H

/// @file Iscas89Parser.h
/// @brief Iscas89Parser のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2021, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/iscas89_nsdef.h"
#include "ym/FileRegion.h"
#include "Iscas89Scanner.h"
#include "Iscas89Token.h"
#include "ModelImpl.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Parser Iscas89Parser.h "Iscas89Parser.h"
/// @brief ISCAS89(.bench) 形式のファイルを読み込むパーサークラス
//////////////////////////////////////////////////////////////////////
class Iscas89Parser
{
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
    const string& filename, ///< [in] ファイル名
    ModelImpl* model        ///< [in] パーズ結果を格納するオブジェクト
  );


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
    Iscas89GateType type,                 ///< [in] ゲートタイプ
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

  /// @brief 識別子番号を得る．
  ///
  /// 登録されていなければ新しく作る．
  SizeType
  find_id(
    const string& name,
    const FileRegion& loc
  )
  {
    if ( mIdHash.count(name) > 0 ) {
      return mIdHash.at(name);
    }
    SizeType id = mRefLocArray.size();
    mRefLocArray.push_back(loc);
    mModel->new_node(name);
    mIdHash.emplace(name, id);
    return id;
  }

  /// @brief 定義済みの印をつける．
  void
  set_defined(
    SizeType id,          ///< [in] ID番号
    const FileRegion& loc ///< ファイル上の位置
  )
  {
    mDefLocDict.emplace(id, loc);
  }

  /// @brief 該当の識別子が定義済みか調べる．
  bool
  is_defined(
    SizeType id ///< [in] ID番号
  ) const
  {
    return mDefLocDict.count(id) > 0;
  }

  /// @brief ID 番号から文字列を得る．
  const string&
  id2str(
    SizeType id ///< [in] ID番号
  ) const
  {
    return mModel->node_name(id);
  }

  /// @brief ID 番号から参照されている位置情報を得る．
  const FileRegion&
  ref_loc(
    SizeType id ///< [in] ID番号
  ) const
  {
    ASSERT_COND( 0 <= id && id < mRefLocArray.size() );
    return mRefLocArray[id];
  }

  /// @brief ID 番号から定義されている位置情報を得る．
  const FileRegion&
  def_loc(
    SizeType id ///< [in] ID番号
  ) const
  {
    ASSERT_COND( mDefLocDict.count(id) > 0 );
    return mDefLocDict.at(id);
  }

  /// @brief トロロジカル順に並べる．
  void
  order_node(
    SizeType id
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 字句解析器
  // この変数の値は read() 内のみで意味を持つ．
  Iscas89Scanner* mScanner;

  // 結果を格納するオブジェクト
  ModelImpl* mModel;

  // 名前をキーにした識別子のハッシュ表
  unordered_map<string, SizeType> mIdHash;

  // 参照された位置を記録する配列
  vector<FileRegion> mRefLocArray;

  // 定義された位置を記録する辞書
  unordered_map<SizeType, FileRegion> mDefLocDict;

  // 処理済みの印
  unordered_set<SizeType> mMark;

};

END_NAMESPACE_YM_ISCAS89

#endif // ISCAS89PARSER_H
