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
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/ym-common/include
  ${PROJECT_SOURCE_DIR}/ym-base/include
  ${PROJECT_SOURCE_DIR}/ym-logic/include
  ${PROJECT_SOURCE_DIR}/ym-cell/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/private_include
  )


# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( tests/gtest )
add_subdirectory ( tests/programs )
#add_subdirectory ( tests/py-test )


# ===================================================================
#  ソースの設定
# ===================================================================

set ( blif_SOURCES
  c++-src/blif/BlifCoverMgr.cc
  c++-src/blif/BlifHandler.cc
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
  c++-src/bnet/WriterBase.cc
  c++-src/bnet/BlifWriter.cc
  c++-src/bnet/Iscas89Writer.cc
  c++-src/bnet/VerilogWriter.cc
  c++-src/bnet/read_truth.cc
  c++-src/bnet/read_aig.cc
  )

set ( iscas89_SOURCES
  c++-src/iscas89/Iscas89Handler.cc
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
