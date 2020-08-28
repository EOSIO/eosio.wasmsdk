#pragma once
#include <setjmp.h>

namespace eosio { namespace cdt {
   enum output_stream_kind {
      std_out,
      std_err,
      none
   };
   struct output_stream {
      static const int MAX_OUTPUT = 1024*2;
      char output[MAX_OUTPUT];
      size_t index = 0;
      std::string to_string()const { return std::string((const char*)output, index); }
      const char* get()const { return output; }
      void push(char c) { 
         if (index+1 < MAX_OUTPUT) {
            output[index++] = c;
         } else {
            // TODO:
            // What should we do here?
         }
      }
      void clear() { index = 0; }
   };
}} //ns eosio::cdt

extern eosio::cdt::output_stream std_out;
extern eosio::cdt::output_stream std_err;
extern "C" jmp_buf* ___env_ptr;
extern "C" char*    ___heap_ptr;

extern "C" {
   void __set_env_test();
   void __reset_env();
   void _prints_l(const char* cstr, uint32_t len, uint8_t which);
   void _prints(const char* cstr, uint8_t which);
}
