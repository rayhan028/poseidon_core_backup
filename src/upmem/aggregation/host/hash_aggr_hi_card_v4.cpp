#include "aggregation.hpp"


#ifndef HASH_BASED_HIGH_CARDINALITY_V4

void hash_aggregation_hi_card_v4(graph_db_ptr &graph) {
    return;
}

#else

/* partition data such that the hash table for each partition fits in WRAM */
void hash_aggregation_hi_card_v4(graph_db_ptr &graph) {

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

        DPU_ASSERT(dpu_alloc(NR_DPUS, DPU_PROFILE, &dpu_set));
        DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &num_dpus));
        DPU_ASSERT(dpu_get_nr_ranks(dpu_set, &num_ranks));
        DPU_ASSERT(dpu_load(dpu_set, HASH_AGGR_HI_CARD_V4_BIN, NULL));
        assert(num_dpus == NR_DPUS);

        #ifdef SF0_1
        uint32_t total_elems = (151043 /* Comment */ + 135701 /* Post */); /* SF 0.1 */
        #elif defined SF1
        uint32_t total_elems = (2052169 /* Comment */ + 1003605 /* Post */); /* SF 1 */
        #elif defined SF10
        uint32_t total_elems = (21865475 /* Comment */ + 7435696 /* Post */); /* SF 10 */
        #endif

        uint32_t total_elem_size = total_elems * ELEM_SIZE;
        uint32_t elems_per_dpu = DIVCEIL(total_elems, NR_DPUS);
        uint32_t elems_size_per_dpu = ELEM_SIZE * elems_per_dpu;
        uint32_t max_elems_per_dpu = (MRAM_INPUT_BUFFER / ELEM_SIZE);
        uint32_t htable_factor = DIVCEIL(HASH_TABLE_SIZE, ELEM_SIZE);
        uint32_t max_htables_per_dpu = DIVCEIL(GROUP_KEY_CARDINALITY, NR_HASH_TABLE_ENTRIES);
        uint32_t htable_offs = max_elems_per_dpu - (max_htables_per_dpu * htable_factor);
        bool dpu_overflow = elems_per_dpu > htable_offs;

        PRINT_TOP_RULE;
        PRINT_INFO(true, "HASH-BASED AGGREGATION WITH HIGH GROUP KEY CARDINALITY (V4)");
        PRINT("DPUs: %u", NR_DPUS);
        PRINT("Ranks: %u", num_ranks);
        PRINT("Elements: %u", total_elems);
        PRINT("Elements per DPU: %u", elems_per_dpu);
        PRINT("Element Size: %lu", ELEM_SIZE);
        PRINT("Total element size: %u (%f MiB)", total_elem_size, total_elem_size / (double)MiB);
        PRINT("Element Size per DPU: %u (%f MiB)", elems_size_per_dpu, (elems_size_per_dpu / (double)MiB));
        PRINT("Group key cardinality: %u", GROUP_KEY_CARDINALITY);
        PRINT("Offset of hash tables: %u", htable_offs);
        PRINT("Maximum hash tables: %u", max_htables_per_dpu);

        dpu_params params[NR_DPUS];
        mrnode* elems_buffer = nullptr;
        htable_entry*** htable_ptrs = nullptr;
        uint32_t num_hash_tables[NR_DPUS];
        uint32_t rem_num_hash_tables[NR_DPUS];
        uint32_t batch_htables[NR_DPUS];
        uint32_t processed_hash_tables[NR_DPUS] = {0};
        htable_entry* hash_tables[NR_DPUS];
        mrnode* global_part_buffer = (mrnode*) malloc (ELEM_SIZE * total_elems);
        uint32_t** batch_htable_entries = (uint32_t**) malloc(NR_DPUS * sizeof(uint32_t*));
        std::unordered_map<uint32_t, aggr_res> seq_cpu_res, par_cpu_res, dpu_res;

        if (dpu_overflow) {
            /* TODO */
        }
        else {

            /* initialize elements */
            elems_buffer = (mrnode*) malloc (ELEM_SIZE * elems_per_dpu * NR_DPUS);
            initialize_props(graph, elems_buffer);

            /* transfer data to DPU */
            PRINT("Transfer input data to DPUs...");
            t.start("CPU to DPU xfer (input data)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &elems_buffer[dpuid * elems_per_dpu]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, ELEM_SIZE * elems_per_dpu, DPU_XFER_DEFAULT));
            t.stop();

            /* transfer input parameters to DPU */
            PRINT("Transfer input parameters to DPUs...");
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

            // /* dump DPU logs */
            // PRINT("Dump DPU Logs...");
            // DPU_FOREACH(dpu_set, dpu, dpuid) {
            //     // std::cout << "DPU " << d << "\n";
            //     DPU_ASSERT(dpu_log_read(dpu, stdout));
            // }

            /* retrieve number of hash tables from DPUs */
            PRINT("Transfer number of hash tables to CPU...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &num_hash_tables[dpuid]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_num_hash_tables", 0, sizeof(uint32_t), DPU_XFER_DEFAULT));

#if 1
            uint32_t max_num_hash_tables = 0;
            for (uint32_t d = 0; d < NR_DPUS; d++) {
                uint32_t htables = num_hash_tables[d];
                rem_num_hash_tables[d] = htables;
                if (htables > max_num_hash_tables) {
                    max_num_hash_tables = htables;
                }

                /* allocate CPU buffers for hash tables */
                hash_tables[d] = (htable_entry*) malloc(HASH_TABLE_SIZE * htables);
            }

            uint32_t batches = 1;
            uint32_t htable_threshold = NR_DPUS; /* transfer a maximum of NR_DPUS hash tables at a time */
            if (max_num_hash_tables > NR_DPUS) {
                batches = DIVCEIL(max_num_hash_tables, htable_threshold);
            }

            PRINT("Transfer hash tables to CPU...");
            PRINT("Hash table batches: %u", batches);
            t.start("DPU to CPU transfer (hash tables)");
            uint32_t total_max_batch_htables = 0;
            for (uint32_t batch = 0; batch < batches; batch++) {
                PRINT("Transfer batch %u...", batch);

                uint32_t max_batch_htables = 0;
                for (uint32_t d = 0; d < NR_DPUS; d++) {
                    if (rem_num_hash_tables[d] > htable_threshold) {
                        batch_htables[d] = htable_threshold;
                    }
                    else {
                        batch_htables[d] = rem_num_hash_tables[d];
                    }
                    if (batch_htables[d] > max_batch_htables) {
                        max_batch_htables = batch_htables[d];
                    }
                    rem_num_hash_tables[d] -= batch_htables[d];
                }

                for (uint32_t d = 0; d < NR_DPUS; d++) {
                    uint32_t htables = batch_htables[d];
                    batch_htable_entries[d] = (uint32_t*) malloc(htables * sizeof(uint32_t));
                    for (uint32_t h = 0; h < htables; h++) {
                        batch_htable_entries[d][h] = NR_HASH_TABLE_ENTRIES;
                    }
                }

                htable_ptrs = (htable_entry***) multidim_malloc(NR_DPUS, batch_htables, sizeof(htable_entry*));
                for (uint32_t d = 0; d < NR_DPUS; d++) {
                    for (uint32_t h = 0; h < batch_htables[d]; h++) {
                        uint32_t offs = (processed_hash_tables[d] + h) * NR_HASH_TABLE_ENTRIES;
                        htable_ptrs[d][h] = &hash_tables[d][offs];
                    }

                    processed_hash_tables[d] += batch_htables[d];
                }

                /* transfer hash tables from DPUs into buffers on CPU */
                sg_hash_table_xfer_ctx get_hash_table_params = {.num_hash_tables = batch_htables, .hash_table_sizes = batch_htable_entries, .hash_table_ptrs = htable_ptrs};
                get_block_t get_hash_table = {.f = &get_hash_table_func, .args = &get_hash_table_params, .args_size = sizeof(sg_hash_table_xfer_ctx)};
                /* TODO: skip data transpose for byte interleaving */
                DPU_ASSERT(dpu_push_sg_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, htable_offs * ELEM_SIZE + total_max_batch_htables * HASH_TABLE_SIZE,
                                            max_batch_htables * HASH_TABLE_SIZE, &get_hash_table, DPU_SG_XFER_DISABLE_LENGTH_CHECK));

                total_max_batch_htables += max_batch_htables;

                for (uint32_t d = 0; d < NR_DPUS; d++) {
                    if (batch_htable_entries[d]) {
                        free(batch_htable_entries[d]);
                    }
                }
                for (uint32_t d = 0; d < NR_DPUS; d++) {
                    if (htable_ptrs[d]) {
                        free(htable_ptrs[d]);
                    }
                }
                free(htable_ptrs);
            }
            t.stop();

