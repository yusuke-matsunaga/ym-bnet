
# ===================================================================
# インクルードパスの設定
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  blif_parser
# ===================================================================

add_executable ( blif_parser
  blif/parser.cc
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( blif_parser
  PRIVATE "-g"
  )

target_link_libraries ( blif_parser
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  iscas89_parser
# ===================================================================

add_executable ( iscas89_parser
  iscas89/parser.cc
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( iscas89_parser
  PRIVATE "-g"
  )

target_link_libraries ( iscas89_parser
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  bnet_read_blif
# ===================================================================

add_executable ( bnet_read_blif
  bnet/read_blif.cc
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( bnet_read_blif
  PRIVATE "-g"
  )

target_link_libraries ( bnet_read_blif
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  bnet_read_iscas89
# ===================================================================

add_executable ( bnet_read_iscas89
  bnet/read_iscas89.cc
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( bnet_read_iscas89
  PRIVATE "-g"
  )

target_link_libraries ( bnet_read_iscas89
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  bnet_loop_test
# ===================================================================

add_executable ( bnet_loop_test
  bnet/loop_test.cc
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_include_directories ( bnet_loop_test
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../c++-srcs/iscas89
  )

target_compile_options ( bnet_loop_test
  PRIVATE "-g"
  )

target_link_libraries ( bnet_loop_test
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  bnet_read_truth
# ===================================================================

add_executable ( bnet_read_truth
  bnet/read_truth.cc
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( bnet_read_truth
  PRIVATE "-g"
  )

target_link_libraries ( bnet_read_truth
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  bnet_read_aig
# ===================================================================

add_executable ( bnet_read_aig
  bnet/read_aig.cc
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( bnet_read_aig
  PRIVATE "-g"
  )

target_link_libraries ( bnet_read_aig
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  bnet_write_aig
# ===================================================================

add_executable ( bnet_write_aig
  bnet/write_aig.cc
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( bnet_write_aig
  PRIVATE "-g"
  )

target_link_libraries ( bnet_write_aig
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  bnet_write_aag
# ===================================================================

add_executable ( bnet_write_aag
  bnet/write_aag.cc
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( bnet_write_aag
  PRIVATE "-g"
  )

target_link_libraries ( bnet_write_aag
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  bnet_dump_restore
# ===================================================================

add_executable ( bnet_dump_restore
  bnet/dump_restore.cc
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( bnet_dump_restore
  PRIVATE "-g"
  )

target_link_libraries ( bnet_dump_restore
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  bnet_import
# ===================================================================

add_executable ( bnet_import
  bnet/import.cc
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options ( bnet_import
  PRIVATE "-g"
  )

target_link_libraries ( bnet_import
  ${YM_LIB_DEPENDS}
  )
