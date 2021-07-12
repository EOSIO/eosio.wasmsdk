#pragma once
#include <cstdint>
#include <softfloat.hpp>

extern "C" {
   __int128 ___fixdfti(uint64_t);
   __int128 ___fixsfti(uint32_t);
   __int128 ___fixtfti( float128_t);
   unsigned __int128 ___fixunsdfti(uint64_t);
   unsigned __int128 ___fixunssfti(uint32_t);
   unsigned __int128 ___fixunstfti(float128_t);
   double ___floattidf(__int128);
   double ___floatuntidf(unsigned __int128);
   __int128 ___multi3(__int128, __int128);
   __int128 ___ashlti3(__int128, int32_t);
   __int128 ___ashrti3(__int128, int32_t);
   __int128 ___lshrti3(__int128, int32_t);
   __int128 ___divti3(__int128, __int128);
   unsigned __int128 ___udivti3(unsigned __int128, unsigned __int128);
   __int128 ___modti3(__int128, __int128);
   unsigned __int128 ___umodti3(unsigned __int128, unsigned __int128);

   void __ashlti3(__int128&, uint64_t, uint64_t, uint32_t);
   void __ashrti3(__int128&, uint64_t, uint64_t, uint32_t);
   void __lshlti3(__int128&, uint64_t, uint64_t, uint32_t);
   void __lshrti3(__int128&, uint64_t, uint64_t, uint32_t);
   void __divti3(__int128&, uint64_t, uint64_t, uint64_t, uint64_t);
   void __udivti3(unsigned __int128&, uint64_t, uint64_t, uint64_t, uint64_t);
   void __multi3(__int128&, uint64_t, uint64_t, uint64_t, uint64_t);
   void __modti3(__int128&, uint64_t, uint64_t, uint64_t, uint64_t);
   void __umodti3(unsigned __int128&, uint64_t, uint64_t, uint64_t, uint64_t);
}
