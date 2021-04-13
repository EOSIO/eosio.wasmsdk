#pragma once
#include <eosio/action.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>

#include <boost/mp11/tuple.hpp>

#ifdef __cplusplus 
extern "C" {
#endif
/**
 * Set the action return value which will be included in the action_receipt
 * @brief Set the action return value
 * @param return_value - serialized return value
 * @param size - size of serialized return value in bytes
 * @pre `return_value` is a valid pointer to an array at least `size` bytes long
 */
__attribute__((eosio_wasm_import))
void set_action_return_value(void *return_value, size_t size);

#ifdef __cplusplus 
} // extern "C"
#endif


namespace eosio {

  /**
   * @defgroup dispatcher Dispatcher
   * @ingroup contracts
   * @brief Defines C++ functions to dispatch action to proper action handler inside a contract
   */


   /// @cond IMPLEMENTATIONS

   template<typename Contract, typename FirstAction>
   bool dispatch( uint64_t code, uint64_t act ) {
      if( code == FirstAction::get_account() && FirstAction::get_name() == act ) {
         Contract().on( unpack_action_data<FirstAction>() );
         return true;
      }
      return false;
   }

   /// @endcond

   /**
    * This method will dynamically dispatch an incoming set of actions to
    *
    * ```
    * static Contract::on( ActionType )
    * ```
    *
    * For this to work the Actions must be derived from eosio::contract
    *
    * @ingroup dispatcher
    *
    */
   template<typename Contract, typename FirstAction, typename SecondAction, typename... Actions>
   bool dispatch( uint64_t code, uint64_t act ) {
      if( code == FirstAction::get_account() && FirstAction::get_name() == act ) {
         Contract().on( unpack_action_data<FirstAction>() );
         return true;
      }
      return eosio::dispatch<Contract,SecondAction,Actions...>( code, act );
   }


   /**
    * Unpack the received action and execute the correponding action handler
    *
    * @ingroup dispatcher
    * @tparam T - The contract class that has the correponding action handler, this contract should be derived from eosio::contract
    * @tparam Q - The namespace of the action handler function
    * @tparam Args - The arguments that the action handler accepts, i.e. members of the action
    * @param obj - The contract object that has the correponding action handler
    * @param func - The action handler
    * @return true
    */
   template<typename T, typename R, typename... Args>
   bool execute_action( name self, name code, R (T::*func)(Args...)  ) {
      size_t size = action_data_size();

      //using malloc/free here potentially is not exception-safe, although WASM doesn't support exceptions
      void* buffer = nullptr;
      if( size > 0 ) {
         buffer = malloc(size);
         read_action_data( buffer, size );
      }

      std::tuple<std::decay_t<Args>...> args;
      datastream<const char*> ds((char*)buffer, size);
      ds >> args;

      T inst(self, code, ds);

      auto f2 = [&]( auto... a ){
         return ((&inst)->*func)( a... );
      };

      if constexpr( !std::is_same_v<void,R> ) {
         auto r = eosio::pack(boost::mp11::tuple_apply( f2, args ));
         ::set_action_return_value( r.data(), r.size() );
      } else {
         boost::mp11::tuple_apply( f2, args );
      }

      if ( size ) {
         free(buffer);
      }
      return true;
   }

   template<typename T, typename R, typename... Args>
   bool execute_action( name self, name code, R (T::*func)(Args...)const  ) {
      size_t size = action_data_size();

      //using malloc/free here potentially is not exception-safe, although WASM doesn't support exceptions
      void* buffer = nullptr;
      if( size > 0 ) {
         buffer = malloc(size);
         read_action_data( buffer, size );
      }

      std::tuple<std::decay_t<Args>...> args;
      datastream<const char*> ds((char*)buffer, size);
      ds >> args;

      T inst(self, code, ds);

      auto f2 = [&]( auto... a ){
         return ((&inst)->*func)( a... );
      };

      if constexpr( !std::is_same_v<void,R> ) {
         auto r = eosio::pack(boost::mp11::tuple_apply( f2, args ));
         ::set_action_return_value( r.data(), r.size() );
      } else {
         boost::mp11::tuple_apply( f2, args );
      }

      if ( size ) {
         free(buffer);
      }
      return true;
   }

  /// @cond INTERNAL

 // Helper macro for EOSIO_DISPATCH_INTERNAL
 #define EOSIO_DISPATCH_INTERNAL( r, OP, elem ) \
   case eosio::hash_name( BOOST_PP_STRINGIZE(elem) ): \
       executed = eosio::execute_action( eosio::name(receiver), eosio::name(code), &OP::elem ); \
       break;

 // Helper macro for EOSIO_DISPATCH
 #define EOSIO_DISPATCH_HELPER( TYPE,  MEMBERS ) \
    BOOST_PP_SEQ_FOR_EACH( EOSIO_DISPATCH_INTERNAL, TYPE, MEMBERS )

/// @endcond

/**
 * Convenient macro to create contract apply handler
 *
 * @ingroup dispatcher
 * @note To be able to use this macro, the contract needs to be derived from eosio::contract
 * @param TYPE - The class name of the contract
 * @param MEMBERS - The sequence of available actions supported by this contract
 *
 * Example:
 * @code
 * EOSIO_DISPATCH( eosio::bios, (setpriv)(setalimits)(setglimits)(setprods)(reqauth) )
 * @endcode
 */
#define EOSIO_DISPATCH( TYPE, MEMBERS ) \
extern "C" { \
   [[eosio::wasm_entry]] \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      if( code == receiver ) { \
         bool executed = false; \
         switch( action ) { \
            EOSIO_DISPATCH_HELPER( TYPE, MEMBERS ) \
         } \
         /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
      } \
   } \
} \

#define EOSIO_ACTION_WRAPPER_DECL(r, data, action) \
   using action = eosio::action_wrapper<BOOST_PP_CAT(BOOST_PP_STRINGIZE(action),_h), &__contract_class::action, __contract_account>;

#define EOSIO_ACTIONS( CONTRACT_CLASS, CONTRACT_ACCOUNT, ... ) \
   namespace actions { \
      static constexpr auto __contract_account = CONTRACT_ACCOUNT; \
      using __contract_class   = CONTRACT_CLASS; \
      BOOST_PP_SEQ_FOR_EACH( EOSIO_ACTION_WRAPPER_DECL, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) )  \
      \
      inline void eosio_apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
         eosio::check( code == receiver, "notifications not supported by dispatcher" );  \
         bool executed = false; \
         switch( action ) {  \
            EOSIO_DISPATCH_HELPER( CONTRACT_CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) ) \
         } \
         eosio::check( executed == true, "unkown action" ); \
      } \
   }

#define EOSIO_ACTION_DISPATCHER( NAMESPACE ) \
extern "C" { \
   [[eosio::wasm_entry]] \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      NAMESPACE :: eosio_apply( receiver, code, action ); \
   }\
}\

}
