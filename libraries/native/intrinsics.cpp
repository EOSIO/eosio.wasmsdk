#include <eosio/action.h>
#include <eosio/chain.h>
#include <eosio/crypto.h>
#include <eosio/db.h>
#include <eosio/permission.h>
#include <eosio/print.h>
#include <eosio/privileged.h>
#include <eosio/system.h>
#include <eosio/transaction.h>
#include <eosio/types.h>
#include "native/eosio/intrinsics.hpp"
#include "native/eosio/crt.hpp"
#include <softfloat.hpp>
#include <float.h>
#include "intrinsics_impl.hpp"

// Boilerplate
using namespace eosio::native;
extern "C" {
   void get_resource_limits( capi_name account, int64_t* ram_bytes, int64_t* net_weight, int64_t* cpu_weight ) {
      return intrinsics::get().call<intrinsics::get_resource_limits>(account, ram_bytes, net_weight, cpu_weight);
   }
   void set_resource_limits( capi_name account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight ) {
      return intrinsics::get().call<intrinsics::set_resource_limits>(account, ram_bytes, net_weight, cpu_weight);
   }
   int64_t set_proposed_producers( char *producer_data, uint32_t producer_data_size ) {
      return intrinsics::get().call<intrinsics::set_proposed_producers>(producer_data, producer_data_size);
   }
   int64_t set_proposed_producers_ex( uint64_t producer_data_format, char *producer_data, uint32_t producer_data_size ) {
      return intrinsics::get().call<intrinsics::set_proposed_producers_ex>(producer_data_format, producer_data, producer_data_size);
   }
   uint32_t get_blockchain_parameters_packed( char* data, uint32_t datalen ) {
      return intrinsics::get().call<intrinsics::get_blockchain_parameters_packed>(data, datalen);
   }
   void set_blockchain_parameters_packed( char* data, uint32_t datalen ) {
      return intrinsics::get().call<intrinsics::set_blockchain_parameters_packed>(data, datalen);
   }
   bool is_privileged( capi_name account ) {
      return intrinsics::get().call<intrinsics::is_privileged>(account);
   }
   void set_privileged( capi_name account, bool is_priv ) {
      return intrinsics::get().call<intrinsics::set_privileged>(account, is_priv);
   }
   bool is_feature_activated( const capi_checksum256* feature_digest ) {
      return intrinsics::get().call<intrinsics::is_feature_activated>(feature_digest);
   }
   void preactivate_feature( const capi_checksum256* feature_digest ) {
      return intrinsics::get().call<intrinsics::preactivate_feature>(feature_digest);
   }
   uint32_t get_active_producers( capi_name* producers, uint32_t datalen ) {
      return intrinsics::get().call<intrinsics::get_active_producers>(producers, datalen);
   }

   int32_t db_idx64_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const uint64_t* secondary) {
      return secondary_indexes->store<uint64_t, idx64>(scope, table, payer, id, secondary);
   }
   void db_idx64_remove(int32_t iterator) {
      return secondary_indexes->remove(iterator);
   }
   void db_idx64_update(int32_t iterator, capi_name payer, const uint64_t* secondary) {
      return secondary_indexes->update<uint64_t>(iterator, payer, secondary);
   }
   int32_t db_idx64_find_primary(capi_name code, uint64_t scope, capi_name table, uint64_t* secondary, uint64_t primary) {
      return secondary_indexes->find_primary<uint64_t>(code, scope, table, secondary, primary);
   }
   int32_t db_idx64_find_secondary(capi_name code, uint64_t scope, capi_name table, const uint64_t* secondary, uint64_t* primary) {
      return secondary_indexes->find_secondary<uint64_t>(code, scope, table, secondary, primary);
   }
   int32_t db_idx64_lowerbound(capi_name code, uint64_t scope, capi_name table, uint64_t* secondary, uint64_t* primary) {
      return secondary_indexes->lowerbound<uint64_t>(code, scope, table, secondary, primary);
   }
   int32_t db_idx64_upperbound(capi_name code, uint64_t scope, capi_name table, uint64_t* secondary, uint64_t* primary) {
      return secondary_indexes->upperbound<uint64_t>(code, scope, table, secondary, primary);
   }
   int32_t db_idx64_end(capi_name code, uint64_t scope, capi_name table) {
      return secondary_indexes->end(code, scope, table);
   }
   int32_t db_idx64_next(int32_t iterator, uint64_t* primary) {
      return secondary_indexes->next(iterator, primary);
   }
   int32_t db_idx64_previous(int32_t iterator, uint64_t* primary) {
      return secondary_indexes->previous(iterator, primary);
   }

   int32_t db_idx128_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const uint128_t* secondary) {
      return secondary_indexes->store<uint128_t, idx128>(scope, table, payer, id, secondary);
   }
   void db_idx128_remove(int32_t iterator) {
      return secondary_indexes->remove(iterator);
   }
   void db_idx128_update(int32_t iterator, capi_name payer, const uint128_t* secondary) {
      return secondary_indexes->update<uint128_t>(iterator, payer, secondary);
   }
   int32_t db_idx128_find_primary(capi_name code, uint64_t scope, capi_name table, uint128_t* secondary, uint64_t primary) {
      return secondary_indexes->find_primary<uint128_t>(code, scope, table, secondary, primary);
   }
   int32_t db_idx128_find_secondary(capi_name code, uint64_t scope, capi_name table, const uint128_t* secondary, uint64_t* primary) {
      return secondary_indexes->find_secondary<uint128_t>(code, scope, table, secondary, primary);
   }
   int32_t db_idx128_lowerbound(capi_name code, uint64_t scope, capi_name table, uint128_t* secondary, uint64_t* primary) {
      return secondary_indexes->lowerbound<uint128_t>(code, scope, table, secondary, primary);
   }
   int32_t db_idx128_upperbound(capi_name code, uint64_t scope, capi_name table, uint128_t* secondary, uint64_t* primary) {
      return secondary_indexes->upperbound<uint128_t>(code, scope, table, secondary, primary);
   }
   int32_t db_idx128_end(capi_name code, uint64_t scope, capi_name table) {
      return secondary_indexes->end(code, scope, table);
   }
   int32_t db_idx128_next(int32_t iterator, uint64_t* primary) {
      return secondary_indexes->next(iterator, primary);
   }
   int32_t db_idx128_previous(int32_t iterator, uint64_t* primary) {
      return secondary_indexes->previous(iterator, primary);
   }

   int32_t db_idx256_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const uint128_t* data, uint32_t datalen) {
      idx256_t secondary;
      secondary.v[0] = *data;
      secondary.v[1] = *(data+1);
      return secondary_indexes->store<idx256_t, idx256>(scope, table, payer, id, &secondary);
   }
   void db_idx256_remove(int32_t iterator) {
      return secondary_indexes->remove(iterator);
   }
   void db_idx256_update(int32_t iterator, capi_name payer, const uint128_t* data, uint32_t datalen) {
      idx256_t secondary;
      secondary.v[0] = *data;
      secondary.v[1] = *(data+1);
      return secondary_indexes->update<idx256_t>(iterator, payer, &secondary);
   }
   int32_t db_idx256_find_primary(capi_name code, uint64_t scope, capi_name table, uint128_t* data, uint32_t datalen,  uint64_t primary) {
      idx256_t secondary;
      secondary.v[0] = *data;
      secondary.v[1] = *(data+1);
      return secondary_indexes->find_primary<idx256_t>(code, scope, table, &secondary, primary);
   }
   int32_t db_idx256_find_secondary(capi_name code, uint64_t scope, capi_name table, const uint128_t* data, uint32_t datalen, uint64_t* primary) {
      idx256_t secondary;
      secondary.v[0] = *data;
      secondary.v[1] = *(data+1);
      return secondary_indexes->find_secondary<idx256_t>(code, scope, table, &secondary, primary);
   }
   int32_t db_idx256_lowerbound(capi_name code, uint64_t scope, capi_name table, uint128_t* data, uint32_t datalen, uint64_t* primary) {
      idx256_t secondary;
      secondary.v[0] = *data;
      secondary.v[1] = *(data+1);
      return secondary_indexes->lowerbound<idx256_t>(code, scope, table, &secondary, primary);
   }
   int32_t db_idx256_upperbound(capi_name code, uint64_t scope, capi_name table, uint128_t* data, uint32_t datalen,  uint64_t* primary) {
      idx256_t secondary;
      secondary.v[0] = *data;
      secondary.v[1] = *(data+1);
      return secondary_indexes->upperbound<idx256_t>(code, scope, table, &secondary, primary);
   }
   int32_t db_idx256_end(capi_name code, uint64_t scope, capi_name table) {
      return secondary_indexes->end(code, scope, table);
   }
   int32_t db_idx256_next(int32_t iterator, uint64_t* primary) {
      return secondary_indexes->next(iterator, primary);
   }
   int32_t db_idx256_previous(int32_t iterator, uint64_t* primary) {
      return secondary_indexes->previous(iterator, primary);
   }

   int32_t db_idx_double_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const double* secondary) {
      return secondary_indexes->store<double, idxdouble>(scope, table, payer, id, secondary);
   }
   void db_idx_double_remove(int32_t iterator) {
      return secondary_indexes->remove(iterator);
   }
   void db_idx_double_update(int32_t iterator, capi_name payer, const double* secondary) {
      return secondary_indexes->update<double>(iterator, payer, secondary);
   }
   int32_t db_idx_double_find_primary(capi_name code, uint64_t scope, capi_name table, double* secondary, uint64_t primary) {
      return secondary_indexes->find_primary<double>(code, scope, table, secondary, primary);
   }
   int32_t db_idx_double_find_secondary(capi_name code, uint64_t scope, capi_name table, const double* secondary, uint64_t* primary) {
      return secondary_indexes->find_secondary<double>(code, scope, table, secondary, primary);
   }
   int32_t db_idx_double_lowerbound(capi_name code, uint64_t scope, capi_name table, double* secondary, uint64_t* primary) {
      return secondary_indexes->lowerbound<double>(code, scope, table, secondary, primary);
   }
   int32_t db_idx_double_upperbound(capi_name code, uint64_t scope, capi_name table, double* secondary, uint64_t* primary) {
      return secondary_indexes->upperbound<double>(code, scope, table, secondary, primary);
   }
   int32_t db_idx_double_end(capi_name code, uint64_t scope, capi_name table) {
      return secondary_indexes->end(code, scope, table);
   }
   int32_t db_idx_double_next(int32_t iterator, uint64_t* primary) {
      return secondary_indexes->next(iterator, primary);
   }
   int32_t db_idx_double_previous(int32_t iterator, uint64_t* primary) {
      return secondary_indexes->previous(iterator, primary);
   }

   int32_t db_idx_long_double_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const long double* secondary) {
      return secondary_indexes->store<long double, idxlongdouble>(scope, table, payer, id, secondary);
   }
   void db_idx_long_double_remove(int32_t iterator) {
      return secondary_indexes->remove(iterator);
   }
   void db_idx_long_double_update(int32_t iterator, capi_name payer, const long double* secondary) {
      return secondary_indexes->update<long double>(iterator, payer, secondary);
   }
   int32_t db_idx_long_double_find_primary(capi_name code, uint64_t scope, capi_name table, long double* secondary, uint64_t primary) {
      return secondary_indexes->find_primary<long double>(code, scope, table, secondary, primary);
   }
   int32_t db_idx_long_double_find_secondary(capi_name code, uint64_t scope, capi_name table, const long double* secondary, uint64_t* primary) {
      return secondary_indexes->find_secondary<long double>(code, scope, table, secondary, primary);
   }
   int32_t db_idx_long_double_lowerbound(capi_name code, uint64_t scope, capi_name table, long double* secondary, uint64_t* primary) {
      return secondary_indexes->lowerbound<long double>(code, scope, table, secondary, primary);
   }
   int32_t db_idx_long_double_upperbound(capi_name code, uint64_t scope, capi_name table, long double* secondary, uint64_t* primary) {
      return secondary_indexes->upperbound<long double>(code, scope, table, secondary, primary);
   }
   int32_t db_idx_long_double_end(capi_name code, uint64_t scope, capi_name table) {
      return secondary_indexes->end(code, scope, table);
   }
   int32_t db_idx_long_double_next(int32_t iterator, uint64_t* primary) {
      return secondary_indexes->next(iterator, primary);
   }
   int32_t db_idx_long_double_previous(int32_t iterator, uint64_t* primary) {
      return secondary_indexes->previous(iterator, primary);
   }

   int32_t db_store_i64(uint64_t scope, capi_name table, capi_name payer, uint64_t id,  const void* data, uint32_t len) {
      return primary_index->store(scope, table, payer, id, data, len);
   }
   void db_update_i64(int32_t iterator, capi_name payer, const void* data, uint32_t len) {
      return primary_index->update(iterator, payer, data, len);
   }
   void db_remove_i64(int32_t iterator) {
      return primary_index->remove(iterator);
   }
   int32_t db_get_i64(int32_t iterator, void* data, uint32_t len) {
      return primary_index->get(iterator, data, len);
   }
   int32_t db_next_i64(int32_t iterator, uint64_t* primary) {
      return primary_index->next(iterator, primary);
   }
   int32_t db_previous_i64(int32_t iterator, uint64_t* primary) {
      return primary_index->previous(iterator, primary);
   }
   int32_t db_find_i64(capi_name code, uint64_t scope, capi_name table, uint64_t id) {
      return primary_index->find(code, scope, table, id);
   }
   int32_t db_lowerbound_i64(capi_name code, uint64_t scope, capi_name table, uint64_t id) {
      return primary_index->lowerbound(code, scope, table, id);
   }
   int32_t db_upperbound_i64(capi_name code, uint64_t scope, capi_name table, uint64_t id) {
      return primary_index->upperbound(code, scope, table, id);
   }
   int32_t db_end_i64(capi_name code, uint64_t scope, capi_name table) {
      return primary_index->end(code, scope, table);
   }

   void assert_recover_key( const capi_checksum256* digest, const char* sig, size_t siglen, const char* pub, size_t publen ) {
      return intrinsics::get().call<intrinsics::assert_recover_key>(digest, sig, siglen, pub, publen);
   }
   int recover_key( const capi_checksum256* digest, const char* sig, size_t siglen, char* pub, size_t publen ) {
      return intrinsics::get().call<intrinsics::recover_key>(digest, sig, siglen, pub, publen);
   }
   void assert_sha256( const char* data, uint32_t length, const capi_checksum256* hash ) {
      return intrinsics::get().call<intrinsics::assert_sha256>(data, length, hash);
   }
   void assert_sha1( const char* data, uint32_t length, const capi_checksum160* hash ) {
      return intrinsics::get().call<intrinsics::assert_sha1>(data, length, hash);
   }
   void assert_sha512( const char* data, uint32_t length, const capi_checksum512* hash ) {
      return intrinsics::get().call<intrinsics::assert_sha512>(data, length, hash);
   }
   void assert_ripemd160( const char* data, uint32_t length, const capi_checksum160* hash ) {
      return intrinsics::get().call<intrinsics::assert_ripemd160>(data, length, hash);
   }
   void sha256( const char* data, uint32_t length, capi_checksum256* hash ) {
      return intrinsics::get().call<intrinsics::sha256>(data, length, hash);
   }
   void sha1( const char* data, uint32_t length, capi_checksum160* hash ) {
      return intrinsics::get().call<intrinsics::sha1>(data, length, hash);
   }
   void sha512( const char* data, uint32_t length, capi_checksum512* hash ) {
      return intrinsics::get().call<intrinsics::sha512>(data, length, hash);
   }
   void ripemd160( const char* data, uint32_t length, capi_checksum160* hash ) {
      return intrinsics::get().call<intrinsics::ripemd160>(data, length, hash);
   }
   int32_t check_transaction_authorization( const char* trx_data,     uint32_t trx_size,
                                    const char* pubkeys_data, uint32_t pubkeys_size,
                                    const char* perms_data,   uint32_t perms_size
                                  ) {
      return intrinsics::get().call<intrinsics::check_transaction_authorization>(trx_data, trx_size, pubkeys_data, pubkeys_size, perms_data, perms_size);
   }
   int32_t check_permission_authorization( capi_name account, capi_name permission,
                                    const char* pubkeys_data, uint32_t pubkeys_size,
                                    const char* perms_data,   uint32_t perms_size, uint64_t delay_us
                                  ) {
      return intrinsics::get().call<intrinsics::check_permission_authorization>(account, permission, pubkeys_data, pubkeys_size, perms_data, perms_size, delay_us);
   }
   int64_t get_permission_last_used( capi_name account, capi_name permission ) {
      return intrinsics::get().call<intrinsics::get_permission_last_used>(account, permission);
   }
   int64_t get_account_creation_time( capi_name account ) {
      return intrinsics::get().call<intrinsics::get_account_creation_time>(account);
   }
   uint64_t  current_time() {
      return intrinsics::get().call<intrinsics::current_time>();
   }
   uint64_t  publication_time() {
      return intrinsics::get().call<intrinsics::publication_time>();
   }
   uint32_t read_action_data( void* msg, uint32_t len ) {
      return intrinsics::get().call<intrinsics::read_action_data>(msg, len);
   }
   uint32_t action_data_size() {
      return intrinsics::get().call<intrinsics::action_data_size>();
   }
   capi_name current_receiver() {
      return intrinsics::get().call<intrinsics::current_receiver>();
   }
   void set_action_return_value( void* rv, size_t len ) {
      intrinsics::get().call<intrinsics::set_action_return_value>(rv, len);
   }
   void require_recipient( capi_name name ) {
      return intrinsics::get().call<intrinsics::require_recipient>(name);
   }
   void require_auth( capi_name name ) {
      return intrinsics::get().call<intrinsics::require_auth>(name);
   }
   void require_auth2( capi_name name, capi_name permission ) {
      return intrinsics::get().call<intrinsics::require_auth2>(name, permission);
   }
   bool has_auth( capi_name name ) {
      return intrinsics::get().call<intrinsics::has_auth>(name);
   }
   bool is_account( capi_name name ) {
      return intrinsics::get().call<intrinsics::is_account>(name);
   }
   size_t read_transaction(char *buffer, size_t size) {
      return intrinsics::get().call<intrinsics::read_transaction>(buffer, size);
   }
   size_t transaction_size() {
      return intrinsics::get().call<intrinsics::transaction_size>();
   }
   uint32_t expiration() {
      return intrinsics::get().call<intrinsics::expiration>();
   }
   int tapos_block_prefix() {
      return intrinsics::get().call<intrinsics::tapos_block_prefix>();
   }
   int tapos_block_num() {
      return intrinsics::get().call<intrinsics::tapos_block_num>();
   }
   int get_action( uint32_t type, uint32_t index, char* buff, size_t size ) {
      return intrinsics::get().call<intrinsics::get_action>(type, index, buff, size);
   }
   void send_inline(char *serialized_action, size_t size) {
      return intrinsics::get().call<intrinsics::send_inline>(serialized_action, size);
   }
   void send_context_free_inline(char *serialized_action, size_t size) {
      return intrinsics::get().call<intrinsics::send_context_free_inline>(serialized_action, size);
   }
   void send_deferred(const uint128_t* sender_id, capi_name payer, const char *serialized_transaction, size_t size, uint32_t replace_existing) {
      return intrinsics::get().call<intrinsics::send_deferred>(sender_id, payer, serialized_transaction, size, replace_existing);
   }
   int cancel_deferred(const uint128_t* sender_id) {
      return intrinsics::get().call<intrinsics::cancel_deferred>(sender_id);
   }
   int get_context_free_data( uint32_t index, char* buff, size_t size ) {
      return intrinsics::get().call<intrinsics::get_context_free_data>(index, buff, size);
   }
   capi_name get_sender() {
      return intrinsics::get().call<intrinsics::get_sender>();
   }

   // softfloat
   static constexpr uint32_t inv_float_eps = 0x4B000000;
   static constexpr uint64_t inv_double_eps = 0x4330000000000000;

   float _eosio_f32_add( float a, float b ) {
      float32_t ret = f32_add( to_softfloat32(a), to_softfloat32(b) );
      return *reinterpret_cast<float*>(&ret);
   }
   float _eosio_f32_sub( float a, float b ) {
      float32_t ret = f32_sub( to_softfloat32(a), to_softfloat32(b) );
      return *reinterpret_cast<float*>(&ret);
   }
   float _eosio_f32_div( float a, float b ) {
      float32_t ret = f32_div( to_softfloat32(a), to_softfloat32(b) );
      return *reinterpret_cast<float*>(&ret);
   }
   float _eosio_f32_mul( float a, float b ) {
      float32_t ret = f32_mul( to_softfloat32(a), to_softfloat32(b) );
      return *reinterpret_cast<float*>(&ret);
   }
   float _eosio_f32_min( float af, float bf ) {
      float32_t a = to_softfloat32(af);
      float32_t b = to_softfloat32(bf);
      if (f32_is_nan(a)) {
         return af;
      }
      if (f32_is_nan(b)) {
         return bf;
      }
      if ( f32_sign_bit(a) != f32_sign_bit(b) ) {
         return f32_sign_bit(a) ? af : bf;
      }
      return f32_lt(a,b) ? af : bf;
   }
   float _eosio_f32_max( float af, float bf ) {
      float32_t a = to_softfloat32(af);
      float32_t b = to_softfloat32(bf);
      if (f32_is_nan(a)) {
         return af;
      }
      if (f32_is_nan(b)) {
         return bf;
      }
      if ( f32_sign_bit(a) != f32_sign_bit(b) ) {
         return f32_sign_bit(a) ? bf : af;
      }
      return f32_lt( a, b ) ? bf : af;
   }
   float _eosio_f32_copysign( float af, float bf ) {
      float32_t a = to_softfloat32(af);
      float32_t b = to_softfloat32(bf);
      uint32_t sign_of_b = b.v >> 31;
      a.v &= ~(1 << 31);             // clear the sign bit
      a.v = a.v | (sign_of_b << 31); // add the sign of b
      return from_softfloat32(a);
   }
   // float unops
   float _eosio_f32_abs( float af ) {
      float32_t a = to_softfloat32(af);
      a.v &= ~(1 << 31);
      return from_softfloat32(a);
   }
   float _eosio_f32_neg( float af ) {
      float32_t a = to_softfloat32(af);
      uint32_t sign = a.v >> 31;
      a.v &= ~(1 << 31);
      a.v |= (!sign << 31);
      return from_softfloat32(a);
   }
   float _eosio_f32_sqrt( float a ) {
      float32_t ret = f32_sqrt( to_softfloat32(a) );
      return from_softfloat32(ret);
   }
   // ceil, floor, trunc and nearest are lifted from libc
   float _eosio_f32_ceil( float af ) {
      float32_t a = to_softfloat32(af);
      int e = (int)(a.v >> 23 & 0xFF) - 0X7F;
      uint32_t m;
      if (e >= 23)
         return af;
      if (e >= 0) {
         m = 0x007FFFFF >> e;
         if ((a.v & m) == 0)
            return af;
         if (a.v >> 31 == 0)
            a.v += m;
         a.v &= ~m;
      } else {
         if (a.v >> 31)
            a.v = 0x80000000; // return -0.0f
         else if (a.v << 1)
            a.v = 0x3F800000; // return 1.0f
      }

      return from_softfloat32(a);
   }
   float _eosio_f32_floor( float af ) {
      float32_t a = to_softfloat32(af);
      int e = (int)(a.v >> 23 & 0xFF) - 0X7F;
      uint32_t m;
      if (e >= 23)
         return af;
      if (e >= 0) {
         m = 0x007FFFFF >> e;
         if ((a.v & m) == 0)
            return af;
         if (a.v >> 31)
            a.v += m;
         a.v &= ~m;
      } else {
         if (a.v >> 31 == 0)
            a.v = 0;
         else if (a.v << 1)
            a.v = 0xBF800000; // return -1.0f
      }
      return from_softfloat32(a);
   }
   float _eosio_f32_trunc( float af ) {
      float32_t a = to_softfloat32(af);
      int e = (int)(a.v >> 23 & 0xff) - 0x7f + 9;
      uint32_t m;
      if (e >= 23 + 9)
         return af;
      if (e < 9)
         e = 1;
      m = -1U >> e;
      if ((a.v & m) == 0)
         return af;
      a.v &= ~m;
      return from_softfloat32(a);
   }
   float _eosio_f32_nearest( float af ) {
      float32_t a = to_softfloat32(af);
      int e = a.v>>23 & 0xff;
      int s = a.v>>31;
      float32_t y;
      if (e >= 0x7f+23)
         return af;
      if (s)
         y = f32_add( f32_sub( a, float32_t{inv_float_eps} ), float32_t{inv_float_eps} );
      else
         y = f32_sub( f32_add( a, float32_t{inv_float_eps} ), float32_t{inv_float_eps} );
      if (f32_eq( y, {0} ) )
         return s ? -0.0f : 0.0f;
      return from_softfloat32(y);
   }

   // float relops
   bool _eosio_f32_eq( float a, float b ) {  return f32_eq( to_softfloat32(a), to_softfloat32(b) ); }
   bool _eosio_f32_ne( float a, float b ) { return !f32_eq( to_softfloat32(a), to_softfloat32(b) ); }
   bool _eosio_f32_lt( float a, float b ) { return f32_lt( to_softfloat32(a), to_softfloat32(b) ); }
   bool _eosio_f32_le( float a, float b ) { return f32_le( to_softfloat32(a), to_softfloat32(b) ); }
   bool _eosio_f32_gt( float af, float bf ) {
      float32_t a = to_softfloat32(af);
      float32_t b = to_softfloat32(bf);
      if (f32_is_nan(a))
         return false;
      if (f32_is_nan(b))
         return false;
      return !f32_le( a, b );
   }
   bool _eosio_f32_ge( float af, float bf ) {
      float32_t a = to_softfloat32(af);
      float32_t b = to_softfloat32(bf);
      if (f32_is_nan(a))
         return false;
      if (f32_is_nan(b))
         return false;
      return !f32_lt( a, b );
   }

   // double binops
   double _eosio_f64_add( double a, double b ) {
      float64_t ret = f64_add( to_softfloat64(a), to_softfloat64(b) );
      return from_softfloat64(ret);
   }
   double _eosio_f64_sub( double a, double b ) {
      float64_t ret = f64_sub( to_softfloat64(a), to_softfloat64(b) );
      return from_softfloat64(ret);
   }
   double _eosio_f64_div( double a, double b ) {
      float64_t ret = f64_div( to_softfloat64(a), to_softfloat64(b) );
      return from_softfloat64(ret);
   }
   double _eosio_f64_mul( double a, double b ) {
      float64_t ret = f64_mul( to_softfloat64(a), to_softfloat64(b) );
      return from_softfloat64(ret);
   }
   double _eosio_f64_min( double af, double bf ) {
      float64_t a = to_softfloat64(af);
      float64_t b = to_softfloat64(bf);
      if (f64_is_nan(a))
         return af;
      if (f64_is_nan(b))
         return bf;
      if (f64_sign_bit(a) != f64_sign_bit(b))
         return f64_sign_bit(a) ? af : bf;
      return f64_lt( a, b ) ? af : bf;
   }
   double _eosio_f64_max( double af, double bf ) {
      float64_t a = to_softfloat64(af);
      float64_t b = to_softfloat64(bf);
      if (f64_is_nan(a))
         return af;
      if (f64_is_nan(b))
         return bf;
      if (f64_sign_bit(a) != f64_sign_bit(b))
         return f64_sign_bit(a) ? bf : af;
      return f64_lt( a, b ) ? bf : af;
   }
   double _eosio_f64_copysign( double af, double bf ) {
      float64_t a = to_softfloat64(af);
      float64_t b = to_softfloat64(bf);
      uint64_t sign_of_b = b.v >> 63;
      a.v &= ~(uint64_t(1) << 63);             // clear the sign bit
      a.v = a.v | (sign_of_b << 63); // add the sign of b
      return from_softfloat64(a);
   }

   // double unops
   double _eosio_f64_abs( double af ) {
      float64_t a = to_softfloat64(af);
      a.v &= ~(uint64_t(1) << 63);
      return from_softfloat64(a);
   }
   double _eosio_f64_neg( double af ) {
      float64_t a = to_softfloat64(af);
      uint64_t sign = a.v >> 63;
      a.v &= ~(uint64_t(1) << 63);
      a.v |= (uint64_t(!sign) << 63);
      return from_softfloat64(a);
   }
   double _eosio_f64_sqrt( double a ) {
      float64_t ret = f64_sqrt( to_softfloat64(a) );
      return from_softfloat64(ret);
   }
   // ceil, floor, trunc and nearest are lifted from libc
   double _eosio_f64_ceil( double af ) {
      float64_t a = to_softfloat64( af );
      float64_t ret;
      int e = a.v >> 52 & 0x7ff;
      float64_t y;
      if (e >= 0x3ff+52 || f64_eq( a, { 0 } ))
         return af;
      /* y = int(x) - x, where int(x) is an integer neighbor of x */
      if (a.v >> 63)
         y = f64_sub( f64_add( f64_sub( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} ), a );
      else
         y = f64_sub( f64_sub( f64_add( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} ), a );
      /* special case because of non-nearest rounding modes */
      if (e <= 0x3ff-1) {
         return a.v >> 63 ? -0.0 : 1.0; //float64_t{0x8000000000000000} : float64_t{0xBE99999A3F800000}; //either -0.0 or 1
      }
      if (f64_lt( y, to_softfloat64(0) )) {
         ret = f64_add( f64_add( a, y ), to_softfloat64(1) ); // 0xBE99999A3F800000 } ); // plus 1
         return from_softfloat64(ret);
      }
      ret = f64_add( a, y );
      return from_softfloat64(ret);
   }
   double _eosio_f64_floor( double af ) {
      float64_t a = to_softfloat64( af );
      float64_t ret;
      int e = a.v >> 52 & 0x7FF;
      float64_t y;
      if ( a.v == 0x8000000000000000) {
         return af;
      }
      if (e >= 0x3FF+52 || a.v == 0) {
         return af;
      }
      if (a.v >> 63)
         y = f64_sub( f64_add( f64_sub( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} ), a );
      else
         y = f64_sub( f64_sub( f64_add( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} ), a );
      if (e <= 0x3FF-1) {
         return a.v>>63 ? -1.0 : 0.0; //float64_t{0xBFF0000000000000} : float64_t{0}; // -1 or 0
      }
      if ( !f64_le( y, float64_t{0} ) ) {
         ret = f64_sub( f64_add(a,y), to_softfloat64(1.0));
         return from_softfloat64(ret);
      }
      ret = f64_add( a, y );
      return from_softfloat64(ret);
   }
   double _eosio_f64_trunc( double af ) {
      float64_t a = to_softfloat64( af );
      int e = (int)(a.v >> 52 & 0x7ff) - 0x3ff + 12;
      uint64_t m;
      if (e >= 52 + 12)
         return af;
      if (e < 12)
         e = 1;
      m = -1ULL >> e;
      if ((a.v & m) == 0)
         return af;
      a.v &= ~m;
      return from_softfloat64(a);
   }

   double _eosio_f64_nearest( double af ) {
      float64_t a = to_softfloat64( af );
      int e = (a.v >> 52 & 0x7FF);
      int s = a.v >> 63;
      float64_t y;
      if ( e >= 0x3FF+52 )
         return af;
      if ( s )
         y = f64_add( f64_sub( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} );
      else
         y = f64_sub( f64_add( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} );
      if ( f64_eq( y, float64_t{0} ) )
         return s ? -0.0 : 0.0;
      return from_softfloat64(y);
   }

   // double relops
   bool _eosio_f64_eq( double a, double b ) { return f64_eq( to_softfloat64(a), to_softfloat64(b) ); }
   bool _eosio_f64_ne( double a, double b ) { return !f64_eq( to_softfloat64(a), to_softfloat64(b) ); }
   bool _eosio_f64_lt( double a, double b ) { return f64_lt( to_softfloat64(a), to_softfloat64(b) ); }
   bool _eosio_f64_le( double a, double b ) { return f64_le( to_softfloat64(a), to_softfloat64(b) ); }
   bool _eosio_f64_gt( double af, double bf ) {
      float64_t a = to_softfloat64(af);
      float64_t b = to_softfloat64(bf);
      if (f64_is_nan(a))
         return false;
      if (f64_is_nan(b))
         return false;
      return !f64_le( a, b );
   }
   bool _eosio_f64_ge( double af, double bf ) {
      float64_t a = to_softfloat64(af);
      float64_t b = to_softfloat64(bf);
      if (f64_is_nan(a))
         return false;
      if (f64_is_nan(b))
         return false;
      return !f64_lt( a, b );
   }

   // float and double conversions
   double _eosio_f32_promote( float a ) {
      return from_softfloat64(f32_to_f64( to_softfloat32(a)) );
   }
   float _eosio_f64_demote( double a ) {
      return from_softfloat32(f64_to_f32( to_softfloat64(a)) );
   }
   int32_t _eosio_f32_trunc_i32s( float af ) {
      float32_t a = to_softfloat32(af);
      if (_eosio_f32_ge(af, 2147483648.0f) || _eosio_f32_lt(af, -2147483648.0f))
         eosio_assert(false,  "Error, f32.convert_s/i32 overflow" );

      if (f32_is_nan(a))
         eosio_assert(false,  "Error, f32.convert_s/i32 unrepresentable");
      return f32_to_i32( to_softfloat32(_eosio_f32_trunc( af )), 0, false );
   }
   int32_t _eosio_f64_trunc_i32s( double af ) {
      float64_t a = to_softfloat64(af);
      if (_eosio_f64_ge(af, 2147483648.0) || _eosio_f64_lt(af, -2147483648.0))
         eosio_assert(false,  "Error, f64.convert_s/i32 overflow");
      if (f64_is_nan(a))
         eosio_assert(false,  "Error, f64.convert_s/i32 unrepresentable");
      return f64_to_i32( to_softfloat64(_eosio_f64_trunc( af )), 0, false );
   }
   uint32_t _eosio_f32_trunc_i32u( float af ) {
      float32_t a = to_softfloat32(af);
      if (_eosio_f32_ge(af, 4294967296.0f) || _eosio_f32_le(af, -1.0f))
         eosio_assert(false,  "Error, f32.convert_u/i32 overflow");
      if (f32_is_nan(a))
         eosio_assert(false,  "Error, f32.convert_u/i32 unrepresentable");
      return f32_to_ui32( to_softfloat32(_eosio_f32_trunc( af )), 0, false );
   }
   uint32_t _eosio_f64_trunc_i32u( double af ) {
      float64_t a = to_softfloat64(af);
      if (_eosio_f64_ge(af, 4294967296.0) || _eosio_f64_le(af, -1.0))
         eosio_assert(false,  "Error, f64.convert_u/i32 overflow");
      if (f64_is_nan(a))
         eosio_assert(false,  "Error, f64.convert_u/i32 unrepresentable");
      return f64_to_ui32( to_softfloat64(_eosio_f64_trunc( af )), 0, false );
   }
   int64_t _eosio_f32_trunc_i64s( float af ) {
      float32_t a = to_softfloat32(af);
      if (_eosio_f32_ge(af, 9223372036854775808.0f) || _eosio_f32_lt(af, -9223372036854775808.0f))
         eosio_assert(false,  "Error, f32.convert_s/i64 overflow");
      if (f32_is_nan(a))
         eosio_assert(false,  "Error, f32.convert_s/i64 unrepresentable");
      return f32_to_i64( to_softfloat32(_eosio_f32_trunc( af )), 0, false );
   }
   int64_t _eosio_f64_trunc_i64s( double af ) {
      float64_t a = to_softfloat64(af);
      if (_eosio_f64_ge(af, 9223372036854775808.0) || _eosio_f64_lt(af, -9223372036854775808.0))
         eosio_assert(false,  "Error, f64.convert_s/i64 overflow");
      if (f64_is_nan(a))
         eosio_assert(false,  "Error, f64.convert_s/i64 unrepresentable");

      return f64_to_i64( to_softfloat64(_eosio_f64_trunc( af )), 0, false );
   }
   uint64_t _eosio_f32_trunc_i64u( float af ) {
      float32_t a = to_softfloat32(af);
      if (_eosio_f32_ge(af, 18446744073709551616.0f) || _eosio_f32_le(af, -1.0f))
         eosio_assert(false,  "Error, f32.convert_u/i64 overflow");
      if (f32_is_nan(a))
         eosio_assert(false,  "Error, f32.convert_u/i64 unrepresentable");
      return f32_to_ui64( to_softfloat32(_eosio_f32_trunc( af )), 0, false );
   }
   uint64_t _eosio_f64_trunc_i64u( double af ) {
      float64_t a = to_softfloat64(af);
      if (_eosio_f64_ge(af, 18446744073709551616.0) || _eosio_f64_le(af, -1.0))
         eosio_assert(false,  "Error, f64.convert_u/i64 overflow");
      if (f64_is_nan(a))
         eosio_assert(false,  "Error, f64.convert_u/i64 unrepresentable");
      return f64_to_ui64( to_softfloat64(_eosio_f64_trunc( af )), 0, false );
   }
   float _eosio_i32_to_f32( int32_t a )  {
      return from_softfloat32(i32_to_f32( a ));
   }
   float _eosio_i64_to_f32( int64_t a ) {
      return from_softfloat32(i64_to_f32( a ));
   }
   float _eosio_ui32_to_f32( uint32_t a ) {
      return from_softfloat32(ui32_to_f32( a ));
   }
   float _eosio_ui64_to_f32( uint64_t a ) {
      return from_softfloat32(ui64_to_f32( a ));
   }
   double _eosio_i32_to_f64( int32_t a ) {
      return from_softfloat64(i32_to_f64( a ));
   }
   double _eosio_i64_to_f64( int64_t a ) {
      return from_softfloat64(i64_to_f64( a ));
   }
   double _eosio_ui32_to_f64( uint32_t a ) {
      return from_softfloat64(ui32_to_f64( a ));
   }
   double _eosio_ui64_to_f64( uint64_t a ) {
      return from_softfloat64(ui64_to_f64( a ));
   }

   void prints_l(const char* cstr, uint32_t len) {
      return intrinsics::get().call<intrinsics::prints_l>(cstr, len);
   }

   void prints(const char* cstr) {
      return intrinsics::get().call<intrinsics::prints>(cstr);
   }

   void printi(int64_t value) {
      return intrinsics::get().call<intrinsics::printi>(value);
   }

   void printui(uint64_t value) {
      return intrinsics::get().call<intrinsics::printui>(value);
   }

   void printi128(const int128_t* value) {
      return intrinsics::get().call<intrinsics::printi128>(value);
   }

    void printui128(const uint128_t* value) {
      return intrinsics::get().call<intrinsics::printui128>(value);
   }

   void printsf(float value) {
      return intrinsics::get().call<intrinsics::printsf>(value);
   }

   void printdf(double value) {
      return intrinsics::get().call<intrinsics::printdf>(value);
   }

   void printqf(const long double* value) {
      return intrinsics::get().call<intrinsics::printqf>(value);
   }

   void printn(uint64_t nm) {
      return intrinsics::get().call<intrinsics::printn>(nm);
   }

   void printhex(const void* data, uint32_t len) {
      return intrinsics::get().call<intrinsics::printhex>(data, len);
   }

   void* memset ( void* ptr, int value, size_t num ) {
      unsigned char v = value;
      char* _ptr = (char*)ptr;
      for (int i=0; i < num; i++) {
         _ptr[i] = v;
      }
      return (void*)_ptr;
   }
   void* memcpy ( void* destination, const void* source, size_t num ) {
      char* dest = (char*)destination;
      char* src = (char*)source;
      for (int i=0; i < num; i++) {
         dest[i] = src[i];
      }
      return (void*)dest;
   }

   void* memmove ( void* destination, const void* source, size_t num ) {
      char tmp_buf[num];
      char* dest = (char*)destination;
      char* src = (char*)source;
      for (int i=0; i < num; i++)
         tmp_buf[i] = src[i];
      for (int i=0; i < num; i++)
         dest[i] = tmp_buf[i];
      return (void*)dest;
   }

   void eosio_assert(uint32_t test, const char* msg) {
      if (test == 0) {
         _prints(msg, eosio::cdt::output_stream_kind::std_err);
         _prints_l("\n", 1, eosio::cdt::output_stream_kind::none);
         longjmp(*___env_ptr, 1);
      }
   }

   void eosio_assert_message(uint32_t test, const char* msg, uint32_t len) {
      if (test == 0) {
         _prints_l(msg, len, eosio::cdt::output_stream_kind::std_err);
         _prints_l("\n", 1, eosio::cdt::output_stream_kind::none);
         longjmp(*___env_ptr, 1);
      }
   }

   void eosio_assert_code(uint32_t test, uint64_t code) {
      if (test == 0) {
         char buff[32];
         snprintf(buff, 32, "%llu", code);
         _prints(buff, eosio::cdt::output_stream_kind::std_err);
         _prints_l("\n", 1, eosio::cdt::output_stream_kind::none);
         longjmp(*___env_ptr, 1);
      }
   }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-noreturn"
   void abort() {
      eosio_assert(false, "abort");
   }
#pragma clang diagnostic pop
}
