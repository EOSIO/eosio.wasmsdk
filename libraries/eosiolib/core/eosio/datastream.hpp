/**
 *  @file datastream.hpp
 *  @copyright defined in eos/LICENSE
 */
#pragma once
#include "check.hpp"
#include "varint.hpp"
#include "print.hpp"

#include <list>
#include <queue>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <string>
#include <optional>
#include <variant>

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>

#include <boost/mp11/tuple.hpp>
#include <boost/pfr.hpp>

namespace eosio {

/**
 * @defgroup datastream Data Stream
 * @ingroup core
 * @brief Defines data stream for reading and writing data in the form of bytes
 */

/**
 *  A data stream for reading and writing data in the form of bytes
 *
 *  @tparam T - Type of the datastream buffer
 */
template<typename T>
class datastream {
   public:
      /**
       * Construct a new datastream object
       *
       * @details Construct a new datastream object given the size of the buffer and start position of the buffer
       * @param start - The start position of the buffer
       * @param s - The size of the buffer
       */
      datastream( T start, size_t s )
      :_start(start),_pos(start),_end(start+s){}

     /**
      *  Skips a specified number of bytes from this stream
      *
      *  @param s - The number of bytes to skip
      */
      inline void skip( size_t s ){ _pos += s; }

     /**
      *  Reads a specified number of bytes from the stream into a buffer
      *
      *  @param d - The pointer to the destination buffer
      *  @param s - the number of bytes to read
      *  @return true
      */
      inline bool read( char* d, size_t s ) {
        eosio::check( size_t(_end - _pos) >= (size_t)s, "datastream attempted to read past the end" );
        memcpy( d, _pos, s );
        _pos += s;
        return true;
      }

     /**
      *  Writes a specified number of bytes into the stream from a buffer
      *
      *  @param d - The pointer to the source buffer
      *  @param s - The number of bytes to write
      *  @return true
      */
      inline bool write( const char* d, size_t s ) {
        eosio::check( _end - _pos >= (int32_t)s, "datastream attempted to write past the end" );
        memcpy( (void*)_pos, d, s );
        _pos += s;
        return true;
      }

     /**
      *  Writes a specified byte into the stream from a buffer
      *
      *  @param d - The byte to be written
      *  @return true
      */
      inline bool write( char d ) {
        eosio::check( _end - _pos >= 1, "datastream attempted to write past the end" );
        *_pos++ = d;
        return true;
      }

     /**
      *  Writes a specified number of bytes into the stream from a buffer
      *
      *  @param d - The pointer to the source buffer
      *  @param s - The number of bytes to write
      *  @return true
      */
      inline bool write( const void* d, size_t s ) {
        eosio::check( _end - _pos >= (int32_t)s, "datastream attempted to write past the end" );
        memcpy( (void*)_pos, d, s );
        _pos += s;
        return true;
      }

     /**
      *  Writes a byte into the stream
      *
      *  @param c byte to write
      *  @return true
      */
      inline bool put(char c) {
        eosio::check( _pos < _end, "put" );
        *_pos = c;
        ++_pos;
        return true;
      }

     /**
      *  Reads a byte from the stream
      *
      *  @param c - The reference to destination byte
      *  @return true
      */
      inline bool get( unsigned char& c ) { return get( *(char*)&c ); }

     /**
      *  Reads a byte from the stream
      *
      *  @param c - The reference to destination byte
      *  @return true
      */
      inline bool get( char& c )
      {
        eosio::check( _pos < _end, "get" );
        c = *_pos;
        ++_pos;
        return true;
      }

     /**
      *  Retrieves the current position of the stream
      *
      *  @return T - The current position of the stream
      */
      T pos()const { return _pos; }
      inline bool valid()const { return _pos <= _end && _pos >= _start;  }

     /**
      *  Sets the position within the current stream
      *
      *  @param p - The offset relative to the origin
      *  @return true if p is within the range
      *  @return false if p is not within the rawnge
      */
      inline bool seekp(size_t p) { _pos = _start + p; return _pos <= _end; }

     /**
      *  Gets the position within the current stream
      *
      *  @return p - The position within the current stream
      */
      inline size_t tellp()const      { return size_t(_pos - _start); }

