cmake_minimum_required(VERSION 3.14)
project(poseidon_core)

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
#
# Set this only if this project is built as a standalone project.
#
#-------------------------------------------------------------------------------
# Set the mount path of your pmem device where the tests/benchmarks are stored
if(NOT PMEM_MNT_PATH)
  set(PMEM_MNT_PATH "/mnt/pmem0/poseidon")
endif()

option(USE_PMDK       "Use Intel PMDK"                         ON)
option(USE_TX         "Provide Transaction Support"            ON)
option(USE_LOGGING    "Use undo logging for failure atomicity" ON)
option(BUILD_PYTHON   "Build python interface for poseidon"    OFF)
#-------------------------------------------------------------------------------
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(USE_PMDK OFF)
  set(LLVM_DIR "/usr/local/opt/llvm/lib/cmake/llvm")
endif()

# Don't try to contact github everytime.
set (FETCHCONTENT_FULLY_DISCONNECTED "OFF")

# Set the searching location for cmake 'include' locations
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include the sanitizer module
include(sanitizers)

set(USE_SANITIZER "Memory")
set(CMAKE_MACOSX_RPATH 1)

# Include the code coverage module
# option(CODE_COVERAGE ON)
# include(code-coverage)
# add_code_coverage()

# Require C++17
include(c++-standards)
cxx_17()

# Include the PMDK module
if(USE_PMDK MATCHES ON)
  message(STATUS "looking for PMDK")
  include(pmdk)
endif()

# Transaction support
if(USE_TX MATCHES ON)
  add_definitions( "-DUSE_TX" )
endif()

if(USE_LOGGING MATCHES ON)
  add_definitions( "-DUSE_LOGGING" )
endif()

configure_file(
  ${PROJECT_SOURCE_DIR}/src/config.h.in
  ${PROJECT_BINARY_DIR}/generated/config.h
)



#---------------------------------------------------------------------------

include_directories(${PROJECT_BINARY_DIR}/generated)
include_directories(${PROJECT_SOURCE_DIR}/src/storage)
include_directories(${PROJECT_SOURCE_DIR}/src/qop)
include_directories(${PROJECT_SOURCE_DIR}/src/tx)
include_directories(${PROJECT_SOURCE_DIR}/src/dict)
include_directories(${PROJECT_SOURCE_DIR}/src/ldbc)
include_directories(${PROJECT_SOURCE_DIR}/src/btree)
include_directories(${PROJECT_SOURCE_DIR}/src/pool)
include_directories(${PROJECT_SOURCE_DIR}/src/log)
include_directories(${PROJECT_SOURCE_DIR}/src/qcomp)
include_directories(${PROJECT_SOURCE_DIR}/src/ql)

#---------------------------------------------------------------------------

set(THIRD_PARTY_DIR "${PROJECT_BINARY_DIR}/_deps")

# C++ compiler flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated -O3 -Wsign-compare")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated -O0 -g -Wsign-compare")
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs -Wno-#pragma-messages")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -Wno-unused")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd488 -wd597")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-unused -Wno-uninitialized")

if(CMAKE_BUILD_TYPE MATCHES Release)
  # this disables asserts for release builds
  add_definitions("-DNDEBUG")
endif()

#---------------------------------------------------------------------------

# Catch =================================================================== #
include(FetchContent REQUIRED)
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.9.1
)
# FetchContent_GetProperties(catch)
if(NOT catch_POPULATED)
  message(STATUS "Populating catch")
  FetchContent_Populate(catch)
  set(CATCH_INCLUDE_DIR ${catch_SOURCE_DIR}/single_include/catch2)
  add_library(Catch2::Catch IMPORTED INTERFACE)
  add_library(Catch2 INTERFACE)
  add_library(Catch2::Test ALIAS Catch2)
  target_include_directories(Catch2 INTERFACE ${CATCH_INCLUDE_DIR})
  set_property(TARGET Catch2::Catch PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CATCH_INCLUDE_DIR}")
