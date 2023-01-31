#ifndef MODELIMPL_H
#define MODELIMPL_H

/// @file ModelImpl.h
/// @brief ModelImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/blif_nsdef.h"
#include "ym/BlifModel.h"
#include "ym/BlifCover.h"
#include "ym/FileRegion.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
/// @class ModelImpl ModelImpl.h "ModelImpl.h"
/// @brief BlifModel の内部情報を表すクラス
//////////////////////////////////////////////////////////////////////
class ModelImpl
{
  friend class BlifParser;
  friend class CoverMgr;

private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードを表すクラス
  class Node
  {
  public:

    /// @brief コンストラクタ
    Node(
      const string& name
    ) : mName{name}
    {
    }

    /// @brief デストラクタ
    ~Node() = default;

    /// @brief 入力として定義されている時 true を返す．
    bool
    is_input() const { return mFlags[0]; }

    /// @brief .names として定義されている時 true を返す．
    bool
    is_cover() const { return mFlags[1]; }

    /// @brief .gate として定義されている時 true を返す．
    bool
    is_cell() const { return mFlags[2]; }

    /// @brief .latch として定義されている時 true を返す．
    bool
    is_dff() const { return mFlags[3]; }

    /// @brief このシンボルの名前を返す．
    const string&
    name() const { return mName; }

    /// @brief ファンイン数を返す．
    SizeType
    fanin_size() const { return mFaninList.size(); }

    /// @brief ファンイン番号のリストを返す．
    const vector<SizeType>&
    fanin_list() const { return mFaninList; }

    /// @brief カバー番号を得る．
    ///
    /// is_cover() == true の時のみ有効
    SizeType
    cover_id() const { return mExtId; }

    /// @brief セル番号を得る．
    ///
    /// is_cell() == true の時のみ有効
    SizeType
    cell_id() const { return mExtId; }

    /// @brief 入力ノード番号を得る．
    ///
    /// is_dff() == true の時のみ有効
    SizeType
    input() const { return mFaninList[0]; }

    ///
    /// is_latch() == true の時のみ有効
    /// @brief リセット値を得る．
    char
    rval() const { return static_cast<char>(mExtId); }

    /// @brief 入力として定義されたことをセットする．
    void
    set_input()
    {
      mFlags.set(1);
    }

    /// @brief .names として定義されていることをセットする．
    void
    set_cover(
      const vector<SizeType>& fanin_list,
      SizeType cover_id
    )
    {
      mFlags.set(1);
      mFaninList = fanin_list;
      mExtId = cover_id;
    }

    /// @brief .gate として定義されていることをセットする．
    void
    set_cell(
      const vector<SizeType>& fanin_list,
      SizeType cell_id
    )
    {
      mFlags.set(2);
      mFaninList = fanin_list;
      mExtId = cell_id;
    }

    /// @brief .latch として定義されていることをセットする．
    void
    set_dff(
      SizeType input_id,
      char rval
    )
    {
      mFlags.set(3);
      mFaninList = {input_id};
      mExtId = static_cast<SizeType>(rval);
    }


  private:
    //////////////////////////////////////////////////////////////////////
    // データメンバ
    //////////////////////////////////////////////////////////////////////

    // いくつかのフラグ
    // 0: input マーク
    // 1: cover マーク
    // 2: cell マーク
    // 3: dff マーク
    std::bitset<4> mFlags{0};

    // 名前
    string mName;

    // ファンインの識別子番号のリスト
    vector<SizeType> mFaninList;

    // カバー番号/セル番号
    SizeType mExtId;

  };


public:

  /// @brief コンストラクタ
  ModelImpl() = default;

  /// @brief デストラクタ
  ~ModelImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を返す．
  const string&
  name() const
  {
    return mName;
  }

  /// @brief 入力のノード番号のリストを返す．
  const vector<SizeType>&
  input_list() const
  {
    return mInputList;
  }

  /// @brief 出力のノード番号のリストを返す．
  const vector<SizeType>&
  output_list() const
  {
    return mOutputList;
  }

  /// @brief DFFのノード番号のリストを返す．
  const vector<SizeType>&
  dff_list() const
  {
    return mDffList;
  }

  /// @brief 論理ノード番号のリストを返す．
  const vector<SizeType>&
  logic_list() const
  {
    return mLogicList;
  }

  /// @brief ノード名を返す．
  const string&
  node_name(
    SizeType id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(id);
    return node.name();
  }

