# ===================================================================
# CMAKE のおまじない
# ===================================================================


# ===================================================================
# プロジェクト名，バージョンの設定
# ===================================================================


# ===================================================================
# オプション
# ===================================================================


# ===================================================================
# パッケージの検査
# ===================================================================


# ===================================================================
# ヘッダファイルの生成
# ===================================================================


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/private_include
  )


# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( c++-test )
add_subdirectory ( py-test )
add_subdirectory ( test-programs )


# ===================================================================
#  ソースの設定
# ===================================================================

set ( blif_SOURCES
  c++-src/blif/BlifCover.cc
  c++-src/blif/BlifCoverMgr.cc
  c++-src/blif/BlifDic.cc
  c++-src/blif/BlifHandler.cc
  c++-src/blif/BlifIdHash.cc
  c++-src/blif/BlifParserImpl.cc
  c++-src/blif/BlifScanner.cc
  )

set ( bnet_SOURCES
  c++-src/bnet/BnBlifHandler.cc
  c++-src/bnet/BnDffImpl.cc
  c++-src/bnet/BnIscas89Handler.cc
  c++-src/bnet/BnLatchImpl.cc
  c++-src/bnet/BnNetwork.cc
  c++-src/bnet/BnNetworkImpl.cc
  c++-src/bnet/BnNodeImpl.cc
  c++-src/bnet/BnInputNode.cc
  c++-src/bnet/BnOutputNode.cc
  c++-src/bnet/BnLogicNode.cc
  c++-src/bnet/BnPortImpl.cc
  c++-src/bnet/FuncAnalyzer.cc
  c++-src/bnet/read_blif.cc
  c++-src/bnet/read_iscas89.cc
  c++-src/bnet/write_blif.cc
  c++-src/bnet/BnNodeEnc.cc
  )

set ( iscas89_SOURCES
  c++-src/iscas89/Iscas89Handler.cc
  c++-src/iscas89/Iscas89IdHash.cc
  c++-src/iscas89/Iscas89Parser.cc
  c++-src/iscas89/Iscas89ParserImpl.cc
  c++-src/iscas89/Iscas89Scanner.cc
  )

set ( ym_bnet_SOURCES
  ${blif_SOURCES}
  ${bnet_SOURCES}
  ${iscas89_SOURCES}
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================

ym_add_object_library ( ym_bnet
  ${ym_bnet_SOURCES}
  )
