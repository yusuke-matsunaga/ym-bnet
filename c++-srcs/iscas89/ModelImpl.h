#ifndef MODELIMPL_H
#define MODELIMPL_H

/// @file ModelImpl.h
/// @brief ModelImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/iscas89_nsdef.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
/// @class ModelImpl ModelImpl.h "ModelImpl.h"
/// @brief Iscas89Model の実装クラス
//////////////////////////////////////////////////////////////////////
class ModelImpl
{
  friend class ParserImpl;

public:

  /// @brief コンストラクタ
  ModelImpl() = default;

  /// @brief デストラクタ
  ~ModelImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

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

  /// @brief 論理ノードのノード番号のリストを返す．
  const vector<SizeType>&
  gate_list() const
  {
    return mGateList;
  }

  /// @brief 論理式のリストを返す．
  const vector<Expr>&
  expr_list() const
  {
    return mExprList;
  }

  /// @brief ノード名を返す．
  const string&
  node_name(
    SizeType node_id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(node_id);
    return node.name();
  }

  /// @brief ノードの種類を返す．
  Iscas89Type
  node_type(
    SizeType node_id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(node_id);
    if ( node.is_input() ) {
      return Iscas89Type::Input;
    }
    if ( node.is_gate() ) {
      return Iscas89Type::Gate;
    }
    if ( node.is_complex() ) {
      return Iscas89Type::Complex;
    }
    if ( node.is_dff() ) {
      return Iscas89Type::Dff;
    }
    ASSERT_NOT_REACHED;
    return Iscas89Type::None;
  }

  /// @brief ノードのファンインのノード番号のリストを返す．
  ///
  /// node_type が Gate の時のみ意味を持つ．
  const vector<SizeType>&
  node_fanin_list(
    SizeType node_id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(node_id);
    return node.fanin_list();
  }

  /// @brief ノードのファンイン数を返す．
  ///
  /// node_type が Gate の時のみ意味を持つ．
  SizeType
  node_fanin_num(
    SizeType node_id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(node_id);
    return node.fanin_num();
  }

  /// @brief 論理ノードの種類を返す．
  ///
  /// node_type が Gate の時のみ意味を持つ．
  PrimType
  node_gate_type(
    SizeType node_id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(node_id);
    return node.gate_type();
  }

  /// @brief 論理式番号を返す．
  ///
  /// node_type が Complex の時のみ意味を持つ．
  SizeType
  node_expr_id(
    SizeType node_id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(node_id);
    return node.expr_id();
  }

  /// @brief 論理式を返す．
  ///
  /// node_type が Complex の時のみ意味を持つ．
  Expr
  node_expr(
    SizeType node_id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(node_id);
    SizeType id = node.expr_id();
    return mExprList[id];
  }