  /// @brief ノードの種類を返す．
  BlifType
  node_type(
    SizeType id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(id);
    if ( node.is_input() ) {
      return BlifType::Input;
    }
    if ( node.is_dff() ) {
      return BlifType::Dff;
    }
    if ( node.is_cover() ) {
      return BlifType::Cover;
    }
    if ( node.is_cell() ) {
      return BlifType::Cell;
    }
    ASSERT_NOT_REACHED;
    return BlifType::None;
  }

  /// @brief ノードのファンインのノード番号のリストを返す．
  ///
  /// node_type が Cover か Cell の時のみ意味を持つ．
  const vector<SizeType>&
  node_fanin_list(
    SizeType id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(id);
    return node.fanin_list();
  }

  /// @brief ノードのカバー番号を返す．
  ///
  /// node_type が Cover の時のみ意味を持つ．
  SizeType
  node_cover_id(
    SizeType id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(id);
    return node.cover_id();
  }

  /// @brief ノードのセル番号を返す．
  ///
  /// node_type が Cell の時のみ意味を持つ．
  SizeType
  node_cell_id(
    SizeType id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(id);
    return node.cell_id();
  }

  /// @brief ノードの入力ノード番号を返す．
  ///
  /// node_type が DFF の時のみ意味を持つ．
  SizeType
  node_input(
    SizeType id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(id);
    return node.input();
  }

  /// @brief ノードのリセット値を返す．
  ///
  /// node_type が DFF の時のみ意味を持つ．
  char
  node_rval(
    SizeType id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(id);
    return node.rval();
  }

  /// @brief カバーを取り出す．
  const BlifCover&
  cover(
    SizeType cover_id ///< [in] カバー番号
  ) const
  {
    ASSERT_COND( 0 <= cover_id && cover_id < mCoverArray.size() );
    return mCoverArray[cover_id];
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 新しいノードを作る．
  void
  new_node(
    const string& name ///< [in] 名前
  )
  {
    mNodeArray.push_back({name});
  }

  /// @brief 対応するID番号に入力用の印を付ける．
  void
  set_input(
    SizeType id ///< [in] ID番号
  )
  {
    auto& node = _node(id);
    node.set_input();
    mInputList.push_back(id);
  }

  /// @brief .names 文の情報をセットする．
  void
  set_cover(
    SizeType id,                        ///< [in] ID番号
    const vector<SizeType>& input_list, ///< [in] 入力の識別子番号のリスト
    SizeType cover_id                   ///< [in] カバー番号
  )
  {
    auto& node = _node(id);
    node.set_cover(input_list, cover_id);
  }

  /// @brief .gate 文の情報をセットする．
  void
  set_cell(
    SizeType id,                        ///< [in] ID番号
    const vector<SizeType>& input_list, ///< [in] 入力の識別子番号のリスト
    SizeType cell_id                    ///< [in] セル番号
  )
  {
    auto& node = _node(id);
    node.set_cell(input_list, cell_id);
  }

  /// @brief .latch 文の情報をセットする．
  void
  set_dff(
    SizeType id,           ///< [in] ID番号
    SizeType input_id,     ///< [in] 入力の識別子番号
    char rval              ///< [in] リセット値
  )
  {
    auto& node = _node(id);
    node.set_dff(input_id, rval);
    mDffList.push_back(id);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードを取り出す．
  Node&
  _node(
    SizeType id ///< [in] ID番号
  )
  {
    ASSERT_COND( 0 <= id && id < mNodeArray.size() );
    return mNodeArray[id];
  }

  /// @brief ノードを取り出す．
  const Node&
  _node(
    SizeType id ///< [in] ID番号
  ) const
  {
    ASSERT_COND( 0 <= id && id < mNodeArray.size() );
    return mNodeArray[id];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 名前のノード
  string mName;

  // 入力のノード番号のリスト
  vector<SizeType> mInputList;

  // 出力のノード番号のリスト
  vector<SizeType> mOutputList;

  // DFFノード番号のリスト
  vector<SizeType> mDffList;

  // 論理ノード番号のリスト
  vector<SizeType> mLogicList;

  // ノードの配列
  vector<Node> mNodeArray;

  // ID 番号をキーにしてカバーを格納する配列
  vector<BlifCover> mCoverArray;

};

END_NAMESPACE_YM_BLIF

#endif // MODELIMPL_H
