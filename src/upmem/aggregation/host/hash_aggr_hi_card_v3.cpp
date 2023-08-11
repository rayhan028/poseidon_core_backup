#include "aggregation.hpp"


#ifndef HASH_BASED_HIGH_CARDINALITY_V3

void hash_aggregation_hi_card_v3(graph_db_ptr &graph) {
    return;
}

#else

/* partition data such that the hash table for each partition fits in WRAM */
void hash_aggregation_hi_card_v3(graph_db_ptr &graph) {

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
        DPU_ASSERT(dpu_load(dpu_set, HASH_AGGR_HI_CARD_V3_BIN, NULL));
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
        uint32_t max_elems_per_dpu_part = (MRAM_INPUT_BUFFER_PARTITION / ELEM_SIZE); /* TODO: reserve space for local partitions */
        uint32_t max_elems_per_dpu_aggr = (MRAM_INPUT_BUFFER_AGGREGATION / ELEM_SIZE); /* TODO: reserve space for hash tables */
        bool dpu_overflow = elems_per_dpu > max_elems_per_dpu_part;

        PRINT_TOP_RULE;
        PRINT_INFO(true, "HASH-BASED AGGREGATION WITH HIGH GROUP KEY CARDINALITY (V3)");
        PRINT("DPUs: %u", NR_DPUS);
        PRINT("Ranks: %u", num_ranks);
        PRINT("Elements: %u", total_elems);
        PRINT("Elements per DPU: %u", elems_per_dpu);
        PRINT("Element Size: %lu", ELEM_SIZE);
        PRINT("Total element size: %u (%f MiB)", total_elem_size, total_elem_size / (double)MiB);
        PRINT("Element Size per DPU: %u (%f MiB)", elems_size_per_dpu, (elems_size_per_dpu / (double)MiB));
        PRINT("Group key cardinality: %u", GROUP_KEY_CARDINALITY);
        PRINT("Partitions: %u", NR_PARTITIONS);

        dpu_params params[NR_DPUS];
        mrnode* elems_buffer = nullptr;
        uint32_t num_partitions[NR_DPUS];
        mrnode*** local_partition_ptrs = nullptr;
        mrnode*** global_partition_ptrs = nullptr;
        uint32_t* partition_sizes[NR_DPUS];
        htable_entry* hash_tables[NR_DPUS];
        mrnode* global_part_buffer = (mrnode*) malloc (ELEM_SIZE * total_elems);
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

            /* transfer partition parameters to DPU */
            PRINT("Transfer partition parameters to DPUs...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t elems = (dpuid == (NR_DPUS - 1)) ?
                                 (total_elems - dpuid * elems_per_dpu) :
                                 elems_per_dpu;
                params[dpuid].num_elems = elems;
                params[dpuid].max_num_elems = max_elems_per_dpu_part;
                params[dpuid].phase = partition_phase;
                DPU_ASSERT(dpu_prepare_xfer(dpu, &params[dpuid]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "dpu_parameters", 0, sizeof(dpu_params), DPU_XFER_DEFAULT));

            /* launch the partition kernel */
            PRINT("Executing partition kernel on DPUs...");
            t.start("DPU exec (partition kernel)");
            DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
            t.stop();

            // /* dump DPU logs */
            // PRINT("Dump DPU Logs...");
            // DPU_FOREACH(dpu_set, dpu, dpuid) {
            //     // std::cout << "DPU " << d << "\n";
            //     DPU_ASSERT(dpu_log_read(dpu, stdout));
            // }

            /* retrieve local histograms from DPUs */
            PRINT("Transfer histograms of local partitions to CPU...");
            uint32_t local_part_sizes[NR_DPUS][NR_PARTITIONS];
            t.start("DPU to CPU transfer (local histograms)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &local_part_sizes[dpuid]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_partition_sizes", 0, sizeof(uint32_t) * NR_PARTITIONS, DPU_XFER_DEFAULT));
            t.stop();

#if 1

            /* compute prefix sum for CPU partition buffers */
            uint32_t prefix = 0;
            uint32_t global_prefix_sum[NR_PARTITIONS][NR_DPUS];
            for (auto p = 0; p < NR_PARTITIONS; p++) {
                for (auto d = 0; d < NR_DPUS; d++) {
                    global_prefix_sum[p][d] = prefix;
                    prefix += local_part_sizes[d][p];
                }
            }
            uint32_t tmp_array[NR_DPUS];
            for (uint32_t d = 0; d < NR_DPUS; d++) {
                tmp_array[d] = NR_PARTITIONS;
            }
            uint32_t* local_part_sizes_ptrs[NR_DPUS];
            for (uint32_t d = 0; d < NR_DPUS; d++) {
                local_part_sizes_ptrs[d] = local_part_sizes[d];
            }
            uint32_t** tmp_ptr = local_part_sizes_ptrs;
            local_partition_ptrs = (mrnode***) multidim_malloc(NR_DPUS, tmp_array, sizeof(mrnode*));
            for (uint32_t d = 0; d < NR_DPUS; d++) {
                for (uint32_t p = 0; p < NR_PARTITIONS; p++) {
                    local_partition_ptrs[d][p] = &global_part_buffer[global_prefix_sum[p][d]];
                }
            }

            /* transfer local partitions from DPUs into global partition buffers on CPU */
            PRINT("Transfer DPU local partitions to CPU global partition buffers...");
            t.start("DPU to CPU xfer (local partitions)");
            sg_xfer_ctx get_local_partition_params = {.num_partitions = tmp_array, .partition_sizes = tmp_ptr, .partition_ptrs = local_partition_ptrs};
            get_block_t get_local_partition = {.f = &get_partition_func, .args = &get_local_partition_params, .args_size = sizeof(sg_xfer_ctx)};
            /* TODO: skip data transpose for byte interleaving */
            DPU_ASSERT(dpu_push_sg_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, max_elems_per_dpu_part * ELEM_SIZE,
                                        elems_per_dpu * ELEM_SIZE, &get_local_partition, DPU_SG_XFER_DISABLE_LENGTH_CHECK));
            t.stop();

