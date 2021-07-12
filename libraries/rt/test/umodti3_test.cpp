// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_umodti3
// REQUIRES: int128

#include "int_t.h"
#include <stdio.h>

#include "compiler_builtins.hpp"

#ifdef CRT_HAS_128BIT

// Returns: a % b

COMPILER_RT_ABI tu_int __umodti3_wrapper(tu_int a, tu_int b)
{
    utwords at;
    at.all = a;
    utwords bt;
    bt.all = b;
    tu_int r;
    __umodti3(r, at.s.low, at.s.high, bt.s.low, bt.s.high);
    return r;
}

int test__umodti3(tu_int a, tu_int b, tu_int expected_r)
{
    tu_int r = __umodti3_wrapper(a, b);
    if (r != expected_r)
    {
        utwords at;
        at.all = a;
        utwords bt;
        bt.all = b;
        utwords rt;
        rt.all = r;
        utwords expected_rt;
        expected_rt.all = expected_r;
        printf("error in __umodti3: 0x%llX%.16llX %% 0x%llX%.16llX = "
               "0x%llX%.16llX, expected 0x%llX%.16llX\n",
               at.s.high, at.s.low, bt.s.high, bt.s.low, rt.s.high, rt.s.low,
               expected_rt.s.high, expected_rt.s.low);
    }
    return r != expected_r;
}

#endif

int main()
{
#ifdef CRT_HAS_128BIT
    if (test__umodti3(0, 1, 0))
        return 1;
    if (test__umodti3(2, 1, 0))
        return 1;
    if (test__umodti3(make_tu(0x8000000000000000uLL, 0), 1, 0x0uLL))
        return 1;
    if (test__umodti3(make_tu(0x8000000000000000uLL, 0), 2, 0x0uLL))
        return 1;
    if (test__umodti3(make_tu(0xFFFFFFFFFFFFFFFFuLL, 0xFFFFFFFFFFFFFFFFuLL),
                      2, 0x1uLL))
        return 1;

#else
    printf("skipped\n");
#endif
    return 0;
}
