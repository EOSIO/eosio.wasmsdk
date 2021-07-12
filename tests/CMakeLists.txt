find_program(SCCACHE_FOUND sccache)
if (SCCACHE_FOUND)
   message(STATUS "Using sccache")
   set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE sccache)
else()
   find_program(CCACHE_FOUND ccache)
   if (CCACHE_FOUND)
      message(STATUS "Using ccache")
      set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
   endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-missing-ricardian-clause")

add_test( asset_tests ${CMAKE_BINARY_DIR}/tests/unit/asset_tests )
set_property(TEST asset_tests PROPERTY LABELS unit_tests)
add_test( binary_extension_tests ${CMAKE_BINARY_DIR}/tests/unit/binary_extension_tests )
set_property(TEST binary_extension_tests PROPERTY LABELS unit_tests)
add_test( crypto_tests ${CMAKE_BINARY_DIR}/tests/unit/crypto_tests )
set_property(TEST crypto_tests PROPERTY LABELS unit_tests)
add_test( datastream_tests ${CMAKE_BINARY_DIR}/tests/unit/datastream_tests )
set_property(TEST datastream_tests PROPERTY LABELS unit_tests)
add_test( fixed_bytes_tests ${CMAKE_BINARY_DIR}/tests/unit/fixed_bytes_tests )
set_property(TEST fixed_bytes_tests PROPERTY LABELS unit_tests)
add_test( name_tests ${CMAKE_BINARY_DIR}/tests/unit/name_tests )
set_property(TEST name_tests PROPERTY LABELS unit_tests)
add_test( rope_tests ${CMAKE_BINARY_DIR}/tests/unit/rope_tests )
set_property(TEST rope_tests PROPERTY LABELS unit_tests)
add_test( print_tests ${CMAKE_BINARY_DIR}/tests/unit/print_tests )
set_property(TEST print_tests PROPERTY LABELS unit_tests)
add_test( serialize_tests ${CMAKE_BINARY_DIR}/tests/unit/serialize_tests )
set_property(TEST serialize_tests PROPERTY LABELS unit_tests)
add_test( string_tests ${CMAKE_BINARY_DIR}/tests/unit/string_tests )
set_property(TEST string_tests PROPERTY LABELS unit_tests)
add_test( symbol_tests ${CMAKE_BINARY_DIR}/tests/unit/symbol_tests )
set_property(TEST symbol_tests PROPERTY LABELS unit_tests)
add_test( system_tests ${CMAKE_BINARY_DIR}/tests/unit/system_tests )
set_property(TEST system_tests PROPERTY LABELS unit_tests)
add_test( time_tests ${CMAKE_BINARY_DIR}/tests/unit/time_tests )
set_property(TEST time_tests PROPERTY LABELS unit_tests)
add_test( varint_tests ${CMAKE_BINARY_DIR}/tests/unit/varint_tests )
set_property(TEST varint_tests PROPERTY LABELS unit_tests)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/unit/version_tests.sh ${CMAKE_BINARY_DIR}/tests/unit/version_tests.sh COPYONLY)
add_test(NAME version_tests COMMAND ${CMAKE_BINARY_DIR}/tests/unit/version_tests.sh "${VERSION_FULL}" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_property(TEST version_tests PROPERTY LABELS unit_tests)

add_test( NAME toolchain_tests COMMAND ${CMAKE_BINARY_DIR}/tools/toolchain-tester/toolchain-tester ${CMAKE_SOURCE_DIR}/tests/toolchain --cdt ${CMAKE_BINARY_DIR}/bin )
set_property(TEST toolchain_tests PROPERTY LABELS toolchain_tests)

add_test(NAME rt_tests COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/libraries/rt/test/rt_tests.cmake WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libraries/rt/test/)
set_property(TEST rt_tests PROPERTY LABELS unit_tests)

if (EOSIO_RUN_INTEGRATION_TESTS)
   find_package(eosio)

   if (eosio_FOUND)
      add_test(integration_tests ${EOSIO_ROOT}/bin/eosio-tester ${CMAKE_BINARY_DIR}/tests/integration/integration_tests.wasm)
      set_property(TEST integration_tests PROPERTY LABELS integration_tests)

      add_test(
         NAME tester_tests
         COMMAND ${EOSIO_ROOT}/bin/eosio-tester ${CMAKE_BINARY_DIR}/tests/tester/tester.wasm
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/tester
      )
      set_property(TEST tester_tests PROPERTY LABELS tester_tests)
   endif()
endif()

ExternalProject_Add(
   TesterTests
   SOURCE_DIR "${CMAKE_SOURCE_DIR}/tests/tester"
   BINARY_DIR "${CMAKE_BINARY_DIR}/tests/tester"
   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_BINARY_DIR}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DEOSIO_CDT_BIN=${CMAKE_BINARY_DIR}/lib/cmake/eosio.cdt/ -DBASE_BINARY_DIR=${CMAKE_BINARY_DIR} -D__APPLE=${APPLE}
   UPDATE_COMMAND ""
   PATCH_COMMAND  ""
   TEST_COMMAND   ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
   DEPENDS EosioTools EosioWasmLibraries
)
