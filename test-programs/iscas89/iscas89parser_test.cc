
/// @file parsertest.cc
/// @brief iscas89 ファイルの読み書きのテスト
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
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
  /// @param[in] parser パーサー
  TestIscas89Handler(Iscas89Parser& parser,
		     ostream* stream);

  /// @brief デストラクタ
  ~TestIscas89Handler();


public:

  /// @brief 初期化
  bool
  init() override;

  /// @brief INPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 入力ピン名の ID 番号
  /// @param[in] name 入力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_input(const FileRegion& loc,
	     int name_id,
	     const string& name) override;

  /// @brief OUTPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 出力ピン名の ID 番号
  /// @param[in] name 出力ピン名
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_output(const FileRegion& loc,
	      int name_id,
	      const string& name) override;

  /// @brief ゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] type ゲートの型
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] iname_list 入力名のリスト
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_gate(const FileRegion& loc,
	    BnNodeType type,
	    int oname_id,
	    const string& oname,
	    const vector<int>& iname_list) override;

  /// @brief D-FF用のゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] oname 出力名
  /// @param[in] iname_id 入力名の ID 番号
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  bool
  read_dff(const FileRegion& loc,
	   int oname_id,
	   const string& oname,
	   int iname_id) override;

  /// @brief 通常終了時の処理
  void
  normal_exit() override;

  /// @brief エラー終了時の処理
  void
  error_exit() override;


private:

  ostream* mStreamPtr;

};


// @brief コンストラクタ
TestIscas89Handler::TestIscas89Handler(Iscas89Parser& parser,
				       ostream* streamptr) :
  Iscas89Handler(parser),
  mStreamPtr(streamptr)
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
  (*mStreamPtr) << "TestIscas89Handler::init()" << endl;
  return true;
}

// @brief INPUT 文を読み込む．
// @param[in] loc ファイル位置
// @param[in] name_id 入力ピン名の ID 番号
// @param[in] name 出力ピン名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
TestIscas89Handler::read_input(const FileRegion& loc,
			       int name_id,
			       const string& name)
{
  (*mStreamPtr) << "TestIscas89Handler::read_input()" << endl
		<< "    " << loc << endl;
  (*mStreamPtr) << "  str(name_id) = " << id2str(name_id) << endl
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
TestIscas89Handler::read_output(const FileRegion& loc,
				int name_id,
				const string& name)
{
  (*mStreamPtr) << "TestIscas89Handler::read_output()" << endl
		<< "    " << loc << endl;
  (*mStreamPtr) << "  str(name_id) = " << id2str(name_id) << endl
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
TestIscas89Handler::read_gate(const FileRegion& loc,
			      BnNodeType type,
			      int oname_id,
			      const string& oname,
			      const vector<int>& iname_list)
{
  (*mStreamPtr) << "TestIscas89Handler::read_gate()" << endl
		<< "    " << loc << endl;
  (*mStreamPtr) << "  str(oname_id) = " << oname << endl
		<< "  loc(oname_id) = " << id2loc(oname_id) << endl;
  (*mStreamPtr) << "  type = ";
  switch ( type ) {
  case BnNodeType::Buff: (*mStreamPtr) << "BUFF"; break;
  case BnNodeType::Not:  (*mStreamPtr) << "NOT"; break;
  case BnNodeType::And:  (*mStreamPtr) << "AND"; break;
  case BnNodeType::Nand: (*mStreamPtr) << "NAND"; break;
  case BnNodeType::Or:   (*mStreamPtr) << "OR"; break;
  case BnNodeType::Nor:  (*mStreamPtr) << "NOR"; break;
  case BnNodeType::Xor:  (*mStreamPtr) << "XOR"; break;
  case BnNodeType::Xnor: (*mStreamPtr) << "XNOR"; break;
  default: ASSERT_NOT_REACHED; break;
  }
  (*mStreamPtr) << endl;
  size_t i = 0;
  for (vector<int>::const_iterator p = iname_list.begin();
       p != iname_list.end(); ++ p, ++ i) {
    int id = *p;
    (*mStreamPtr) << "  str(iname_list[" << i << "]) = " << id2str(id) << endl
		  << "  loc(iname_list[" << i << "]) = " << id2loc(id) << endl;
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
TestIscas89Handler::read_dff(const FileRegion& loc,
			     int oname_id,
			     const string& oname,
			     int iname_id)
{
  (*mStreamPtr) << "TestIscas89Handler::read_dff()" << endl
		<< "    " << loc << endl;
  (*mStreamPtr) << "  str(oname_id) = " << oname << endl
		<< "  loc(oname_id) = " << id2loc(oname_id) << endl;
  (*mStreamPtr) << "  type = DFF";
  (*mStreamPtr) << endl;
  (*mStreamPtr) << "  str(iname_id) = " << id2str(iname_id) << endl
		<< "  loc(iname_id) = " << id2loc(iname_id) << endl;
  return true;
}

// @brief 通常終了時の処理
void
TestIscas89Handler::normal_exit()
{
  (*mStreamPtr) << "TestIscas89Handler::normal_exit()" << endl;
}

// @brief エラー終了時の処理
void
TestIscas89Handler::error_exit()
{
  (*mStreamPtr) << "TestIscas89Handler::error_exit()" << endl;
}

END_NAMESPACE_YM_BNET


int
main(int argc,
     char** argv)
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
    TestIscas89Handler handler(parser, &cout);

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
