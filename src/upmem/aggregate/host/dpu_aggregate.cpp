#include "graph_db.hpp"
#include "thread_pool.hpp"
#include <dpu>
#include <dpu_log.h>

#include <random>

#include "../common/timer.hpp"
#include "../common/common.h"
#include "../common/hash.h"


void validate_aggr(const std::unordered_map<uint32_t, aggr_res> &lhs, const std::unordered_map<uint32_t, aggr_res> &rhs) {
    assert(lhs.size() == rhs.size());

    bool equal = true;
    for (auto &[lhs_key, lhs_res] : lhs) {
        auto iter = rhs.find(lhs_key);
        if (iter == rhs.end()) {
            equal = false;
            PRINT_ERROR("Group with key %u not found", lhs_key);
        }
        else {
            auto rhs_res = iter->second;

            if (lhs_res.cnt != rhs_res.cnt) {
                equal = false;
                PRINT_ERROR("\"cnt\" of group with key %u is not equal", lhs_key);
            }
            if (lhs_res.sum != rhs_res.sum) {
                equal = false;
                PRINT_ERROR("\"sum\" of group with key %u is not equal", lhs_key);
            }
            if (lhs_res.avg != rhs_res.avg) {
                equal = false;
                PRINT_ERROR("\"avg\" of group with key %u is not equal", lhs_key);
            }
            if (lhs_res.min != rhs_res.min) {
                equal = false;
                PRINT_ERROR("\"min\" of group with key %u is not equal", lhs_key);
            }
            if (lhs_res.max != rhs_res.max) {
                equal = false;
                PRINT_ERROR("\"max\" of group with key %u is not equal", lhs_key);
            }
        }

    }

    if (equal) {
        PRINT_INFO(true, "Aggregate results are equal");
    }
    else {
        PRINT_ERROR("Aggregate results are not equal");
    }
}

struct aggr_task {
    using range = std::pair<std::size_t, std::size_t>;
    aggr_task(mrnode* elems_buf, std::unordered_map<uint32_t, aggr_res>* htable, std::size_t first, std::size_t last, std::size_t tid)
        : elems_buf_(elems_buf), htable_(htable), range_(first, last), tid_(tid) {}

    static void aggregate(struct mrnode* elems_buf, std::unordered_map<uint32_t, aggr_res>* htable, uint32_t first, uint32_t last, uint32_t tid) {

        for (uint32_t e = first; e < last; e++) {
            mrnode &elem = elems_buf[e];
            uint32_t group_key = elem.properties[GROUP_KEY];
            // uint32_t hash = aggr_hash(group_key) % NR_HASH_TABLE_ENTRIES;
            uint32_t hash = group_key;

            auto iter = htable->find(hash);
            if (iter != htable->end()) {
                auto &aggr = iter->second;

                #if defined(COUNT) || defined(AVERAGE)
                aggr.cnt++;
                #endif

                #if defined(SUM) || defined(AVERAGE)
                aggr.sum += elem.properties[SUM_KEY];
                #endif

                #ifdef MINIMUM
                if (elem.properties[MIN_KEY] < aggr.min) {
                    aggr.min = elem.properties[MIN_KEY];
                }
                #endif

                #ifdef MAXIMUM
                if (elem.properties[MAX_KEY] > aggr.max) {
                    aggr.max = elem.properties[MAX_KEY];
                }
                #endif
            }
            else {
                aggr_res aggr;
                aggr.cnt = 0;
                aggr.sum = 0;
                aggr.avg = 0.0;
                aggr.min = UNKNOWN_CODE;
                aggr.max = 0;

                #if defined(COUNT) || defined(AVERAGE)
                aggr.cnt++;
                #endif

                #if defined(SUM) || defined(AVERAGE)
                aggr.sum += elem.properties[SUM_KEY];
                #endif

                #ifdef MINIMUM
                if (elem.properties[MIN_KEY] < aggr.min) {
                    aggr.min = elem.properties[MIN_KEY];
                }
                #endif

                #ifdef MAXIMUM
                if (elem.properties[MAX_KEY] > aggr.max) {
                    aggr.max = elem.properties[MAX_KEY];
                }
                #endif

                htable->emplace(std::piecewise_construct, std::forward_as_tuple(hash), std::forward_as_tuple(aggr));
            }
        }
    }

    void operator()() {
        aggregate(elems_buf_, htable_, range_.first, range_.second, tid_);
    }

    mrnode* elems_buf_;
    std::unordered_map<uint32_t, aggr_res>* htable_;
    range range_;
    std::size_t tid_;
};

