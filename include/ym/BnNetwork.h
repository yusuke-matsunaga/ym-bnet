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
/// @brief ブーリアンネットワークを表すクラス
/// @sa BnNode
/// @sa BnPort
///
/// ただし，このネットワークはラッチノードを含むので正確には
/// syncronous boolean network かもしれない．
/// 以下の種類のノードを持つ．
/// - 外部入力ノード
/// - 論理ノード
///   論理関数表現とファンインのノードを持つ．
/// - D-FFノード
///   リセット状態(値)と1つのファンインのノードを持つ．
/// すべてのノードは任意で名前を持つ．
/// 外部出力は上記いずれかのノードの出力を指す．
/// 独立のノードとしては存在しない．
///
/// 回路全体の入出力インターフェイスとしてポートを持つ．
/// ポートは複数ビットをひとまとめにしたもので名前を持つ．
/// - ポートの名前空間はノードとは別に設ける．
/// - 一つのポートに入力ノードと出力ノード(※詳細は上記参照)が混在しても良い．
/// 通常の blif ファイルや .bench(iscas89) ファイルを読んだ場合，ポートは1つのノードに対応する．
///
/// このクラスはファイル入出力用のモデルであり，このクラス上で
/// 回路変換などの処理を行うことは考えていない．
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

  /// @brief ポート数を返す．
  ymuint
  port_num() const;

  /// @brief ポートを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < port_num() )
  const BnPort*
  port(ymuint pos) const;

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

  /// @brief D-FF数を得る．
  ymuint
  dff_num() const;

  /// @brief D-FFを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < ff_num() )
  const BnNode*
  dff(ymuint pos) const;

  /// @brief 論理ノード数を得る．
  ymuint
  logic_num() const;

  /// @brief 論理ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
  const BnNode*
  logic(ymuint pos) const;

  /// @brief 論理ノードを入力側からのトポロジカル順に並べる．
  /// @param[out] node_list ノードを格納するリスト
  void
  topological_sort(vector<const BnNode*>& node_list) const;


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