     /**
      *  Returns the number of remaining bytes that can be read/skipped
      *
      *  @return size_t - The number of remaining bytes
      */
      inline size_t remaining()const  { return _end - _pos; }
    private:
      /**
       * The start position of the buffer
       */
      T _start;
      /**
       * The current position of the buffer
       */
      T _pos;
      /**
       * The end position of the buffer
       */
      T _end;
};

/**
 * Specialization of datastream used to help determine the final size of a serialized value
 */
template<>
class datastream<size_t> {
   public:
      /**
       * Construct a new specialized datastream object given the initial size
       *
       * @param init_size - The initial size
       */
     datastream( size_t init_size = 0):_size(init_size){}

     /**
      *  Increment the size by s. This behaves the same as write( const char* ,size_t s ).
      *
      *  @param s - The amount of size to increase
      *  @return true
      */
     inline bool     skip( size_t s )                 { _size += s; return true;  }

     /**
      *  Increment the size by s. This behaves the same as skip( size_t s )
      *
      *  @param s - The amount of size to increase
      *  @return true
      */
     inline bool     write( const char* ,size_t s )  { _size += s; return true;  }

     /**
      *  Increment the size by s. This behaves the same as skip( size_t s )
      *
      *  @param s - The amount of size to increase
      *  @return true
      */
     inline bool     write( char )  { _size++; return true;  }

     /**
      *  Increment the size by s. This behaves the same as skip( size_t s )
      *
      *  @param s - The amount of size to increase
      *  @return true
      */
     inline bool     write( const void* ,size_t s )  { _size += s; return true;  }

     /**
      *  Increment the size by one
      *
      *  @return true
      */
     inline bool     put(char )                      { ++_size; return  true;    }

     /**
      *  Check validity. It's always valid
      *
      *  @return true
      */
     inline bool     valid()const                     { return true;              }

     /**
      * Set new size
      *
      * @param p - The new size
      * @return true
      */
     inline bool     seekp(size_t p)                  { _size = p;  return true;  }

     /**
      * Get the size
      *
      * @return size_t - The size
      */
     inline size_t   tellp()const                     { return _size;             }

