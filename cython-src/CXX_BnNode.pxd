
### @file CXX_BnNode.pxd
### @brief CXX_BnNode 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
from CXX_BnNodeType cimport BnNodeType

cdef extern from "ym/BnNode.h" namespace "nsYm::nsBnet" :

    ### @brief BnNode の cython バージョン
    cdef cppclass BnNode :
        int id()
        string name()
        BnNodeType type()
        int fanout_num()
        int fanout(int)
        const vector[int]& fanout_list()
        int input_id()
        bool is_port_input()
        bool is_dff_output()
        bool is_dff_xoutput()
        bool is_latch_output()
        int output_id()
        bool is_port_output()
        bool is_dff_input()
        bool is_dff_clock()
        bool is_dff_clear()
        bool is_dff_preset()
        bool is_latch_input()
        bool is_latch_enable()
        bool is_latch_clear()
        bool is_latch_preset()
        int fanin()
        int port_id()
        int port_bit()
        int dff_id()
        int latch_id()
        int fanin_num()
        int fanin(int)
        int expr_id()
        int func_id()
