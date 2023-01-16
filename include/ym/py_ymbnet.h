#ifndef PY_YMBNET_H
#define PY_YMBNET_H

/// @file py_ymbnet.h
/// @brief py_ymbnet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym_config.h"


BEGIN_NAMESPACE_YM

/// @brief ymbnet モジュールの初期化関数
extern "C" PyObject* PyInit_ymbnet();

END_NAMESPACE_YM

#endif // PY_YMBNET_H
