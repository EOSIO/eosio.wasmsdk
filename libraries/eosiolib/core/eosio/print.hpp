/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once
#include <utility>
#include <string>
#include <string_view>
#include <type_traits>

namespace eosio {
   namespace internal_use_do_not_use {
      extern "C" {
         __attribute__((eosio_wasm_import))
         void prints(const char*);

         __attribute__((eosio_wasm_import))
         void prints_l(const char*, uint32_t);

         __attribute__((eosio_wasm_import))
         void printi(int64_t);

         __attribute__((eosio_wasm_import))
         void printui(uint64_t);

         __attribute__((eosio_wasm_import))
         void printi128(const int128_t*);

         __attribute__((eosio_wasm_import))
         void printui128(const uint128_t*);

         __attribute__((eosio_wasm_import))
         void printsf(float);

         __attribute__((eosio_wasm_import))
         void printdf(double);

         __attribute__((eosio_wasm_import))
         void printqf(const long double*);

         __attribute__((eosio_wasm_import))
         void printn(uint64_t);

         __attribute__((eosio_wasm_import))
         void printhex(const void*, uint32_t);
      }

   };

   /**
    *  @defgroup console Console
    *  @ingroup core
    *  @brief Defines C++ wrapper to log/print text messages
    *
    *  @details This API uses C++ variadic templates and type detection to
    *  make it easy to print any native type. You can even overload
    *  the `print()` method for your own custom types.
    *
    *  **Example:**
    *  ```
    *     print( "hello world, this is a number: ", 5 );
    *  ```
    *
    *  @section override Overriding Print for your Types
    *
    *  There are two ways to overload print:
    *  1. implement void print( const T& )
    *  2. implement T::print()const
    */

   /**
    *  Prints a block of bytes in hexadecimal
    *
    *  @ingroup console
    *  @param ptr  - pointer to bytes of interest
    *  @param size - number of bytes to print
    */
   inline void printhex( const void* ptr, uint32_t size) {
      internal_use_do_not_use::printhex(ptr, size);
   }

   /**
    *  Prints string to a given length
    *
    *  @ingroup console
    *  @param ptr - a string
    *  @param len - number of chars to print
    */
   inline void printl( const char* ptr, size_t len ) {
      internal_use_do_not_use::prints_l(ptr, len);
   }

   /**
    *  Prints string
    *
    *  @ingroup console
    *  @param ptr - a null terminated string
    */
   inline void print( const char* ptr ) {
      internal_use_do_not_use::prints(ptr);
   }

   /**
    *  Prints string
    *
    *  @ingroup console
    *  @param str - an std::string
    */
   inline void print( const std::string& str ) {
      internal_use_do_not_use::prints_l(str.c_str(), str.size());
   }

   /**
    *  Prints string
    *
    *  @ingroup console
    *  @param str - an std::string_view
    */
   inline void print( std::string_view str ) {
      internal_use_do_not_use::prints_l(str.data(), str.size());
   }

   /**
    * Prints 8-128 bit signed integer
    *
    * @param num to be printed
    */
   template <typename T, std::enable_if_t<std::is_integral<std::decay_t<T>>::value &&
                                          std::is_signed<std::decay_t<T>>::value, int> = 0>
   inline void print( T num ) {
      if constexpr(std::is_same<T, int128_t>::value)
        internal_use_do_not_use::printi128(&num);
      else if constexpr(std::is_same<T, char>::value)
        internal_use_do_not_use::prints_l( &num, 1 );
      else
        internal_use_do_not_use::printi(num);
   }

   /**
    *  Prints 8-128 bit unsigned integer
    *
    *  @param num to be printed
    */
   template <typename T, std::enable_if_t<std::is_integral<std::decay_t<T>>::value &&
                                          !std::is_signed<std::decay_t<T>>::value, int> = 0>
   inline void print( T num ) {
      if constexpr(std::is_same<T, uint128_t>::value)
         internal_use_do_not_use::printui128(&num);
      else if constexpr(std::is_same<T, bool>::value)
         internal_use_do_not_use::prints(num?"true":"false");
      else
        internal_use_do_not_use::printui(num);
   }

