#ifndef YM_BNNETWORK_H
#define YM_BNNETWORK_H

/// @file ym/BnNetwork.h
/// @brief BnNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_bnet.h"
#include "ym/ym_cell.h"


BEGIN_NAMESPACE_YM_BNET

class BnNetworkImpl;
class BnNode;

//////////////////////////////////////////////////////////////////////
/// @class BnNetwork BnNetwork.h "ym/BnNetwork.h"
/// @ingroup BlifGroup
/// @brief blif ファイルの表しているネットワーク
/// @sa BnNode
//////////////////////////////////////////////////////////////////////
class BnNetwork
{
public:

  /// @brief コンストラクタ
  BnNetwork();

  /// @brief デストラクタ
  ~BnNetwork();


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を読み出す外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief model 名を得る．
  string
  name() const;

  /// @brief ノード数を返す．
  ymuint
  node_num() const;

  /// @brief ID 番号からノードを得る．
  /// @param[in] id ID 番号 ( 0 <= id < node_num() )
  const BnNode*
  node(ymuint id) const;

  /// @brief 外部入力数を得る．
  ymuint
  input_num() const;

  /// @brief 外部入力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
  const BnNode*
  input(ymuint pos) const;

  /// @brief 外部出力数を得る．
  ymuint
  output_num() const;

  /// @brief 外部出力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  const BnNode*
  output(ymuint pos) const;

  /// @brief ラッチ数を得る．
  ymuint
  ff_num() const;

  /// @brief ラッチを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < ff_num() )
  const BnNode*
  ff(ymuint pos) const;

  /// @brief 論理ノード数を得る．
  ymuint
  logic_num() const;

  /// @brief 論理ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
  const BnNode*
  logic(ymuint pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // blif 形式のファイルとの間で入出力を行なう関数
  //////////////////////////////////////////////////////////////////////

  /// @brief blif 形式のファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @param[in] cell_library セルライブラリ
  /// @retval true 正常に読み込めた
  /// @retval false 読み込み中にエラーが起こった．
  bool
  read_blif(const string& filename,
	    const CellLibrary* cell_library = nullptr);

  /// @brief 内容を blif 形式で出力する．
  /// @param[in] s 出力先のストリーム
  void
  write_blif(ostream& s) const;


public:
  //////////////////////////////////////////////////////////////////////
  // iscas89 形式のファイルとの間で入出力を行なう関数
  //////////////////////////////////////////////////////////////////////

  /// @brief iscas89(.bench) 形式のファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @retval true 正常に読み込めた
  /// @retval false 読み込み中にエラーが起こった．
  bool
  read_iscas89(const string& filename);

  /// @brief 内容を iscas89 形式で出力する．
  /// @param[in] s 出力先のストリーム
  void
  write_iscas89(ostream& s) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装オブジェクト
  BnNetworkImpl* mImpl;

};

END_NAMESPACE_YM_BNET

#endif // YM_BNNETWORK_H
