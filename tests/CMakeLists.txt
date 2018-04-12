
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
#  blifparser_test
# ===================================================================

set ( blifparser_test_SOURCES
  blif/parser_test.cc
  blif/NullBlifHandler.cc
  blif/TestBlifHandler.cc
  )

add_executable ( blifparser_test
  ${blifparser_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_base_d>
  )

target_compile_options ( blifparser_test
  PRIVATE "-g"
  )

target_link_libraries ( blifparser_test
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  iscas89parset_test
# ===================================================================

set ( iscas89parser_test_SOURCES
  iscas89/iscas89parser_test.cc
  )

add_executable ( iscas89parser_test
  ${iscas89parser_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_base_d>
  )

target_compile_options ( iscas89parser_test
  PRIVATE "-g"
  )

target_link_libraries ( iscas89parser_test
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  BnBlifReaderTest
# ===================================================================

set ( BnBlifReaderTest_SOURCES
  bnet/BnBlifReaderTest.cc
  )

add_executable ( BnBlifReaderTest
  ${BnBlifReaderTest_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_base_d>
  )

target_compile_options ( BnBlifReaderTest
  PRIVATE "-g"
  )

target_link_libraries ( BnBlifReaderTest
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  BnIscas89ReaderTest
# ===================================================================

set ( BnIscas89ReaderTest_SOURCES
  bnet/BnIscas89ReaderTest.cc
  )

add_executable ( BnIscas89ReaderTest
  ${BnIscas89ReaderTest_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_base_d>
  )

target_compile_options ( BnIscas89ReaderTest
  PRIVATE "-g"
  )

target_link_libraries ( BnIscas89ReaderTest
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  loop_test
# ===================================================================

set ( loop_test_SOURCES
  bnet/loop_test.cc
  )

add_executable ( loop_test
  ${loop_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_base_d>
  )

target_compile_options ( loop_test
  PRIVATE "-g"
  )

target_link_libraries ( loop_test
  ${YM_LIB_DEPENDS}
  )
