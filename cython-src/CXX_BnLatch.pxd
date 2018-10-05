
### @file CXX_BnLatch.pxd
### @brief CXX_BnLatch 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp.string cimport string
from CXX_ClibCell cimport ClibCell

cdef extern from "ym/BnLatch.h" namespace "nsYm::nsBnet" :

    ### @brief BnLatch の cython バージョン
    cdef cppclass BnLatch :
        int id()
        string name()
        int output()
        int input()
        int enable()
        int clear()
        int preset()
        const ClibCell* cell()
        int output_pin_id()
        int input_pin_id()
        int enable_pin_id()
        int clear_pin_id()
        int preset_pin_id()
