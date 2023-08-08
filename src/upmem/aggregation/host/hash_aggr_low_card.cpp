#include "aggregation.hpp"


#ifndef HASH_BASED_LOW_CARDINALITY

void hash_aggregation_low_card(graph_db_ptr &graph) {
    return;
}

#else

/* hash table for entire data fits in WRAM */
void hash_aggregation_low_card(graph_db_ptr &graph) {

#ifdef PRINTER
    {
        node n;
        auto &nodes = graph->get_nodes();
        PRINT_TOP_RULE;
        std::cout << "chunk_size for nodes: " << nodes->as_vec().real_chunk_size() << "\n"
                  << "elements per chunk:   " << nodes->as_vec().elements_per_chunk() << "\n"
                  << "number of chunks:     " << nodes->as_vec().num_chunks() << "\n"
                  << "offset of id_:        " << n._offset() << std::endl;
        std::cout << "number of nodes:      " << nodes->as_vec().last_used() << "\n";
        std::cout << "number of rships:     " << graph->get_relationships()->as_vec().last_used() << "\n";
        std::cout << "size of node:         " << sizeof(node) << std::endl;
        std::cout << "size of mr_node:      " << sizeof(mr_node) << std::endl;
        std::cout << "size of mrchunk:      " << sizeof(mrchunk) << std::endl;
        std::cout << "align of mr_node:      " << alignof(mr_node) << std::endl;
        std::cout << "align of mrchunk:      " << alignof(mrchunk) << std::endl;
    }
#endif

    try {

        timer t;

        /* allocate DPUs and load binary */
        struct dpu_set_t dpu_set, dpu;
        uint32_t num_ranks, num_dpus;
        uint32_t dpuid = 0;

        std::cout << "start\n";

        DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &dpu_set));
        DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &num_dpus));
        DPU_ASSERT(dpu_get_nr_ranks(dpu_set, &num_ranks));
        DPU_ASSERT(dpu_load(dpu_set, HASH_AGGR_LOW_CARD_BIN, NULL));
        assert(num_dpus == NR_DPUS);

        #ifdef SF0_1
        uint32_t total_elems = (151043 /* Comment */ + 135701 /* Post */); /* SF 0.1 */
        #elif defined SF1
        uint32_t total_elems = (2052169 /* Comment */ + 1003605 /* Post */); /* SF 1 */
        #elif defined SF10
        uint32_t total_elems = (21865475 /* Comment */ + 7435696 /* Post */); /* SF 10 */
        #endif
        uint32_t elems_per_dpu = DIVCEIL(total_elems, NR_DPUS);
        uint32_t elems_size_per_dpu = ELEM_SIZE * elems_per_dpu;
        uint32_t htable_offs = elems_per_dpu; /* TODO: tune */
        bool dpu_overflow = elems_size_per_dpu > MRAM_INPUT_BUFFER_SIZE;

        PRINT_TOP_RULE;
        #ifdef PER_TASKLET_HASH_TABLE
        PRINT_INFO(true, "HASH-BASED AGGREGATION WITH LOW GROUP KEY CARDINALITY (Per-tasklet Hash Table)");
        #elif defined SINGLE_HASH_TABLE
        PRINT_INFO(true, "HASH-BASED AGGREGATION WITH LOW GROUP KEY CARDINALITY (Single Hash Table)");
        #endif
        PRINT("DPUs: %u", NR_DPUS);
        PRINT("Ranks: %u", num_ranks);
        PRINT("Elements: %u", total_elems);
        PRINT("Elements per DPU: %u", elems_per_dpu);
        PRINT("Element Size: %lu", ELEM_SIZE);
        PRINT("Element Size per DPU: %u (%f MB)", elems_size_per_dpu, (elems_size_per_dpu / (double)MB));

        mrnode* elems_buffer = nullptr;
        htable_entry* hash_tables[NR_DPUS];
        std::unordered_map<uint32_t, aggr_res> seq_cpu_res, par_cpu_res, dpu_res;

        if (dpu_overflow) {
            PRINT_INFO(true, "Max. %d (%f MB) MRAM input buffer exceeded. Adjusting...", MRAM_INPUT_BUFFER_SIZE, (MRAM_INPUT_BUFFER_SIZE / (double)MB));
            elems_per_dpu = (MRAM_INPUT_BUFFER_SIZE / 2) / ELEM_SIZE;
            elems_size_per_dpu = ELEM_SIZE * elems_per_dpu;
            uint32_t dpu_exec_rounds = DIVCEIL(total_elems, (elems_per_dpu * NR_DPUS));

            PRINT("DPU execution rounds: %u", dpu_exec_rounds);
            PRINT("Elements per DPU: %u", elems_per_dpu);
            PRINT("Element Size per DPU: %u (%f MB)", elems_size_per_dpu, (elems_size_per_dpu / (double)MB));

            /* initialize elements */
            elems_buffer = (mrnode*) malloc (ELEM_SIZE * elems_per_dpu * NR_DPUS * dpu_exec_rounds);
            initialize_props(graph, elems_buffer);

            for (uint32_t round = 0; round < dpu_exec_rounds; round++) {
                PRINT("Execution Round: %u", (round + 1));

                if (round == (dpu_exec_rounds - 1)) {
                    /* make a balanced distribution of elements across DPUs */

                    uint32_t remaining_elems = total_elems - (round * NR_DPUS * elems_per_dpu);
                    uint32_t last_round_elems_per_dpu = DIVCEIL(remaining_elems, NR_DPUS);

                    PRINT("Remaining elements in last DPU execution round: %u", remaining_elems);
                    PRINT("Elements per DPU in last DPU execution round: %u", last_round_elems_per_dpu);

                    /* transfer data to DPU */
                    PRINT("Transfer input data to DPUs...");
                    t.start("CPU to DPU xfer (input data)");
                    DPU_FOREACH(dpu_set, dpu, dpuid) {
                        DPU_ASSERT(dpu_prepare_xfer(dpu, &elems_buffer[round * NR_DPUS * elems_per_dpu + dpuid * last_round_elems_per_dpu]));
                    }
                    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, ELEM_SIZE * elems_per_dpu, DPU_XFER_DEFAULT));
                    t.stop();

                    /* transfer DPU parameters to DPU */
                    PRINT("Transfer input parameters to DPUs...");
                    dpu_params params[NR_DPUS];
                    DPU_FOREACH(dpu_set, dpu, dpuid) {
                        uint32_t elems = (dpuid == (NR_DPUS - 1)) ?
                                         (total_elems - (round * NR_DPUS * elems_per_dpu + dpuid * last_round_elems_per_dpu)) :
                                         last_round_elems_per_dpu;
                        params[dpuid].num_elems = elems;
                        params[dpuid].htable_offset = htable_offs;
                        DPU_ASSERT(dpu_prepare_xfer(dpu, &params[dpuid]));
                    }
                    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "dpu_parameters", 0, sizeof(dpu_params), DPU_XFER_DEFAULT));

                }
                else {

                    /* transfer data to DPU */
                    PRINT("Transfer input data to DPUs...");
                    t.start("CPU to DPU xfer (input data)");
                    DPU_FOREACH(dpu_set, dpu, dpuid) {
                        DPU_ASSERT(dpu_prepare_xfer(dpu, &elems_buffer[(round * NR_DPUS + dpuid) * elems_per_dpu]));
                    }
                    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, ELEM_SIZE * elems_per_dpu, DPU_XFER_DEFAULT));
                    t.stop();

                    /* transfer DPU parameters to DPU */
                    PRINT("Transfer input parameters to DPUs...");
                    dpu_params param{elems_per_dpu, htable_offs};
                    DPU_ASSERT(dpu_copy_to(dpu_set, "dpu_parameters", 0, (const void*)&param, sizeof(dpu_params)));

                }

                /* launch the aggregation kernel */
                PRINT("Executing aggregation kernel on DPUs...");
                t.start("DPU exec (aggregation kernel)");
                DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
                t.stop();

                // /* dump DPU logs */
                // PRINT("Dump DPU Logs...");
                // DPU_FOREACH(dpu_set, dpu, dpuid) {
                //     // std::cout << "DPU " << d << "\n";
                //     DPU_ASSERT(dpu_log_read(dpu, stdout));
                // }

                /* retrieve hash tables from DPUs */
                PRINT("Transfer hash tables to CPU...");
                t.start("DPU to CPU transfer (hash tables)");
                DPU_FOREACH(dpu_set, dpu, dpuid) {
#ifdef PER_TASKLET_HASH_TABLE /* a separate hash table for each tasklet */
                    hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE * NR_TASKLETS);
