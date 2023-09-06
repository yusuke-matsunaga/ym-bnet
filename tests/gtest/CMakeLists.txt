
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

ym_add_gtest ( bnet_read_blif_test
  read_blif_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  DEFINITIONS
  "-DDATAPATH=\"${TESTDATA_DIR}\""
  )

ym_add_gtest ( bnet_read_iscas89_test
  read_iscas89_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  DEFINITIONS
  "-DDATAPATH=\"${TESTDATA_DIR}\""
  )

ym_add_gtest ( bnet_read_truth_test
  read_truth_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  DEFINITIONS
  "-DDATAPATH=\"${TESTDATA_DIR}\""
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

ym_add_gtest ( bnet_make_skelton_copy_test
  make_skelton_copy_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  DEFINITIONS
  "-DDATAPATH=\"${TESTDATA_DIR}\""
  )

ym_add_gtest ( bnet_dump_restore_test
  dump_restore_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  DEFINITIONS
  "-DDATAPATH=\"${TESTDATA_DIR}\""
  )

ym_add_gtest ( bnet_BlifCoverTest
  BlifCoverTest.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  )