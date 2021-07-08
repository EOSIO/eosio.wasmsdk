/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once

#include "check.hpp"
#include "name.hpp"
#include "serialize.hpp"
#include "print.hpp"
#include "datastream.hpp"

#include <tuple>
#include <limits>
#include <string_view>

namespace eosio {
  /**
   *  @defgroup symbol Symbol
   *  @ingroup core
   *  @brief Defines C++ API for managing symbols
   */

   /**
    *  Stores the symbol code as a uint64_t value
    *
    *  @ingroup symbol
    */
   class symbol_code {
   public:

      /**
       * Default constructor, construct a new symbol_code
       *
       * @brief Construct a new symbol_code object defaulting to a value of 0
       *
       */
      constexpr symbol_code() : value(0) {}

      /**
       * Construct a new symbol_code given a scoped enumerated type of raw (uint64_t).
       *
       * @brief Construct a new symbol_code object initialising value with raw
       * @param raw - The raw value which is a scoped enumerated type of unit64_t
       *
       */
      constexpr explicit symbol_code( uint64_t raw )
      :value(raw)
      {}

      /**
       * Construct a new symbol_code given an string.
       *
       * @brief Construct a new symbol_code object initialising value with str
       * @param str - The string value which validated then converted to unit64_t
       *
       */
      constexpr explicit symbol_code( std::string_view str )
      :value(0)
      {
         if( str.size() > 7 ) {
            eosio::check( false, "string is too long to be a valid symbol_code" );
         }
         for( auto itr = str.rbegin(); itr != str.rend(); ++itr ) {
            if( *itr < 'A' || *itr > 'Z') {
               eosio::check( false, "only uppercase letters allowed in symbol_code string" );
            }
            value <<= 8;
            value |= *itr;
         }
      }

      /**
       * Checks if the symbol code is valid
       * @return true - if symbol is valid
       */
      constexpr bool is_valid()const {
         auto sym = value;
         for ( int i=0; i < 7; i++ ) {
            char c = (char)(sym & 0xFF);
            if ( !('A' <= c && c <= 'Z') ) return false;
            sym >>= 8;
            if ( !(sym & 0xFF) ) {
               do {
                  sym >>= 8;
                  if ( (sym & 0xFF) ) return false;
                  i++;
               } while( i < 7 );
            }
         }
         return true;
      }

      /**
       * Returns the character length of the provided symbol
       *
       * @return length - character length of the provided symbol
       */
      constexpr uint32_t length()const {
         auto sym = value;
         uint32_t len = 0;
         while (sym & 0xFF && len <= 7) {
            len++;
            sym >>= 8;
         }
         return len;
      }

      /**
       * Casts a symbol code to raw
       *
       * @return Returns an instance of raw based on the value of a symbol_code
       */
      constexpr uint64_t raw()const { return value; }

      /**
       * Explicit cast to bool of the symbol_code
       *
       * @return Returns true if the symbol_code is set to the default value of 0 else true.
       */
      constexpr explicit operator bool()const { return value != 0; }

      /**
       *  Writes the symbol_code as a string to the provided char buffer
       *
       *
       *  @brief Writes the symbol_code as a string to the provided char buffer
       *  @pre is_valid() == true
       *  @pre The range [begin, end) must be a valid range of memory to write to.
       *  @param begin - The start of the char buffer
       *  @param end - Just past the end of the char buffer
       *  @param dry_run - If true, do not actually write anything into the range.
       *  @return char* - Just past the end of the last character that would be written assuming dry_run == false and end was large enough to provide sufficient space. (Meaning only applies if returned pointer >= begin.)
       *  @post If the output string fits within the range [begin, end) and dry_run == false, the range [begin, returned pointer) contains the string representation of the symbol_code. Nothing is written if dry_run == true or returned pointer > end (insufficient space) or if returned pointer < begin (overflow in calculating desired end).
       */
      char* write_as_string( char* begin, char* end, bool dry_run = false )const {
         constexpr uint64_t mask = 0xFFull;

         if( dry_run || (begin + 7 < begin) || (begin + 7 > end) ) {
            char* actual_end = begin + length();
            if( dry_run || (actual_end < begin) || (actual_end > end) ) return actual_end;
         }

         auto v = value;
         for( auto i = 0; i < 7; ++i, v >>= 8 ) {
            if( v == 0 ) return begin;

            *begin = static_cast<char>(v & mask);
            ++begin;
         }

         return begin;
      }

      /**
       *  Returns the name value as a string by calling write_as_string() and returning the buffer produced by write_as_string()
       */
      std::string to_string()const {
         char buffer[7];
         auto end = write_as_string( buffer, buffer + sizeof(buffer) );
         return {buffer, end};
      }

      /**
       * Prints a symbol_code
       *
       * @param sym_code symbol code to be printed
       */
      inline void print()const {
         char buffer[7];
         auto end = write_as_string( buffer, buffer + sizeof(buffer) );
         if( buffer < end )
           printl( buffer, (end-buffer) );
      }