     /**
      * Always returns 0
      *
      * @return size_t - 0
      */
     inline size_t   remaining()const                 { return 0;                 }
  private:
     /**
      * The size used to determine the final size of a serialized value.
      */
     size_t _size;
};

/**
 *  Serialize an std::list into a stream
 *
 *  @param ds - The stream to write
 *  @param opt - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::list<T>& l) {
   ds << unsigned_int( l.size() );
   for ( const auto& elem : l )
      ds << elem;
  return ds;
}

/**
 *  Deserialize an std::list from a stream
 *
 *  @param ds - The stream to read
 *  @param opt - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::list<T>& l) {
   unsigned_int s;
   ds >> s;
   l.resize(s.value);
   for( auto& i : l )
      ds >> i;
   return ds;
}

/**
 *  Serialize an std::deque into a stream
 *
 *  @param ds - The stream to write
 *  @param opt - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::deque<T>& d) {
   ds << unsigned_int( d.size() );
   for ( const auto& elem : d )
      ds << elem;
  return ds;
}

/**
 *  Deserialize an std::deque from a stream
 *
 *  @param ds - The stream to read
 *  @param opt - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::deque<T>& d) {
   unsigned_int s;
   ds >> s;
   d.resize(s.value);
   for( auto& i : d )
      ds >> i;
   return ds;
}

/**
 *  Serialize an std::variant into a stream
 *
 *  @param ds - The stream to write
 *  @param opt - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename... Ts>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::variant<Ts...>& var) {
  unsigned_int index = var.index();
  ds << index;
  std::visit([&ds](auto& val){ ds << val; }, var);
  return ds;
}

template<int I, typename Stream, typename... Ts>
void deserialize(datastream<Stream>& ds, std::variant<Ts...>& var, int i) {
   if constexpr (I < std::variant_size_v<std::variant<Ts...>>) {
      if (i == I) {
         std::variant_alternative_t<I, std::variant<Ts...>> tmp;
         ds >> tmp;
         var.template emplace<I>(std::move(tmp));
      } else {
         deserialize<I+1>(ds,var,i);
      }
   } else {
      eosio::check(false, "invalid variant index");
   }
}

/**
 *  Deserialize an std::variant from a stream
 *
 *  @param ds - The stream to read
 *  @param opt - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename... Ts>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::variant<Ts...>& var) {
  unsigned_int index;
  ds >> index;
  deserialize<0>(ds,var,index);
  return ds;
}

/**
 *  Serialize an std::pair
 *
 *  @param ds - The stream to write
 *  @param t - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @tparam Args - Type of the objects contained in the tuple
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T1, typename T2>
datastream<Stream>& operator<<( datastream<Stream>& ds, const std::pair<T1, T2>& t ) {
   ds << std::get<0>(t);
   ds << std::get<1>(t);
   return ds;
}

/**
 *  Deserialize an std::pair
 *
 *  @param ds - The stream to read
 *  @param t - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @tparam Args - Type of the objects contained in the tuple
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T1, typename T2>
datastream<Stream>& operator>>( datastream<Stream>& ds, std::pair<T1, T2>& t ) {
   T1 t1;
   T2 t2;
   ds >> t1;
   ds >> t2;
   t = std::pair<T1, T2>{t1, t2};
   return ds;
}

/**
 *  Serialize an optional into a stream
 *
 *  @param ds - The stream to write
 *  @param opt - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::optional<T>& opt) {
  char valid = opt.has_value();
  ds << valid;
  if (valid)
     ds << *opt;
  return ds;
}

/**
 *  Deserialize an optional from a stream
 *
 *  @param ds - The stream to read
 *  @param opt - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::optional<T>& opt) {
  char valid = 0;
  ds >> valid;
  if (valid) {
     T val;
     ds >> val;
     opt = val;
  }
  return ds;
}


/**
 *  Serialize a bool into a stream
 *
 *  @param ds - The stream to read
 *  @param d - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const bool& d) {
  return ds << uint8_t(d);
}

/**
 *  Deserialize a bool from a stream
 *
 *  @param ds - The stream to read
 *  @param d - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, bool& d) {
  uint8_t t;
  ds >> t;
  d = t;
  return ds;
}

/**
 *  Serialize a string into a stream
 *
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::string& v ) {
   ds << unsigned_int( v.size() );
   if (v.size())
      ds.write(v.data(), v.size());
   return ds;
}

/**
 *  Deserialize a string from a stream
 *
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::string& v ) {
   std::vector<char> tmp;
   ds >> tmp;
   if( tmp.size() )
      v = std::string(tmp.data(),tmp.data()+tmp.size());
   else
      v = std::string();
   return ds;
}

/**
 *  Serialize a fixed size std::array
 *
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T, std::size_t N>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::array<T,N>& v ) {
   for( const auto& i : v )
      ds << i;
   return ds;
}


/**
 *  Deserialize a fixed size std::array
 *
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T, std::size_t N>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::array<T,N>& v ) {
   for( auto& i : v )
      ds >> i;
   return ds;
}

namespace _datastream_detail {
   /**
    * Check if type T is a pointer
    *
    * @tparam T - The type to be checked
    * @return true if T is a pointer
    * @return false otherwise
    */
   template<typename T>
   constexpr bool is_pointer() {
      return std::is_pointer<T>::value ||
             std::is_null_pointer<T>::value ||
             std::is_member_pointer<T>::value;
   }

   /**
    * Check if type T is a primitive type
    *
    * @tparam T - The type to be checked
    * @return true if T is a primitive type
    * @return false otherwise
    */
   template<typename T>
   constexpr bool is_primitive() {
      return std::is_arithmetic<T>::value ||
             std::is_enum<T>::value;
   }

   /*
    * Check if type T is a specialization of datastream
    *
    * @tparam T - The type to be checked
    */
   template<typename T>
   struct is_datastream { static constexpr bool value = false; };
   template<typename T>
   struct is_datastream<datastream<T>> { static constexpr bool value = true; };
}

