/*
 * Copyright 2016 WebAssembly Community Group participants
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cassert>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "src/apply-names.h"
#include "src/binary-reader.h"
#include "src/binary-writer.h"
#include "src/binary-reader-ir.h"
#include "src/error-handler.h"
#include "src/feature.h"
#include "src/generate-names.h"
#include "src/ir.h"
#include "src/leb128.h"
#include "src/option-parser.h"
#include "src/stream.h"
#include "src/validator.h"
#include "src/wast-lexer.h"
#include "src/wat-writer.h"

using namespace wabt;

static int s_verbose;
static std::string s_infile;
static std::string s_outfile;
static Features s_features;
static WriteBinaryOptions s_write_binary_options;
static std::unique_ptr<FileStream> s_log_stream;

static const char s_description[] =
R"(  Read a file in the WebAssembly binary format, strip bss or any data segment that is only initialized to zeros, and other post processing.

  $ eosio-pp test.wasm -o test.stripped.wasm

  # or original replacement
  $ wasm2wat test.wasm
)";

static void ParseOptions(int argc, char** argv) {
  OptionParser parser("postprocess", s_description);

  parser.AddOption('v', "verbose", "Use multiple times for more info", []() {
    s_verbose++;
    s_log_stream = FileStream::CreateStdout();
  });
  parser.AddHelpOption();
  parser.AddOption(
      'o', "output", "FILENAME",
      "Output file for the generated wast file, by default use stdout",
      [](const char* argument) {
        s_outfile = argument;
        ConvertBackslashToSlash(&s_outfile);
      });
  parser.AddOption(
      'n', "allow-names",
      "Preserve the name section",
      []() {
         s_write_binary_options.write_debug_names = true;
      });
  parser.AddArgument("filename", OptionParser::ArgumentCount::One,
                     [](const char* argument) {
                       s_infile = argument;
                       ConvertBackslashToSlash(&s_infile);
                     });
  parser.Parse(argc, argv);
}

uint32_t GetHeapPtr( Module& mod, const std::vector<uint8_t>& buff ) {
   size_t offset = mod.GetGlobal(Var(1))->init_expr.begin()->loc.offset;
   uint32_t heap_ptr;
   ReadS32Leb128(buff.data()+offset+4, buff.data()+offset+9, &heap_ptr);
   return heap_ptr;
}

uint32_t GetStackPtr( Module& mod, const std::vector<uint8_t>& buff ) {
   size_t offset = mod.GetGlobal(Var(0))->init_expr.begin()->loc.offset;
   uint32_t stack_ptr;
   ReadS32Leb128(buff.data()+offset+4, buff.data()+offset+9, &stack_ptr);
   return stack_ptr;
}

void StripZeroedData( Module& mod, size_t& fix_bytes ) {
   std::vector<DataSegment*> ds;
   for ( auto DS : mod.data_segments ) {
      bool isZeroed = true;
      for ( auto datum : DS->data ) {
         isZeroed &= datum == 0;
      }
      if (!isZeroed) {
         ds.push_back(DS);
      } else {
         fix_bytes += DS->data.size();
      }
   }
   mod.data_segments = ds;
}

void AddHeapPointerData( Module& mod, size_t fixup, const std::vector<uint8_t>& buff, DataSegment& ds ) {
   uint32_t heap_ptr  = ((GetHeapPtr(mod, buff)) + 7) & ~7; // align to 8 bytes
   Const c;
   c.I32(0);
   std::unique_ptr<Expr> ce(new ConstExpr(c));
   ds.memory_var = Var(0);
   ds.offset = ExprList{std::move(ce)};
   uint8_t* dat = reinterpret_cast<uint8_t*>(&heap_ptr);
   ds.data = std::vector<uint8_t>{dat[0],
                                   dat[1],
                                   dat[2],
                                   dat[3]};
   mod.data_segments.push_back(&ds);
}

void construct_apply( Module& mod ) {
}

void WriteBufferToFile(string_view filename,
                       const OutputBuffer& buffer) {
  buffer.WriteToFile(filename);
}

int ProgramMain(int argc, char** argv) {
  Result result;

  InitStdio();
  ParseOptions(argc, argv);

  std::vector<uint8_t> file_data;
  bool stub = false;
  std::unique_ptr<FileStream> s_log_stream_s;
  result = ReadFile(s_infile.c_str(), &file_data);
  DataSegment _hds;
  if (Succeeded(result)) {
    ErrorHandlerFile error_handler(Location::Type::Binary);
    Module module;
    const bool kStopOnFirstError = true;
    ReadBinaryOptions options(s_features, s_log_stream_s.get(),
                              s_write_binary_options.write_debug_names, kStopOnFirstError,
                              stub);
    result = ReadBinaryIr(s_infile.c_str(), file_data.data(),
                          file_data.size(), &options, &error_handler, &module);

    if (Succeeded(result)) {
      size_t fixup = 0;
      StripZeroedData(module, fixup);
      AddHeapPointerData(module, fixup, file_data, _hds);
     if (Succeeded(result)) {
      MemoryStream stream(s_log_stream.get());
      result =
          WriteBinaryModule(&stream, &module, &s_write_binary_options);

      if (Succeeded(result)) {
        if (s_outfile.empty()) {
          s_outfile = s_infile;
        }
        WriteBufferToFile(s_outfile.c_str(), stream.output_buffer());
      }
    }
   }

  }
  return result != Result::Ok;
}

int main(int argc, char** argv) {
  WABT_TRY
  return ProgramMain(argc, argv);
  WABT_CATCH_BAD_ALLOC_AND_EXIT
}
