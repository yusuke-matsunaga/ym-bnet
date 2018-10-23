
### @file CXX_BnNetwork.pxd
### @brief CXX_BnNetwork 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
from CXX_Expr cimport Expr
from CXX_TvFunc cimport TvFunc
from CXX_BnPort cimport BnPort
from CXX_BnDff cimport BnDff
from CXX_BnLatch cimport BnLatch
from CXX_BnNode cimport BnNode
from CXX_ClibCellLibrary cimport ClibCellLibrary


cdef extern from "ym/BnNetwork.h" namespace "nsYm::nsBnet" :

    ### @brief BnNetwork の cython バージョン
    cdef cppclass BnNetwork :
        BnNetwork()
        void clear()
        void copy(const BnNetwork& src)
        void set_library(const ClibCellLibrary&)
        void set_name(const string& name)
        string name()
        const ClibCellLibrary& library()
        int port_num()
        const BnPort& port(int)
        int dff_num()
        const BnDff& dff(int)
        int latch_num()
        const BnLatch& latch(int)
        int node_num()
        const BnNode& node(int)
        int input_num()
        int input_id(int)
        int output_num()
        int output_id(int)
        int output_src_id(int)
        int logic_num()
        int logic_id(int)
        int func_num()
        const TvFunc& func(int)
        int expr_num()
        const Expr& expr(int)
        @staticmethod
        BnNetwork read_blif(const string&, const ClibCellLibrary&, const string&, const string&)
        @staticmethod
        BnNetwork read_iscas89(const string&, const string&)
        void write_blif(const string&)
