

# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/ym-base/cython-src
  ${PROJECT_SOURCE_DIR}/ym-logic/cython-src
  ${PROJECT_SOURCE_DIR}/ym-cell/cython-src
  ${PROJECT_SOURCE_DIR}/ym-bnet/cython-src
  )


# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  cython 用のソースファイルの設定
# ===================================================================

set ( CYTHON_SOURCES
  pym_bnet.pyx
  )

set_source_files_properties (
  ${CYTHON_SOURCES}
  PROPERTIES CYTHON_IS_CXX TRUE
  )


# ===================================================================
#  module ターゲットの設定
# ===================================================================

cython_add_module ( pym_bnet
  ${CYTHON_SOURCES}
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_cell_obj_d>
  $<TARGET_OBJECTS:ym_bnet_obj_d>
  )

target_compile_options ( pym_bnet
  PRIVATE "-g"
  )


target_link_libraries ( pym_bnet
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  テストの設定
# ===================================================================

set ( RUN_PYM_BNET
  env PYTHONPATH=${PROJECT_BINARY_DIR}/ym-bnet/py-test
  ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/pym_bnet_test.py)

add_test ( bnet_py_read_blif_test
  ${RUN_PYM_BNET} ReadBlifTest
  )

set_property ( TEST bnet_py_read_blif_test
  PROPERTY ENVIRONMENT TESTDATA_DIR=${PROJECT_SOURCE_DIR}/ym-bnet/testdata
  )

add_test ( bnet_py_read_iscas89_test
  ${RUN_PYM_BNET} ReadIscas89Test
  )

set_property ( TEST bnet_py_read_iscas89_test
  PROPERTY ENVIRONMENT TESTDATA_DIR=${PROJECT_SOURCE_DIR}/ym-bnet/testdata
  )
