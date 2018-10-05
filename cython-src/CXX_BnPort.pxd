
### @file CXX_BnPort.pxd
### @brief CXX_BnPort 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp.string cimport string


cdef extern from "ym/BnPort.h" namespace "nsYm::nsBnet" :

    ### @brief BnPort の cython バージョン
    cdef cppclass BnPort :
        int id()
        string name()
        int bit_width()
        int bit(int)
