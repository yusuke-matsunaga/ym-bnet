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

if ( Python3_FOUND )
  add_subdirectory ( py_ymbnet )
endif ( Python3_FOUND )
add_subdirectory ( tests/gtest )
add_subdirectory ( tests/programs )


# ===================================================================
#  ソースの設定
# ===================================================================

set ( aig_SOURCES
  c++-srcs/aig/Aig2Bnet.cc
  c++-srcs/aig/AigModel.cc
  c++-srcs/aig/ModelImpl.cc
  )

set ( bnet_SOURCES
  c++-srcs/bnet/BinIO.cc
  c++-srcs/bnet/BnDff.cc
  c++-srcs/bnet/BnDffImpl.cc
  c++-srcs/bnet/BnModifier.cc
  c++-srcs/bnet/BnNetwork.cc
  c++-srcs/bnet/BnNetworkImpl.cc
  c++-srcs/bnet/BnNetworkImpl_copy.cc
  c++-srcs/bnet/BnNode.cc
  c++-srcs/bnet/BnNodeImpl.cc
  c++-srcs/bnet/BnInputNode.cc
  c++-srcs/bnet/BnOutputNode.cc
  c++-srcs/bnet/BnLogicNode.cc
  c++-srcs/bnet/BnPort.cc
  c++-srcs/bnet/BnPortImpl.cc
  c++-srcs/bnet/ReadTruth.cc
  c++-srcs/bnet/SimpleDecomp.cc
  c++-srcs/bnet/OutputSplit.cc
  )

set ( blif_SOURCES
  c++-srcs/blif/Blif2Bnet.cc
  c++-srcs/blif/BlifModel.cc
  c++-srcs/blif/BlifParser.cc
  c++-srcs/blif/BlifScanner.cc
  c++-srcs/blif/CoverMgr.cc
  c++-srcs/blif/ModelImpl.cc
  )

set ( iscas89_SOURCES
  c++-srcs/iscas89/Bench2Bnet.cc
  c++-srcs/iscas89/Iscas89Handler.cc
  c++-srcs/iscas89/Iscas89Model.cc
  c++-srcs/iscas89/Iscas89Parser.cc
  c++-srcs/iscas89/Iscas89Scanner.cc
  c++-srcs/iscas89/ParserImpl.cc
  c++-srcs/iscas89/Iscas89ExParser.cc
  c++-srcs/iscas89/MuxHandler.cc
  c++-srcs/iscas89/C0Handler.cc
  c++-srcs/iscas89/C1Handler.cc
  )

set ( writer_SOURCES
  c++-srcs/writer/AigWriter.cc
  c++-srcs/writer/BlifWriter.cc
  c++-srcs/writer/Bnet2Aig.cc
  c++-srcs/writer/BnNetwork_write.cc
  c++-srcs/writer/Iscas89Writer.cc
  c++-srcs/writer/VerilogWriter.cc
  c++-srcs/writer/WriterBase.cc
  )

set ( ym_bnet_SOURCES
  ${aig_SOURCES}
  ${blif_SOURCES}
  ${bnet_SOURCES}
  ${iscas89_SOURCES}
  ${writer_SOURCES}
  )

set ( ym_luabnet_SOURCES
  c++-srcs/lua/LuaBnet.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================

ym_add_object_library ( ym_bnet
  ${ym_bnet_SOURCES}
  )
