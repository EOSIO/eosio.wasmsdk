#pragma once

#include "function_traits.hpp"
#include "utility.hpp"
#include "preprocessor.hpp"

#include <array>
#include <string_view>
#include <tuple>

namespace bluegrass { namespace meta {
   template <typename T>
   constexpr inline std::string_view type_name() {
      constexpr std::string_view full_name = __PRETTY_FUNCTION__;
      constexpr auto start = full_name.find("T = ");
#ifdef __clang__
      constexpr auto end = full_name.find("]");
#elif __GNUC__
      constexpr auto end = full_name.find(";");
#else
#error "Currently only supporting Clang and GCC compilers"
#endif

      return full_name.substr(start+4, end - start - 4);
   }

   // tag used to define an invalid fields for field_types
   struct invalid_fields {};

   namespace detail {
      template <typename C>
      constexpr inline auto which_field_types() {
         if constexpr ( BLUEGRASS_HAS_MEMBER_TY(C, _bluegrass_meta_refl_valid) )
            return flatten_parameters_t<&C::_bluegrass_meta_refl_fields>{};
         else
            return invalid_fields{};
      }

      template <typename T>
      constexpr inline std::size_t fields_size() {
         if constexpr (std::is_same_v<T, invalid_fields>)
            return 0;
         else
            return std::tuple_size_v<T>;
      }

      template<typename... Args>
      constexpr inline auto va_args_count_helper(Args&&...) { return sizeof...(Args); }

      template <typename T, std::size_t>
      struct fwd_t { using type = T; };

      template <typename T, std::size_t...Is>
      constexpr inline auto produce_tuple(std::index_sequence<Is...>)
         -> std::tuple< typename fwd_t<T, Is>::type ...>;

      template <std::size_t N, typename T>
      constexpr inline auto homogeneous_field_types()
         -> decltype(produce_tuple<T>(std::make_index_sequence<N>{}));

      template <std::size_t Max, std::size_t N, typename T>
      constexpr inline std::size_t homogeneous_field_offset() {
         static_assert(N <= Max);
         return sizeof(T) * N;
      }
   } // ns bluegrass::meta::detail

   template <typename C, typename Derived>
   struct meta_object_base {
      constexpr static inline std::string_view name = type_name<C>();
      constexpr static inline auto get_name() { return name; }

      using field_types = invalid_fields;
      template <std::size_t N>
      using field_type  = invalid_fields;
      constexpr static inline std::size_t field_count = 0;
      constexpr static auto field_names = std::array<std::string_view, 0>{};

      constexpr static inline auto get_field_count() { return Derived::field_count; }
      constexpr static inline auto get_field_name(std::size_t n) { return Derived::field_names[n]; }
      constexpr static inline auto get_field_names() { return Derived::field_names; }

      template <std::size_t N, typename T>
      constexpr static inline auto& get_field(T&& t) {
         return Derived::template get_field<N>(std::forward<T>(t));
      }

      template <std::size_t N, typename T>
      constexpr static inline auto& get_field(const T& t) {
         return Derived::template get_field<N>(t);
      }

      template <std::size_t N, typename T, typename F>
      constexpr inline static auto for_each_field_impl( T&& t, F&& f ) {
         if constexpr (N+1 == get_field_count())
            return f(get_field<N>(std::forward<T>(t)));
         else {
            f(get_field<N>(std::forward<T>(t)));
            return for_each_field_impl<N+1>(std::forward<T>(t), std::forward<F>(f));
         }
      }
      template <typename T, typename F>
      constexpr inline static void for_each_field( T&& t, F&& f ) {
         if constexpr (get_field_count() == 0)
            return;
         else
            return for_each_field_impl<0>(t, f);
      }
   };

   template <typename C>
   struct meta_object : meta_object_base<C, meta_object<C>> {
      using base_t = meta_object_base<C, meta_object<C>>;
      using base_t::name;
      using base_t::get_name;
      using base_t::for_each_field;
      using base_t::get_field_count;
      using base_t::get_field_name;
      using base_t::get_field_names;

