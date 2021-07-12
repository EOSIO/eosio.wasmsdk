#ifndef __compiler_rt_int_t_h__
#define __compiler_rt_int_t_h__
#include <stdint.h>
#include <limits.h>

#define COMPILER_RT_ABI
#define CRT_HAS_128BIT
#define UNUSED __attribute__((unused))

#ifdef si_int
#undef si_int
#endif
typedef int32_t si_int;
typedef uint32_t su_int;

typedef long long di_int;
typedef unsigned long long du_int;

typedef union
{
    __int128 all;
    struct
    {
        uint64_t low;
        int64_t high;
    }s;
} twords;

typedef union
{
    unsigned __int128 all;
    struct
    {
        uint64_t low;
        uint64_t high;
    }s;
} utwords;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t low;
        uint32_t high;
    }s;
} udwords;

typedef union
{
    udwords u;
    double  f;
} double_bits;


typedef __int128 ti_int;
typedef unsigned __int128 tu_int;

static __inline ti_int make_ti(di_int h, di_int l) {
  twords r;
  r.s.high = h;
  r.s.low = l;
  return r.all;
}

static __inline tu_int make_tu(du_int h, du_int l) {
  utwords r;
  r.s.high = h;
  r.s.low = l;
  return r.all;
}

inline __int128 __clzti2(__int128 a)
{
    twords x;
    x.all = a;
    const int64_t f = -(x.s.high == 0);
    return __builtin_clzll((x.s.high & ~f) | (x.s.low & f)) +
           ((int32_t)f & ((int32_t)(sizeof(int64_t) * CHAR_BIT)));
}

#endif// __compiler_rt_int_t_h__
