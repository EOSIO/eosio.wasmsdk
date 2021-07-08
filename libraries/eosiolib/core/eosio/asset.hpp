#pragma once

#include "serialize.hpp"
#include "print.hpp"
#include "check.hpp"
#include "symbol.hpp"

#include <tuple>
#include <limits>

namespace eosio {

   char* write_decimal( char* begin, char* end, bool dry_run, uint64_t number, uint8_t num_decimal_places, bool negative );

   /**
    *  @defgroup asset Asset
    *  @ingroup core
    *  @brief Defines C++ API for managing assets
    */

   /**
    *  Stores information for owner of asset
    *
    *  @ingroup asset
    */
   struct asset {
      /**
       * The amount of the asset
       */
      int64_t      amount = 0;

      /**
       * The symbol name of the asset
       */
      symbol  symbol;

      /**
       * Maximum amount possible for this asset. It's capped to 2^62 - 1
       */
      static constexpr int64_t max_amount    = (1LL << 62) - 1;

      asset() {}

      /**
       * Construct a new asset given the symbol name and the amount
       *
       * @param a - The amount of the asset
       * @param s - The name of the symbol
       */
      asset( int64_t a, class symbol s )
      :amount(a),symbol{s}
      {
         eosio::check( is_amount_within_range(), "magnitude of asset amount must be less than 2^62" );
         eosio::check( symbol.is_valid(),        "invalid symbol name" );
      }

      /**
       * Check if the amount doesn't exceed the max amount
       *
       * @return true - if the amount doesn't exceed the max amount
       * @return false - otherwise
       */
      bool is_amount_within_range()const { return -max_amount <= amount && amount <= max_amount; }

      /**
       * Check if the asset is valid. %A valid asset has its amount <= max_amount and its symbol name valid
       *
       * @return true - if the asset is valid
       * @return false - otherwise
       */
      bool is_valid()const               { return is_amount_within_range() && symbol.is_valid(); }

      /**
       * Set the amount of the asset
       *
       * @param a - New amount for the asset
       */
      void set_amount( int64_t a ) {
         amount = a;
         eosio::check( is_amount_within_range(), "magnitude of asset amount must be less than 2^62" );
      }

      /// @cond OPERATORS

      /**
       * Unary minus operator
       *
       * @return asset - New asset with its amount is the negative amount of this asset
       */
      asset operator-()const {
         asset r = *this;
         r.amount = -r.amount;
         return r;
      }

      /**
       * Subtraction assignment operator
       *
       * @param a - Another asset to subtract this asset with
       * @return asset& - Reference to this asset
       * @post The amount of this asset is subtracted by the amount of asset a
       */
      asset& operator-=( const asset& a ) {
         eosio::check( a.symbol == symbol, "attempt to subtract asset with different symbol" );
         amount -= a.amount;
         eosio::check( -max_amount <= amount, "subtraction underflow" );
         eosio::check( amount <= max_amount,  "subtraction overflow" );
         return *this;
      }

      /**
       * Addition Assignment  operator
       *
       * @param a - Another asset to subtract this asset with
       * @return asset& - Reference to this asset
       * @post The amount of this asset is added with the amount of asset a
       */
      asset& operator+=( const asset& a ) {
         eosio::check( a.symbol == symbol, "attempt to add asset with different symbol" );
         amount += a.amount;
         eosio::check( -max_amount <= amount, "addition underflow" );
         eosio::check( amount <= max_amount,  "addition overflow" );
         return *this;
      }

      /**
       * Addition operator
       *
       * @param a - The first asset to be added
       * @param b - The second asset to be added
       * @return asset - New asset as the result of addition
       */
      inline friend asset operator+( const asset& a, const asset& b ) {
         asset result = a;
         result += b;
         return result;
      }

      /**
       * Subtraction operator
       *
       * @param a - The asset to be subtracted
       * @param b - The asset used to subtract
       * @return asset - New asset as the result of subtraction of a with b
       */
      inline friend asset operator-( const asset& a, const asset& b ) {
         asset result = a;
         result -= b;
         return result;
      }

      /**
       * Multiplication assignment operator, with a number
       *
       * @details Multiplication assignment operator. Multiply the amount of this asset with a number and then assign the value to itself.
       * @param a - The multiplier for the asset's amount
       * @return asset - Reference to this asset
       * @post The amount of this asset is multiplied by a
       */
      asset& operator*=( int64_t a ) {
         int128_t tmp = (int128_t)amount * (int128_t)a;
         eosio::check( tmp <= max_amount, "multiplication overflow" );
         eosio::check( tmp >= -max_amount, "multiplication underflow" );
         amount = (int64_t)tmp;
         return *this;
      }

