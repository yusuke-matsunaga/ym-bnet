
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
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  )

ym_add_gtest ( bnet_read_blif_test
  read_blif_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  DEFINITIONS
  "-DDATAPATH=\"${PROJECT_SOURCE_DIR}/ym-bnet/testdata/\""
  )

ym_add_gtest ( bnet_read_iscas89_test
  read_iscas89_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  DEFINITIONS
  "-DDATAPATH=\"${PROJECT_SOURCE_DIR}/ym-bnet/testdata/\""
  )

ym_add_gtest ( bnet_import_test
  import_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  )

ym_add_gtest ( bnet_substitute_fanout_test
  substitute_fanout_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  )

ym_add_gtest ( bnet_simple_decomp_test
  simple_decomp_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  )
