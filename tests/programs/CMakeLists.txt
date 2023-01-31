
# ===================================================================
# インクルードパスの設定
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  blifparser_test
# ===================================================================

set ( blifparser_test_SOURCES
  blif/parser_test.cc
  )

add_executable ( blifparser_test
  ${blifparser_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_include_directories ( blifparser_test
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../c++-srcs/blif
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
if ( 0 )
set ( iscas89parser_test_SOURCES
  iscas89/iscas89parser_test.cc
  )

add_executable ( iscas89parser_test
  ${iscas89parser_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_include_directories ( iscas89parser_test
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../c++-srcs/iscas89
  )

target_compile_options ( iscas89parser_test
  PRIVATE "-g"
  )

target_link_libraries ( iscas89parser_test
  ${YM_LIB_DEPENDS}
  )
endif ()

# ===================================================================
#  BnBlifReaderTest
# ===================================================================

set ( BnBlifReaderTest_SOURCES
  bnet/BnBlifReaderTest.cc
  )

add_executable ( BnBlifReaderTest
  ${BnBlifReaderTest_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
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
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
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
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_include_directories ( loop_test
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../c++-srcs/iscas89
  )

target_compile_options ( loop_test
  PRIVATE "-g"
  )

target_link_libraries ( loop_test
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  read_truth_test
# ===================================================================

set ( read_truth_test_SOURCES
  bnet/read_truth_test.cc
  )

add_executable ( read_truth_test
  ${read_truth_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( read_truth_test
  PRIVATE "-g"
  )

target_link_libraries ( read_truth_test
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  read_aig_test
# ===================================================================

set ( read_aig_test_SOURCES
  bnet/read_aig_test.cc
  )

add_executable ( read_aig_test
  ${read_aig_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( read_aig_test
  PRIVATE "-g"
  )

target_link_libraries ( read_aig_test
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  write_aig_test
# ===================================================================

set ( write_aig_test_SOURCES
  bnet/write_aig_test.cc
  )

add_executable ( write_aig_test
  ${write_aig_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( write_aig_test
  PRIVATE "-g"
  )

target_link_libraries ( write_aig_test
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  write_aag_test
# ===================================================================

set ( write_aag_test_SOURCES
  bnet/write_aag_test.cc
  )

add_executable ( write_aag_test
  ${write_aag_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( write_aag_test
  PRIVATE "-g"
  )

target_link_libraries ( write_aag_test
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  dump_restore_test
# ===================================================================

set ( dump_restore_test_SOURCES
  bnet/dump_restore_test.cc
  )

add_executable ( dump_restore_test
  ${dump_restore_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( dump_restore_test
  PRIVATE "-g"
  )

target_link_libraries ( dump_restore_test
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  import_test
# ===================================================================

set ( import_test_SOURCES
  bnet/import_test.cc
  )

add_executable ( import_test
  ${import_test_SOURCES}
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( import_test
  PRIVATE "-g"
  )

target_link_libraries ( import_test
  ${YM_LIB_DEPENDS}
  )
