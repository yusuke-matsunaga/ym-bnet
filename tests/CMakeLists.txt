
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${GTEST_INCLUDE_DIR}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( blifparser_test_SOURCES
  blif/parser_test.cc
  blif/NullBlifHandler.cc
  blif/TestBlifHandler.cc
  )

set ( BnBuilderTest_SOURCES
  builder/BnBuilderTest.cc
  )

set ( BnBuilder_iscas89_test_SOURCES
  iscas89/BnBuilder_iscas89_test.cc
  )

set ( iscas89parser_test_SOURCES
  iscas89/iscas89parser_test.cc
  )


# ===================================================================
#  テスト用のターゲットの設定
# ===================================================================

add_executable ( blifparser_test
  ${blifparser_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_common_d>
  )

target_compile_options ( blifparser_test
  PRIVATE "-g"
  )

target_link_libraries ( blifparser_test
  ${YM_LIB_DEPENDS}
  )

add_executable ( iscas89parser_test
  ${iscas89parser_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_common_d>
  )

target_compile_options ( iscas89parser_test
  PRIVATE "-g"
  )

target_link_libraries ( iscas89parser_test
  ${YM_LIB_DEPENDS}
  )

add_executable ( BnBuilderTest
  ${BnBuilderTest_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_common_d>
  )

target_compile_options ( BnBuilderTest
  PRIVATE "-g"
  )

target_link_libraries ( BnBuilderTest
  ${YM_LIB_DEPENDS}
  )

add_executable ( BnBuilder_iscas89_test
  ${BnBuilder_iscas89_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_common_d>
  )

target_compile_options ( BnBuilder_iscas89_test
  PRIVATE "-g"
  )

target_link_libraries ( BnBuilder_iscas89_test
  ${YM_LIB_DEPENDS}
  )
