
### @file CXX_BnDff.pxd
### @brief CXX_BnDff 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp.string cimport string
from CXX_ClibCell cimport ClibCell

cdef extern from "ym/BnDff.h" namespace "nsYm::nsBnet" :

    ### @brief BnDff の cython バージョン
    cdef cppclass BnDff :
        int id()
        string name()
        int output()
        int xoutput()
        int input()
        int clock()
        int clear()
        int preset()
        const ClibCell* cell()
        int output_pin_id()
        int xoutput_pin_id()
        int input_pin_id()
        int clock_pin_id()
        int clear_pin_id()
        int preset_pin_id()