#else /* #if 1 */

            /* compute prefix sum for CPU partition buffers */
            uint32_t prefix = 0;
            uint32_t global_prefix_sum[NR_PARTITIONS];
            for (auto p = 0; p < NR_PARTITIONS; p++) {
                global_prefix_sum[p] = prefix;
                for (auto d = 0; d < NR_DPUS; d++) {
                    prefix += local_part_sizes[d][p];
                }
            }

            /* copy elements into global partition buffers on CPU */
            PRINT("Copy elements to CPU global partition buffers...");
            uint32_t copy_count[NR_PARTITIONS] = {0};
            t.start("CPU exec (copy to global partitions)");
            for (uint32_t e = 0; e < total_elems; e++) {
                prop_code_t grp_key = elems_buffer[e].properties[GROUP_KEY];
                // uint32_t partition = global_partition_hash(grp_key) % NR_PARTITIONS;
                uint32_t partition = grp_key % NR_PARTITIONS;

                uint32_t offs = global_prefix_sum[partition] + copy_count[partition];
                copy_count[partition]++;
                memcpy(&global_part_buffer[offs], &elems_buffer[e], ELEM_SIZE);
            }
            t.stop();

#endif /* #if 1 */

            /* send global partitions to DPUs */
            PRINT("Transfer global partitions to DPUs...");
            uint32_t d = 0;
            uint32_t assigned_part_sizes[NR_DPUS] = {0};
            std::unordered_map<uint32_t, std::vector<uint32_t>> dpu_to_assigned_parts;
            for (uint32_t p = 0; p < NR_PARTITIONS; p++) {
                uint32_t global_part_size = (p == (NR_PARTITIONS - 1)) ?
                                            (total_elems - global_prefix_sum[p][0]) :
                                            (global_prefix_sum[p + 1][0] - global_prefix_sum[p][0]);
                /* uint32_t global_part_size = (p == (NR_PARTITIONS - 1)) ?
                                            (total_elems - global_prefix_sum[p]) :
                                            (global_prefix_sum[p + 1] - global_prefix_sum[p]); */
                while ((assigned_part_sizes[d] + global_part_size) >= max_elems_per_dpu_aggr) {
                    /* TODO: avoid looping infinitely when all partitions cannot fit in DPU */
                    d++;
                    d %= NR_DPUS; /* we assign global partitions to DPUs in a round-robin fashion */
                }
                if (dpu_to_assigned_parts.find(d) == dpu_to_assigned_parts.end()) {
                    dpu_to_assigned_parts[d];
                }
                dpu_to_assigned_parts[d].push_back(p);
                dpu_to_assigned_parts[d].push_back(global_part_size);
                assigned_part_sizes[d] += global_part_size;
                d++;
                d %= NR_DPUS;
            }

            uint32_t max_num_parts = 0;
            for (uint32_t d = 0; d < NR_DPUS; d++) {
                uint32_t parts = dpu_to_assigned_parts[d].size() / 2; /* for each partition, we store the partition's index and size consecutively */
                num_partitions[d] = parts;
                if (parts > max_num_parts) {
                    max_num_parts = parts;
                }
            }

            uint32_t htable_offsets[NR_DPUS];
            uint32_t max_num_parts_aligned = (max_num_parts % 2 == 0) ? max_num_parts : (max_num_parts + 1);
            global_partition_ptrs = (mrnode***) multidim_malloc(NR_DPUS, num_partitions, sizeof(mrnode*));

            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t i = 0;
                uint32_t part_offs = 0;
                uint32_t num_parts = num_partitions[d];
                uint32_t num_parts_aligned = (num_parts % 2 == 0) ? num_parts : (num_parts + 1);

                auto &partitions = dpu_to_assigned_parts[dpuid];
                partition_sizes[dpuid] = (uint32_t*) malloc(sizeof(uint32_t) * num_parts);
                for (uint32_t j = 0; j < partitions.size(); j += 2) { /* for each partition, we store the partition's index and size consecutively */
                    uint32_t p = partitions[j];
                    uint32_t part_size = partitions[j + 1];
                    global_partition_ptrs[dpuid][i] = &global_part_buffer[global_prefix_sum[p][0]];
                    /* global_partition_ptrs[dpuid][i] = &global_part_buffer[global_prefix_sum[p]]; */
                    partition_sizes[dpuid][i] = part_size;
                    part_offs += part_size;
                    i++;
                }

                /* send the sizes of the partitions */
                DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME, 0, partition_sizes[dpuid], sizeof(uint32_t) * num_parts_aligned));

                htable_offsets[dpuid] = part_offs;
            }

            uint32_t size_of_max_num_parts = 0;
            uint32_t size_of_min_num_parts = (uint32_t)(-1);
            uint32_t max_loaded_dpu, min_loaded_dpu;
            for (uint32_t d = 0; d < NR_DPUS; d++) {
                if (htable_offsets[d] > size_of_max_num_parts) {
                    size_of_max_num_parts = htable_offsets[d];
                    max_loaded_dpu = d;
                }
                else if (htable_offsets[d] < size_of_min_num_parts) {
                    size_of_min_num_parts = htable_offsets[d];
                    min_loaded_dpu = d;
                }
            }

            PRINT("Most loaded DPU: %u (partitions: %u, elements: %u, size: %f MiB)", max_loaded_dpu, num_partitions[max_loaded_dpu], htable_offsets[max_loaded_dpu], (htable_offsets[max_loaded_dpu] * ELEM_SIZE) / (double)MiB);
            PRINT("Least loaded DPU: %u (partitions: %u, elements: %u, size: %f MiB)", min_loaded_dpu, num_partitions[min_loaded_dpu], htable_offsets[min_loaded_dpu], (htable_offsets[min_loaded_dpu] * ELEM_SIZE) / (double)MiB);

            t.start("CPU to DPU xfer (global partitions)");
            sg_xfer_ctx get_global_partition_params = {.num_partitions = num_partitions, .partition_sizes = partition_sizes, .partition_ptrs = global_partition_ptrs};
            get_block_t get_global_partition = {.f = &get_partition_func, .args = &get_global_partition_params, .args_size = sizeof(sg_xfer_ctx)};
            /* TODO: skip data transpose for byte interleaving */
            DPU_ASSERT(dpu_push_sg_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, max_num_parts_aligned * sizeof(uint32_t),
                                        size_of_max_num_parts * ELEM_SIZE, &get_global_partition, DPU_SG_XFER_DISABLE_LENGTH_CHECK));
            t.stop();

            /* transfer aggregation parameters to DPUs */
            PRINT("Transfer aggregation parameters to DPUs...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                params[dpuid].num_partitions = num_partitions[dpuid];
                params[dpuid].max_num_partitions = max_num_parts_aligned;
                params[dpuid].phase = aggregation_phase;
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

            /* retrieve all global partitions' hash tables from DPUs */
            PRINT("Transfer hash tables of global partitions to CPU...");
            t.start("DPU to CPU transfer (hash tables of global partitions)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE * num_partitions[dpuid]);
                /* TODO: 1) scatter gather xfer, 2) skip data transpose for byte interleaving */
                DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(uint32_t) * max_num_parts_aligned + ELEM_SIZE * htable_offsets[dpuid],
                                         hash_tables[dpuid], sizeof(htable_entry) * num_partitions[dpuid] * NR_HASH_TABLE_ENTRIES));
            }
            t.stop();

            PRINT("Collect aggregation results of global partitions...");
            t.start("CPU exec (aggregation results of global partitions)");
            for (uint32_t d = 0; d < NR_DPUS; d++) {
                for (uint32_t idx = 0; idx < (num_partitions[d] * NR_HASH_TABLE_ENTRIES); idx++) {
                    if (hash_tables[d][idx].key != (uint32_t)(-1)) {
                        uint32_t key = hash_tables[d][idx].key;
                        aggr_res res;
                        #if defined(COUNT) || defined(AVERAGE)
                        res.cnt = hash_tables[d][idx].val.cnt;
                        #endif

                        #if defined(SUM) || defined(AVERAGE)
                        res.sum = hash_tables[d][idx].val.sum;
                        #endif

                        #ifdef AVERAGE
                        if (hash_tables[d][idx].val.cnt > 0) {
                            hash_tables[d][idx].val.avg = hash_tables[d][idx].val.sum / (double)hash_tables[d][idx].val.cnt;
                        }
                        res.avg = hash_tables[d][idx].val.avg;
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

        /* free DPUs */
        PRINT("Free DPUs...");
        DPU_ASSERT(dpu_free(dpu_set));

        /* free CPU buffers */
        PRINT("Free CPU buffers...");
        free(elems_buffer);
        free(global_part_buffer);
        for (uint32_t d = 0; d < NR_DPUS; d++) {
            free(hash_tables[d]);
            free(global_partition_ptrs[d]);
        }
        free(global_partition_ptrs);

        PRINT_TOP_RULE;
        t.print();
        auto mark = std::to_string(NR_DPUS) + "DPUS_" +
                    std::to_string(NR_PARTITIONS) + "PARTITIONS_" +
                    std::to_string(GROUP_KEY_CARDINALITY) + "GRP_KEYS_" +
                    "SF10";
        t.print_to_csv(CSV_FILE, mark);
    }
    catch (const dpu::DpuError &e) {
        std::cerr << e.what() << std::endl;
    }
}

#endif /* #ifndef HASH_BASED_HIGH_CARDINALITY_V3 */
