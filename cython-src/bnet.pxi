
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
from CXX_BnPort cimport BnPort as CXX_BnPort
from CXX_BnDff cimport BnDff as CXX_BnDff
from CXX_BnLatch cimport BnLatch as CXX_BnLatch
from CXX_BnNode cimport BnNode as CXX_BnNode
from CXX_BnNodeType cimport BnNodeType as CXX_BnNodeType
from CXX_BnNodeType cimport __bnnodetype_to_int
from CXX_ClibCellLibrary cimport ClibCellLibrary as CXX_ClibCellLibrary


### @brief BnPort クラスの Python バージョン
###
### 実際には本当の BnPort の情報をコピーした全くの別物
cdef class BnPort :

    # データメンバ
    cdef int __id
    cdef str __name
    cdef list __bit_list

    ### @brief 初期化
    def __init__(self) :
        pass

    ### @brief ID番号を返す．
    @property
    def id(self) :
        return self.__id

    @property
    def name(self) :
        return self.__name

    @property
    def bit_list(self) :
        return self.__bit_list


### @brief BnDff クラスの Python バージョン
###
### 実際には本当の BnDff の情報をコピーした 全くの別物
cdef class BnDff :

    # データメンバ
    cdef int __id
    cdef str __name
    cdef int __input
    cdef int __output
    cdef int __xoutput
    cdef int __clock
    cdef int __clear
    cdef int __preset

    def __init__(self) :
        pass

    @property
    def id(self) :
        return self.__id

    @property
    def name(self) :
        return self.__name

    @property
    def input(self) :
        return self.__input

    @property
    def output(self) :
        return self.__output

    @property
    def xoutput(self) :
        return self.__xoutput

    @property
    def clock(self) :
        return self.__clock

    @property
    def clear(self) :
        return self.__clear

    @property
    def preset(self) :
        return self.__preset


### @brief BnLatch クラスの Python バージョン
###
### 実際には本当の BnLatch の情報をコピーした 全くの別物
cdef class BnLatch :

    # データメンバ
    cdef int __id
    cdef str __name
    cdef int __input
    cdef int __output
    cdef int __enable
    cdef int __clear
    cdef int __preset

    def __init__(self) :
        pass

    @property
    def id(self) :
        return self.__id

    @property
    def name(self) :
        return self.__name

    @property
    def input(self) :
        return self.__input

    @property
    def output(self) :
        return self.__output

    @property
    def enable(self) :
        return self.__enable

    @property
    def clear(self) :
        return self.__clear

    @property
    def preset(self) :
        return self.__preset