      /**
       * Multiplication operator, with a number proceeding
       *
       * @brief Multiplication operator, with a number proceeding
       * @param a - The asset to be multiplied
       * @param b - The multiplier for the asset's amount
       * @return asset - New asset as the result of multiplication
       */
      friend asset operator*( const asset& a, int64_t b ) {
         asset result = a;
         result *= b;
         return result;
      }


      /**
       * Multiplication operator, with a number preceeding
       *
       * @param a - The multiplier for the asset's amount
       * @param b - The asset to be multiplied
       * @return asset - New asset as the result of multiplication
       */
      friend asset operator*( int64_t b, const asset& a ) {
         asset result = a;
         result *= b;
         return result;
      }

      /**
       * @brief Division assignment operator, with a number
       *
       * @details Division assignment operator. Divide the amount of this asset with a number and then assign the value to itself.
       * @param a - The divisor for the asset's amount
       * @return asset - Reference to this asset
       * @post The amount of this asset is divided by a
       */
      asset& operator/=( int64_t a ) {
         eosio::check( a != 0, "divide by zero" );
         eosio::check( !(amount == std::numeric_limits<int64_t>::min() && a == -1), "signed division overflow" );
         amount /= a;
         return *this;
      }

      /**
       * Division operator, with a number proceeding
       *
       * @param a - The asset to be divided
       * @param b - The divisor for the asset's amount
       * @return asset - New asset as the result of division
       */
      friend asset operator/( const asset& a, int64_t b ) {
         asset result = a;
         result /= b;
         return result;
      }

      /**
       * Division operator, with another asset
       *
       * @param a - The asset which amount acts as the dividend
       * @param b - The asset which amount acts as the divisor
       * @return int64_t - the resulted amount after the division
       * @pre Both asset must have the same symbol
       */
      friend int64_t operator/( const asset& a, const asset& b ) {
         eosio::check( b.amount != 0, "divide by zero" );
         eosio::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
         return a.amount / b.amount;
      }

      /**
       * Equality operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if both asset has the same amount
       * @return false - otherwise
       * @pre Both asset must have the same symbol
       */
      friend bool operator==( const asset& a, const asset& b ) {
         eosio::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
         return a.amount == b.amount;
      }

      /**
       * Inequality operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if both asset doesn't have the same amount
       * @return false - otherwise
       * @pre Both asset must have the same symbol
       */
      friend bool operator!=( const asset& a, const asset& b ) {
         return !( a == b);
      }

      /**
       * Less than operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if the first asset's amount is less than the second asset amount
       * @return false - otherwise
       * @pre Both asset must have the same symbol
       */
      friend bool operator<( const asset& a, const asset& b ) {
         eosio::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
         return a.amount < b.amount;
      }

      /**
       * Less or equal to operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if the first asset's amount is less or equal to the second asset amount
       * @return false - otherwise
       * @pre Both asset must have the same symbol
       */
      friend bool operator<=( const asset& a, const asset& b ) {
         eosio::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
         return a.amount <= b.amount;
      }

      /**
       * Greater than operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if the first asset's amount is greater than the second asset amount
       * @return false - otherwise
       * @pre Both asset must have the same symbol
       */
      friend bool operator>( const asset& a, const asset& b ) {
         eosio::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
         return a.amount > b.amount;
      }

      /**
       * Greater or equal to operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if the first asset's amount is greater or equal to the second asset amount
       * @return false - otherwise
       * @pre Both asset must have the same symbol
       */
      friend bool operator>=( const asset& a, const asset& b ) {
         eosio::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
         return a.amount >= b.amount;
      }

      /// @endcond