#else /* #if 1 */

            /* retrieve hash tables from DPUs */
            PRINT("Transfer hash tables to CPU...");
            t.start("DPU to CPU transfer (hash tables)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE * num_hash_tables[dpuid]);
                DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, ELEM_SIZE * htable_offs, hash_tables[dpuid], HASH_TABLE_SIZE * num_hash_tables[dpuid]));
            }
            t.stop();

#endif /* #if 1 */

            PRINT("Collect DPU aggregation results...");
            t.start("CPU exec (DPU aggregation results)");
            for (uint32_t d = 0; d < NR_DPUS; d++) {
                for (uint32_t idx = 0; idx < NR_HASH_TABLE_ENTRIES * num_hash_tables[d]; idx++) {
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
        assert(std::thread::hardware_concurrency() >= NR_CPU_THREADS);
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
        // print_aggr(seq_cpu_res);
        print_aggr(dpu_res);

        /* free DPUs */
        PRINT("Free DPUs...");
        DPU_ASSERT(dpu_free(dpu_set));

        /* free CPU buffers */
        PRINT("Free CPU buffers...");
        free(elems_buffer);
        free(global_part_buffer);
        free(batch_htable_entries);
        for (uint32_t d = 0; d < NR_DPUS; d++) {
            free(hash_tables[d]);
        }

        PRINT_TOP_RULE;
        t.print();
        auto mark = std::to_string(NR_DPUS) + "DPUS_" +
                    std::to_string(GROUP_KEY_CARDINALITY) + "GRP_KEYS_" +
                    "SF10";
        t.print_to_csv(CSV_FILE, mark);
    }
    catch (const dpu::DpuError &e) {
        std::cerr << e.what() << std::endl;
    }
}

#endif /* #ifndef HASH_BASED_HIGH_CARDINALITY_V4 */
