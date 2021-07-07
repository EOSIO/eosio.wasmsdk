/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once
#include "action.hpp"
#include "system.hpp"
#include "../../core/eosio/time.hpp"
#include "../../core/eosio/serialize.hpp"

#include <vector>

namespace eosio {
   namespace internal_use_do_not_use {
      extern "C" {
         __attribute__((eosio_wasm_import))
         void send_deferred(const uint128_t*, uint64_t, const char*, size_t, uint32_t);

         __attribute__((eosio_wasm_import))
         int cancel_deferred(const uint128_t*);

         __attribute__((eosio_wasm_import))
         size_t read_transaction(char*, size_t);

         __attribute__((eosio_wasm_import))
         size_t transaction_size();

         __attribute__((eosio_wasm_import))
         int tapos_block_num();

         __attribute__((eosio_wasm_import))
         int tapos_block_prefix();

         __attribute__((eosio_wasm_import))
         uint32_t expiration();

         __attribute__((eosio_wasm_import))
         int get_action( uint32_t, uint32_t, char*, size_t);

         __attribute__((eosio_wasm_import))
         int get_context_free_data( uint32_t, char*, size_t);
      }
   }

  /**
   *  @defgroup transaction Transaction
   *  @ingroup contracts
   *  @brief Type-safe C++ wrappers for transaction C API
   *
   *  @details An inline message allows one contract to send another contract a message
   *  which is processed immediately after the current message's processing
   *  ends such that the success or failure of the parent transaction is
   *  dependent on the success of the message. If an inline message fails in
   *  processing then the whole tree of transactions and actions rooted in the
   *  block will me marked as failing and none of effects on the database will
   *  persist.
   *
   *  Inline actions and Deferred transactions must adhere to the permissions
   *  available to the parent transaction or, in the future, delegated to the
   *  contract account for future use.
   *
   *  @note There are some methods from the @ref transactioncapi that can be used directly from C++
   */

  /**
   *  @ingroup transaction
   */
   typedef std::tuple<uint16_t, std::vector<char>> extension;

   /**
    *  @ingroup transaction
    */
   typedef std::vector<extension> extensions_type;

   /**
    *  Class transaction_header contains details about the transaction
    *
    *  @ingroup transaction
    *  @brief Contains details about the transaction
    */

   class transaction_header {
   public:

      /**
       * Construct a new transaction_header with an expiration of now + 60 seconds.
       *
       * @brief Construct a new transaction_header object initialising the transaction header expiration to now + 60 seconds
       */
      transaction_header( time_point_sec exp = time_point_sec(current_time_point()) + 60)
         :expiration(exp)
      {}

      time_point_sec  expiration;
      uint16_t        ref_block_num;
      uint32_t        ref_block_prefix;
      unsigned_int    max_net_usage_words = 0UL; /// number of 8 byte words this transaction can serialize into after compressions
      uint8_t         max_cpu_usage_ms = 0UL; /// number of CPU usage units to bill transaction for
      unsigned_int    delay_sec = 0UL; /// number of seconds to delay transaction, default: 0

      EOSLIB_SERIALIZE( transaction_header, (expiration)(ref_block_num)(ref_block_prefix)(max_net_usage_words)(max_cpu_usage_ms)(delay_sec) )
   };

   /**
    *  Class transaction contains the actions, context_free_actions and extensions type for a transaction
    *
    *  @ingroup transaction
    */
   class transaction : public transaction_header {
   public:

      /**
       * Construct a new transaction with an expiration of now + 60 seconds.
       */
      transaction(time_point_sec exp = time_point_sec(current_time_point()) + 60) : transaction_header( exp ) {}

      /**
       *  Sends this transaction, packs the transaction then sends it as a deferred transaction
       *
       *  @details Writes the symbol_code as a string to the provided char buffer
       *  @param sender_id - ID of sender
       *  @param payer - Account paying for RAM
       *  @param replace_existing - Defaults to false, if this is `0`/false then if the provided sender_id is already in use by an in-flight transaction from this contract, which will be a failing assert. If `1` then transaction will atomically cancel/replace the inflight transaction
       */
      void send(const uint128_t& sender_id, name payer, bool replace_existing = false) const {
         auto serialize = pack(*this);
         internal_use_do_not_use::send_deferred(&sender_id, payer.value, serialize.data(), serialize.size(), replace_existing);
      }

      std::vector<action>  context_free_actions;
      std::vector<action>  actions;
      extensions_type      transaction_extensions;

      EOSLIB_SERIALIZE_DERIVED( transaction, transaction_header, (context_free_actions)(actions)(transaction_extensions) )
   };

   /**
    *  Struct onerror contains and sender id and packed transaction
    *
    *  @ingroup transaction
    */
   struct onerror {
      uint128_t          sender_id;
      std::vector<char> sent_trx;