  /// @brief ノードの入力ノード番号を返す．
  ///
  /// node_type が DFF の時のみ意味を持つ．
  SizeType
  node_input(
    SizeType node_id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(node_id);
    return node.input();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief 識別子に対応するノードの情報
  class Node
  {
  public:

    /// @brief コンストラクタ
    Node(
      const string& name ///< [in] 名前
    ) : mFlags{0},
	mName{name}
    {
    }

    /// @brief デストラクタ
    ~Node() = default;

    /// @brief このシンボルの名前を返す．
    const string&
    name() const { return mName; }

    /// @brief 入力の時 true を返す．
    bool
    is_input() const { return mFlags[1]; }

    /// @brief 論理ノードの時 true を返す．
    bool
    is_gate() const { return mFlags[2]; }

    /// @brief 複合ゲートの時 true を返す．
    bool
    is_complex() const { return mFlags[3]; }

    /// @brief DFFの時 true を返す．
    bool
    is_dff() const { return mFlags[4]; }

    /// @brief ゲートの種類を返す．
    PrimType
    gate_type() const { return static_cast<PrimType>(mType); }

    /// @brief 論理式番号を返す．
    SizeType
    expr_id() const { return mType; }

    /// @brief ファンインのノード番号のリストを返す．
    SizeType
    fanin_num() const { return mInodeList.size(); }

    /// @brief ファンインのノード番号のリストを返す．
    const vector<SizeType>&
    fanin_list() const { return mInodeList; }

    /// @brief 入力のノード番号のリストを返す．
    SizeType
    input() const { return mInodeList[0]; }

    /// @brief 入力にセットする．
    void
    set_input()
    {
      mFlags.set(1);
    }

    /// @brief 論理ゲートにセットする．
    void
    set_gate(
      PrimType gate_type,
      const vector<SizeType>& inode_list
    )
    {
      mFlags.set(2);
      mType = static_cast<SizeType>(gate_type);
      mInodeList = inode_list;
    }

    /// @brief 複合ゲートにセットする．
    void
    set_complex(
      SizeType expr_id,
      const vector<SizeType>& inode_list
    )
    {
      mFlags.set(3);
      mType = expr_id;
      mInodeList = inode_list;
    }

    /// @brief DFFにセットする．
    void
    set_dff(
      SizeType inode
    )
    {
      mFlags.set(4);
      mInodeList = {inode};
    }


  private:
    //////////////////////////////////////////////////////////////////////
    // データメンバ
    //////////////////////////////////////////////////////////////////////

    // 名前
    string mName;

    // いくつかのフラグ
    // 1: 入力
    // 2: 論理ゲート
    // 3: 複合ゲート
    // 4: DFF
    std::bitset<5> mFlags;

    // 論理ノードの種類/論理式番号
    SizeType mType;

    // 入力ノード番号のリスト
    vector<SizeType> mInodeList;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 新しいノード番号を得る．
  void
  new_node(
    const string& name
  )
  {
    mNodeArray.push_back({name});
  }

  /// @brief 入力ノードの設定を行う．
  void
  set_input(
    SizeType id
  )
  {
    auto& node = _node(id);
    node.set_input();
    mInputList.push_back(id);
  }

  /// @brief 論理ノードの設定を行う．
  void
  set_gate(
    SizeType id,
    PrimType gate_type,
    const vector<SizeType>& inode_list
  )
  {
    auto& node = _node(id);
    node.set_gate(gate_type, inode_list);
  }

  /// @brief 複合ゲートの設定を行う．
  void
  set_complex(
    SizeType id,
    SizeType expr_id,
    const vector<SizeType>& inode_list
  )
  {
    auto& node = _node(id);
    node.set_complex(expr_id, inode_list);
  }

  /// @brief DFF の設定を行う．
  void
  set_dff(
    SizeType id,
    SizeType inode
  )
  {
    auto& node = _node(id);
    node.set_dff(inode);
    mDffList.push_back(id);
  }

  /// @brief 論理式を追加する．
  /// @return 論理式番号を返す．
  SizeType
  add_expr(
    const Expr& expr
  )
  {
    SizeType id = mExprList.size();
    mExprList.push_back(expr);
    return id;
  }

  Node&
  _node(
    SizeType id
  )
  {
    ASSERT_COND( 0 <= id && id < mNodeArray.size() );
    return mNodeArray[id];
  }

  const Node&
  _node(
    SizeType id
  ) const
  {
    ASSERT_COND( 0 <= id && id < mNodeArray.size() );
    return mNodeArray[id];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力のノード番号のリスト
  vector<SizeType> mInputList;

  // 出力のノード番号のリスト
  vector<SizeType> mOutputList;

  // DFFのノード番号のリスト
  vector<SizeType> mDffList;

  // 論理ノードのノード番号のリスト
  vector<SizeType> mGateList;

  // 論理式の配列
  vector<Expr> mExprList;

  // ノードの配列
  vector<Node> mNodeArray;

};

END_NAMESPACE_YM_ISCAS89

#endif // MODELIMPL_H
