#include "core/eosio/datastream.hpp"
#include "contracts/eosio/system.hpp"
#include "contracts/eosio/privileged.hpp"

#include <algorithm>

namespace eosio {
   extern "C" {
      __attribute__((eosio_wasm_import))
      uint64_t current_time();
     __attribute__((eosio_wasm_import))
     void set_blockchain_parameters_packed(char*, uint32_t);
     __attribute__((eosio_wasm_import))
     uint32_t get_blockchain_parameters_packed(char*, uint32_t);
     __attribute__((eosio_wasm_import))
     int64_t set_proposed_producers( char *producer_data, uint32_t producer_data_size );
     __attribute__((eosio_wasm_import))
     uint32_t get_active_producers(uint64_t*, uint32_t);
   }

   // producer_schedule.hpp
   bool block_signing_authority_v0::is_valid()const {
      uint32_t sum_weights = 0;
      std::set<eosio::public_key> unique_keys;

      for (const auto& kw: keys ) {
         if( std::numeric_limits<uint32_t>::max() - sum_weights <= kw.weight ) {
            sum_weights = std::numeric_limits<uint32_t>::max();
         } else {
            sum_weights += kw.weight;
         }

         unique_keys.insert(kw.key);
      }

      if( keys.size() != unique_keys.size() )
         return false; // producer authority includes a duplicated key

      if( threshold == 0 )
         return false; // producer authority has a threshold of 0

      if( sum_weights < threshold )
         return false; // producer authority is unsatisfiable

      return true;
   }

   // privileged.hpp
   void set_blockchain_parameters(const eosio::blockchain_parameters& params) {
      char buf[sizeof(eosio::blockchain_parameters)];
      eosio::datastream<char *> ds( buf, sizeof(buf) );
      ds << params;
      set_blockchain_parameters_packed( buf, ds.tellp() );
   }

   void get_blockchain_parameters(eosio::blockchain_parameters& params) {
      char buf[sizeof(eosio::blockchain_parameters)];
      size_t size = get_blockchain_parameters_packed( buf, sizeof(buf) );
      eosio::check( size <= sizeof(buf), "buffer is too small" );
      eosio::datastream<const char*> ds( buf, size_t(size) );
      ds >> params;
   }

   std::optional<uint64_t> set_proposed_producers( const std::vector<producer_key>& prods ) {
      auto packed_prods = eosio::pack( prods );
      int64_t ret = set_proposed_producers((char*)packed_prods.data(), packed_prods.size());
      if (ret >= 0)
        return static_cast<uint64_t>(ret);
      return {};
   }

   // system.hpp
   time_point current_time_point() {
      static auto ct = time_point(microseconds(static_cast<int64_t>(current_time())));
      return ct;
   }

   block_timestamp current_block_time() {
      static auto bt = block_timestamp(current_time_point());
      return bt;
   }

   std::vector<name> get_active_producers() {
      const auto buffer_size = get_active_producers(nullptr, 0);
      const auto prod_cnt = buffer_size / sizeof(name);
      std::vector<name> active_prods(prod_cnt);
      get_active_producers((uint64_t*)active_prods.data(), buffer_size);
      return active_prods;
   }

} // namespace eosio
