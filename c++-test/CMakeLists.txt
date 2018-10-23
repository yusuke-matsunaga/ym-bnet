
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${GTEST_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/ym-bnet/c++-src/bnet
  ${PROJECT_SOURCE_DIR}/ym-bnet/c++-src/blif
  ${PROJECT_SOURCE_DIR}/ym-bnet/c++-src/iscas89
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================



# ===================================================================
#  テスト用のターゲットの設定
# ===================================================================

ym_add_gtest ( bnet_FuncAnalyzer_test
  FuncAnalyzerTest.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  )

ym_add_gtest ( bnet_BnNodeEnc_test
  BnNodeEncTest.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  )

ym_add_gtest ( bnet_read_blif_test
  read_blif_test.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  DEFINITIONS
  "-DDATAPATH=\"${PROJECT_SOURCE_DIR}/ym-bnet/testdata/\""
  )

ym_add_gtest ( bnet_read_iscas89_test
  read_iscas89_test.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  DEFINITIONS
  "-DDATAPATH=\"${PROJECT_SOURCE_DIR}/ym-bnet/testdata/\""
  )

ym_add_gtest ( bnet_import_test
  import_test.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  )
