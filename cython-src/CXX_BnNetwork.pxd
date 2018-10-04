
### @file CXX_BnNetwork.pxd
### @brief CXX_BnNetwork 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_ClibCellLibrary cimport ClibCellLibrary


cdef extern from "ym/BnNetwork.h" namespace "nsYm::nsBnet" :

    ### @brief BnNetwork の cython バージョン
    cdef cppclass BnNetwork :
        BnNetwork()
        void clear()
        void copy(const BnNetwork& src)
        void set_library(const ClibCellLibrary*)
        void set_name(const string& name)
        string name()
        const ClibCellLibrary* library()
        int port_num()
        int dff_num()
        int latch_num()
        int node_num()
        int input_num()
        int output_num()
        int logic_num()
        int func_num()
        int expr_num()

    void write_blif(const BnNetwork&, const string&)
    bool read_blif(BnNetwork&, const string&, const string&, const string&)
    bool read_blif(BnNetwork&, const string&, const ClibCellLibrary&, const string&, const string&)
    bool read_iscas89(BnNetwork&, const string&, const string&)
