#ifndef ISCAS89HANDLER_H
#define ISCAS89HANDLER_H

/// @file Iscas89Handler.h
/// @brief Iscas89Handler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/iscas89_nsdef.h"
#include "ym/FileRegion.h"
#include "ym/logic.h" // for PrimType


BEGIN_NAMESPACE_YM_ISCAS89

class ParserImpl;

//////////////////////////////////////////////////////////////////////
/// @class Iscas89Handler Iscas89Handler.h "Iscas89Handler.h"
/// @brief iscas89(.bench) 形式の読み込みハンドラの基底クラス
//////////////////////////////////////////////////////////////////////
class Iscas89Handler
{
public:

  /// @brief コンストラクタ
  Iscas89Handler();

  /// @brief デストラクタ
  virtual
  ~Iscas89Handler();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートの読み込みを行う．
  /// @return エラーが起きたら false を返す．
  virtual
  bool
  read(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType oname_id      ///< [in] 出力名の ID 番号
  ) = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // Iscas89Parser から使われる関数
  //////////////////////////////////////////////////////////////////////

  bool
  _read(
    ParserImpl* parser,    ///< [in] パーサー
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType oname_id      ///< [in] 出力名の ID 番号
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートの設定を行う．
  void
  set_gate(
    SizeType id,                       ///< [in] ID番号
    const FileRegion& loc,             ///< [in] ファイル上の位置
    PrimType gate_type,                ///< [in] ゲートの種類
    const vector<SizeType>& fanin_list ///< [in] ファンインのIDのリスト
  );

  /// @brief 複合ゲートの設定を行う．
  void
  set_complex(
    SizeType id,                       ///< [in] ID番号
    const FileRegion& loc,             ///< [in] ファイル上の位置
    const Expr& expr,                  ///< [in] 論理式
    const vector<SizeType>& fanin_list ///< [in] ファンインのIDのリスト
  );

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
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パーサー
  ParserImpl* mParser{nullptr};

};

END_NAMESPACE_YM_ISCAS89

#endif // ISCAS89HANDLER_H
