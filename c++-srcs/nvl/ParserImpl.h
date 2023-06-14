#ifndef PARSERIMPL_H
#define PARSERIMPL_H

/// @file ParserImpl.h
/// @brief ParserImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/nvl_nsdef.h"
#include "ym/FileRegion.h"
#include "NvlScanner.h"
#include "NvlToken.h"
#include "ModelImpl.h"


BEGIN_NAMESPACE_YM_NVL

//////////////////////////////////////////////////////////////////////
/// @class ParserImpl ParserImpl.h "ParserImpl.h"
/// @brief netlist 形式の verilog ファイルを読み込むパーサークラス
//////////////////////////////////////////////////////////////////////
class ParserImpl
{
public:

  /// @brief コンストラクタ
  ParserImpl() = default;

  /// @brief デストラクタ
  ~ParserImpl();


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


public:
  //////////////////////////////////////////////////////////////////////
  // NvlHandler から用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートの設定を行う．
  void
  set_gate(
    SizeType id,                       ///< [in] ID番号
    const FileRegion& loc,             ///< [in] ファイル上の位置
    PrimType gate_type,                ///< [in] ゲートの種類
    const vector<SizeType>& fanin_list ///< [in] ファンインのIDのリスト
  )
  {
    set_defined(id, loc);
    mModel->set_gate(id, gate_type, fanin_list);
  }

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

  /// @brief ID 番号から文字列を得る．
  const string&
  id2str(
    SizeType id ///< [in] ID番号
  ) const
  {
    return mModel->node_name(id);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief INPUT 文を読み込む．
  /// @return エラーが起きたら false を返す．
  bool
  read_input(
    const FileRegion& first_loc ///< [in] 先頭のファイル位置
  );

  /// @brief OUTPUT 文を読み込む．
  /// @return エラーが起きたら false を返す．
  bool
  read_output(
    const FileRegion& first_loc ///< [in] 先頭のファイル位置
  );

  /// @brief ゲート/DFF 文を読み込む．
  /// @return エラーが起きたら false を返す．
  bool
  read_gate(
    const FileRegion& first_loc, ///< [in] 先頭のファイル位置
    SizeType name_id             ///< [in] 出力名の ID 番号
  );

  /// @brief ゲート文を読み込む．
  /// @return エラーが起きたら false を返す．
  bool
  read_gate(
    const FileRegion& loc,             ///< [in] ファイル位置
    SizeType oname_id,                 ///< [in] 出力名の ID 番号
    PrimType type,                     ///< [in] ゲートタイプ
    const vector<SizeType>& iname_list ///< [in] 入力名のID番号のリスト
  );

  /// @brief D-FF用のゲート文を読み込む．
  /// @return エラーが起きたら false を返す．
  bool
  read_dff(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType oname_id,     ///< [in] 出力名の ID 番号
    SizeType iname_id      ///< [in] 入力名のID番号
  );

  /// @brief 論理式を登録する．
  /// @return 論理式番号を返す．
  SizeType
  reg_expr(
    const Expr& expr ///< [in] 論理式
  );

  /// @brief 次のトークンが期待されている型か調べる．
  /// @return ok/ng, 識別子番号, ファイル位置のタプルを返す．
  ///
  /// トークンが exp_token と同じなら ok/ng は true となる．
  tuple<bool, SizeType, FileRegion>
  expect(
    NvlToken::Type exp_type ///< [in] トークンの期待値
  );

  /// @brief トークンを一つ読みだす．
  /// @return トークンを返す．
  ///
  /// 識別子番号に値が入るのはトークンが NvlToken::NAME の時だけ
  NvlToken
  read_token(
    SizeType& name_id
  );

  /// @brief 新しいノードを確保する．
  /// @return ID番号を返す．
  SizeType
  new_node(
    const string& name,   ///< [in] 名前
    const FileRegion& loc ///< [in] ファイル上の位置
  )
  {
    SizeType id = mRefLocArray.size();
    mRefLocArray.push_back(loc);
    mModel->new_node(name);
    return id;
  }

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
    SizeType id = new_node(name, loc);
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
  NvlScanner* mScanner;

  // 拡張ハンドラのリスト
  vector<NvlHandler*> mHandlerList;

  // 拡張ハンドラ番号の辞書
  unordered_map<string, SizeType> mHandlerDict;

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

  // 論理式の辞書
  // キーは Expr::rep_string()
  // 値は論理式番号
  unordered_map<string, SizeType> mExprDict;

};

END_NAMESPACE_YM_NVL

#endif // PARSERIMPL_H