endif()

# CSVParser =================================================================== #
include(FetchContent REQUIRED)
 FetchContent_Declare(
  CSV
  GIT_REPOSITORY https://github.com/AriaFallah/csv-parser.git
#  GIT_TAG        1.2.1
  )
FetchContent_GetProperties(CSV)

if(NOT CSV_POPULATED)
  message(STATUS "Populating CSV")
  FetchContent_Populate(CSV)
  set(CSV_INCLUDE_DIR ${csv_SOURCE_DIR})
  add_library(CSV IMPORTED INTERFACE)
  set_property(TARGET CSV PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CSV_INCLUDE_DIR}")
  set(CSV_CXX_STANDARD 11)
endif()

# spdlog =================================================================== #

include(FetchContent REQUIRED)
 FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
   GIT_TAG        v1.5.0
  )
FetchContent_GetProperties(spdlog)

if(NOT spdlog_POPULATED)
  message(STATUS "Populating spdlog")
  FetchContent_Populate(spdlog)
  # find_package(spdlog REQUIRED)
  set(SPDLOG_BUILD_TESTS OFF CACHE INTERNAL "")
  add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR} EXCLUDE_FROM_ALL)
  set(SPDLOG_INCLUDE_DIR ${spdlog_SOURCE_DIR}/include)
endif()

# Google Benchmark ============================================================== #

FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.5.0
  GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(benchmark)
if(NOT benchmark_POPULATED)
  message(STATUS "Populating benchmark (google)")
  set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
  set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE INTERNAL "")
  FetchContent_Populate(benchmark)
  add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR} EXCLUDE_FROM_ALL)
  set(BENCHMARK_INCLUDE_DIR ${benchmark_SOURCE_DIR}/include)
endif()

# LLVM ============================================================== #
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(LLVM_LIBRARIES
        AllTargetsAsmParsers
        AllTargetsCodeGens
        AllTargetsDescs
        AllTargetsInfos
        AggressiveInstCombine
        Analysis
        AsmParser
        AsmPrinter
        BinaryFormat
        BitReader
        BitWriter
        BitstreamReader
        CodeGen
        Core
        Demangle
        ExecutionEngine
        GlobalISel
        InstCombine
        Instrumentation
        JITLink
        Linker
        IRReader
        native
        Object
        OrcJIT
        ProfileData
        Remarks
        RuntimeDyld
        ScalarOpts
        SelectionDAG
        Support
        Target
        TransformUtils
        Vectorize
        X86AsmParser
        X86CodeGen
        X86Desc
        X86Info
        X86Utils)

# PEGTL =================================================================== #
include(FetchContent REQUIRED)
 FetchContent_Declare(
  pegtl
  GIT_REPOSITORY https://github.com/taocpp/PEGTL.git
  GIT_TAG        2.8.3
  )
