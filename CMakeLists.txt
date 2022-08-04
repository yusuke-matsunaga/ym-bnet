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
ym_init_lua()


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
  c++-srcs/blif/BlifCoverMgr.cc
  c++-srcs/blif/BlifHandler.cc
  c++-srcs/blif/BlifParserImpl.cc
  c++-srcs/blif/BlifScanner.cc
  )

set ( bnet_SOURCES
  c++-srcs/bnet/AigReader.cc
  c++-srcs/bnet/AigWriter.cc
  c++-srcs/bnet/Bnet2Aig.cc
  c++-srcs/bnet/BnetWriter.cc
  c++-srcs/bnet/BnBlifHandler.cc
  c++-srcs/bnet/BnDffImpl.cc
  c++-srcs/bnet/BnIscas89Handler.cc
  c++-srcs/bnet/BnLatchImpl.cc
  c++-srcs/bnet/BnNetwork.cc
  c++-srcs/bnet/BnNetworkImpl.cc
  c++-srcs/bnet/BnNodeImpl.cc
  c++-srcs/bnet/BnInputNode.cc
  c++-srcs/bnet/BnOutputNode.cc
  c++-srcs/bnet/BnLogicNode.cc
  c++-srcs/bnet/BnPortImpl.cc
  c++-srcs/bnet/FuncAnalyzer.cc
  c++-srcs/bnet/WriterBase.cc
  c++-srcs/bnet/BlifWriter.cc
  c++-srcs/bnet/Iscas89Writer.cc
  c++-srcs/bnet/VerilogWriter.cc
  c++-srcs/bnet/read_truth.cc
  c++-srcs/bnet/read_aig.cc
  )

set ( iscas89_SOURCES
  c++-srcs/iscas89/Iscas89Handler.cc
  c++-srcs/iscas89/Iscas89Parser.cc
  c++-srcs/iscas89/Iscas89ParserImpl.cc
  c++-srcs/iscas89/Iscas89Scanner.cc
  )

set ( ym_bnet_SOURCES
  ${blif_SOURCES}
  ${bnet_SOURCES}
  ${iscas89_SOURCES}
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

if ( LUA_FOUND )
  ym_add_object_library ( ym_luabnet
    ${ym_luabnet_SOURCES}
    )

  target_include_directories ( ym_luabnet_obj
    PRIVATE ${LUA_INCLUDE_DIR}
    )

  target_include_directories ( ym_luabnet_obj_d
    PRIVATE ${LUA_INCLUDE_DIR}
    )

  target_include_directories ( ym_luabnet_obj_p
    PRIVATE ${LUA_INCLUDE_DIR}
    )
endif ( LUA_FOUND )