// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_divti3
// REQUIRES: int128

#include "int_t.h"
#include <stdio.h>

#include "compiler_builtins.hpp"

#ifdef CRT_HAS_128BIT

// Returns: a / b

COMPILER_RT_ABI ti_int __divti3_wrapper(ti_int a, ti_int b)
{
    twords at;
    at.all = a;
    twords bt;
    bt.all = b;
    ti_int r;
    __divti3(r, at.s.low, at.s.high, bt.s.low, bt.s.high);
    return r;
}

int test__divti3(ti_int a, ti_int b, ti_int expected)
{
    ti_int x = __divti3_wrapper(a, b);
    if (x != expected)
    {
        twords at;
        at.all = a;
        twords bt;
        bt.all = b;
        twords xt;
        xt.all = x;
        twords expectedt;
        expectedt.all = expected;
        printf("error in __divti3: 0x%llX%.16llX / 0x%llX%.16llX = "
               "0x%llX%.16llX, expected 0x%llX%.16llX\n",
               at.s.high, at.s.low, bt.s.high, bt.s.low, xt.s.high, xt.s.low,
               expectedt.s.high, expectedt.s.low);
    }
    return x != expected;
}

char assumption_1[sizeof(ti_int) == 2*sizeof(di_int)] = {0};

#endif

int main()
{
#ifdef CRT_HAS_128BIT
    if (test__divti3(0, 1, 0))
        return 1;
    if (test__divti3(0, -1, 0))
        return 1;

    if (test__divti3(2, 1, 2))
        return 1;
    if (test__divti3(2, -1, -2))
        return 1;
    if (test__divti3(-2, 1, -2))
        return 1;
    if (test__divti3(-2, -1, 2))
        return 1;

    if (test__divti3(make_ti(0x8000000000000000LL, 0), 1, make_ti(0x8000000000000000LL, 0)))
        return 1;
    if (test__divti3(make_ti(0x8000000000000000LL, 0), -1, make_ti(0x8000000000000000LL, 0)))
        return 1;
    if (test__divti3(make_ti(0x8000000000000000LL, 0), -2, make_ti(0x4000000000000000LL, 0)))
        return 1;
    if (test__divti3(make_ti(0x8000000000000000LL, 0), 2, make_ti(0xC000000000000000LL, 0)))
        return 1;

#else
    printf("skipped\n");
#endif
    return 0;
}
