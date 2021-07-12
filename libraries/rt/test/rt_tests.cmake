execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_ashlti3 RESULT_VARIABLE res)
if(res EQUAL "1")
    message( FATAL_ERROR "test_ashlti3 failed")
endif()

execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_ashrti3 RESULT_VARIABLE res)
if(res EQUAL "1")
    message( FATAL_ERROR "test_ashrti3 failed")
endif()

execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_lshrti3 RESULT_VARIABLE res)
if(res EQUAL "1")
    message( FATAL_ERROR "test_lshrti3 failed")
endif()

execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_divti3 RESULT_VARIABLE res)
if(res EQUAL "1")
    message( FATAL_ERROR "test_divti3 failed")
endif()

execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_udivti3 RESULT_VARIABLE res)
if(res EQUAL "1")
    message( FATAL_ERROR "test_udivti3 failed")
endif()

execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_multi3 RESULT_VARIABLE res)
if(res EQUAL "1")
    message( FATAL_ERROR "test_multi3 failed")
endif()

execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_modti3 RESULT_VARIABLE res)
if(res EQUAL "1")
    message( FATAL_ERROR "test_modti3 failed")
endif()

execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_umodti3 RESULT_VARIABLE res)
if(res EQUAL "1")
    message( FATAL_ERROR "test_umodti3 failed")
endif()