#elif defined SINGLE_HASH_TABLE /* a single hash table shared by all tasklets */
                    hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE);
#endif
                    DPU_ASSERT(dpu_prepare_xfer(dpu, hash_tables[dpuid]));
                }
#ifdef PER_TASKLET_HASH_TABLE
                DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, ELEM_SIZE * htable_offs, HASH_TABLE_SIZE * NR_TASKLETS, DPU_XFER_DEFAULT));
#elif defined SINGLE_HASH_TABLE
                DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, ELEM_SIZE * htable_offs, HASH_TABLE_SIZE, DPU_XFER_DEFAULT));
#endif
                t.stop();

                PRINT("Collect DPU aggregation results...");
                t.start("CPU exec (DPU aggregation results)");
                for (uint32_t d = 0; d < NR_DPUS; d++) {
#ifdef PER_TASKLET_HASH_TABLE
                    for (uint32_t idx = 0; idx < NR_HASH_TABLE_ENTRIES * NR_TASKLETS; idx++) {
#elif defined SINGLE_HASH_TABLE
                    for (uint32_t idx = 0; idx < NR_HASH_TABLE_ENTRIES; idx++) {
#endif
                        if (hash_tables[d][idx].key != (uint32_t)(-1)) {
                            uint32_t key = hash_tables[d][idx].key;
                            auto iter = dpu_res.find(key);
                            if (iter != dpu_res.end()) {
                                auto &res = iter->second;

                                #if defined(COUNT) || defined(AVERAGE)
                                res.cnt += hash_tables[d][idx].val.cnt;
                                #endif

                                #if defined(SUM) || defined(AVERAGE)
                                res.sum += hash_tables[d][idx].val.sum;
                                #endif

                                #ifdef AVERAGE
                                if (res.cnt > 0) {
                                    res.avg = res.sum / (double)res.cnt;
                                }
                                #endif

                                #ifdef MINIMUM
                                if (hash_tables[d][idx].val.min < res.min) {
                                    res.min = hash_tables[d][idx].val.min;
                                }
                                #endif

                                #ifdef MAXIMUM
                                if (hash_tables[d][idx].val.max > res.max) {
                                    res.max = hash_tables[d][idx].val.max;
                                }
                                #endif
                            }
                            else {
                                aggr_res res;
                                #if defined(COUNT) || defined(AVERAGE)
                                res.cnt = hash_tables[d][idx].val.cnt;
                                #endif

                                #if defined(SUM) || defined(AVERAGE)
                                res.sum = hash_tables[d][idx].val.sum;
                                #endif

                                #ifdef AVERAGE
                                res.avg = 0.0;
                                if (res.cnt > 0) {
                                    res.avg = res.sum / (double)res.cnt;
                                }
                                #endif

                                #ifdef MINIMUM
                                res.min = hash_tables[d][idx].val.min;
                                #endif

                                #ifdef MAXIMUM
                                res.max = hash_tables[d][idx].val.max;
                                #endif

                                dpu_res.emplace(std::piecewise_construct,
                                            std::forward_as_tuple(key),
                                            std::forward_as_tuple(res));
                            }
                        }
                    }
                }
                t.stop();

                /* reset CPU buffers */
                PRINT("Reset CPU buffers...");
                for (uint32_t d = 0; d < NR_DPUS; d++) {
#ifdef PER_TASKLET_HASH_TABLE
                    memset(hash_tables[d], 0, HASH_TABLE_SIZE * NR_TASKLETS);
#elif defined SINGLE_HASH_TABLE
                    memset(hash_tables[d], 0, HASH_TABLE_SIZE);
#endif
                }
            }
        }
        else {

            /* initialize elements */
            elems_buffer = (mrnode*) malloc (elems_size_per_dpu * NR_DPUS);
            initialize_props(graph, elems_buffer);

            /* transfer data to DPU */
            PRINT("Transfer input data to DPUs...");
            t.start("CPU to DPU xfer (input data)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &elems_buffer[dpuid * elems_per_dpu]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, elems_size_per_dpu, DPU_XFER_DEFAULT));
            t.stop();

            /* transfer parameters to DPU */
            PRINT("Transfer input parameters to DPUs...");
            dpu_params params[NR_DPUS];
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t elems = (dpuid == (NR_DPUS - 1)) ?
                                 (total_elems - dpuid * elems_per_dpu) :
                                 elems_per_dpu;
                params[dpuid].num_elems = elems;
                params[dpuid].htable_offset = htable_offs;
                DPU_ASSERT(dpu_prepare_xfer(dpu, &params[dpuid]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "dpu_parameters", 0, sizeof(dpu_params), DPU_XFER_DEFAULT));

            /* launch the aggregation kernel */
            PRINT("Executing aggregation kernel on DPUs...");
            t.start("DPU exec (aggregation kernel)");
            DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
            t.stop();

            /* dump DPU logs */
            PRINT("Dump DPU Logs...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                // std::cout << "DPU " << d << "\n";
                DPU_ASSERT(dpu_log_read(dpu, stdout));
            }

            /* retrieve hash tables from DPUs */
            PRINT("Transfer hash tables to CPU...");
            t.start("DPU to CPU transfer (hash tables)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
#ifdef PER_TASKLET_HASH_TABLE /* a separate hash table for each tasklet */
                hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE * NR_TASKLETS);
#elif defined SINGLE_HASH_TABLE /* a single hash table shared by all tasklets */
                hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE);
#endif
                DPU_ASSERT(dpu_prepare_xfer(dpu, hash_tables[dpuid]));
            }