      using field_types = decltype(detail::which_field_types<C>());
      template <std::size_t N>
      using field_type = std::tuple_element_t<N, field_types>;
      constexpr static inline std::size_t field_count = detail::fields_size<field_types>();
      constexpr static auto field_names = C::_bluegrass_meta_refl_field_names();

      template <std::size_t N, typename T>
      constexpr static inline auto& get_field(T&& t) {
         static_assert(std::is_same_v<std::decay_t<T>, C>, "get_field<N, T>(T), T should be the same type as C");
         using type = std::tuple_element_t<N, field_types>;
         return *reinterpret_cast<type*>(t.template _bluegrass_meta_refl_field_ptr<N>());
      }

      template <std::size_t N, typename T>
      constexpr static inline auto& get_field(const T& t) {
         static_assert(std::is_same_v<std::decay_t<T>, C>, "get_field<N, T>(T), T should be the same type as C");
         using type = std::tuple_element_t<N, field_types>;
         return *reinterpret_cast<type*>(t.template _bluegrass_meta_refl_field_ptr<N>());
      }
   };

}} // ns bluegrass::meta

#define BLUEGRASS_META_ADDRESS( ignore, FIELD ) (void*)&FIELD
#define BLUEGRASS_META_DECLTYPE( ignore, FIELD ) decltype(FIELD)
#define BLUEGRASS_META_PASS_STR( ignore, X ) #X

#define BLUEGRASS_META_VA_ARGS_SIZE(...)                         \
   bluegrass::meta::detail::va_args_count_helper(                \
         BLUEGRASS_META_FOREACH(                                 \
            BLUEGRASS_META_PASS_STR, "ignored", ##__VA_ARGS__))

#define BLUEGRASS_META_REFL(...)                                                      \
   constexpr void _bluegrass_meta_refl_valid();                                       \
   void _bluegrass_meta_refl_fields                                                   \
      ( BLUEGRASS_META_FOREACH(BLUEGRASS_META_DECLTYPE, "ignored", ##__VA_ARGS__) ){} \
   inline auto _bluegrass_meta_refl_field_ptrs() const {                              \
      return std::array<void *, BLUEGRASS_META_VA_ARGS_SIZE(__VA_ARGS__)>{            \
         BLUEGRASS_META_FOREACH(BLUEGRASS_META_ADDRESS, "ignored", ##__VA_ARGS__)};   \
   }                                                                                  \
   template <std::size_t N>                                                           \
   inline void* _bluegrass_meta_refl_field_ptr() const {                              \
     return _bluegrass_meta_refl_field_ptrs()[N];                                     \
   }                                                                                  \
   constexpr inline static auto _bluegrass_meta_refl_field_names() {                  \
      return std::array<std::string_view, BLUEGRASS_META_VA_ARGS_SIZE(__VA_ARGS__)> { \
         BLUEGRASS_META_FOREACH(BLUEGRASS_META_PASS_STR, "ignored", ##__VA_ARGS__)    \
      };                                                                              \
   }

// EXPERIMENTAL macro to produce meta_object specializations for homogeneous structures
#define BLUEGRASS_HOM_META(_C, _FT, ...)                                                    \
   namespace bluegrass { namespace meta {                                                   \
      template <__VA_ARGS__>                                                                \
      struct meta_object<_C> : meta_object_base<_C, meta_object<_C>> {                      \
         using base_t = meta_object_base<_C, meta_object<_C>>;                              \
         using base_t::name;                                                                \
         using base_t::field_names;                                                         \
         using base_t::get_name;                                                            \
         using base_t::for_each_field;                                                      \
         using base_t::get_field_count;                                                     \
         using base_t::get_field_name;                                                      \
         using base_t::get_field_names;                                                     \
         constexpr static inline std::size_t field_count = sizeof(_C) / sizeof(_FT);        \
         using field_types = decltype(detail::homogeneous_field_types<field_count, _FT>()); \
         template <std::size_t _N>                                                          \
         using field_type = std::tuple_element_t<_N, field_types>;                          \
         template <std::size_t _N>                                                          \
         constexpr static inline auto& get_field(_C& t) {                                   \
            /* very gross and bad code is about to follow */                                \
            return *(reinterpret_cast<_FT*>(&t)+_N);                                        \
         }                                                                                  \
      };                                                                                    \
   }}
