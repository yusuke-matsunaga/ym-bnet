
### @file bnet.pxi
### @brief bnet の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_BnNetwork cimport BnNetwork as CXX_BnNetwork
from CXX_BnNetwork cimport read_blif as cxx_read_blif
from CXX_BnNetwork cimport read_iscas89 as cxx_read_iscas89
from CXX_ClibCellLibrary cimport ClibCellLibrary as CXX_ClibCellLibrary


### @brief BnNetwork クラスの Python バージョン
cdef class BnNetwork :

    # C++ レベルのオブジェクト本体
    cdef CXX_BnNetwork _this

    ### @blif blif ファイルの読み込み
    @staticmethod
    def read_blif(str filename, CellLibrary cell_library = None, clock_name = None, reset_name = None) :
        cdef string c_filename = filename.encode('UTF-8')
        cdef string c_clock
        cdef string c_reset
        cdef bool stat
        if clock_name :
            c_clock = clock_name.encode('UTF-8')
        if reset_name :
            c_reset = reset_name.encode('UTF-8')
        network = BnNetwork()
        if cell_library == None :
            stat = cxx_read_blif(network._this, c_filename, c_clock, c_reset)
        else :
            stat = cxx_read_blif(network._this, c_filename, cell_library._this, c_clock, c_reset)
        if stat :
            return network
        else :
            return None

    ### @brief iscas89 ファイルの読み込み
    @staticmethod
    def read_iscas89(str filename, clock_name = None) :
        cdef string c_filename = filename.encode('UTF-8')
        cdef string c_clock
        cdef bool stat
        if clock_name :
            c_clock = clock_name.encode('UTF-8')
        network = BnNetwork()
        stat = cxx_read_iscas89(network._this, c_filename, c_clock)
        if stat :
            return network
        else :
            return None
