
/// @file BnNetwork.cc
/// @brief BnNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/BnNetwork.h"
#include "BnNetworkImpl.h"
#include "ym/BlifParser.h"
#include "ym/Iscas89Parser.h"

#include "BlifBnNetworkHandler.h"
#include "Iscas89BnNetworkHandler.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
// クラス BnNetwork
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BnNetwork::BnNetwork() :
  mImpl(new BnNetworkImpl)
{
}

// @brief デストラクタ
BnNetwork::~BnNetwork()
{
  delete mImpl;
}

// @brief model 名を得る．
string
BnNetwork::name() const
{
  return mImpl->name();
}

// @brief ノード数を得る．
ymuint
BnNetwork::node_num() const
{
  return mImpl->node_num();
}

// @brief ID 番号からノードを得る．
// @param[in] id ID 番号 ( 0 <= id < node_num() )
const BnNode*
BnNetwork::node(ymuint id) const
{
  return mImpl->node(id);
}

// @brief 外部入力数を得る．
ymuint
BnNetwork::input_num() const
{
  return mImpl->input_num();
}

// @brief 外部入力を得る．
// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
const BnNode*
BnNetwork::input(ymuint pos) const
{
  return mImpl->input(pos);
}

// @brief 外部出力数を得る．
ymuint
BnNetwork::output_num() const
{
  return mImpl->output_num();
}

// @brief 外部出力を得る．
// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
const BnNode*
BnNetwork::output(ymuint pos) const
{
  return mImpl->output(pos);
}

// @brief D-FF数を得る．
ymuint
BnNetwork::dff_num() const
{
  return mImpl->dff_num();
}

// @brief D-FFを得る．
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
const BnNode*
BnNetwork::dff(ymuint pos) const
{
  return mImpl->dff(pos);
}

// @brief 論理ノード数を得る．
ymuint
BnNetwork::logic_num() const
{
  return mImpl->logic_num();
}

// @brief 論理ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < logic_num() )
const BnNode*
BnNetwork::logic(ymuint pos) const
{
  return mImpl->logic(pos);
}

BEGIN_NONAMESPACE

// キューに積む．
void
put_queue(vector<const BnNode*>& queue,
	  ymuint& wpos,
	  vector<bool>& mark,
	  const BnNode* node)
{
  if ( !mark[node->id()] ) {
    mark[node->id()] = true;
    queue[wpos] = node;
    ++ wpos;
  }
}

END_NONAMESPACE

// @brief ノードを入力側からのトポロジカル順に並べる．
// @param[out] node_list ノードを格納するリスト
void
BnNetwork::topological_sort(vector<const BnNode*>& node_list) const
{
  // キュー
  vector<const BnNode*> queue(node_num());
  ymuint wpos = 0;
  // キューに入っているかを示すフラグの配列
  // node->id() をキーにする．
  vector<bool> mark(node_num(), false);

  // 外部入力をキューに積む．
  for (ymuint i = 0; i < input_num(); ++ i) {
    const BnNode* node = input(i);
    put_queue(queue, wpos, mark, node);
  }
  // DFFをキューに積む．
  for (ymuint i = 0; i < dff_num(); ++ i) {
    const BnNode* node = dff(i);
    put_queue(queue, wpos, mark, node);
  }

  node_list.clear();
  node_list.reserve(node_num());
  // キューが空になるまで処理を繰り返す．
  for (ymuint rpos = 0; rpos < wpos; ++ rpos) {
    const BnNode* node = queue[rpos];
    if ( node->is_logic() ) {
      node_list.push_back(node);
    }
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const BnNode* onode = mImpl->node(node->fanout_id(i));
      if ( onode->is_dff() || onode->is_output() ) {
	continue;
      }
      ymuint ni = onode->fanin_num();
      bool ready = true;
      for (ymuint j = 0; j < ni; ++ j) {
	ymuint iid = onode->fanin_id(j);
	if ( !mark[iid] ) {
	  ready = false;
	  break;
	}
      }
      if ( ready ) {
	put_queue(queue, wpos, mark, onode);
      }
    }
  }
}

// @brief blif 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @retval true 正常に読み込めた
// @retval false 読み込み中にエラーが起こった．
bool
BnNetwork::read_blif(const string& filename,
		     const CellLibrary* cell_library)
{
  BlifParser parser;
  BlifBnNetworkHandler* handler = new BlifBnNetworkHandler(mImpl);
  parser.add_handler(handler);

  bool stat = parser.read(filename, cell_library);

  // handler は parser が解放してくれる．

  if ( stat ) {
    mImpl->wrap_up();
  }

  return stat;
}

// @brief 内容を blif 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnNetwork::write_blif(ostream& s) const
{
  mImpl->write_blif(s);
}

// @brief iscas89(.bench) 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @retval true 正常に読み込めた
// @retval false 読み込み中にエラーが起こった．
bool
BnNetwork::read_iscas89(const string& filename)
{
  Iscas89Parser parser;
  Iscas89BnNetworkHandler* handler = new Iscas89BnNetworkHandler(mImpl);
  parser.add_handler(handler);

  bool stat = parser.read(filename);

  // handler は parser が解放してくれる．

  if ( stat ) {
    mImpl->wrap_up();
  }

  return stat;
}

// @brief 内容を iscas89 形式で出力する．
// @param[in] s 出力先のストリーム
void
BnNetwork::write_iscas89(ostream& s) const
{
}

END_NAMESPACE_YM_BNET
