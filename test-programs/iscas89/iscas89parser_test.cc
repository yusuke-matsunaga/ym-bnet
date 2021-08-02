
/// @file parsertest.cc
/// @brief iscas89 ファイルの読み書きのテスト
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2021 Yusuke Matsunaga
/// All rights reserved.

#include "ym/Iscas89Parser.h"
#include "ym/Iscas89Handler.h"
#include "ym/MsgMgr.h"
#include "ym/StreamMsgHandler.h"


BEGIN_NAMESPACE_YM_BNET

//////////////////////////////////////////////////////////////////////
/// @class TestIscas89Handler parsertest.cc
/// @brief テスト用の Iscas89Handler
//////////////////////////////////////////////////////////////////////
class TestIscas89Handler :
  public Iscas89Handler
{
public:

  /// @brief コンストラクタ
  TestIscas89Handler(
    Iscas89Parser& parser, ///< パーサー
    ostream& stream        ///< メッセージ出力先のストリーム
  );

  /// @brief デストラクタ
  ~TestIscas89Handler();


public:

  /// @brief 初期化
  bool
  init() override;

  /// @brief INPUT 文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_input(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType name_id,      ///< [in] 入力ピン名の ID 番号
    const string& name     ///< [in] 入力ピン名
  ) override;

  /// @brief OUTPUT 文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_output(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType name_id,      ///< [in] 出力ピン名の ID 番号
    const string& name     ///< [in] 出力ピン名
  ) override;

  /// @brief ゲート文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_gate(
    const FileRegion& loc,             ///< [in] ファイル位置
    BnNodeType type,                   ///< [in] ゲートの型
    SizeType oname_id,                 ///< [in] 出力名の ID 番号
    const string& oname,               ///< [in] 出力名
    const vector<SizeType>& iname_list ///< [in] 入力名のリスト
  ) override;

  /// @brief D-FF用のゲート文を読み込む．
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_dff(
    const FileRegion& loc, ///< [in] ファイル位置
    SizeType oname_id,     ///< [in] 出力名の ID 番号
    const string& oname,   ///< [in] 出力名
    SizeType iname_id      ///< [in] 入力名の ID 番号
  ) override;

  /// @brief 通常終了時の処理
  void
  normal_exit() override;

  /// @brief エラー終了時の処理
  void
  error_exit() override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  ostream& mStream;

};


// @brief コンストラクタ
TestIscas89Handler::TestIscas89Handler(
  Iscas89Parser& parser,
  ostream& stream
) : Iscas89Handler(parser),
    mStream{stream}
{
}

// @brief デストラクタ
TestIscas89Handler::~TestIscas89Handler()
{
}

// @brief 初期化
bool
TestIscas89Handler::init()
{
  mStream << "TestIscas89Handler::init()" << endl;
  return true;
}

// @brief INPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 入力ピン名の ID 番号
// @param[in] name 出力ピン名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
TestIscas89Handler::read_input(
  const FileRegion& loc,
  SizeType name_id,
  const string& name
)
{
  mStream << "TestIscas89Handler::read_input()" << endl
	  << "    " << loc << endl;
  mStream << "  str(name_id) = " << id2str(name_id) << endl
	  << "  loc(name_id) = " << id2loc(name_id) << endl;
  return true;
}

// @brief OUTPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 出力ピン名の ID 番号
// @param[in] name 出力ピン名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
TestIscas89Handler::read_output(
  const FileRegion& loc,
  SizeType name_id,
  const string& name
)
{
  mStream << "TestIscas89Handler::read_output()" << endl
	  << "    " << loc << endl;
  mStream << "  str(name_id) = " << id2str(name_id) << endl
	  << "  loc(name_id) = " << id2loc(name_id) << endl;
  return true;
}

// @brief ゲート文を読み込む．
// @param[in] loc ファイル位置
// @param[in] type ゲートの型
// @param[in] oname_id 出力名の ID 番号
// @param[in] iname_list 入力名のリスト
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
TestIscas89Handler::read_gate(
  const FileRegion& loc,
  BnNodeType type,
  SizeType oname_id,
  const string& oname,
  const vector<SizeType>& iname_list
)
{
  mStream << "TestIscas89Handler::read_gate()" << endl
	  << "    " << loc << endl;
  mStream << "  str(oname_id) = " << oname << endl
	  << "  loc(oname_id) = " << id2loc(oname_id) << endl;
  mStream << "  type = ";
  switch ( type ) {
  case BnNodeType::Buff: mStream << "BUFF"; break;
  case BnNodeType::Not:  mStream << "NOT"; break;
  case BnNodeType::And:  mStream << "AND"; break;
  case BnNodeType::Nand: mStream << "NAND"; break;
  case BnNodeType::Or:   mStream << "OR"; break;
  case BnNodeType::Nor:  mStream << "NOR"; break;
  case BnNodeType::Xor:  mStream << "XOR"; break;
  case BnNodeType::Xnor: mStream << "XNOR"; break;
  default: ASSERT_NOT_REACHED; break;
  }
  mStream << endl;
  int i = 0;
  for ( auto id: iname_list ) {
    mStream << "  str(iname_list[" << i << "]) = " << id2str(id) << endl
	    << "  loc(iname_list[" << i << "]) = " << id2loc(id) << endl;
    ++ i;
  }
  return true;
}

// @brief D-FF用のゲート文を読み込む．
// @param[in] loc ファイル位置
// @param[in] oname_id 出力名の ID 番号
// @param[in] iname_list 入力名のリスト
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
TestIscas89Handler::read_dff(
  const FileRegion& loc,
  SizeType oname_id,
  const string& oname,
  SizeType iname_id
)
{
  mStream << "TestIscas89Handler::read_dff()" << endl
	  << "    " << loc << endl;
  mStream << "  str(oname_id) = " << oname << endl
	  << "  loc(oname_id) = " << id2loc(oname_id) << endl;
  mStream << "  type = DFF";
  mStream << endl;
  mStream << "  str(iname_id) = " << id2str(iname_id) << endl
	  << "  loc(iname_id) = " << id2loc(iname_id) << endl;
  return true;
}

// @brief 通常終了時の処理
void
TestIscas89Handler::normal_exit()
{
  mStream << "TestIscas89Handler::normal_exit()" << endl;
}

// @brief エラー終了時の処理
void
TestIscas89Handler::error_exit()
{
  mStream << "TestIscas89Handler::error_exit()" << endl;
}

END_NAMESPACE_YM_BNET


int
main(
  int argc,
  char** argv
)
{
  using namespace std;
  using namespace nsYm;
  using namespace nsYm::nsBnet;

  if ( argc != 2 ) {
    cerr << "USAGE : " << argv[0] << " iscas89-file" << endl;
    return 2;
  }
  string filename = argv[1];

  try {
    Iscas89Parser parser;
    TestIscas89Handler handler(parser, cout);

    StreamMsgHandler msg_handler(cerr);
    MsgMgr::attach_handler(&msg_handler);

    if ( !parser.read(filename) ) {
      cerr << "Error in reading " << filename << endl;
      return 4;
    }
  }
  catch ( const AssertError& x) {
    cout << x << endl;
  }

  return 0;
}