### @brief BnNode クラスの Python バージョン
###
### 実際には本当の BnNode の情報をコピーした全くの別物
cdef class BnNode :

    # データメンバ
    cdef int __id
    cdef str __name
    cdef str __type
    cdef str __subtype
    cdef int __subid
    cdef int __obj_id
    cdef int __bit
    cdef list __fanin_list
    cdef list __fanout_list

    ### @brief 初期化
    def __init__(self) :
        pass

    @property
    def id(self) :
        return self.__id

    @property
    def name(self) :
        return self.__name

    @property
    def type(self) :
        return self.__type

    @property
    def input_id(self) :
        assert self.type == 'Input'
        return self.__subid

    @property
    def input_type(self) :
        assert self.type == 'Input'
        return self.__subtype

    @property
    def port_id(self) :
        assert self.type == 'Input' and self.input_type == 'primary_input'
        return self.__obj_id

    @property
    def port_bit(self) :
        assert self.type == 'Input' and self.input_type == 'primary_input'
        return self.__bit

    @property
    def dff_id(self) :
        if self.type == 'Input' :
            if self.input_type == 'dff_output' or self.input_type == 'dff_xoutput' :
                return self.__obj_id
        elif self.type == 'Output' :
            if self.output_type == 'dff_input' or self.output_type == 'dff_clock' or \
               self.output_type == 'dff_clear' or self.output_type == 'dff_preset' :
               return self.__obj_id
        assert False
        return -1

    @property
    def latch_id(self) :
        if self.type == 'Input' and self.input_type == 'latch_output' :
            return self.__obj_id
        elif self.type == 'Output' :
            if self.output_type == 'latch_input' or self.output_type == 'latch_enable' or \
            self.output_type == 'latch_clear' or self.output_type == 'latch_preset' :
                return self.__obj_id
        assert False
        return -1

    @property
    def expr_id(self) :
        if self.type == 'Expr' :
            return self.__obj_id
        assert False
        return -1

    @property
    def func_id(self) :
        if self.type == 'TvFunc' :
            return self.__obj_id

    @property
    def fanin_id_list(self) :
        return self.__fanin_list

    @property
    def fanout_id_list(self) :
        return self.__fanout_list


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

    ### @brief セルライブラリを設定する．
    def set_library(self, CellLibrary library) :
        self._this.set_library(library._this)

    ### @brief 名前を返す．
    @property
    def name(self) :
        cdef string c_name = self._this.name()
        return c_name.decode('UTF-8')

    ### @brief セルライブラリを返す．
    @property
    def library(self) :
        ans = CellLibrary()
        ans._this = self._this.library()
        return ans

    ### @brief ポート数を返す．
    @property
    def port_num(self) :
        return self._this.port_num()

    ### @brief ポートを返す．
    ### @param[in] port_id ( port_id >= 0 && port_id < port_num() )
    def port(self, int port_id) :
        cdef const CXX_BnPort* c_port = &self._this.port(port_id)
        ans = BnPort()
        ans.__id = c_port.id()
        ans.__name = c_port.name().decode('UTF-8')
        ans.__bit_list = [ c_port.bit(i) for i in range(c_port.bit_width()) ]
        return ans

    ### @brief DFF数を返す．
    @property
    def dff_num(self) :
        return self._this.dff_num()

    ### @brief DFFを返す．
    def dff(self, int dff_id) :
        cdef const CXX_BnDff* c_dff = &self._this.dff(dff_id)
        ans = BnDff()
        ans.__id = c_dff.id()
        ans.__name = c_dff.name().decode('UTF-8')
        ans.__input = c_dff.input()
        ans.__output = c_dff.output()
        ans.__xoutput = c_dff.xoutput()
        ans.__clock = c_dff.clock()
        ans.__clear = c_dff.clear()
        ans.__preset = c_dff.preset()
        return ans

    ### @brief ラッチ数を返す．
    @property
    def latch_num(self) :
        return self._this.latch_num()

    ### @brief ラッチを返す．
    def latch(self, int latch_id) :
        cdef const CXX_BnLatch* c_latch = &self._this.latch(latch_id)
        ans = BnLatch()
        ans.__id = c_latch.id()
        ans.__name = c_latch.name().decode('UTF-8')
        ans.__input = c_latch.input()
        ans.__output = c_latch.output()
        ans.__enable = c_latch.enable()
        ans.__clear = c_latch.clear()
        ans.__preset = c_latch.preset()
        return ans

    ### @brief ノード数を返す．
    @property
    def node_num(self) :
        return self._this.node_num()

    ### @brief ノードを返す．
    def node(self, int node_id) :
        cdef const CXX_BnNode* c_node = &self._this.node(node_id)
        cdef CXX_BnNodeType c_type = c_node.type()
        cdef int c_type_int = __bnnodetype_to_int(c_type)
        node = BnNode()
        node.__id = c_node.id()
        node.__name = c_node.name().decode('UTF-8')
        if c_type_int == 0 :
            type_str = 'None'
        elif c_type_int == 1 :
            type_str = 'Input'
        elif c_type_int == 2 :
            type_str = 'Output'
        elif c_type_int == 3 :
            type_str = 'C0'
        elif c_type_int == 4 :
            type_str = 'C1'
        elif c_type_int == 5 :
            type_str = 'Buff'
        elif c_type_int == 6 :
            type_str = 'Not'
        elif c_type_int == 7 :
            type_str = 'And'
        elif c_type_int == 8 :
            type_str = 'Nand'
        elif c_type_int == 9 :
            type_str = 'Or'
        elif c_type_int == 10 :
            type_str = 'Nor'
        elif c_type_int == 11 :
            type_str = 'Xor'
        elif c_type_int == 12 :
            type_str = 'Xnor'
        elif c_type_int == 13 :
            type_str = 'Expr'
        elif c_type_int == 14 :
            type_str = 'TvFunc'
        node.__type = type_str
        node.__fanin_list = []
        node.__fanout_list = [ c_node.fanout_id(i) for i in range(c_node.fanout_num()) ]
        if type_str == 'Input' :
            node.__subid = c_node.input_id()
            if c_node.is_port_input() :
                node.__subtype = 'primary_input'
                node.__obj_id = c_node.port_id()
                node.__bit = c_node.port_bit()
            elif c_node.is_dff_output() :
                node.__subtype = 'dff_output'
                node.__obj_id = c_node.dff_id()
            elif c_node.is_dff_xoutput() :
                node.__subtype = 'dff_xoutput'
                node.__obj_id = c_node.dff_id()
            elif c_node.is_latch_output() :
                node.__subtype = 'latch_output'
                node.__obj_id = c_node.latch_id()
            else :
                assert False
        elif type_str == 'Output' :
            node.__subid = c_node.output_id()
            node.__fanin_list = [ c_node.fanin_id(0) ]
            if c_node.is_port_output() :
                node.__subtype = 'primary_output'
                node.__obj_id = c_node.port_id()
                node.__bit = c_node.port_bit()
            elif c_node.is_dff_input() :
                node.__subtype = 'dff_input'
                node.__obj_id = c_node.dff_id()
            elif c_node.is_dff_clock() :
                node.__subtype = 'dff_clock'
                node.__obj_id = c_node.dff_id()
            elif c_node.is_dff_clear() :
                node.__subtype = 'dff_clear'
                node.__obj_id = c_node.dff_id()
            elif c_node.is_dff_preset() :
                node.__subtype = 'dff_preset'
                node.__obj_id = c_node.dff_id()
            elif c_node.is_latch_input() :
                node.__subtype = 'latch_input'
                node.__obj_id = c_node.latch_id()
            elif c_node.is_latch_enable() :
                node.__subtype = 'latch_enable'
                node.__obj_id = c_node.latch_id()
            elif c_node.is_latch_clear() :
                node.__subtype = 'latch_clear'
                node.__obj_id = c_node.latch_id()
            elif c_node.is_latch_preset() :
                node.__subtype = 'latch_preset'
                node.__obj_id = c_node.latch_id()
            else :
                assert False
        else :
            ni = c_node.fanin_num()
            node.__fanin_list = [ c_node.fanin_id(i) for i in range(ni) ]
            if type_str == 'Expr' :
                node.__obj_id = c_node.expr_id()
            elif type_str == 'TvFunc' :
                node.p__obj_id = c_node.func_id()
        return node

    ### @brief 入力数を返す．
    @property
    def input_num(self) :
        return self._this.input_num()

    ### @brief 入力のノード番号のリストを返す．
    @property
    def input_id_list(self) :
        return [ self._this.input_id(i) for i in range(self._this.input_num()) ]

    ### @brief 出力数を返す．
    @property
    def output_num(self) :
        return self._this.output_num()

    ### @brief 出力のノード番号のリストを返す．
    @property
    def output_id_list(self) :
        return [ self._this.output_id(i) for i in range(self._this.output_num()) ]

    ### @brief 出力のソースノード番号のリストを返す．
    @property
    def output_src_id_list(self) :
        return [ self._this.output_src_id(i) for i in range(self._this.output_num()) ]

    ### @brief 論理ゲート数を返す．
    @property
    def logic_num(self) :
        return self._this.logic_num()

    ### @brief 論理ゲートのノード番号のリストを返す．
    @property
    def logic_id_list(self) :
        return [ self._this.logic_id(i) for i in range(self._this.logic_num()) ]

    ### @brief 論理ゲート中で用いられている関数の種類を返す．
    @property
    def func_num(self) :
        return self._this.func_num()

    ### @brief 関数を返す．
    ### @param[in] func_id 関数番号 ( 0 <= func_id < func_num() )
    def func(self, int func_id) :
        ans = TvFunc()
        ans._this = self._this.func(func_id)
        return ans

    ### @brief 論理ゲート数で用いられrている論理式の種類を返す．
    @property
    def expr_num(self) :
        return self._this.expr_num()

    ### @brief 論理式を返す．
    ### @param[in] expr_id 論理式番号( 0 <= expr_id < expr_num() )
    def expr(self, int expr_id) :
        ans = Expr()
        ans._this = self._this.expr(expr_id)
        return ans

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
