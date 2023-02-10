#ifndef PYBNNETWORK_H
#define PYBNNETWORK_H

/// @file PyBnNetwork.h
/// @brief PyBnNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class PyBnNetwork PyBnNetwork.h "PyBnNetwork.h"
/// @brief Python 用の BnNetwork 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyBnNetwork
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  /// @return 初期化が成功したら true を返す．
  static
  bool
  init(
    PyObject* m ///< [in] 親のモジュールを表す PyObject
  );

  /// @brief BnNetwork を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const BnNetwork& val ///< [in] 値
  );

  /// @brief PyObject が BnNetwork タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief BnNetwork を表す PyObject から BnNetwork を取り出す．
  /// @return BnNetwork を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const BnNetwork&
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief BnNetwork を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_YM

#endif // PYBNNETWORK_H
