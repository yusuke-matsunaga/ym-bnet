
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${GTEST_INCLUDE_DIR}
  ../c++-src/bnet
  ../c++-src/blif
  ../c++-src/iscas89
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

ym_add_gtest ( FuncAnalyzerTest
  FuncAnalyzerTest.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  )

ym_add_gtest ( BnNodeEncTest
  BnNodeEncTest.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  )

add_executable ( read_blif_test
  read_blif_test.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  )

target_compile_options ( read_blif_test
  PRIVATE "-g"
  )

target_compile_definitions ( read_blif_test
  PRIVATE "-DDATAPATH=\"${CMAKE_CURRENT_SOURCE_DIR}/../testdata/\""
  )

target_link_libraries ( read_blif_test
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_test ( read_blif_test
  read_blif_test
  )

add_executable ( read_iscas89_test
  read_iscas89_test.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  )

target_compile_options ( read_iscas89_test
  PRIVATE "-g"
  )

target_compile_definitions ( read_iscas89_test
  PRIVATE "-DDATAPATH=\"${CMAKE_CURRENT_SOURCE_DIR}/../testdata/\""
  )

target_link_libraries ( read_iscas89_test
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_test ( read_iscas89_test
  read_iscas89_test
  )

ym_add_gtest ( import_test
  import_test.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  )
