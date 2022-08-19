
/// @file read_truth.cc
/// @brief read_truth の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BddMgr.h"


BEGIN_NAMESPACE_YM_BNET

// @brief .truth 形式のファイルを読み込む．
// @return ネットワークを返す．
BnNetwork
BnNetwork::read_truth(
  const string& filename ///< [in] ファイル名
)
{
  ifstream s{filename};
  if ( !s ) {
    throw BnetError{"Error in read_truth"};
  }

  BddMgr mgr;
  vector<Bdd> func_vect;
  string line;
  SizeType ni_exp = 0;
  while ( getline(s, line) ) {
    auto bdd = mgr.from_truth(line);
    func_vect.push_back(bdd);
    ni_exp = line.size();
  }

  SizeType ni = 0;
  while ( (1 << ni) < ni_exp ) {
    ++ ni;
  }
  ASSERT_COND( (1 << ni) == ni_exp );

  BnNetwork network;
  SizeType no = func_vect.size();
  if ( no > 0 ) {
    // 入力ポートの生成
    vector<SizeType> input_list(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      ostringstream buf;
      buf << "i" << i;
      auto port_id = network.new_input_port(buf.str());
      const auto& port = network.port(port_id);
      input_list[i] = port.bit(0);
    }
    // 出力ポートの生成
    vector<SizeType> output_list(no);
    for ( SizeType i = 0; i < no; ++ i ) {
      ostringstream buf;
      buf << "o" << i;
      auto port_id = network.new_output_port(buf.str());
      const auto& port = network.port(port_id);
      output_list[i] = port.bit(0);
    }
    // 論理ノードの生成
    // 注意が必要なのは .truth フォーマットでは最上位の変数が
    // 最後の変数だということ．
    // ファンインリストは input_list の逆順になる．
    vector<SizeType> fanin_id_list(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      fanin_id_list[i] = input_list[ni - i - 1];
    }
    for ( SizeType i = 0; i < no; ++ i ) {
      ostringstream buf;
      buf << "l" << i;
      auto node_id = network.new_logic(buf.str(), func_vect[i],
				       fanin_id_list);
      network.set_output(output_list[i], node_id);
    }
    if ( !network.wrap_up() ) {
      throw BnetError{"Error in read_truth"};
    }
  }
  return network;
}

END_NAMESPACE_YM_BNET