void parallel_cpu_aggr(mrnode* elems_buf, uint32_t num_elems, std::unordered_map<uint32_t, aggr_res>* htables) {

    uint32_t elems_per_thread = DIVCEIL(num_elems, NR_CPU_THREADS);
    PRINT("CPU threads: %d", NR_CPU_THREADS);
    PRINT("Total elements: %d", num_elems);
    PRINT("Elements per thread: %u", elems_per_thread);

    std::vector<std::future<void>> res;
    res.reserve(NR_CPU_THREADS);
    thread_pool pool(NR_CPU_THREADS);
    uint32_t start = 0, end = elems_per_thread;
    for (uint32_t tid = 0; tid < NR_CPU_THREADS; tid++) {
        res.push_back(pool.submit(
            aggr_task(elems_buf, &htables[tid], start, end, tid)));
        start = end;
        end += (tid == (NR_CPU_THREADS - 2)) ?
               (num_elems - end) :
               elems_per_thread;
    }

    for (auto &f : res) {
        f.get();
    }
}

void sequential_cpu_aggr(mrnode* elems_buf, uint32_t num_elems, std::unordered_map<uint32_t, aggr_res> &hash_table) {

    for (uint32_t e = 0; e < num_elems; e++) {
        mrnode &elem = elems_buf[e];
        uint32_t group_key = elem.properties[GROUP_KEY];
        // uint32_t hash = aggr_hash(group_key) % NR_HASH_TABLE_ENTRIES;
        uint32_t hash = group_key;

        if (hash_table.find(hash) != hash_table.end()) {
            auto &aggr = hash_table[hash];

            #if defined(COUNT) || defined(AVERAGE)
            aggr.cnt++;
            #endif

            #if defined(SUM) || defined(AVERAGE)
            aggr.sum += elem.properties[SUM_KEY];
            #endif

            #ifdef MINIMUM
            if (elem.properties[MIN_KEY] < aggr.min) {
                aggr.min = elem.properties[MIN_KEY];
            }
            #endif

            #ifdef MAXIMUM
            if (elem.properties[MAX_KEY] > aggr.max) {
                aggr.max = elem.properties[MAX_KEY];
            }
            #endif
        }
        else {
            aggr_res aggr;
            aggr.cnt = 0;
            aggr.sum = 0;
            aggr.avg = 0.0;
            aggr.min = UNKNOWN_CODE;
            aggr.max = 0;

            #if defined(COUNT) || defined(AVERAGE)
            aggr.cnt++;
            #endif

            #if defined(SUM) || defined(AVERAGE)
            aggr.sum += elem.properties[SUM_KEY];
            #endif

            #ifdef MINIMUM
            if (elem.properties[MIN_KEY] < aggr.min) {
                aggr.min = elem.properties[MIN_KEY];
            }
            #endif

            #ifdef MAXIMUM
            if (elem.properties[MAX_KEY] > aggr.max) {
                aggr.max = elem.properties[MAX_KEY];
            }
            #endif

            hash_table.emplace(std::piecewise_construct, std::forward_as_tuple(hash), std::forward_as_tuple(aggr));
        }
    }

#ifdef AVERAGE
    for (auto &res : hash_table) {
        auto &rs = res.second;
        if (rs.cnt > 0) {
            rs.avg = rs.sum / (double)rs.cnt;
        }
    }
#endif
}