      /**
       * Equivalency operator. Returns true if a == b (are the same)
       *
       * @return boolean - true if both provided symbol_codes are the same
       */
      friend constexpr bool operator == ( const symbol_code& a, const symbol_code& b ) {
         return a.value == b.value;
      }

      /**
       * Inverted equivalency operator. Returns true if a != b (are different)
       *
       * @return boolean - true if both provided symbol_codes are not the same
       */
      friend constexpr bool operator != ( const symbol_code& a, const symbol_code& b ) {
         return a.value != b.value;
      }

      /**
       * Less than operator. Returns true if a < b.
       * @brief Less than operator
       * @return boolean - true if symbol_code `a` is less than `b`
       */
      friend constexpr bool operator < ( const symbol_code& a, const symbol_code& b ) {
         return a.value < b.value;
      }

      /**
       * Greater than operator. Returns true if a > b.
       * @brief Greater than operator
       * @return boolean - true if symbol_code `a` is greater than `b`
       */
      friend constexpr bool operator > ( const symbol_code& a, const symbol_code& b ) {
         return a.value > b.value;
      }

      /**
       * Comparison operator. Returns true if a <= b.
       * @brief Comparison operator
       * @return boolean - true if symbol_code `a` is less or equal `b`
       */
      friend constexpr bool operator <= ( const symbol_code& a, const symbol_code& b ) {
         return a.value <= b.value;
      }

      /**
       * Comparison operator. Returns true if a >= b.
       * @brief Comparison operator
       * @return boolean - true if symbol_code `a` is greater or equal `b`
       */
      friend constexpr bool operator >= ( const symbol_code& a, const symbol_code& b ) {
         return a.value >= b.value;
      }

   private:
      uint64_t value = 0;
   };

   /**
    *  Serialize a symbol_code into a stream
    *
    *  @param ds - The stream to write
    *  @param sym - The value to serialize
    *  @tparam DataStream - Type of datastream buffer
    *  @return DataStream& - Reference to the datastream
    */
   template<typename DataStream>
   inline DataStream& operator<<(DataStream& ds, const eosio::symbol_code sym_code) {
     uint64_t raw = sym_code.raw();
     ds.write( (const char*)&raw, sizeof(raw));
     return ds;
   }

   /**
    *  Deserialize a symbol_code from a stream
    *
    *  @param ds - The stream to read
    *  @param symbol - The destination for deserialized value
    *  @tparam DataStream - Type of datastream buffer
    *  @return DataStream& - Reference to the datastream
    */
   template<typename DataStream>
   inline DataStream& operator>>(DataStream& ds, eosio::symbol_code& sym_code) {
     uint64_t raw = 0;
     ds.read((char*)&raw, sizeof(raw));
     sym_code = symbol_code(raw);
     return ds;
   }

   /**
    *  Stores information about a symbol, the symbol can be 7 characters long.
    *
    *  @ingroup symbol
    */
   class symbol {
   public:
      /**
       * Construct a new symbol object defaulting to a value of 0
       */
      constexpr symbol() : value(0) {}

      /**
       * Construct a new symbol given a scoped enumerated type of raw (uint64_t).
       *
       * @param raw - The raw value which is a scoped enumerated type of unit64_t
       */
      constexpr explicit symbol( uint64_t raw ) : value(raw) {}

      /**
       * Construct a new symbol given a symbol_code and a uint8_t precision.
       *
       * @param sc - The symbol_code
       * @param precision - The number of decimal places used for the symbol
       */
      constexpr symbol( symbol_code sc, uint8_t precision )
      : value( (sc.raw() << 8) | static_cast<uint64_t>(precision) )
      {}

      /**
       * Construct a new symbol given a string and a uint8_t precision.
       *
       * @param ss - The string containing the symbol
       * @param precision - The number of decimal places used for the symbol
       */
      constexpr symbol( std::string_view ss, uint8_t precision )
      : value( (symbol_code(ss).raw() << 8)  | static_cast<uint64_t>(precision) )
      {}

      /**
       * Is this symbol valid
       */
      constexpr bool is_valid()const                 { return code().is_valid(); }

      /**
       * This symbol's precision
       */
      constexpr uint8_t precision()const             { return static_cast<uint8_t>( value & 0xFFull ); }

      /**
       * Returns representation of symbol name
       */
      constexpr symbol_code code()const              { return symbol_code{value >> 8};   }

      /**
       * Returns uint64_t repreresentation of the symbol
       */
      constexpr uint64_t raw()const                  { return value; }

      constexpr explicit operator bool()const { return value != 0; }

      /**
       * %Print the symbol
       */
      void print( bool show_precision = true )const {
         if( show_precision ){
            ::eosio::print( static_cast<uint64_t>(precision()), "," );
         }
         char buffer[7];
         auto end = code().write_as_string( buffer, buffer + sizeof(buffer) );
         if( buffer < end )
            printl( buffer, (end-buffer) );
      }

