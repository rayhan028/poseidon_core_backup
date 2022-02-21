include(../cmake/Testing.cmake.in)

include_directories(${CATCH_INCLUDE_DIR})

#=============================================

file(GLOB_RECURSE ALL_TEST_CPP *_test.cpp)

foreach(ONE_TEST_CPP ${ALL_TEST_CPP})
    get_filename_component(ONE_TEST_EXEC ${ONE_TEST_CPP} NAME_WLE)
    if (BUILD_LDBC MATCHES OFF AND ONE_TEST_EXEC STREQUAL "ldbc_test")
      message(STATUS "skipping ldbc_test ...")
    elseif (CSR_DELTA MATCHES OFF AND ONE_TEST_EXEC STREQUAL "csr_delta_test")
      message(STATUS "skipping csr_delta_test ...")
    elseif (USE_GUNROCK MATCHES OFF AND ONE_TEST_EXEC STREQUAL "gunrock_test")
      message(STATUS "skipping gunrock_test ...")
    else()
      do_test(${ONE_TEST_EXEC})
    endif()
endforeach()

#=============================================

add_executable(abort_insert
  abort_insert.cpp
 )

target_link_libraries(abort_insert
  poseidon_core
  ${PMDK_LIBRARIES}
)

#=============================================

add_test (NAME python-tests
  COMMAND ${PYTHON_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test
)