#ifdef PER_TASKLET_HASH_TABLE
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, ELEM_SIZE * htable_offs, HASH_TABLE_SIZE * NR_TASKLETS, DPU_XFER_DEFAULT));
#elif defined SINGLE_HASH_TABLE
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, ELEM_SIZE * htable_offs, HASH_TABLE_SIZE, DPU_XFER_DEFAULT));
#endif
            t.stop();

            PRINT("Collect DPU aggregation results...");
            t.start("CPU exec (DPU aggregation results)");
            for (uint32_t d = 0; d < NR_DPUS; d++) {
#ifdef PER_TASKLET_HASH_TABLE
                for (uint32_t idx = 0; idx < NR_HASH_TABLE_ENTRIES * NR_TASKLETS; idx++) {
#elif defined SINGLE_HASH_TABLE
                for (uint32_t idx = 0; idx < NR_HASH_TABLE_ENTRIES; idx++) {
#endif
                    if (hash_tables[d][idx].key != (uint32_t)(-1)) {
                        uint32_t key = hash_tables[d][idx].key;
                        auto iter = dpu_res.find(key);
                        if (iter != dpu_res.end()) {
                            auto &res = iter->second;

                            #if defined(COUNT) || defined(AVERAGE)
                            res.cnt += hash_tables[d][idx].val.cnt;
                            #endif

                            #if defined(SUM) || defined(AVERAGE)
                            res.sum += hash_tables[d][idx].val.sum;
                            #endif

                            #ifdef AVERAGE
                            if (res.cnt > 0) {
                                res.avg = res.sum / (double)res.cnt;
                            }
                            #endif

                            #ifdef MINIMUM
                            if (hash_tables[d][idx].val.min < res.min) {
                                res.min = hash_tables[d][idx].val.min;
                            }
                            #endif

                            #ifdef MAXIMUM
                            if (hash_tables[d][idx].val.max > res.max) {
                                res.max = hash_tables[d][idx].val.max;
                            }
                            #endif
                        }
                        else {
                            aggr_res res;
                            #if defined(COUNT) || defined(AVERAGE)
                            res.cnt = hash_tables[d][idx].val.cnt;
                            #endif

                            #if defined(SUM) || defined(AVERAGE)
                            res.sum = hash_tables[d][idx].val.sum;
                            #endif

                            #ifdef AVERAGE
                            res.avg = 0.0;
                            if (res.cnt > 0) {
                                res.avg = res.sum / (double)res.cnt;
                            }
                            #endif

                            #ifdef MINIMUM
                            res.min = hash_tables[d][idx].val.min;
                            #endif

                            #ifdef MAXIMUM
                            res.max = hash_tables[d][idx].val.max;
                            #endif

                            dpu_res.emplace(std::piecewise_construct,
                                        std::forward_as_tuple(key),
                                        std::forward_as_tuple(res));
                        }
                    }
                }
            }
            t.stop();
        }

        /* execute baseline aggregation on CPU */
        PRINT("Execute aggregation baselines on CPU...");
        t.start("Parallel CPU exec (aggregation kernel)");
        std::size_t max_threads = std::thread::hardware_concurrency();
        assert(max_threads >= NR_CPU_THREADS);
        std::unordered_map<uint32_t, aggr_res> htables[NR_CPU_THREADS];
        parallel_cpu_aggr(elems_buffer, total_elems, htables);
        t.stop();

        PRINT("Collect CPU aggregation results...");
        t.start("CPU exec (CPU aggregation results)");
        for (uint32_t t = 0; t < NR_CPU_THREADS; t++) {
            for (auto &[key, rhs_res] : htables[t]) {
                auto iter = par_cpu_res.find(key);
                if (iter != par_cpu_res.end()) {
                    auto &lhs_res = iter->second;

                    #if defined(COUNT) || defined(AVERAGE)
                    lhs_res.cnt += rhs_res.cnt;
                    #endif

                    #if defined(SUM) || defined(AVERAGE)
                    lhs_res.sum += rhs_res.sum;
                    #endif

                    #ifdef AVERAGE
                    if (lhs_res.cnt > 0) {
                        lhs_res.avg = lhs_res.sum / (double)lhs_res.cnt;
                    }
                    #endif

                    #ifdef MINIMUM
                    if (rhs_res.min < lhs_res.min) {
                        lhs_res.min = rhs_res.min;
                    }
                    #endif

                    #ifdef MAXIMUM
                    if (rhs_res.max > lhs_res.max) {
                        lhs_res.max = rhs_res.max;
                    }
                    #endif
                }
                else {
                    aggr_res lhs_res;
                    #if defined(COUNT) || defined(AVERAGE)
                    lhs_res.cnt = rhs_res.cnt;
                    #endif

                    #if defined(SUM) || defined(AVERAGE)
                    lhs_res.sum = rhs_res.sum;
                    #endif

                    #ifdef AVERAGE
                    lhs_res.avg = 0.0;
                    if (lhs_res.cnt > 0) {
                        lhs_res.avg = lhs_res.sum / (double)lhs_res.cnt;
                    }
                    #endif

                    #ifdef MINIMUM
                    lhs_res.min = rhs_res.min;
                    #endif

                    #ifdef MAXIMUM
                    lhs_res.max = rhs_res.max;
                    #endif

                    par_cpu_res.emplace(std::piecewise_construct,
                                std::forward_as_tuple(key),
                                std::forward_as_tuple(lhs_res));
                }
            }
        }
        t.stop();

        /* validate CPU and DPU results */
        PRINT("Validate CPU baseline results...");
        t.start("Sequential CPU exec (aggregation kernel)");
        sequential_cpu_aggr(elems_buffer, total_elems, seq_cpu_res);
        t.stop();
        validate_aggr(seq_cpu_res, par_cpu_res);

        PRINT("Validate DPU results...");
        validate_aggr(seq_cpu_res, dpu_res);

        /* free DPUs */
        PRINT("Free DPUs...");
        DPU_ASSERT(dpu_free(dpu_set));

        /* free input element buffer */
        PRINT("Free input element buffer...");
        free(elems_buffer);
        for (uint32_t d = 0; d < NR_DPUS; d++) {
            free(hash_tables[d]);
        }

        PRINT_TOP_RULE;
        t.print();
        auto mark = std::to_string(NR_DPUS) + "DPUS_SF10";
        t.print_to_csv(CSV_FILE, mark);
    }
    catch (const dpu::DpuError &e) {
        std::cerr << e.what() << std::endl;
    }
}

#endif /* #ifndef LOW_CARDINALITY */