   /**
    *  Prints single-precision floating point number (i.e. float)
    *
    *  @ingroup console
    *  @param num to be printed
    */
   inline void print( float num ) { internal_use_do_not_use::printsf( num ); }

   /**
    *  Prints double-precision floating point number (i.e. double)
    *
    *  @ingroup console
    *  @param num to be printed
    */
   inline void print( double num ) { internal_use_do_not_use::printdf( num ); }

   /**
    *  Prints quadruple-precision floating point number (i.e. long double)
    *
    *  @ingroup console
    *  @param num to be printed
    */
   inline void print( long double num ) { internal_use_do_not_use::printqf( &num ); }

  /**
    *  Prints class object
    *
    *  @ingroup console
    *  @param t to be printed
    *  @pre T must implement print() function
    */
   template <typename T>
   inline auto print(T&& t) -> std::void_t<decltype(t.print())> 
   {
      std::forward<T>(t).print();
   }

   /**
    *  Prints null terminated string
    *
    *  @ingroup console
    *  @param s null terminated string to be printed
    */
   inline void print_f( const char* s ) {
      internal_use_do_not_use::prints(s);
   }

   /**
    *  Prints formatted string. It behaves similar to C printf/
    *
    *  @tparam Arg - Type of the value used to replace the format specifier
    *  @tparam Args - Type of the value used to replace the format specifier
    *  @param s - Null terminated string with to be printed (it can contains format specifier)
    *  @param val - The value used to replace the format specifier
    *  @param rest - The values used to replace the format specifier
    *
    *  Example:
    *  @code
    *  print_f("Number of apples: %", 10);
    *  @endcode
    */
   template <typename Arg, typename... Args>
   inline void print_f( const char* s, Arg val, Args... rest ) {
      while ( *s != '\0' ) {
         if ( *s == '%' ) {
            print( val );
            print_f( s+1, rest... );
            return;
         }
         internal_use_do_not_use::prints_l( s, 1 );
         s++;
      }
   }

    /**
     *  Print out value / list of values
     *
     *  @tparam Arg - Type of the value used to replace the format specifier
     *  @tparam Args - Type of the value used to replace the format specifier
     *  @param a - The value to be printed
     *  @param args - The other values to be printed
     *
     *  Example:
     *
     *  @code
     *  const char *s = "Hello World!";
     *  uint64_t unsigned_64_bit_int = 1e+18;
     *  uint128_t unsigned_128_bit_int (87654323456);
     *  uint64_t string_as_unsigned_64_bit = "abcde"_n;
     *  print(s , unsigned_64_bit_int, unsigned_128_bit_int, string_as_unsigned_64_bit);
     *  // Ouput: Hello World!100000000000000000087654323456abcde
     *  @endcode
     */
   template<typename Arg, typename... Args>
   auto print( Arg&& a, Args&&... args ) -> std::enable_if_t<sizeof...(Args) != 0, void>
   {
      print(std::forward<Arg>(a));
      print(std::forward<Args>(args)...);
   }

   /**
    * Simulate C++ style streams
    *
    * @ingroup console
    */
   class iostream {};

   /// @cond OPERATORS

   /**
    *  Overload c++ iostream
    *
    *  @tparam Arg - Type of the value used to replace the format specifier
    *  @tparam Args - Type of the value used to replace the format specifier
    *  @param out - Output strem
    *  @param v - The value to be printed
    *  @return iostream& - Reference to the input output stream
    *
    *  Example:
    *
    *  @code
    *  const char *s = "Hello World!";
    *  uint64_t unsigned_64_bit_int = 1e+18;
    *  uint128_t unsigned_128_bit_int (87654323456);
    *  uint64_t string_as_unsigned_64_bit = "abcde"_n;
    *  std::out << s << " " << unsigned_64_bit_int << " "  << unsigned_128_bit_int << " " << string_as_unsigned_64_bit;
    *  // Output: Hello World! 1000000000000000000 87654323456 abcde
    *  @endcode
    */
   template<typename T>
   inline iostream& operator<<( iostream& out, const T& v ) {
      print( v );
      return out;
   }

   /// @endcond

   static iostream cout;
}