      /**
       * Equivalency operator. Returns true if a == b (are the same)
       *
       * @return boolean - true if both provided symbols are the same
       */
      friend constexpr bool operator == ( const symbol& a, const symbol& b ) {
         return a.value == b.value;
      }

      /**
       * Inverted equivalency operator. Returns true if a != b (are different)
       *
       * @return boolean - true if both provided symbols are not the same
       */
      friend constexpr bool operator != ( const symbol& a, const symbol& b ) {
         return a.value != b.value;
      }

      /**
       * Less than operator. Returns true if a < b.
       * @brief Less than operator
       * @return boolean - true if symbol `a` is less than `b`
       */
      friend constexpr bool operator < ( const symbol& a, const symbol& b ) {
         return a.value < b.value;
      }

      /**
       * Greater than operator. Returns true if a > b.
       * @brief Greater than operator
       * @return boolean - true if symbol `a` is greater than `b`
       */
      friend constexpr bool operator > ( const symbol& a, const symbol& b ) {
         return a.value > b.value;
      }

      /**
       * Comparison operator. Returns true if a <= b.
       * @brief Comparison operator
       * @return boolean - true if symbol `a` is less or equal `b`
       */
      friend constexpr bool operator <= ( const symbol& a, const symbol& b ) {
         return a.value <= b.value;
      }

      /**
       * Comparison operator. Returns true if a >= b.
       * @brief Comparison operator
       * @return boolean - true if symbol `a` is greater or equal `b`
       */
      friend constexpr bool operator >= ( const symbol& a, const symbol& b ) {
         return a.value >= b.value;
      }

   private:
      uint64_t value = 0;
   };

   /**
    *  Serialize a symbol into a stream
    *
    *  @brief Serialize a symbol
    *  @param ds - The stream to write
    *  @param sym - The value to serialize
    *  @tparam DataStream - Type of datastream buffer
    *  @return DataStream& - Reference to the datastream
    */
   template<typename DataStream>
   inline DataStream& operator<<(DataStream& ds, const eosio::symbol sym) {
     uint64_t raw = sym.raw();
     ds.write( (const char*)&raw, sizeof(raw));
     return ds;
   }

   /**
    *  Deserialize a symbol from a stream
    *
    *  @brief Deserialize a symbol
    *  @param ds - The stream to read
    *  @param symbol - The destination for deserialized value
    *  @tparam DataStream - Type of datastream buffer
    *  @return DataStream& - Reference to the datastream
    */
   template<typename DataStream>
   inline DataStream& operator>>(DataStream& ds, eosio::symbol& sym) {
     uint64_t raw = 0;
     ds.read((char*)&raw, sizeof(raw));
     sym = symbol(raw);
     return ds;
   }

   /**
    *  Extended asset which stores the information of the owner of the symbol
    *
    *  @ingroup symbol
    */
   class extended_symbol
   {
   public:

      /**
       * Default constructor, construct a new extended_symbol
       */
      constexpr extended_symbol() {}

      /**
       * Construct a new symbol_code object initialising symbol and contract with the passed in symbol and name
       *
       * @param sym - The symbol
       * @param con - The name of the contract
       */
      constexpr extended_symbol( symbol s, name con ) : sym(s), contract(con) {}

      /**
       * Returns the symbol in the extended_contract
       *
       * @return symbol
       */
      constexpr symbol get_symbol() const { return sym; }

      /**
       * Returns the name of the contract in the extended_symbol
       *
       * @return name
       */
      constexpr name  get_contract() const { return contract; }

      /**
       * %Print the extended symbol
       *
       * @brief %Print the extended symbol
       */
      void print( bool show_precision = true )const {
         sym.print( show_precision );
         ::eosio::print("@", contract);
      }

      /**
       * Equivalency operator. Returns true if a == b (are the same)
       *
       * @return boolean - true if both provided extended_symbols are the same
       */
      friend constexpr bool operator == ( const extended_symbol& a, const extended_symbol& b ) {
        return std::tie( a.sym, a.contract ) == std::tie( b.sym, b.contract );
      }

      /**
       * Inverted equivalency operator. Returns true if a != b (are different)
       *
       * @return boolean - true if both provided extended_symbols are not the same
       */
      friend constexpr bool operator != ( const extended_symbol& a, const extended_symbol& b ) {
        return std::tie( a.sym, a.contract ) != std::tie( b.sym, b.contract );
      }

      /**
       * Less than operator. Returns true if a < b.
       *
       * @return boolean - true if extended_symbol `a` is less than `b`
       */
      friend constexpr bool operator < ( const extended_symbol& a, const extended_symbol& b ) {
        return std::tie( a.sym, a.contract ) < std::tie( b.sym, b.contract );
      }

   private:
      symbol sym; ///< the symbol
      name   contract; ///< the token contract hosting the symbol

      EOSLIB_SERIALIZE( extended_symbol, (sym)(contract) )
   };
}