     /**
      *  from_current_action unpacks and returns a onerror struct
      *
      *  @ingroup transaction
      */
      static onerror from_current_action() {
         return unpack_action_data<onerror>();
      }

     /**
      * Unpacks and returns a transaction
      */
      transaction unpack_sent_trx() const {
         return unpack<transaction>(sent_trx);
      }

      EOSLIB_SERIALIZE( onerror, (sender_id)(sent_trx) )
   };

   /**
    *  Send a deferred transaction
    *
    *  @ingroup transaction
    *  @param sender_id - Account name of the sender of this deferred transaction
    *  @param payer - Account name responsible for paying the RAM for this deferred transaction
    *  @param serialized_transaction - The packed transaction to be deferred
    *  @param size - The size of the packed transaction, required for persistence.
    *  @param replace - If true, will replace an existing transaction.
    */
   inline void send_deferred(const uint128_t& sender_id, name payer, const char* serialized_transaction, size_t size, bool replace = false) {
     internal_use_do_not_use::send_deferred(&sender_id, payer.value, serialized_transaction, size, replace);
   }
   /**
    *  Retrieve the indicated action from the active transaction.
    *
    *  @ingroup transaction
    *  @param type - 0 for context free action, 1 for action
    *  @param index - the index of the requested action
    *  @return the indicated action
    */
   inline action get_action( uint32_t type, uint32_t index ) {
      constexpr size_t max_stack_buffer_size = 512;
      int s = internal_use_do_not_use::get_action( type, index, nullptr, 0 );
      eosio::check( s > 0, "get_action size failed" );
      size_t size = static_cast<size_t>(s);
      char* buffer = (char*)( max_stack_buffer_size < size ? malloc(size) : alloca(size) );
      auto size2 = internal_use_do_not_use::get_action( type, index, buffer, size );
      eosio::check( size == static_cast<size_t>(size2), "get_action failed" );
      return eosio::unpack<eosio::action>( buffer, size );
   }

   /**
    *  Access a copy of the currently executing transaction.
    *
    *  @ingroup transaction
    *  @return the currently executing transaction
    */
   inline size_t read_transaction(char* ptr, size_t sz) {
      return internal_use_do_not_use::read_transaction( ptr, sz );
   }

    /**
     *  Cancels a deferred transaction.
     *
     *  @ingroup transaction
     *  @param sender_id - The id of the sender
     *
     *  @pre The deferred transaction ID exists.
     *  @pre The deferred transaction ID has not yet been published.
     *  @post Deferred transaction canceled.
     *
     *  @return 1 if transaction was canceled, 0 if transaction was not found
     *
     *  Example:
*
     *  @code
     *  id = 0xffffffffffffffff
     *  cancel_deferred( id );
     *  @endcode
     */
   inline int cancel_deferred(const uint128_t& sender_id) {
      return internal_use_do_not_use::cancel_deferred(&sender_id);
   }

   /**
    *  Gets the size of the currently executing transaction.
    *
    *  @ingroup transaction
    *  @return size of the currently executing transaction
    */
   inline size_t transaction_size() {
      return internal_use_do_not_use::transaction_size();
   }

   /**
    *  Gets the block number used for TAPOS on the currently executing transaction.
    *
    *  @ingroup transaction
    *  @return block number used for TAPOS on the currently executing transaction
    *  Example:
    *  @code
    *  int tbn = tapos_block_num();
    *  @endcode
    */
   inline int tapos_block_num() {
      return internal_use_do_not_use::tapos_block_num();
   }

   /**
    *  Gets the block prefix used for TAPOS on the currently executing transaction.
    *
    *  @ingroup transaction
    *  @return block prefix used for TAPOS on the currently executing transaction
    *  Example:
    *  @code
    *  int tbp = tapos_block_prefix();
    *  @endcode
    */
   inline int tapos_block_prefix() {
      return internal_use_do_not_use::tapos_block_prefix();
   }

   /**
    *  Gets the expiration of the currently executing transaction.
    *
    *  @ingroup transaction
    *  @brief Gets the expiration of the currently executing transaction.
    *  @return expiration of the currently executing transaction in seconds since Unix epoch
    */
   inline uint32_t expiration() {
      return internal_use_do_not_use::expiration();
   }

   /**
    * Retrieve the signed_transaction.context_free_data[index].
    *
    *  @ingroup transaction
    *  @param index - the index of the context_free_data entry to retrieve
    *  @param buff - output buff of the context_free_data entry
    *  @param size - amount of context_free_data[index] to retrieve into buff, 0 to report required size
    *  @return size copied, or context_free_data[index].size() if 0 passed for size, or -1 if index not valid
    */
   inline int get_context_free_data( uint32_t index, char* buff, size_t size ) {
      return internal_use_do_not_use::get_context_free_data(index, buff, size);
   }
}
