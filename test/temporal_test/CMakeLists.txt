cmake_minimum_required(VERSION 3.16)
project(temporal_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-pthread)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Executable
add_executable(temporal_test
        src/main.cpp

        # LOCAL copies for storage layer 
        poseidon_src/storage/nodes.cpp
        poseidon_src/storage/relationships.cpp
        poseidon_src/storage/graph_db.cpp
        poseidon_src/storage/graph_db_gc.cpp
        poseidon_src/storage/graph_db_recovery.cpp
        poseidon_src/storage/graph_db_index.cpp

        # --- History subsystem ---
        poseidon_src/storage/history_manager.cpp
        poseidon_src/btree/temporal_index.cpp

        # --- Temporal Query Operators ---
        poseidon_src/query/temporal_query/qop_as_of_scan.cpp
        poseidon_src/query/temporal_query/qop_timeline_scan.cpp
        poseidon_src/query/temporal_query/qop_temporal_expand.cpp
        poseidon_src/query/temporal_query/qop_time_slice_expand.cpp
        poseidon_src/query/temporal_query/qop_temporal_project.cpp
        poseidon_src/query/temporal_query/qop_temporal_filter.cpp

        # --- Query Engine Support Files (Local Sandbox) ---
        poseidon_src/query/plan_op/qop.cpp
        poseidon_src/query/plan_op/query_batch.cpp
        poseidon_src/query/utils/query_printer.cpp
        poseidon_src/query/interp/query_ctx.cpp
        poseidon_src/query/utils/qresult.cpp
        poseidon_src/query/utils/qresult_iterator.cpp
        poseidon_src/query/interp/expr_interpreter.cpp
        poseidon_src/query/expr/expression.cpp
        poseidon_src/query/expr/binary_expression.cpp

        # --- Support files from Poseidon Core ---
        ../../src/storage/properties.cpp
        ../../src/dict/dict.cpp
        ../../src/dict/code_table.cpp
        ../../src/dict/string_pool.cpp
        ../../src/dict/paged_string_pool.cpp
        ../../src/log/walog.cpp
        ../../src/tx/transaction.cpp
        #../../src/btree/index_map.cpp
        poseidon_src/btree/index_map.cpp
        ../../src/bp_file/bufferpool.cpp
        ../../src/bp_file/lru_list.cpp
        ../../src/bp_file/paged_file.cpp
        ../../src/query/utils/thread_pool.cpp
        ../../src/query/plan_op/query_pipeline.cpp
)

# Include directories
target_include_directories(temporal_test PRIVATE
        BEFORE
        poseidon_src
        poseidon_src/storage
        poseidon_src/vec
        poseidon_src/tx
        poseidon_src/query
        poseidon_src/query/temporal_query
        poseidon_src/query/plan_op
        poseidon_src/query/interp
        poseidon_src/query/utils
        poseidon_src/query/expr        # Path for expression headers
        poseidon_src/btree

        include
        ../../src/vec
        ../../src/dict
        ../../src/btree
        ../../src/bp_file
        ../../src/tx
        ../../build/_deps/csv-src
        ../../src/log
        ../../src/query/expr
        ../../src/analytics
        /home/rayhan/poseidon_core/external/spdlog-1.x/spdlog-1.x/include
        /home/rayhan/poseidon_core/external/fmt/fmt-master/include
)

# --- Backend selection ---
# Force in-memory mode 
target_compile_definitions(temporal_test PRIVATE USE_IN_MEMORY)
#target_compile_definitions(temporal_test PRIVATE USE_PFILES)

find_package(Threads REQUIRED)
#find_package(spdlog QUIET)
#find_package(fmt QUIET)
find_package(Boost REQUIRED COMPONENTS date_time system)
target_include_directories(temporal_test PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(temporal_test PRIVATE Boost::boost Boost::date_time)
add_subdirectory(/home/rayhan/poseidon_core/external/spdlog-1.x/spdlog-1.x spdlog_build EXCLUDE_FROM_ALL)
add_subdirectory(/home/rayhan/poseidon_core/external/fmt/fmt-master fmt_build EXCLUDE_FROM_ALL)

target_compile_definitions(temporal_test PRIVATE SPDLOG_FMT_EXTERNAL)

# Link against Threads and the HEADER-ONLY versions of the libraries
target_link_libraries(temporal_test PRIVATE
        Threads::Threads
        ${CMAKE_THREAD_LIBS_INIT}
        spdlog::spdlog_header_only
        fmt::fmt-header-only
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(temporal_test PRIVATE stdc++fs)
endif()