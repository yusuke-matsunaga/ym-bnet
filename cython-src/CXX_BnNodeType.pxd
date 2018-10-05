
### @file CXX_BnNodeType.pxd
### @brief CXX_BnNodeType 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.


cdef extern from "ym/BnNodeType.h" namespace "nsYm::nsBnet" :

    ### @brief BnNodeType の cython バージョン
    cdef enum BnNodeType :
        Input,
        Output,
        C0,
        C1,
        Buff,
        Not,
        And,
        Nand,
        Or,
        Nor,
        Xor,
        Xnor,
        Expr,
        TvFunc,
        None

    cdef int __bnnodetype_to_int(BnNodeType)
    cdef BnNodeType __int_to_bnnodetype(int)