/**
 *  Deserialize a pointer
 *
 *  @brief Pointer should not be serialized, so this function will always throws an error
 *  @param ds - The stream to read
 *  @tparam Stream - Type of datastream buffer
 *  @tparam T - Type of the pointer
 *  @return datastream<Stream>& - Reference to the datastream
 *  @post Throw an exception if it is a pointer
 */
template<typename Stream, typename T, std::enable_if_t<_datastream_detail::is_pointer<T>()>* = nullptr>
datastream<Stream>& operator >> ( datastream<Stream>& ds, T ) {
   static_assert(!_datastream_detail::is_pointer<T>(), "Pointers should not be serialized" );
   return ds;
}

/**
 *  Serialize a fixed size C array of non-primitive and non-pointer type
 *
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @tparam T - Type of the pointer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T, std::size_t N,
         std::enable_if_t<!_datastream_detail::is_primitive<T>() &&
                          !_datastream_detail::is_pointer<T>()>* = nullptr>
datastream<Stream>& operator << ( datastream<Stream>& ds, const T (&v)[N] ) {
   ds << unsigned_int( N );
   for( uint32_t i = 0; i < N; ++i )
      ds << v[i];
   return ds;
}

/**
 *  Serialize a fixed size C array of primitive type
 *
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @tparam T - Type of the pointer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T, std::size_t N,
         std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator << ( datastream<Stream>& ds, const T (&v)[N] ) {
   ds << unsigned_int( N );
   ds.write((char*)&v[0], sizeof(v));
   return ds;
}

/**
 *  Deserialize a fixed size C array of non-primitive and non-pointer type
 *
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T, std::size_t N,
         std::enable_if_t<!_datastream_detail::is_primitive<T>() &&
                          !_datastream_detail::is_pointer<T>()>* = nullptr>
datastream<Stream>& operator >> ( datastream<Stream>& ds, T (&v)[N] ) {
   unsigned_int s;
   ds >> s;
   eosio::check( N == s.value, "T[] size and unpacked size don't match");
   for( uint32_t i = 0; i < N; ++i )
      ds >> v[i];
   return ds;
}

/**
 *  Deserialize a fixed size C array of primitive type
 *
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T, std::size_t N,
         std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator >> ( datastream<Stream>& ds, T (&v)[N] ) {
   unsigned_int s;
   ds >> s;
   eosio::check( N == s.value, "T[] size and unpacked size don't match");
   ds.read((char*)&v[0], sizeof(v));
   return ds;
}

/**
 *  Serialize a vector of char
 *
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::vector<char>& v ) {
   ds << unsigned_int( v.size() );
   ds.write( v.data(), v.size() );
   return ds;
}

/**
 *  Serialize a vector
 *
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @tparam T - Type of the object contained in the vector
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::vector<T>& v ) {
   ds << unsigned_int( v.size() );
   for( const auto& i : v )
      ds << i;
   return ds;
}

/**
 *  Deserialize a vector of char
 *
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::vector<char>& v ) {
   unsigned_int s;
   ds >> s;
   v.resize( s.value );
   ds.read( v.data(), v.size() );
   return ds;
}

/**
 *  Deserialize a vector
 *
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @tparam T - Type of the object contained in the vector
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::vector<T>& v ) {
   unsigned_int s;
   ds >> s;
   v.resize(s.value);
   for( auto& i : v )
      ds >> i;
   return ds;
}

/**
 *  Serialize a set
 *
 *  @param ds - The stream to write
 *  @param s - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @tparam T - Type of the object contained in the set
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::set<T>& s ) {
   ds << unsigned_int( s.size() );
   for( const auto& i : s ) {
      ds << i;
   }
   return ds;
}


/**
 *  Deserialize a set
 *
 *  @param ds - The stream to read
 *  @param s - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @tparam T - Type of the object contained in the set
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::set<T>& s ) {
   s.clear();
   unsigned_int sz; ds >> sz;

   for( uint32_t i = 0; i < sz.value; ++i ) {
      T v;
      ds >> v;
      s.emplace( std::move(v) );
   }
   return ds;
}

/**
 *  Serialize a map
 *
 *  @param ds - The stream to write
 *  @param m - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @tparam K - Type of the key contained in the map
 *  @tparam V - Type of the value contained in the map
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename K, typename V>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::map<K,V>& m ) {
   ds << unsigned_int( m.size() );
   for( const auto& i : m ) {
      ds << i.first << i.second;
   }
   return ds;
}

/**
 *  Deserialize a map
 *
 *  @param ds - The stream to read
 *  @param m - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @tparam K - Type of the key contained in the map
 *  @tparam V - Type of the value contained in the map
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename K, typename V>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::map<K,V>& m ) {
   m.clear();
   unsigned_int s; ds >> s;

   for (uint32_t i = 0; i < s.value; ++i) {
      K k; V v;
      ds >> k >> v;
      m.emplace( std::move(k), std::move(v) );
   }
   return ds;
}

/**
 *  Serialize a tuple
 *
 *  @param ds - The stream to write
 *  @param t - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @tparam Args - Type of the objects contained in the tuple
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename... Args>
datastream<Stream>& operator<<( datastream<Stream>& ds, const std::tuple<Args...>& t ) {
   boost::fusion::for_each( t, [&]( const auto& i ) {
       ds << i;
   });
   return ds;
}

/**
 *  Deserialize a tuple
 *
 *  @param ds - The stream to read
 *  @param t - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @tparam Args - Type of the objects contained in the tuple
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename... Args>
datastream<Stream>& operator>>( datastream<Stream>& ds, std::tuple<Args...>& t ) {
   boost::fusion::for_each( t, [&]( auto& i ) {
       ds >> i;
   });
   return ds;
}

/**
 *  Serialize a class
 *
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of class
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::enable_if_t<std::is_class<T>::value && _datastream_detail::is_datastream<DataStream>::value>* = nullptr>
DataStream& operator<<( DataStream& ds, const T& v ) {
   boost::pfr::for_each_field(v, [&](const auto& field) {
      ds << field;
   });
   return ds;
}

/**
 *  Deserialize a class
 *
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of class
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::enable_if_t<std::is_class<T>::value && _datastream_detail::is_datastream<DataStream>::value>* = nullptr>
DataStream& operator>>( DataStream& ds, T& v ) {
   boost::pfr::for_each_field(v, [&](auto& field) {
      ds >> field;
   });
   return ds;
}

/**
 *  Serialize a primitive type
 *
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @tparam T - Type of the primitive type
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T, std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator<<( datastream<Stream>& ds, const T& v ) {
   ds.write( (const char*)&v, sizeof(T) );
   return ds;
}

/**
 *  Deserialize a primitive type
 *
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @tparam T - Type of the primitive type
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T, std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator>>( datastream<Stream>& ds, T& v ) {
   ds.read( (char*)&v, sizeof(T) );
   return ds;
}

/**
 * Unpack data inside a fixed size buffer as T
 *
 * @ingroup datastream
 * @tparam T - Type of the unpacked data
 * @param buffer - Pointer to the buffer
 * @param len - Length of the buffer
 * @return T - The unpacked data
 */
template<typename T>
T unpack( const char* buffer, size_t len ) {
   T result;
   datastream<const char*> ds(buffer,len);
   ds >> result;
   return result;
}

/**
 * Unpack data inside a variable size buffer as T
 *
 * @ingroup datastream
 * @tparam T - Type of the unpacked data
 * @param bytes - Buffer
 * @return T - The unpacked data
 */
template<typename T>
T unpack( const std::vector<char>& bytes ) {
   return unpack<T>( bytes.data(), bytes.size() );
}

/**
 * Get the size of the packed data
 *
 * @ingroup datastream
 * @tparam T - Type of the data to be packed
 * @param value - Data to be packed
 * @return size_t - Size of the packed data
 */
template<typename T>
size_t pack_size( const T& value ) {
  datastream<size_t> ps;
  ps << value;
  return ps.tellp();
}

/**
 * Get packed data
 *
 * @ingroup datastream
 * @tparam T - Type of the data to be packed
 * @param value - Data to be packed
 * @return bytes - The packed data
 */
template<typename T>
std::vector<char> pack( const T& value ) {
  std::vector<char> result;
  result.resize(pack_size(value));

  datastream<char*> ds( result.data(), result.size() );
  ds << value;
  return result;
}
}
