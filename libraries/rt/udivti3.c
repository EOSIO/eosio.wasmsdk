//===-- udivti3.c - Implement __udivti3 -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __udivti3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "int_t.h"
#include "udivmodti4.c"

#ifdef CRT_HAS_128BIT

// Returns: a / b

COMPILER_RT_ABI tu_int ___udivti3(tu_int a, tu_int b) {
  return __udivmodti4(a, b, 0);
}

#endif // CRT_HAS_128BIT
