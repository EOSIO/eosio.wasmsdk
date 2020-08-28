#include <eosio/name.hpp>
#include <eosio/action.hpp>
#include "native/eosio/intrinsics.hpp"
#include "native/eosio/crt.hpp"
#include <cstdint>
#include <functional>
#include <stdio.h>
#include <setjmp.h>
#include "intrinsics_impl.hpp"

eosio::cdt::output_stream std_out;
eosio::cdt::output_stream std_err;

primary_index_store* primary_index;
secondary_index_store* secondary_indexes;

contract_state* global_state;

extern "C" {
   int main(int, char**);
   char* _mmap();

   static jmp_buf env;
   static jmp_buf test_env;
   static volatile int jmp_ret;
   jmp_buf* ___env_ptr = &env;
   char* ___heap;
   char* ___heap_ptr;
   char* ___heap_base_ptr;
   size_t ___pages;
   void ___putc(char c);
   bool ___disable_output;
   bool ___has_failed;
   bool ___earlier_unit_test_has_failed;

   void* __get_heap_base() {
      return ___heap_base_ptr;
   }

   size_t _current_memory() {
      return ___pages;
   }

   size_t _grow_memory(size_t size) {
      if ((___heap_ptr + (size*64*1024)) > (___heap_ptr + 100*1024*1024))
         eosio_assert(false, "__builtin_wasm_grow_memory");
      ___heap_ptr += (size*64*1024);
      return ++___pages;
   }

   void _prints_l(const char* cstr, uint32_t len, uint8_t which) {
      for (int i=0; i < len; i++) {
         if (which == eosio::cdt::output_stream_kind::std_out)
            std_out.push(cstr[i]);
         else if (which == eosio::cdt::output_stream_kind::std_err)
            std_err.push(cstr[i]);
         if (!___disable_output)
            ___putc(cstr[i]);
      }
   }

   void _prints(const char* cstr, uint8_t which) {
      for (int i=0; cstr[i] != '\0'; i++) {
         if (which == eosio::cdt::output_stream_kind::std_out)
            std_out.push(cstr[i]);
         else if (which == eosio::cdt::output_stream_kind::std_err)
            std_err.push(cstr[i]);
         if (!___disable_output)
            ___putc(cstr[i]);
      }
   }

   void __set_env_test() {
      ___env_ptr = &test_env;
   }
   void __reset_env() {
      ___env_ptr = &env;
   }

   int _wrap_main(int argc, char** argv) {
      using namespace eosio::native;
      int ret_val = 0;
      ___heap = _mmap();
      ___heap_ptr = ___heap;
      ___heap_base_ptr = ___heap;
      ___pages = 1;
      ___disable_output = false;
      ___has_failed = false;
      ___earlier_unit_test_has_failed = false;

      primary_index = new primary_index_store;
      secondary_indexes = new secondary_index_store;

      global_state = new contract_state;

      // preset the print functions
      intrinsics::set_intrinsic<intrinsics::prints_l>([](const char* cs, uint32_t l) {
            _prints_l(cs, l, eosio::cdt::output_stream_kind::std_out);
         });
      intrinsics::set_intrinsic<intrinsics::prints>([](const char* cs) {
            _prints(cs, eosio::cdt::output_stream_kind::std_out);
         });
      intrinsics::set_intrinsic<intrinsics::printi>([](int64_t v) {
            printf("%lli", v);
         });
      intrinsics::set_intrinsic<intrinsics::printui>([](uint64_t v) {
            printf("%llu", v);
         });
      intrinsics::set_intrinsic<intrinsics::printi128>([](const int128_t* v) {
            int* tmp = (int*)v;
            printf("0x%04x%04x%04x%04x", tmp[0], tmp[1], tmp[2], tmp[3]);
         });
      intrinsics::set_intrinsic<intrinsics::printui128>([](const uint128_t* v) {
            int* tmp = (int*)v;
            printf("0x%04x%04x%04x%04x", tmp[0], tmp[1], tmp[2], tmp[3]);
         });
      intrinsics::set_intrinsic<intrinsics::printsf>([](float v) {
            char buff[512] = {0};
            std::string ret = std::to_string((int)v);
            memcpy(buff, ret.c_str(), ret.size());
            v -= (int)v;
            buff[ret.size()] = '.';
            size_t size = ret.size();
            for (size_t i=size+1; i < size+10; i++) {
               v *= 10;
               buff[i] = ((int)v)+'0';
               v -= (int)v;
            }
            prints(buff);
         });
      intrinsics::set_intrinsic<intrinsics::printdf>([](double v) {
            char buff[512] = {0};
            std::string ret = std::to_string((long)v);
            memcpy(buff, ret.c_str(), ret.size());
            v -= (long)v;
            buff[ret.size()] = '.';
            size_t size = ret.size();
            for (size_t i=size+1; i < size+10; i++) {
               v *= 10;
               buff[i] = ((int)v)+'0';
               v -= (int)v;
            }
            prints(buff);
         });
      intrinsics::set_intrinsic<intrinsics::printqf>([](const long double* v) {
            int* tmp = (int*)v;
            printf("0x%04x%04x%04x%04x", tmp[0], tmp[1], tmp[2], tmp[3]);
         });
      intrinsics::set_intrinsic<intrinsics::printn>([](uint64_t nm) {
            std::string s = eosio::name(nm).to_string();
            prints_l(s.c_str(), s.length());
         });
      intrinsics::set_intrinsic<intrinsics::printhex>([](const void* data, uint32_t len) {
            constexpr static uint32_t max_stack_buffer_size = 512;
            const char* hex_characters = "0123456789abcdef";

            uint32_t buffer_size = 2*len;
            if(buffer_size < len) eosio_assert( false, "length passed into printhex is too large" );

            void* buffer = (max_stack_buffer_size < buffer_size) ? malloc(buffer_size) : alloca(buffer_size);

            char*          b = reinterpret_cast<char*>(buffer);
            const uint8_t* d = reinterpret_cast<const uint8_t*>(data);
            for( uint32_t i = 0; i < len; ++i ) {
               *b = hex_characters[d[i] >> 4];
               ++b;
               *b = hex_characters[d[i] & 0x0f];
               ++b;
            }

            prints_l(reinterpret_cast<const char*>(buffer), buffer_size);

            if(max_stack_buffer_size < buffer_size) free(buffer);
         });

      jmp_ret = setjmp(env);
      if (jmp_ret == 0) {
         ret_val = main(argc, argv);
      } else {
         ret_val = -1;
      }
      return ret_val;
   }

   extern "C" void* memset(void*, int, size_t);
   extern "C" void __bzero(void* to, size_t cnt) {
      char* cp{static_cast<char*>(to)};
      while (cnt--) *cp++ = 0;
   }
}
