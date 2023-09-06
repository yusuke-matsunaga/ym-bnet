

# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${Python3_INCLUDE_DIRS}
  )


# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  テストの設定
# ===================================================================

add_executable( py_ymbnet
  py_ymbnet.cc
  $<TARGET_OBJECTS:py_ymbnet_obj_d>
  $<TARGET_OBJECTS:py_ymcell_obj_d>
  $<TARGET_OBJECTS:py_ymlogic_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options( py_ymbnet
  PRIVATE "-g"
  )

target_link_libraries ( py_ymbnet
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )

add_test ( bnet_py_bnet_test
  py_ymbnet -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
  )

set_property ( TEST bnet_py_bnet_test
  PROPERTY ENVIRONMENT TESTDATA_DIR=${TESTDATA_DIR}
  )
