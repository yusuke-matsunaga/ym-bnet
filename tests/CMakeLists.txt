
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

set ( BlifBnBuilderTest_SOURCES
  blif/BlifBnBuilderTest.cc
  )

set ( Iscas89BnBuilderTest_SOURCES
  iscas89/Iscas89BnBuilderTest.cc
  )

set ( iscas89parser_test_SOURCES
  iscas89/iscas89parser_test.cc
  )

set ( BnBlifReaderTest_SOURCES
  bnet/BnBlifReaderTest.cc
  )

set ( BnIscas89ReaderTest_SOURCES
  bnet/BnIscas89ReaderTest.cc
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

add_executable ( BlifBnBuilderTest
  ${BlifBnBuilderTest_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_common_d>
  )

target_compile_options ( BlifBnBuilderTest
  PRIVATE "-g"
  )

target_link_libraries ( BlifBnBuilderTest
  ${YM_LIB_DEPENDS}
  )

add_executable ( Iscas89BnBuilderTest
  ${Iscas89BnBuilderTest_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_common_d>
  )

target_compile_options ( Iscas89BnBuilderTest
  PRIVATE "-g"
  )

target_link_libraries ( Iscas89BnBuilderTest
  ${YM_LIB_DEPENDS}
  )

add_executable ( BnBlifReaderTest
  ${BnBlifReaderTest_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_common_d>
  )

target_compile_options ( BnBlifReaderTest
  PRIVATE "-g"
  )

target_link_libraries ( BnBlifReaderTest
  ${YM_LIB_DEPENDS}
  )

add_executable ( BnIscas89ReaderTest
  ${BnIscas89ReaderTest_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_common_d>
  )

target_compile_options ( BnIscas89ReaderTest
  PRIVATE "-g"
  )

target_link_libraries ( BnIscas89ReaderTest
  ${YM_LIB_DEPENDS}
  )