void initialize_props(graph_db_ptr &graph, struct mrnode* nodes_ptr) {

    auto &nodes = graph->get_nodes();
    auto iter = nodes->as_vec().chunk_list_begin();
    auto end = nodes->as_vec().chunk_list_end();
    auto elems_per_chunk = nodes->as_vec().elements_per_chunk();
    auto last_offs = nodes->as_vec().last_used();

    uint64_t cmt_code = 1;
    // uint64_t post_code = 5827195; /* SF 0.1 */
    uint64_t post_code = 67111801; /* SF 1 */
    // uint64_t post_code = 692931280; /* SF 10 */

    std::random_device rd;
    std::mt19937::result_type seed = rd() ^ (
        (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() +
        (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

    std::mt19937 gen(seed);
    std::uniform_int_distribution<prop_code_t> dist(0, (GROUP_KEY_CARDINALITY - 1)); /* [x, y] */

    uint32_t ch = 0;
    uint32_t offs = 0;
    uint32_t pos = 0;
    while (iter != end) {
        for (uint32_t j = 0; j < elems_per_chunk; j++) {
            offs = ch * elems_per_chunk + j;
            // if ((*iter)->is_used(j)) { /* TODO: implement on DPU program */
                if (offs > last_offs) {
                    break;
                }

                auto &n = (*iter)->data_[j];
                uint64_t nid = n.id();
                // assert(nid == offs);

                uint32_t lc = n.node_label;
                if (lc == cmt_code || lc == post_code) {
                    struct mrnode &mrn = nodes_ptr[pos++];
                    mrn.id_ = nid;
                    mrn.node_label = lc;

                    graph->begin_transaction();

                    auto prop = 0;
                    auto node_desc = graph->get_node_description(nid);
                    auto iter = node_desc.properties.find("creationDate");
                    auto value = iter->second;
                    auto dt = any_cast<boost::posix_time::ptime>(&value);
                    auto dtstr = to_iso_extended_string(dt->date());
                    auto yrstr = dtstr.substr(0, dtstr.find("-"));
                    mrn.properties[prop++] = (prop_code_t)std::stoi(yrstr); /* Key: 0 */

                    if (lc == cmt_code) { /* Key: 1 */
                        mrn.properties[prop++] = (prop_code_t)1;
                    }
                    else if (lc == post_code) {
                        mrn.properties[prop++] = (prop_code_t)0;
                    }

                    iter = node_desc.properties.find("length");
                    value = iter->second;
                    auto len = any_cast<int>(&value);

                    mrn.properties[prop++] = (prop_code_t)(*len); /* Key: 2 */

                    #if defined HIGH_CARDINALITY || defined HIGH_CARDINALITY_V2
                    #if 1
                    iter = node_desc.properties.find("id");
                    value = iter->second;
                    auto id = any_cast<uint64_t>(&value);
                    mrn.properties[prop++] = (prop_code_t)(*id % GROUP_KEY_CARDINALITY); /* Key: 3 */
                    #else
                    mrn.properties[prop++] = dist(gen);
                    #endif
                    #elif defined LOW_CARDINALITY
                    prop_code_t val = (*len >= 0 && *len < 40) ?
                                      0 :
                                      (*len >= 40 && *len < 80) ?
                                      1 :
                                      (*len >= 80 && *len < 160) ?
                                      2 : 3;
                    mrn.properties[prop++] = val % GROUP_KEY_CARDINALITY; /* Key: 3 */
                    #endif

                    graph->commit_transaction();
                }
            // }
        }
        ch++;
        iter++;
    }
}


#ifdef HIGH_CARDINALITY_V2

void dpu_aggregate(graph_db_ptr &graph, uint32_t num_of_dpus, std::string dpu_binary) {

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

        DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &dpu_set));
        DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &num_dpus));
        DPU_ASSERT(dpu_get_nr_ranks(dpu_set, &num_ranks));
        DPU_ASSERT(dpu_load(dpu_set, dpu_binary.c_str(), NULL));

        assert(num_dpus == NR_DPUS);
        // uint32_t total_elems = (151043 /* Comment */ + 135701 /* Post */); /* SF 0.1 */
        uint32_t total_elems = (2052169 /* Comment */ + 1003605 /* Post */); /* SF 1 */
        // uint32_t total_elems = (21865475 /* Comment */ + 7435696 /* Post */); /* SF 10 */
        uint32_t total_elem_size = total_elems * ELEM_SIZE;
        uint32_t elems_per_dpu = DIVCEIL(total_elems, NR_DPUS);
        uint32_t grp_key_size_per_dpu = sizeof(prop_code_t) * elems_per_dpu;
        uint32_t grp_keys_per_dpu_aligned = (elems_per_dpu % 2 == 0) ? elems_per_dpu : (elems_per_dpu + 1);
        bool dpu_overflow = grp_key_size_per_dpu > MRAM_INPUT_BUFFER_PARTITION;

        PRINT_TOP_RULE;
        PRINT("DPUs: %u", NR_DPUS);
        PRINT("Ranks: %u", num_ranks);
        PRINT("Total elements: %u", total_elems);
        PRINT("Element size: %lu", ELEM_SIZE);
        PRINT("Total element size: %u", total_elem_size);
        PRINT("Elements per DPU: %u", elems_per_dpu);
        PRINT("Group key cardinality: %u", GROUP_KEY_CARDINALITY);
        PRINT("Group key size: %lu", sizeof(prop_code_t));
        PRINT("Group key size per DPU: %u (%f MB)", grp_key_size_per_dpu, (grp_key_size_per_dpu / (double)MB));
        PRINT("Partitions: %u", NR_PARTITIONS);

        mrnode* elems_buffer;
        prop_code_t* grp_key_buffer;
        htable_entry* hash_tables[NR_DPUS];
        mrnode* global_part_buffer = (mrnode*) malloc (ELEM_SIZE * total_elems);
        std::unordered_map<uint32_t, aggr_res> seq_cpu_res, par_cpu_res, dpu_res;

        if (dpu_overflow) {
            /* TODO */
        }
        else {

            /* initialize elements */
            elems_buffer = (mrnode*) malloc (elems_per_dpu * ELEM_SIZE * NR_DPUS);
            initialize_props(graph, elems_buffer);

            /* get grouping keys */
            PRINT("Get grouping keys...");
            t.start("CPU exec (get group keys)");
            grp_key_buffer = (prop_code_t*) malloc (elems_per_dpu * sizeof(prop_code_t) * NR_DPUS);
            for (uint32_t e = 0; e < total_elems; e++) {
                grp_key_buffer[e] = elems_buffer[e].properties[GROUP_KEY];
            }
            t.stop();

            /* transfer grouping keys to DPU */
            PRINT("Transfer group keys to DPUs...");
            t.start("CPU to DPU xfer (group keys)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &grp_key_buffer[dpuid * elems_per_dpu]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, grp_keys_per_dpu_aligned * sizeof(prop_code_t), DPU_XFER_DEFAULT));
            t.stop();

            /* transfer partition parameters to DPU */
            PRINT("Transfer partition parameters to DPUs...");
            dpu_params params[NR_DPUS];
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t elems = (dpuid == (NR_DPUS - 1)) ?
                                 (total_elems - dpuid * elems_per_dpu) :
                                 elems_per_dpu;
                params[dpuid].num_elems = elems;
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
            t.start("DPU to CPU xfer (local histograms)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &local_part_sizes[dpuid]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_partition_sizes", 0, sizeof(uint32_t) * NR_PARTITIONS, DPU_XFER_DEFAULT));
            t.stop();

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

            /* send global partitions to DPUs */
            PRINT("Transfer global partitions to DPUs...");
            uint32_t d = 0;
            uint32_t max_elems_per_dpu = (MRAM_INPUT_BUFFER_AGGREGATION / ELEM_SIZE); /* TODO: reserve space for hash tables */
            uint32_t assigned_part_sizes[NR_DPUS] = {0};
            std::unordered_map<uint32_t, std::vector<uint32_t>> dpu_to_assigned_parts;
            for (uint32_t p = 0; p < NR_PARTITIONS; p++) {
                uint32_t global_part_size = (p == (NR_PARTITIONS - 1)) ?
                                            (total_elems - global_prefix_sum[p]) :
                                            (global_prefix_sum[p + 1] - global_prefix_sum[p]);
                while ((assigned_part_sizes[d] + global_part_size) >= max_elems_per_dpu) {
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

            t.start("CPU to DPU xfer (global partitions)");
            uint32_t htable_offsets[NR_DPUS];
            uint32_t num_partitions[NR_DPUS];
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t i = 0;
                uint32_t part_offs = 0;
                auto &partitions = dpu_to_assigned_parts[dpuid];
                uint32_t num_parts = partitions.size() / 2; /* for each partition, we store the partition's index and size consecutively */
                uint32_t partition_sizes[num_parts];
                uint32_t num_parts_aligned = (num_parts % 2 == 0) ? num_parts : (num_parts + 1);

                /* transfer the partitions */
                for (uint32_t j = 0; j < partitions.size(); j += 2) {
                    uint32_t p = partitions[j];
                    uint32_t part_size = partitions[j + 1];
                    partition_sizes[i++] = part_size;
                    /* TODO: 1) scatter gather xfer, 2) skip data transpose for byte interleaving */
                    DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(uint32_t) * num_parts_aligned + ELEM_SIZE * part_offs, /* we store partition sizes at the beginning of MRAM */
                                           &global_part_buffer[global_prefix_sum[p]], ELEM_SIZE * part_size));
                    part_offs += part_size;
                }
                /* send the sizes of the partitions */
                DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME, 0, partition_sizes, sizeof(uint32_t) * num_parts_aligned));

                htable_offsets[dpuid] = part_offs;
                num_partitions[dpuid] = num_parts;
            }
            t.stop();

            /* transfer aggregation parameters to DPUs */
            PRINT("Transfer aggregation parameters to DPUs...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                params[dpuid].num_partitions = num_partitions[dpuid];
                params[dpuid].phase = aggregation_phase;
                DPU_ASSERT(dpu_prepare_xfer(dpu, &params[dpuid]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "dpu_parameters", 0, sizeof(dpu_params), DPU_XFER_DEFAULT));

            uint32_t max_part_size = 0;
            uint32_t min_part_size = (-1);
            uint32_t max_loaded_dpu, min_loaded_dpu;
            for (uint32_t d = 0; d < NR_DPUS; d++) {
                if (htable_offsets[d] > max_part_size) {
                    max_part_size = htable_offsets[d];
                    max_loaded_dpu = d;
                }
                else if (htable_offsets[d] < min_part_size) {
                    min_part_size = htable_offsets[d];
                    min_loaded_dpu = d;
                }
            }

            PRINT("Most loaded DPU: %u (partitions: %u, elements: %u, size: %f MB)", max_loaded_dpu, num_partitions[max_loaded_dpu], htable_offsets[max_loaded_dpu], (htable_offsets[max_loaded_dpu] * ELEM_SIZE) / (double)MB);
            PRINT("Least loaded DPU: %u (partitions: %u, elements: %u, size: %f MB)", min_loaded_dpu, num_partitions[min_loaded_dpu], htable_offsets[min_loaded_dpu], (htable_offsets[min_loaded_dpu] * ELEM_SIZE) / (double)MB);

            /* launch the aggregation kernel */
            PRINT("Execute aggregation kernel on DPUs...");
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
            t.start("DPU to CPU xfer (hash tables of global partitions)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE * num_partitions[dpuid]);
                uint32_t num_parts_aligned = (num_partitions[dpuid] % 2 == 0) ? num_partitions[dpuid] : (num_partitions[dpuid] + 1);
                /* TODO: 1) scatter gather xfer, 2) skip data transpose for byte interleaving */
                DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(uint32_t) * num_parts_aligned + ELEM_SIZE * htable_offsets[dpuid],
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

        /* free CPU buffers */
        PRINT("Free CPU buffers...");
        free(elems_buffer);
        free(grp_key_buffer);
        free(global_part_buffer);
        for (uint32_t d = 0; d < NR_DPUS; d++) {
            free(hash_tables[d]);
        }

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

#elif defined HIGH_CARDINALITY /* partition data such that hash table for each partition fits in WRAM */

void dpu_aggregate(graph_db_ptr &graph, uint32_t num_of_dpus, std::string dpu_binary) {

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

        DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &dpu_set));
        DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &num_dpus));
        DPU_ASSERT(dpu_get_nr_ranks(dpu_set, &num_ranks));
        DPU_ASSERT(dpu_load(dpu_set, dpu_binary.c_str(), NULL));

        assert(num_dpus == NR_DPUS);
        uint32_t total_elems = (151043 /* Comment */ + 135701 /* Post */); /* SF 0.1 */
        // uint32_t total_elems = (2052169 /* Comment */ + 1003605 /* Post */); /* SF 1 */
        // uint32_t total_elems = (21865475 /* Comment */ + 7435696 /* Post */); /* SF 10 */
        uint32_t elems_per_dpu = DIVCEIL(total_elems, NR_DPUS);
        uint32_t elems_size_per_dpu = ELEM_SIZE * elems_per_dpu;
        bool dpu_overflow = elems_size_per_dpu > MRAM_INPUT_BUFFER_PARTITION;

        PRINT_TOP_RULE;
        PRINT("DPUs: %u", NR_DPUS);
        PRINT("Ranks: %u", num_ranks);
        PRINT("Elements: %u", total_elems);
        PRINT("Elements per DPU: %u", elems_per_dpu);
        PRINT("Element Size: %lu", ELEM_SIZE);
        PRINT("Element Size per DPU: %u (%f MB)", elems_size_per_dpu, (elems_size_per_dpu / (double)MB));
        PRINT("Partitions: %u", NR_PARTITIONS);

        mrnode* elems_buffer;
        htable_entry* hash_tables[NR_DPUS];
        mrnode* global_part_buffer = (mrnode*) malloc (ELEM_SIZE * total_elems);
        std::unordered_map<uint32_t, aggr_res> seq_cpu_res, par_cpu_res, dpu_res;

        if (dpu_overflow) {
            #if 0 /* TODO */
            PRINT_INFO(true, "Max. %d (%f MB) MRAM input buffer exceeded. Adjusting...", MRAM_INPUT_BUFFER_PARTITION, (MRAM_INPUT_BUFFER_PARTITION / (double)MB));
            elems_per_dpu = (MRAM_INPUT_BUFFER_PARTITION / 2) / ELEM_SIZE;
            elems_size_per_dpu = ELEM_SIZE * elems_per_dpu;
            uint32_t dpu_exec_rounds = DIVCEIL(total_elems, (elems_per_dpu * NR_DPUS));

            PRINT("DPU execution rounds: %u", dpu_exec_rounds);
            PRINT("Elements per DPU: %u", elems_per_dpu);
            PRINT("Element Size per DPU: %u (%f MB)", elems_size_per_dpu, (elems_size_per_dpu / (double)MB));

            /* initialize elements */
            elems_buffer = (mrnode*) malloc (ELEM_SIZE * elems_per_dpu * NR_DPUS * dpu_exec_rounds);
            initialize_props(graph, elems_buffer);

            for (uint32_t round = 0; round < dpu_exec_rounds; round++) {
                PRINT("Execution Round %u...", (round + 1));

                dpu_params params[NR_DPUS];
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

                    /* transfer partition parameters to DPU */
                    PRINT("Transfer partition parameters to DPUs...");
                    DPU_FOREACH(dpu_set, dpu, dpuid) {
                        uint32_t elems = (dpuid == (NR_DPUS - 1)) ?
                                         (total_elems - (round * NR_DPUS * elems_per_dpu + dpuid * last_round_elems_per_dpu)) :
                                         last_round_elems_per_dpu;
                        params[dpuid].num_elems = elems;
                        params[dpuid].phase = partition_phase;
                        DPU_ASSERT(dpu_prepare_xfer(dpu, &params[dpuid]));
                    }
                    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "dpu_parameters", 0, sizeof(dpu_params), DPU_XFER_DEFAULT));

                }
                else {

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
                        params[dpuid].phase = partition_phase;
                        DPU_ASSERT(dpu_prepare_xfer(dpu, &params[dpuid]));
                    }
                    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "dpu_parameters", 0, sizeof(dpu_params), DPU_XFER_DEFAULT));
                }

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
                PRINT("Transfer histograms of local paritions to CPU...");
                uint32_t local_part_sizes[NR_DPUS][NR_PARTITIONS];
                t.start("DPU to CPU transfer (local histograms)");
                DPU_FOREACH(dpu_set, dpu, dpuid) {
                    DPU_ASSERT(dpu_prepare_xfer(dpu, &local_part_sizes[dpuid]));
                }
                DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_partition_sizes", 0, sizeof(uint32_t) * NR_PARTITIONS, DPU_XFER_DEFAULT));
                t.stop();

                /* compute prefix sum for CPU partition buffers */
                uint32_t prefix = 0;
                uint32_t global_prefix_sum[NR_PARTITIONS][NR_DPUS];
                for (auto p = 0; p < NR_PARTITIONS; p++) {
                    for (auto d = 0; d < NR_DPUS; d++) {
                        global_prefix_sum[p][d] = prefix;
                        prefix += local_part_sizes[d][p];
                    }
                }

                /* copy local partitions from DPUs into global partition buffers on CPU */
                PRINT("Transfer DPU local partitions to CPU global partition buffers...");
                uint32_t mroffs[NR_DPUS] = {0};
                t.start("DPU to CPU xfer (local partitions)");
                for (uint32_t p = 0; p < NR_PARTITIONS; p++) {
                    DPU_FOREACH(dpu_set, dpu, dpuid) {
                        uint32_t input_elems = (dpuid == (NR_DPUS - 1)) ?
                                            (total_elems - dpuid * elems_per_dpu) :
                                            elems_per_dpu;
                        /* TODO: 1) scatter gather xfer, 2) skip data transpose for byte interleaving */
                        DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, ELEM_SIZE * (input_elems + mroffs[dpuid]),
                                                &global_part_buffer[global_prefix_sum[p][dpuid]], ELEM_SIZE * local_part_sizes[dpuid][p]));
                        mroffs[dpuid] += local_part_sizes[dpuid][p];
                    }
                }
                t.stop();

                /* send global partitions to DPUs */
                PRINT("Transfer global partitions to DPUs...");
                uint32_t d = 0;
                uint32_t max_elems_per_dpu = (MRAM_INPUT_BUFFER_AGGREGATION / ELEM_SIZE); /* TODO: reserve space for hash tables */
                uint32_t assigned_part_sizes[NR_DPUS] = {0};
                std::unordered_map<uint32_t, std::vector<uint32_t>> dpu_to_assigned_parts;
                for (uint32_t p = 0; p < NR_PARTITIONS; p++) {
                    uint32_t global_part_size = (p == (NR_PARTITIONS - 1)) ?
                                                (total_elems - global_prefix_sum[p][0]) :
                                                (global_prefix_sum[p + 1][0] - global_prefix_sum[p][0]);
                    while ((assigned_part_sizes[d] + global_part_size) >= max_elems_per_dpu) {
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

                t.start("CPU to DPU xfer (global partitions)");
                uint32_t htable_offsets[NR_DPUS];
                uint32_t num_partitions[NR_DPUS];
                DPU_FOREACH(dpu_set, dpu, dpuid) {
                    uint32_t i = 0;
                    uint32_t part_offs = 0;
                    auto &partitions = dpu_to_assigned_parts[dpuid];
                    uint32_t num_parts = partitions.size() / 2; /* for each partition, we store the partition's index and size consecutively */
                    uint32_t partition_sizes[num_parts];
                    uint32_t num_parts_aligned = (num_parts % 2 == 0) ? num_parts : (num_parts + 1);

                    /* transfer the partitions */
                    for (uint32_t j = 0; j < partitions.size(); j += 2) {
                        uint32_t p = partitions[j];
                        uint32_t part_size = partitions[j + 1];
                        partition_sizes[i++] = part_size;
                        /* TODO: 1) scatter gather xfer, 2) skip data transpose for byte interleaving */
                        DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(uint32_t) * num_parts_aligned + ELEM_SIZE * part_offs, /* we store partition sizes at the beginning of MRAM */
                                            &global_part_buffer[global_prefix_sum[p][0]], ELEM_SIZE * part_size));
                        part_offs += part_size;
                    }
                    /* send the sizes of the partitions */
                    DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME, 0, partition_sizes, sizeof(uint32_t) * num_parts_aligned));

                    htable_offsets[dpuid] = part_offs;
                    num_partitions[dpuid] = num_parts;
                }
                t.stop();

                /* transfer aggregation parameters to DPUs */
                PRINT("Transfer aggregation parameters to DPUs...");
                DPU_FOREACH(dpu_set, dpu, dpuid) {
                    params[dpuid].num_partitions = num_partitions[dpuid];
                    params[dpuid].phase = aggregation_phase;
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

                /* retrieve all global partitions' hash tables from DPUs */
                PRINT("Transfer hash tables of global partitions to CPU...");
                t.start("DPU to CPU transfer (hash tables of global partitions)");
                DPU_FOREACH(dpu_set, dpu, dpuid) {
                    hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE * num_partitions[dpuid]);
                    uint32_t num_parts_aligned = (num_partitions[dpuid] % 2 == 0) ? num_partitions[dpuid] : (num_partitions[dpuid] + 1);
                    /* TODO: 1) scatter gather xfer, 2) skip data transpose for byte interleaving */
                    DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(uint32_t) * num_parts_aligned + ELEM_SIZE * htable_offsets[dpuid],
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

                /* reset CPU buffers */
                PRINT("Reset CPU buffers...");
                memset(global_part_buffer, 0, ELEM_SIZE * total_elems);
                for (uint32_t d = 0; d < NR_DPUS; d++) {
                    memset(hash_tables[d], 0, HASH_TABLE_SIZE * num_partitions[d]);
                }
            }
            #endif
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
            dpu_params params[NR_DPUS];
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t elems = (dpuid == (NR_DPUS - 1)) ?
                                 (total_elems - dpuid * elems_per_dpu) :
                                 elems_per_dpu;
                params[dpuid].num_elems = elems;
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
            PRINT("Transfer histograms of local paritions to CPU...");
            uint32_t local_part_sizes[NR_DPUS][NR_PARTITIONS];
            t.start("DPU to CPU transfer (local histograms)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &local_part_sizes[dpuid]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_partition_sizes", 0, sizeof(uint32_t) * NR_PARTITIONS, DPU_XFER_DEFAULT));
            t.stop();

            /* compute prefix sum for CPU partition buffers */
            uint32_t prefix = 0;
            uint32_t global_prefix_sum[NR_PARTITIONS][NR_DPUS];
            for (auto p = 0; p < NR_PARTITIONS; p++) {
                for (auto d = 0; d < NR_DPUS; d++) {
                    global_prefix_sum[p][d] = prefix;
                    prefix += local_part_sizes[d][p];
                }
            }

            /* copy local partitions from DPUs into global partition buffers on CPU */
            PRINT("Transfer DPU local partitions to CPU global partition buffers...");
            uint32_t mroffs[NR_DPUS] = {0};
            t.start("DPU to CPU xfer (local partitions)");
            for (uint32_t p = 0; p < NR_PARTITIONS; p++) {
                DPU_FOREACH(dpu_set, dpu, dpuid) {
                    uint32_t input_elems = (dpuid == (NR_DPUS - 1)) ?
                                           (total_elems - dpuid * elems_per_dpu) :
                                           elems_per_dpu;
                    /* TODO: 1) scatter gather xfer, 2) skip data transpose for byte interleaving */
                    DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, ELEM_SIZE * (input_elems + mroffs[dpuid]),
                                             &global_part_buffer[global_prefix_sum[p][dpuid]], ELEM_SIZE * local_part_sizes[dpuid][p]));
                    mroffs[dpuid] += local_part_sizes[dpuid][p];
                }
            }
            t.stop();

            /* send global partitions to DPUs */
            PRINT("Transfer global partitions to DPUs...");
            uint32_t d = 0;
            uint32_t max_elems_per_dpu = (MRAM_INPUT_BUFFER_AGGREGATION / ELEM_SIZE); /* TODO: reserve space for hash tables */
            uint32_t assigned_part_sizes[NR_DPUS] = {0};
            std::unordered_map<uint32_t, std::vector<uint32_t>> dpu_to_assigned_parts;
            for (uint32_t p = 0; p < NR_PARTITIONS; p++) {
                uint32_t global_part_size = (p == (NR_PARTITIONS - 1)) ?
                                            (total_elems - global_prefix_sum[p][0]) :
                                            (global_prefix_sum[p + 1][0] - global_prefix_sum[p][0]);
                while ((assigned_part_sizes[d] + global_part_size) >= max_elems_per_dpu) {
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

            t.start("CPU to DPU xfer (global partitions)");
            uint32_t htable_offsets[NR_DPUS];
            uint32_t num_partitions[NR_DPUS];
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t i = 0;
                uint32_t part_offs = 0;
                auto &partitions = dpu_to_assigned_parts[dpuid];
                uint32_t num_parts = partitions.size() / 2; /* for each partition, we store the partition's index and size consecutively */
                uint32_t partition_sizes[num_parts];
                uint32_t num_parts_aligned = (num_parts % 2 == 0) ? num_parts : (num_parts + 1);

                /* transfer the partitions */
                for (uint32_t j = 0; j < partitions.size(); j += 2) {
                    uint32_t p = partitions[j];
                    uint32_t part_size = partitions[j + 1];
                    partition_sizes[i++] = part_size;
                    /* TODO: 1) scatter gather xfer, 2) skip data transpose for byte interleaving */
                    DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(uint32_t) * num_parts_aligned + ELEM_SIZE * part_offs, /* we store partition sizes at the beginning of MRAM */
                                           &global_part_buffer[global_prefix_sum[p][0]], ELEM_SIZE * part_size));
                    part_offs += part_size;
                }
                /* send the sizes of the partitions */
                DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME, 0, partition_sizes, sizeof(uint32_t) * num_parts_aligned));
                htable_offsets[dpuid] = part_offs;
                num_partitions[dpuid] = num_parts;
            }
            t.stop();

            /* transfer aggregation parameters to DPUs */
            PRINT("Transfer aggregation parameters to DPUs...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                params[dpuid].num_partitions = num_partitions[dpuid];
                params[dpuid].phase = aggregation_phase;
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

            /* retrieve all global partitions' hash tables from DPUs */
            PRINT("Transfer hash tables of global partitions to CPU...");
            t.start("DPU to CPU transfer (hash tables of global partitions)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE * num_partitions[dpuid]);
                uint32_t num_parts_aligned = (num_partitions[dpuid] % 2 == 0) ? num_partitions[dpuid] : (num_partitions[dpuid] + 1);
                /* TODO: 1) scatter gather xfer, 2) skip data transpose for byte interleaving */
                DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(uint32_t) * num_parts_aligned + ELEM_SIZE * htable_offsets[dpuid],
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

        /* free CPU buffers */
        PRINT("Free CPU buffers...");
        free(elems_buffer);
        free(global_part_buffer);
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

#elif defined LOW_CARDINALITY /* hash table for entire data fits in WRAM */

void dpu_aggregate(graph_db_ptr &graph, uint32_t num_of_dpus, std::string dpu_binary) {

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

        DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &dpu_set));
        DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &num_dpus));
        DPU_ASSERT(dpu_get_nr_ranks(dpu_set, &num_ranks));
        DPU_ASSERT(dpu_load(dpu_set, dpu_binary.c_str(), NULL));

        assert(num_dpus == NR_DPUS);
        uint32_t total_elems = (151043 /* Comment */ + 135701 /* Post */); /* SF 0.1 */
        // uint32_t total_elems = (2052169 /* Comment */ + 1003605 /* Post */); /* SF 1 */
        // uint32_t total_elems = (21865475 /* Comment */ + 7435696 /* Post */); /* SF 10 */
        uint32_t elems_per_dpu = DIVCEIL(total_elems, NR_DPUS);
        uint32_t elems_size_per_dpu = ELEM_SIZE * elems_per_dpu;
        uint32_t htable_offs = elems_per_dpu; /* TODO: tune */
        bool dpu_overflow = elems_size_per_dpu > MRAM_INPUT_BUFFER_SIZE;

        PRINT_TOP_RULE;
        PRINT("DPUs: %u", NR_DPUS);
        PRINT("Ranks: %u", num_ranks);
        PRINT("Elements: %u", total_elems);
        PRINT("Elements per DPU: %u", elems_per_dpu);
        PRINT("Element Size: %lu", ELEM_SIZE);
        PRINT("Element Size per DPU: %u (%f MB)", elems_size_per_dpu, (elems_size_per_dpu / (double)MB));

        mrnode* elems_buffer;
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
#ifdef LOW_CARD_1 /* a separate hash table for each tasklet */
                    hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE * NR_TASKLETS);
