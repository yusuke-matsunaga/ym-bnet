
### @file pym_bnet_test.pyx
### @brief pym_bnet のテストプログラム
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

import unittest
from read_blif_test import ReadBlifTest
from read_iscas89_test import ReadIscas89Test

suite = unittest.TestSuite()
suite.addTest(ReadBlifTest())
suite.addTest(ReadIscas89Test())


if __name__ == '__main__' :

    unittest.main()
