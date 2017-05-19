
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

add_executable ( FuncAnalyzerTest
  FuncAnalyzerTest.cc
  $<TARGET_OBJECTS:ym_base_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_bnet_d>
  )

target_compile_options ( FuncAnalyzerTest
  PRIVATE "-g"
  )

target_link_libraries ( FuncAnalyzerTest
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_test ( BnetTest
  FuncAnalyzerTest
  )