FetchContent_GetProperties(pegtl)
if(NOT pegtl_POPULATED)
  message(STATUS "Populating PEGTL")
  FetchContent_Populate(pegtl)
  set(PEGTL_INCLUDE_DIR ${pegtl_SOURCE_DIR}/include)
  add_library(PEGTL IMPORTED INTERFACE)
  set_property(TARGET PEGTL PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${PEGTL_INCLUDE_DIR}")
endif()

# linenoise =================================================================== #
include(FetchContent REQUIRED)
 FetchContent_Declare(
  linenoise
  GIT_REPOSITORY https://github.com/yhirose/cpp-linenoise.git
 #  GIT_TAG        1.2.1
  )
FetchContent_GetProperties(linenoise)

if(NOT linenoise_POPULATED)
  message(STATUS "Populating linenoise")
  FetchContent_Populate(linenoise)
  set(LINENOISE_INCLUDE_DIR ${linenoise_SOURCE_DIR})
  add_library(LINENOISE IMPORTED INTERFACE)
  set_property(TARGET LINENOISE PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${LINENOISE_INCLUDE_DIR}")
endif()

#---------------------------------------------------------------------------

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
  set(DYLIB_LIBRARY "-ldl")
else()
  set(DYLIB_LIBRARY "")
endif()

###########################
# Python Interface
###########################
#
find_package(Python3 COMPONENTS Interpreter)

#---------------------------------------------------------------------------

if (BUILD_PYTHON)
message(STATUS "PYTHON")
  find_package(PythonLibs)
  find_package(PythonInterp)
  if (PYTHONLIBS_FOUND)
    message(STATUS "Python Version ${PYTHON_VERSION_MAJOR} found - building the Python API.")
    message("PythonInclude ${PYTHON_INCLUDE_DIRS}")
    message("PythonLibs ${PYTHON_LIBDIR}")

    include_directories(${PYTHON_INCLUDE_DIRS})
    link_directories(${PYTHON_LIBDIR})

    set(PYTHON_SOURCES
      src/py/py_poseidon.cpp
    )
  else()
    set (PYTHON_SOURCES "")
  endif(PYTHONLIBS_FOUND)
else ()
        message(STATUS "Don't build python interface")
        set (PYTHON_SOURCES "")
endif()

######################
# Boost C++ library
######################
#
SET(BOOST_MIN_VERSION "1.60.0")

if (BUILD_PYTHON)
  if(Python3_FOUND)
    set(BOOST_PYTHON "python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
  else()
    message(FATAL_ERROR "Fatal error: BUILD_PYTHON was set, but no python3 interpreter could be found")
  endif()
  find_package(Boost ${BOOST_MIN_VERSION} COMPONENTS
    program_options
    date_time
    ${BOOST_PYTHON}
    REQUIRED)
else()
  find_package(Boost ${BOOST_MIN_VERSION} COMPONENTS
    program_options
    date_time
    REQUIRED)
endif()

if (NOT Boost_FOUND)
     message(FATAL_ERROR "Fatal error: Boost (version >= ${BOOST_MIN_VERSION}) required.\n"
 )
endif (NOT Boost_FOUND)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    add_definitions( "-DHAS_BOOST" )
endif()

#-----------------------------------------------------------------------------------------

include_directories("${PEGTL_INCLUDE_DIR}")
include_directories("${CSV_INCLUDE_DIR}")
include_directories("${SPDLOG_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${LINENOISE_INCLUDE_DIR}")

#-----------------------------------------------------------------------------------------

if(USE_PMDK MATCHES ON)
  set(dict_src src/dict/dict_kv.cpp)
else()
  set(dict_src src/dict/dict_bimap.cpp)
endif()

set(src_files 
  ${dict_src} 
  src/storage/nodes.cpp
  src/storage/relationships.cpp
  src/storage/properties.cpp
  src/storage/graph_db.cpp
  src/storage/graph_db_import.cpp
  src/storage/graph_db_index.cpp
  src/storage/graph_db_query.cpp
  src/storage/graph_db_recovery.cpp
  src/storage/graph_db_gc.cpp
  src/btree/index_map.cpp
  src/qop/qop.cpp
  src/qop/update.cpp
  src/qop/join.cpp
  src/qop/query.cpp
  src/qop/bfs.cpp
  src/qop/thread_pool.cpp
  src/tx/transaction.cpp
  src/pool/graph_pool.cpp
  src/log/pmlog.cpp
  src/ldbc/ldbc.cpp
  src/qcomp/codegen_inline.cpp
  src/qcomp/filter_expression.cpp
  src/qcomp/filter_expression_inline.cpp
  src/qcomp/global_definitions.cpp
  src/qcomp/interpreter.cpp
  src/qcomp/jit_cache.cpp
  src/qcomp/JitFromScratch.cpp
  src/qcomp/Optimizer.cpp
  src/qcomp/main.cpp
  src/qcomp/p_context.cpp
  src/qcomp/query_engine.cpp
  src/qcomp/qoperator.cpp
  src/qcomp/joiner.cpp
  src/qcomp/interprete_filter_visitor.cpp
  src/ql/queryc.cpp
  src/ql/ast.cpp
  )

# Using OBJECT to avoid to build object files multiple times due to inclusion
# in different libraries.
# macOS doesn't like SHARED, Linux needs it
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
add_library(poseidon_obj OBJECT ${src_files})
else()
add_library(poseidon_obj OBJECT SHARED ${src_files})
endif()

# Poseidon core library for building C++ applications
add_library(poseidon_core SHARED
  $<TARGET_OBJECTS:poseidon_obj>
)

target_link_libraries(poseidon_core
  ${LLVM_LIBRARIES}
  Boost::date_time
  spdlog::spdlog_header_only
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

if (BUILD_PYTHON)
# Python Poseidon library for loading a module into the python3 interpreter
add_library(poseidon SHARED
  $<TARGET_OBJECTS:poseidon_obj>
  ${PYTHON_SOURCES}
)

target_link_libraries(poseidon
  ${LLVM_LIBRARIES}
  Boost::date_time
  Boost::${BOOST_PYTHON}
  ${PYTHON_LIBRARIES}
  ${PMDK_LIBRARIES}
  spdlog::spdlog_header_only
)

set_target_properties(poseidon PROPERTIES PREFIX "" SUFFIX ".so")
endif()

#-----------------------------------------------------------------------------------------
#
# Some programs for LDBC benchmarking.
#

# The program for loading CSV files into PMem.
#
add_executable(ldbc_loader
  src/ldbc/loader.cpp
 )

target_link_libraries(ldbc_loader
  poseidon_core
  ${PMDK_LIBRARIES}
  ${LLVM_LIBRARIES}
  Boost::program_options
)

# The program for executing short read queries.
#
add_executable(run_ldbc_reads
  src/ldbc/ldbc_reads.cpp
 )

target_link_libraries(run_ldbc_reads
  poseidon_core
  ${PMDK_LIBRARIES}
  ${LLVM_LIBRARIES}
  Boost::program_options
)

# The program for executing interactive update queries.
#
add_executable(run_ldbc_updates
  src/ldbc/ldbc_updates.cpp
 )

target_link_libraries(run_ldbc_updates
  poseidon_core
  ${PMDK_LIBRARIES}
  ${LLVM_LIBRARIES}
  Boost::program_options
)

# The program for executing graphalytics.
#
add_executable(run_ldbc_graphalytics
  src/ldbc/ldbc_graphalytics.cpp
 )

target_link_libraries(run_ldbc_graphalytics
  poseidon_core
  ${PMDK_LIBRARIES}
  ${LLVM_LIBRARIES}
  Boost::program_options
)

# The program for executing jitted short read queries.
#
add_executable(run_ldbc_jit_reads
  src/ldbc/ldbc_jit_reads.cpp
 )

target_link_libraries(run_ldbc_jit_reads
  poseidon_core
  ${PMDK_LIBRARIES}
  ${LLVM_LIBRARIES}
  Boost::program_options
)

# The program for executing the LLVM query compiler
add_executable(qcomp
	  src/qcomp/main.cpp
	   )

target_link_libraries(qcomp
	     poseidon_core
	     ${PMDK_LIBRARIES}
	     ${LLVM_LIBRARIES}
	     Boost::program_options
             )

#-----------------------------------------------------------------------------------------

add_executable(qlc
#  src/ql/ast.cpp
  src/ql/qlc.cpp
 )

target_link_libraries(qlc
  poseidon_core
  ${PMDK_LIBRARIES}
  ${LLVM_LIBRARIES}
  Boost::program_options
)
#-----------------------------------------------------------------------------------------
#
# Unit tests using Catch
#
enable_testing()
add_subdirectory(test)

#-----------------------------------------------------------------------------------------
#
# Micro-benchmarking using Google Benchmark
#
add_subdirectory(bench)

#-----------------------------------------------------------------------------------------

# Create logs directory
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")
