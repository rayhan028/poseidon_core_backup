file(GLOB_RECURSE ALL_BENCH_CPP *.cpp)

foreach(ONE_BENCH_CPP ${ALL_BENCH_CPP})

   get_filename_component(ONE_BENCH_EXEC ${ONE_BENCH_CPP} NAME_WE)

   if (BUILD_LDBC MATCHES OFF AND ONE_BENCH_EXEC STREQUAL "ldbc_bench")
      message(STATUS "skipping ldbc_bench ...")
   else()
   # Avoid name collision 
   set(TARGET_NAME Bench_${ONE_BENCH_EXEC})

   add_executable(${TARGET_NAME} ${ONE_BENCH_CPP})
   set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${ONE_BENCH_EXEC}) 
   include_directories("${benchmark_SOURCE_DIR}/include")

   target_link_libraries(${TARGET_NAME} 
      ${POSEIDON_GTPC}      
      ${POSEIDON_LDBC}
      poseidon_core
      benchmark
      ${PMDK_LIBRARIES}
      Boost::program_options
   )
   endif()
   
   # If you want to run benchmarks with the "make test" command, uncomment me
   # add_test(${TARGET_NAME} ${ONE_BENCH_EXEC})
endforeach()