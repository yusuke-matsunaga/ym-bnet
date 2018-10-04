
### @file bnet.pxi
### @brief bnet の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_BnNetwork cimport BnNetwork as CXX_BnNetwork
from CXX_BnNetwork cimport write_blif as cxx_write_blif
from CXX_BnNetwork cimport read_blif as cxx_read_blif
from CXX_BnNetwork cimport read_iscas89 as cxx_read_iscas89
from CXX_ClibCellLibrary cimport ClibCellLibrary as CXX_ClibCellLibrary


### @brief BnNetwork クラスの Python バージョン
cdef class BnNetwork :

    # C++ レベルのオブジェクト本体
    cdef CXX_BnNetwork _this

    ### @brief 内容をクリアする．
    def clear(self) :
        self._this.clear()

    ### @brief コピーする．
    def copy(self, BnNetwork src) :
        self._this.copy(src._this)

    ### @brief 名前をつける．
    def set_name(self, str name) :
        cdef string c_name = name.encode('UTF-8')
        self._this.set_name(c_name)

    ### @brief 名前を返す．
    @property
    def name(self) :
        cdef string c_name = self._this.name()
        return c_name.decode('UTF-8')

    ### @brief ポート数を返す．
    @property
    def port_num(self) :
        return self._this.port_num()

    ### @brief DFF数を返す．
    @property
    def dff_num(self) :
        return self._this.dff_num()

    ### @brief ラッチ数を返す．
    @property
    def latch_num(self) :
        return self._this.latch_num()

    ### @brief ノード数を返す．
    @property
    def node_num(self) :
        return self._this.node_num()

    ### @brief 入力数を返す．
    @property
    def input_num(self) :
        return self._this.input_num()

    ### @brief 出力数を返す．
    @property
    def output_num(self) :
        return self._this.output_num()

    ### @brief 論理ゲート数を返す．
    @property
    def logic_num(self) :
        return self._this.logic_num()

    ### @brief 論理ゲート中で用いられている関数の種類を返す．
    @property
    def func_num(self) :
        return self._this.func_num()

    ### @brief 論理ゲート数で用いられrている論理式の種類を返す．
    @property
    def expr_num(self) :
        return self._this.expr_num()

    ### @brief blif ファイルの書き出し
    def write_blif(self, str filename) :
        cdef string c_filename = filename.encode('UTF-8')
        cxx_write_blif(self._this, c_filename)

    ### @blief blif ファイルの読み込み
    @staticmethod
    def read_blif(str filename, CellLibrary cell_library = None, clock_name = None, reset_name = None) :
        cdef string c_filename = filename.encode('UTF-8')
        cdef string c_clock
        cdef string c_reset
        cdef bool stat
        if clock_name :
            c_clock = clock_name.encode('UTF-8')
        if reset_name :
            c_reset = reset_name.encode('UTF-8')
        network = BnNetwork()
        if cell_library == None :
            stat = cxx_read_blif(network._this, c_filename, c_clock, c_reset)
        else :
            stat = cxx_read_blif(network._this, c_filename, cell_library._this, c_clock, c_reset)
        if stat :
            return network
        else :
            return None

    ### @brief iscas89 ファイルの読み込み
    @staticmethod
    def read_iscas89(str filename, clock_name = None) :
        cdef string c_filename = filename.encode('UTF-8')
        cdef string c_clock
        cdef bool stat
        if clock_name :
            c_clock = clock_name.encode('UTF-8')
        network = BnNetwork()
        stat = cxx_read_iscas89(network._this, c_filename, c_clock)
        if stat :
            return network
        else :
            return None
