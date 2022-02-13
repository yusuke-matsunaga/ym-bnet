
### @file CXX_BnDff.pxd
### @brief CXX_BnDff 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp.string cimport string
#from CXX_ClibCell cimport ClibCell

cdef extern from "ym/BnDff.h" namespace "nsYm::nsBnet" :

    ### @brief BnDff の cython バージョン
    cdef cppclass BnDff :
        int id()
        string name()
        int output()
        int input()
        int clock()
        int clear()
        int preset()
        int cell_id()
