
/// @file read_truth.cc
/// @brief read_truth の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/BnNetwork.h"
#include "ym/BnPort.h"


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

  vector<TvFunc> func_vect;
  string line;
  while ( getline(s, line) ) {
    func_vect.push_back(TvFunc{line});
  }

  BnNetwork network;
  SizeType no = func_vect.size();
  if ( no > 0 ) {
    SizeType ni = func_vect[0].input_num();
    for ( SizeType i = 1; i < no; ++ i ) {
      if ( func_vect[i].input_num() != ni ) {
	// TODO: マシなメッセージを作る．
	throw BnetError{"Error in read_truth"};
      }
    }
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
    for ( SizeType i = 0; i < no; ++ i ) {
      ostringstream buf;
      buf << "l" << i;
      auto node_id = network.new_logic(buf.str(), func_vect[i],
				       input_list);
      network.connect(node_id, output_list[i], 0);
    }
    if ( !network.wrap_up() ) {
      throw BnetError{"Error in read_truth"};
    }
  }
  return network;
}

END_NAMESPACE_YM_BNET
