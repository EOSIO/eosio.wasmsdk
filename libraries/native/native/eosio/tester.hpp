#pragma once
#include <eosio/eosio.hpp>
#include "crt.hpp"
#include "intrinsics.hpp"
#include <setjmp.h>
#include <vector>

extern "C" bool ___disable_output;
extern "C" bool ___has_failed;
extern "C" bool ___earlier_unit_test_has_failed;

inline void silence_output(bool t) {
   ___disable_output = t;
}
inline bool has_failed() {
   return ___has_failed;
}

extern "C" void apply(uint64_t, uint64_t, uint64_t);

template <typename Pred, typename F, typename... Args>
inline bool expect_assert(bool check, const std::string& li, Pred&& pred, F&& func, Args... args) {
   std_err.clear();
   __set_env_test();
   int ret = setjmp(*___env_ptr);
   bool disable_out = ___disable_output;
   if (ret == 0) {
      func(args...);
      __reset_env();
      silence_output(false);
      if (!check)
         eosio::check(false, std::string("error : expect_assert, no assert {"+li+"}").c_str());
      eosio::print("error : expect_assert, no assert {"+li+"}\n");
      silence_output(disable_out);
      return false;
   }
   __reset_env();
   bool passed = pred(std_err.get());
   std_err.clear();
   silence_output(false);
   if (!check)
      eosio::check(passed, std::string("error : expect_assert, wrong assert {"+li+"}").c_str());
   if (!passed)
      eosio::print("error : expect_assert, wrong assert {"+li+"}\n");
   silence_output(disable_out);

   return passed;
}

template <size_t N, typename F, typename... Args>
inline bool expect_assert(bool check, const std::string& li, const char (&expected)[N], F&& func, Args... args) {
   return expect_assert(check, li,
         [&](const std::string& s) {
            return std_err.index == N-1 &&
            memcmp(expected, s.c_str(), N-1) == 0; }, func, args...);
}

template <typename Pred, typename F, typename... Args>
inline bool expect_print(bool check, const std::string& li, Pred&& pred, F&& func, Args... args) {
   std_out.clear();
   func(args...);
   bool passed = pred(std_out.get());
   auto actual = std::string(std_out.get());
   std_out.clear();
   bool disable_out = ___disable_output;
   silence_output(false);
   if (!check) {
      eosio::check(passed, std::string("error : wrong print message {"+li+"}\nGot: ") + actual);
   }
   if (!passed) {
      eosio::print("Error : wrong print message {"+li+"}\n");
      eosio::print("Got: " + actual + "\n");
   }
   silence_output(disable_out);
   return passed;
}

template <size_t N, typename F, typename... Args>
inline bool expect_print(bool check, const std::string& li, const char (&expected)[N], F&& func, Args... args) {
   return expect_print(check, li,
         [&](const std::string& s) {
            return std_out.index == N-1 &&
            memcmp(expected, s.c_str(), N-1) == 0; }, func, args...);

}

#define CHECK_ASSERT(...) \
   ___has_failed |= !expect_assert(true, std::string(__FILE__)+":"+__func__+":"+(std::to_string(__LINE__)), __VA_ARGS__);

#define REQUIRE_ASSERT(...) \
   expect_assert(false, std::string(__FILE__)+":"+__func__+":"+(std::to_string(__LINE__)),  __VA_ARGS__);

#define CHECK_PRINT(...) \
   ___has_failed |= !expect_print(true, std::string(__FILE__)+":"+__func__+":"+(std::to_string(__LINE__)), __VA_ARGS__);

#define REQUIRE_PRINT(...) \
   expect_print(false, std::string(__FILE__)+":"+__func__+":"+(std::to_string(__LINE__)),  __VA_ARGS__);

#define CHECK_EQUAL(X, Y) \
   if (!(X == Y)) { \
      ___has_failed = true; \
      eosio::print(std::string("CHECK_EQUAL failed (")+#X+" != "+#Y+") {"+__FILE__+":"+std::to_string(__LINE__)+"}\n"); \
   }

#define REQUIRE_EQUAL(X, Y) \
   eosio::check(X == Y, std::string(std::string("REQUIRE_EQUAL failed (")+#X+" != "+#Y+") {"+__FILE__+":"+std::to_string(__LINE__)+"}").c_str());

#define CHECK_NOT_EQUAL(X, Y) \
   if (!(X != Y)) { \
      ___has_failed = true; \
      eosio::print(std::string("CHECK_NOT_EQUAL failed (")+#X+" == "+#Y+") {"+__FILE__+":"+std::to_string(__LINE__)+"}\n"); \
   }

#define REQUIRE_NOT_EQUAL(X, Y) \
   eosio::check(X != Y, std::string(std::string("REQUIRE_NOT_EQUAL failed (")+#X+" == "+#Y+") {"+__FILE__+":"+std::to_string(__LINE__)+"}").c_str());

#define EOSIO_TEST(X) \
   int X ## _ret = setjmp(*___env_ptr); \
   if ( X ## _ret == 0 ) \
      X(); \
   else { \
      bool ___original_disable_output = ___disable_output; \
      silence_output(false); \
      eosio::print("\033[1;37m", #X, " \033[0;37munit test \033[1;31mfailed\033[0m (aborted)\n"); \
      ___has_failed = true; \
      silence_output(___original_disable_output); \
   }

#define EOSIO_TEST_BEGIN(X) \
   void X() { \
      static constexpr const char* __test_name = #X; \
      ___earlier_unit_test_has_failed = ___has_failed; \
      ___has_failed = false;

#define EOSIO_TEST_END \
      bool ___original_disable_output = ___disable_output; \
      silence_output(false); \
      if (___has_failed) \
         eosio::print("\033[1;37m", __test_name, " \033[0;37munit test \033[1;31mfailed\033[0m\n"); \
      else \
         eosio::print("\033[1;37m", __test_name, " \033[0;37munit test \033[1;32mpassed\033[0m\n"); \
      silence_output(___original_disable_output); \
      ___has_failed |= ___earlier_unit_test_has_failed; \
      ___earlier_unit_test_has_failed = ___has_failed; \
   }

struct contract_state {
   eosio::name code;
};
extern contract_state* global_state;
void apply_state(contract_state* cs) {
   global_state = cs;
}