#ifndef MODELIMPL_H
#define MODELIMPL_H

/// @file ModelImpl.h
/// @brief ModelImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/nvl_nsdef.h"


BEGIN_NAMESPACE_YM_NVL

//////////////////////////////////////////////////////////////////////
/// @class ModelImpl ModelImpl.h "ModelImpl.h"
/// @brief NvlModel の実装クラス
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

  /// @brief 論理ノードのノード番号のリストを返す．
  const vector<SizeType>&
  gate_list() const
  {
    return mGateList;
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
  NvlType
  node_type(
    SizeType node_id ///< [in] ノード番号
  ) const
  {
    auto& node = _node(node_id);
    if ( node.is_input() ) {
      return NvlType::Input;
    }
    if ( node.is_gate() ) {
      return NvlType::Gate;
    }
    ASSERT_NOT_REACHED;
    return NvlType::None;
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

    /// @brief ゲートの種類を返す．
    PrimType
    gate_type() const { return static_cast<PrimType>(mType); }

    /// @brief ファンインのノード番号のリストを返す．
    SizeType
    fanin_num() const { return mInodeList.size(); }

    /// @brief ファンインのノード番号のリストを返す．
    const vector<SizeType>&
    fanin_list() const { return mInodeList; }

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


  private:
    //////////////////////////////////////////////////////////////////////
    // データメンバ
    //////////////////////////////////////////////////////////////////////

    // 名前
    string mName;

    // いくつかのフラグ
    // 1: 入力
    // 2: 論理ゲート
    std::bitset<3> mFlags;

    // 論理ノードの種類
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

  // 論理ノードのノード番号のリスト
  vector<SizeType> mGateList;

  // ノードの配列
  vector<Node> mNodeArray;

};

END_NAMESPACE_YM_NVL

#endif // MODELIMPL_H
