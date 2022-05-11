
# ===================================================================
# インクルードパスの設定
# ===================================================================


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
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  )

target_include_directories ( bnet_FuncAnalyzer_test
  PRIVATE
  ${PROJECT_SOURCE_DIR}/ym-bnet/c++-src/bnet
  )

ym_add_gtest ( bnet_read_blif_test
  read_blif_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  DEFINITIONS
  "-DDATAPATH=\"${CMAKE_CURRENT_SOURCE_DIR}/../testdata/\""
  )

target_include_directories ( bnet_read_blif_test
  PRIVATE
  ${PROJECT_SOURCE_DIR}/ym-bnet/c++-src/blif
  )

ym_add_gtest ( bnet_read_iscas89_test
  read_iscas89_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  DEFINITIONS
  "-DDATAPATH=\"${CMAKE_CURRENT_SOURCE_DIR}/../testdata/\""
  )

target_include_directories ( bnet_read_iscas89_test
  PRIVATE
  ${PROJECT_SOURCE_DIR}/ym-bnet/c++-src/iscas89
  )

ym_add_gtest ( bnet_import_test
  import_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  )

ym_add_gtest ( bnet_substitute_fanout_test
  substitute_fanout_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  )

ym_add_gtest ( bnet_simple_decomp_test
  simple_decomp_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  )
