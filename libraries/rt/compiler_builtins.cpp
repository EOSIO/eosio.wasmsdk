#include "compiler_builtins.hpp"
#include <stdint.h>

static constexpr uint32_t SHIFT_WIDTH = (sizeof(uint64_t)*8)-1;

extern "C" {
void eosio_assert(int32_t, const char*);
void __ashlti3(__int128& ret, uint64_t low, uint64_t high, uint32_t shift) {
   __int128 i = high;
   i <<= 64;
   i |= low;
   ret = ___ashlti3(i, shift);
}

void __ashrti3(__int128& ret, uint64_t low, uint64_t high, uint32_t shift) {
   __int128 i = high;
   i <<= 64;
   i |= low;
   ret = ___ashrti3(i, shift);
}

void __lshlti3(__int128& ret, uint64_t low, uint64_t high, uint32_t shift) {
   __int128 i = high;
   i <<= 64;
   i |= low;
   ret = ___ashlti3(i, shift);
}

void __lshrti3(__int128& ret, uint64_t low, uint64_t high, uint32_t shift) {
   __int128 i = high;
   i <<= 64;
   i |= low;
   ret = ___lshrti3(i, shift);
}

void __divti3(__int128& ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
   __int128 lhs = ha;
   __int128 rhs = hb;

   lhs <<= 64;
   lhs |=  la;

   rhs <<= 64;
   rhs |=  lb;

   ret = ___divti3(lhs, rhs);
}

void __udivti3(unsigned __int128& ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
   unsigned __int128 lhs = ha;
   unsigned __int128 rhs = hb;

   lhs <<= 64;
   lhs |=  la;

   rhs <<= 64;
   rhs |=  lb;

   ret = ___udivti3(lhs, rhs);
}

void __multi3(__int128& ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
   __int128 lhs = ha;
   __int128 rhs = hb;

   lhs <<= 64;
   lhs |=  la;

   rhs <<= 64;
   rhs |=  lb;

   ret = ___multi3(lhs, rhs);
}

void __modti3(__int128& ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
   __int128 lhs = ha;
   __int128 rhs = hb;

   lhs <<= 64;
   lhs |=  la;

   rhs <<= 64;
   rhs |=  lb;

   ret = ___modti3(lhs, rhs);
}

void __umodti3(unsigned __int128& ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
   unsigned __int128 lhs = ha;
   unsigned __int128 rhs = hb;

   lhs <<= 64;
   lhs |=  la;

   rhs <<= 64;
   rhs |=  lb;

   ret = ___umodti3(lhs, rhs);
}

// arithmetic long double
void __addtf3( float128_t& ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   ret = f128_add( a, b );
}
void __subtf3( float128_t& ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   ret = f128_sub( a, b );
}
void __multf3( float128_t& ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   ret = f128_mul( a, b );
}
void __divtf3( float128_t& ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   ret = f128_div( a, b );
}
void __negtf2( float128_t& ret, uint64_t la, uint64_t ha ) {
   ret = {{ la, (ha ^ (uint64_t)1 << 63) }};
}

// conversion long double
void __extendsftf2( float128_t& ret, float f ) {
   ret = f32_to_f128( to_softfloat32(f) );
}
void __extenddftf2( float128_t& ret, double d ) {
   ret = f64_to_f128( to_softfloat64(d) );
}
double __trunctfdf2( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return from_softfloat64(f128_to_f64( f ));
}
float __trunctfsf2( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return from_softfloat32(f128_to_f32( f ));
}
int32_t __fixtfsi( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return f128_to_i32( f, 0, false );
}
int64_t __fixtfdi( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return f128_to_i64( f, 0, false );
}
void __fixtfti( __int128& ret, uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   ret = ___fixtfti( f );
}
uint32_t __fixunstfsi( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return f128_to_ui32( f, 0, false );
}
uint64_t __fixunstfdi( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return f128_to_ui64( f, 0, false );
}
void __fixunstfti( unsigned __int128& ret, uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   ret = ___fixunstfti( f );
}
void __fixsfti( __int128& ret, float a ) {
   ret = ___fixsfti( to_softfloat32(a).v );
}
void __fixdfti( __int128& ret, double a ) {
   ret = ___fixdfti( to_softfloat64(a).v );
}
void __fixunssfti( unsigned __int128& ret, float a ) {
   ret = ___fixunssfti( to_softfloat32(a).v );
}
void __fixunsdfti( unsigned __int128& ret, double a ) {
   ret = ___fixunsdfti( to_softfloat64(a).v );
}
double __floatsidf( int32_t i ) {
   return from_softfloat64(i32_to_f64(i));
}
void __floatsitf( float128_t& ret, int32_t i ) {
   ret = i32_to_f128(i);
}
void __floatditf( float128_t& ret, uint64_t a ) {
   ret = i64_to_f128( a );
}
void __floatunsitf( float128_t& ret, uint32_t i ) {
   ret = ui32_to_f128(i);
}
void __floatunditf( float128_t& ret, uint64_t a ) {
   ret = ui64_to_f128( a );
}
double __floattidf( uint64_t l, uint64_t h ) {
   uint128_t val = h;
   val <<= 64;
   val |= l;
   return ___floattidf( *(__int128*)&val );
}
double __floatuntidf( uint64_t l, uint64_t h ) {
   uint128_t v = h;
   v <<= 64;
   v |= l;
   return ___floatuntidf( (unsigned __int128)v );
}
int __unordtf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   if ( f128_is_nan(a) || f128_is_nan(b) )
      return 1;
   return 0;
}
int ___cmptf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb, int return_value_if_nan ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   if ( __unordtf2(la, ha, lb, hb) )
      return return_value_if_nan;
   if ( f128_lt( a, b ) )
      return -1;
   if ( f128_eq( a, b ) )
      return 0;
   return 1;
}
int __eqtf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return ___cmptf2(la, ha, lb, hb, 1);
}
int __netf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return ___cmptf2(la, ha, lb, hb, 1);
}
int __getf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return ___cmptf2(la, ha, lb, hb, -1);
}
int __gttf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return ___cmptf2(la, ha, lb, hb, 0);
}
int __letf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return ___cmptf2(la, ha, lb, hb, 1);
}
int __lttf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return ___cmptf2(la, ha, lb, hb, 0);
}
int __cmptf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return ___cmptf2(la, ha, lb, hb, 1);
}
}

