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

#add_subdirectory ( cython-src )


# ===================================================================
#  ソースの設定
# ===================================================================

set ( blif_SOURCES
  src/blif/BlifBnNetworkHandler.cc
  src/blif/BlifCover.cc
  src/blif/BlifCoverMgr.cc
  src/blif/BlifDic.cc
  src/blif/BlifHandler.cc
  src/blif/BlifIdHash.cc
  src/blif/BlifParserImpl.cc
  src/blif/BlifScanner.cc
  )

set ( bnet_SOURCES
  src/bnet/BnFuncTypeImpl.cc
  src/bnet/BnFuncTypeMgr.cc
  src/bnet/BnNetwork.cc
  src/bnet/BnNetworkImpl.cc
  src/bnet/BnNodeImpl.cc
  )

set ( iscas89_SOURCES
  src/iscas89/Iscas89BnNetworkHandler.cc
  src/iscas89/Iscas89Handler.cc
  src/iscas89/Iscas89IdHash.cc
  src/iscas89/Iscas89Parser.cc
  src/iscas89/Iscas89ParserImpl.cc
  src/iscas89/Iscas89Scanner.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================
ym_add_library ( ym_bnet
  SHARED
  ${blif_SOURCES}
  ${bnet_SOURCES}
  ${iscas89_SOURCES}
  DESTINATION ${YM_INSTALL_LIBDIR}
  )

target_link_libraries ( ym_bnet
  ym_logic
  ym_cell
  ym_common
  )

target_link_libraries ( ym_bnet_d
  ym_logic_d
  ym_cell_d
  ym_common_d
  )

target_link_libraries ( ym_bnet_p
  ym_logic_p
  ym_cell_p
  ym_common_p
  )