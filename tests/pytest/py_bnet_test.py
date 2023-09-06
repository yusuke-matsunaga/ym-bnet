#! /usr/bin/env python3

"""ymbnet パッケージのテストプログラム

:file: read_iscas89_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga
"""

import pytest
import os
from ymbnet import BnNetwork


def test_read_blif():
    TESTDATA_DIR = os.environ.get('TESTDATA_DIR')
    filename = os.path.join(TESTDATA_DIR, 's5378.blif')
    network = BnNetwork.read_blif(filename)
    ni = 35
    no = 49
    nd = 179
    ng = 2779
    assert ni + nd + 1 == network.input_num
    assert no + nd + nd == network.output_num
    assert ng == network.logic_num
    assert ni + no + 1 == network.port_num
    assert nd == network.dff_num

def test_read_iscas89():
    TESTDATA_DIR = os.environ.get('TESTDATA_DIR')
    filename = os.path.join(TESTDATA_DIR, 'b10.bench')
    network = BnNetwork.read_iscas89(filename)
    ni = 11
    no = 6
    nd = 17
    ng = 172
    assert ni + nd + 1 == network.input_num
    assert no + nd + nd == network.output_num
    assert ng == network.logic_num
    assert ni + no + 1 == network.port_num
    assert nd == network.dff_num
