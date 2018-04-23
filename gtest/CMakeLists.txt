
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${GTEST_INCLUDE_DIR}
  ../src/bnet
  ../src/blif
  ../src/iscas89
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
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  )

add_executable ( ReadBlifTest
  ReadBlifTest.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  )

target_compile_options ( ReadBlifTest
  PRIVATE "-g"
  )

target_compile_definitions ( ReadBlifTest
  PRIVATE "-DDATAPATH=\"${CMAKE_CURRENT_SOURCE_DIR}/data/\""
  )

target_link_libraries ( ReadBlifTest
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_test ( ReadBlifTest
  ReadBlifTest
  )