      /**
       *  Writes the asset as a string to the provided char buffer
       *
       *  @brief Writes the asset as a string to the provided char buffer
       *  @pre is_valid() == true
       *  @pre The range [begin, end) must be a valid range of memory to write to.
       *  @param begin - The start of the char buffer
       *  @param end - Just past the end of the char buffer
       *  @param dry_run - If true, do not actually write anything into the range.
       *  @return char* - Just past the end of the last character that would be written assuming dry_run == false and end was large enough to provide sufficient space. (Meaning only applies if returned pointer >= begin.)
       *  @post If the output string fits within the range [begin, end) and dry_run == false, the range [begin, returned pointer) contains the string representation of the asset. Nothing is written if dry_run == true or returned pointer > end (insufficient space) or if returned pointer < begin (overflow in calculating desired end).
       */
      char* write_as_string( char* begin, char* end, bool dry_run = false )const {
         bool negative = (amount < 0);
         uint64_t abs_amount = static_cast<uint64_t>(negative ? -amount : amount);
         // 0 <= abs_amount <= std::numeric_limits<int64_t>::max() < 10^19 < std::numeric_limits<uint64_t>::max()

         uint8_t precision = symbol.precision();

         int sufficient_size = std::max(static_cast<int>(precision), 19) + 11;
         if( dry_run || (begin + sufficient_size < begin) || (begin + sufficient_size > end) ) {
            char* start_of_symbol = write_decimal( begin, end, true, abs_amount, precision, negative ) + 1;
            char* actual_end = symbol.code().write_as_string( start_of_symbol, end, true );
            if( dry_run || (actual_end < begin) || (actual_end > end) ) return actual_end;
         }

         char* end_of_number = write_decimal( begin, end, false, abs_amount, precision, negative );
         *(end_of_number) = ' ';

         return symbol.code().write_as_string( end_of_number + 1, end );
      }

      /**
       * %asset to std::string
       *
       * @brief %asset to std::string
       */
      std::string to_string()const {
         int buffer_size = std::max(static_cast<int>(symbol.precision()), 19) + 11;
         char buffer[buffer_size];
         char* end = write_as_string( buffer, buffer + buffer_size );
         check( end <= buffer + buffer_size, "insufficient space in buffer" ); // should never fail

         return {buffer, end};
      }

      /**
       * %Print the asset
       *
       * @brief %Print the asset
       */
      void print()const {
         int buffer_size = std::max(static_cast<int>(symbol.precision()), 19) + 11;
         char buffer[buffer_size];
         char* end = write_as_string( buffer, buffer + buffer_size );
         check( end <= buffer + buffer_size, "insufficient space in buffer" ); // should never fail

         if( buffer < end )
            printl( buffer, (end-buffer) );
      }

      EOSLIB_SERIALIZE( asset, (amount)(symbol) )
   };

  /**
   *  Extended asset which stores the information of the owner of the asset
   *
   *  @ingroup asset
   */
   struct extended_asset {
      /**
       * The asset
       */
      asset quantity;

      /**
       * The owner of the asset
       */
      name contract;

      /**
       * Get the extended symbol of the asset
       *
       * @return extended_symbol - The extended symbol of the asset
       */
      extended_symbol get_extended_symbol()const { return extended_symbol{ quantity.symbol, contract }; }

      /**
       * Default constructor
       */
      extended_asset() = default;

       /**
       * Construct a new extended asset given the amount and extended symbol
       */
      extended_asset( int64_t v, extended_symbol s ):quantity(v,s.get_symbol()),contract(s.get_contract()){}
      /**
       * Construct a new extended asset given the asset and owner name
       */
      extended_asset( asset a, name c ):quantity(a),contract(c){}

      /**
       * %Print the extended asset
       */
      void print()const {
         quantity.print();
         ::eosio::print("@", contract);
      }

      /// @cond OPERATORS

      // Unary minus operator
      extended_asset operator-()const {
         return {-quantity, contract};
      }

      // Subtraction operator
      friend extended_asset operator - ( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return {a.quantity - b.quantity, a.contract};
      }

      // Addition operator
      friend extended_asset operator + ( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return {a.quantity + b.quantity, a.contract};
      }

      /// Addition operator.
      friend extended_asset& operator+=( extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         a.quantity += b.quantity;
         return a;
      }

      /// Subtraction operator.
      friend extended_asset& operator-=( extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         a.quantity -= b.quantity;
         return a;
      }

      /// Less than operator
      friend bool operator<( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return a.quantity < b.quantity;
      }

      /// Greater than operator
      friend bool operator>( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return a.quantity > b.quantity;
      }

      /// Comparison operator
      friend bool operator==( const extended_asset& a, const extended_asset& b ) {
         return std::tie(a.quantity, a.contract) == std::tie(b.quantity, b.contract);
      }

      /// Comparison operator
      friend bool operator!=( const extended_asset& a, const extended_asset& b ) {
         return std::tie(a.quantity, a.contract) != std::tie(b.quantity, b.contract);
      }

      /// Comparison operator
      friend bool operator<=( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return a.quantity <= b.quantity;
      }

      /// Comparison operator
      friend bool operator>=( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return a.quantity >= b.quantity;
      }

      /// @endcond

      EOSLIB_SERIALIZE( extended_asset, (quantity)(contract) )
   };
}