#elif defined LOW_CARD_2 /* a single hash table shared by all tasklets */
                    hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE);
#endif
                    DPU_ASSERT(dpu_prepare_xfer(dpu, hash_tables[dpuid]));
                }
#ifdef LOW_CARD_1
                DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, ELEM_SIZE * htable_offs, HASH_TABLE_SIZE * NR_TASKLETS, DPU_XFER_DEFAULT));
#elif defined LOW_CARD_2
                DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, ELEM_SIZE * htable_offs, HASH_TABLE_SIZE, DPU_XFER_DEFAULT));
#endif
                t.stop();

                PRINT("Collect DPU aggregation results...");
                t.start("CPU exec (DPU aggregation results)");
                for (uint32_t d = 0; d < NR_DPUS; d++) {
#ifdef LOW_CARD_1
                    for (uint32_t idx = 0; idx < NR_HASH_TABLE_ENTRIES * NR_TASKLETS; idx++) {
#elif defined LOW_CARD_2
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
#ifdef LOW_CARD_1
                    memset(hash_tables[d], 0, HASH_TABLE_SIZE * NR_TASKLETS);
#elif defined LOW_CARD_2
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
#ifdef LOW_CARD_1 /* a separate hash table for each tasklet */
                hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE * NR_TASKLETS);
#elif defined LOW_CARD_2 /* a single hash table shared by all tasklets */
                hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE);
#endif
                DPU_ASSERT(dpu_prepare_xfer(dpu, hash_tables[dpuid]));
            }
#ifdef LOW_CARD_1
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, ELEM_SIZE * htable_offs, HASH_TABLE_SIZE * NR_TASKLETS, DPU_XFER_DEFAULT));
#elif defined LOW_CARD_2
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, ELEM_SIZE * htable_offs, HASH_TABLE_SIZE, DPU_XFER_DEFAULT));
#endif
            t.stop();

            PRINT("Collect DPU aggregation results...");
            t.start("CPU exec (DPU aggregation results)");
            for (uint32_t d = 0; d < NR_DPUS; d++) {
#ifdef LOW_CARD_1
                for (uint32_t idx = 0; idx < NR_HASH_TABLE_ENTRIES * NR_TASKLETS; idx++) {
#elif defined LOW_CARD_2
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

#endif