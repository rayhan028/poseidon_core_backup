#include "graph_db.hpp"
#include "thread_pool.hpp"
#include <dpu>
#include <dpu_log.h>

#include <random>

#include "../common/timer.hpp"
#include "../common/common.h"
#include "../common/hash.h"

#define NR_TASKLETS 16
#define MAX_THREADS 32

uint64_t cpu_cnt_res[MAX_THREADS];
uint64_t cpu_sum_res[MAX_THREADS];
double cpu_avg_res[MAX_THREADS];
uint64_t cpu_min_res[MAX_THREADS];
uint64_t cpu_max_res[MAX_THREADS];

void collect_res(struct aggr_res &res, uint32_t num, uint64_t* cnt_vals,
                uint64_t* sum_vals, double* avg_vals, uint64_t* min_vals, uint64_t* max_vals) {
    uint64_t cnt = 0;
    uint64_t sum = 0;
    double avg = 0.0;
    uint64_t min = UNKNOWN;
    uint64_t max = 0;

    for (uint32_t d = 0; d < num; d++) {
        #if defined(COUNT) || defined(AVERAGE)
        cnt += cnt_vals[d];
        #endif

        #if defined(SUM) || defined(AVERAGE)
        sum += sum_vals[d];
        #endif

        #ifdef MINIMUM
        if (min > min_vals[d]) {
            min = min_vals[d];
        }
        #endif

        #ifdef MAXIMUM
        if (max < max_vals[d]) {
            max = max_vals[d];
        }
        #endif
    }

    #if defined(COUNT) || defined(AVERAGE)
    res.cnt = cnt;
    #endif
    #if defined(SUM) || defined(AVERAGE)
    res.sum = sum;
    #endif
    #ifdef AVERAGE
    if (cnt > 0) {
        avg = sum / (double)cnt;
    }
    res.avg = avg;
    #endif
    #ifdef MINIMUM
    res.min = min;
    #endif
    #ifdef MAXIMUM
    res.max = max;
    #endif
}

void validate_aggr(const std::unordered_map<uint32_t, aggr_res> &lhs, const std::unordered_map<uint32_t, aggr_res> &rhs) {
    assert(lhs.size() == rhs.size());

    bool equal = true;
    for (auto &[lhs_key, lhs_res] : lhs) {
        auto iter = rhs.find(lhs_key);
        if (iter == rhs.end()) {
            equal = false;
            PRINT_ERROR("Key not found");
        }
        else {
            auto rhs_res = iter->second;

            if (lhs_res.cnt != rhs_res.cnt) {
                equal = false;
                PRINT_ERROR("\"cnt\" is not equal");
            }
            if (lhs_res.sum != rhs_res.sum) {
                equal = false;
                PRINT_ERROR("\"sum\" is not equal");
            }
            if (lhs_res.avg != rhs_res.avg) {
                equal = false;
                PRINT_ERROR("\"avg\" is not equal");
            }
            if (lhs_res.min != rhs_res.min) {
                equal = false;
                PRINT_ERROR("\"min\" is not equal");
            }
            if (lhs_res.max != rhs_res.max) {
                equal = false;
                PRINT_ERROR("\"max\" is not equal");
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
    aggr_task(graph_db_ptr graph, std::size_t first, std::size_t last, std::size_t tid)
        : graph_(graph), range_(first, last), tid_(tid) {}

    static void aggregate(graph_db_ptr graph, std::size_t first, std::size_t last, unsigned int tid) {
        // auto last_offs = graph->get_nodes()->as_vec().last_used();
        auto iter = graph->get_nodes()->range(first, last);
        while (iter) {
            auto &n = *iter;
            uint64_t nid = n.id();

            // if (nid <= last_offs) { /* TODO */
            //     break;
            // }

            #if defined(COUNT) || defined(AVERAGE)
            cpu_cnt_res[tid]++;
            #endif

            #if defined(SUM) || defined(AVERAGE)
            cpu_sum_res[tid] += nid;
            #endif

            #ifdef MINIMUM
            if (nid < cpu_min_res[tid]) {
                cpu_min_res[tid] = nid;
            }
            #endif

            #ifdef MAXIMUM
            if (nid > cpu_max_res[tid]) {
                cpu_max_res[tid] = nid;
            }
            #endif

            ++iter;
        }
    }

    void operator()() {
        aggregate(graph_, range_.first, range_.second, tid_);
    }

    graph_db_ptr graph_;
    range range_;
    std::size_t tid_;
};

void parallel_cpu_aggr(graph_db_ptr &graph, struct aggr_res &cpu_res, std::size_t threads) {
    // assert(MAX_THREADS >= threads);
    for (auto t = 0; t <  MAX_THREADS; t++) {
        #if defined(COUNT) || defined(AVERAGE)
        cpu_cnt_res[t] = 0;
        #endif

        #if defined(SUM) || defined(AVERAGE)
        cpu_sum_res[t] = 0;
        #endif

        #ifdef AVERAGE
        cpu_avg_res[t] = 0.0;
        #endif

        #ifdef MINIMUM
        cpu_min_res[t] = UNKNOWN_CODE;
        #endif

        #ifdef MAXIMUM
        cpu_max_res[t] = 0;
        #endif
    }

    auto num_chunks = graph->get_nodes()->as_vec().num_chunks();
    auto chunks_per_thread = DIVCEIL(num_chunks, threads);
    PRINT("CPU threads: %lu", threads);
    PRINT("Chunks per thread: %lu", chunks_per_thread);

    std::vector<std::future<void>> res;
    res.reserve(threads);
    thread_pool pool(threads);
    std::size_t start = 0, end = chunks_per_thread - 1;
    for (std::size_t tid = 0; tid < threads; tid++) {
        res.push_back(pool.submit(
            aggr_task(graph, start, end, tid)));
        start = end + 1;
        end += chunks_per_thread;
    }

    for (auto &f : res) {
        f.get();
    }

    PRINT("Collecting CPU results...");
    collect_res(cpu_res, threads, cpu_cnt_res, cpu_sum_res, cpu_avg_res, cpu_min_res, cpu_max_res);
    PRINT("CPU results collected");
}

void sequential_cpu_aggr(struct mrnode* elems_buf, uint32_t num_elems, std::unordered_map<uint32_t, aggr_res> &hash_table) {

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

    uint64_t cmt_cnt = 0;
    uint64_t post_cnt = 0;

    std::random_device rd;
    std::mt19937::result_type seed = rd() ^ (
        (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() +
        (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

    std::mt19937 gen(seed);
    std::uniform_int_distribution<prop_code_t> dist(1, 20);

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
                if (lc == 1 /* Comment */ || lc == 5827195 /* Post */) {
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

                    if (lc == 1) { /* Key: 1 */
                        mrn.properties[prop++] = (prop_code_t)1;
                        cmt_cnt++;
                    }
                    else if (lc == 5827195) {
                        mrn.properties[prop++] = (prop_code_t)0;
                        post_cnt++;
                    }

                    iter = node_desc.properties.find("length");
                    value = iter->second;
                    auto len = any_cast<int>(&value);

                    mrn.properties[prop++] = (prop_code_t)(*len); /* Key: 2 */

                    prop_code_t val = (*len >= 0 && *len < 40) ?
                                   0 :
                                   (*len >= 40 && *len < 80) ?
                                   1 :
                                   (*len >= 80 && *len < 160) ?
                                   2 : 3;
                    mrn.properties[prop++] = val; /* Key: 3 */

                    // mrn.properties[prop++] = dist(gen);

                    graph->commit_transaction();
                }
            // }
        }
        ch++;
        iter++;
    }
}

void dpu_aggregate(graph_db_ptr &graph, uint32_t num_of_dpus, std::string dpu_binary) {
    auto &nodes = graph->get_nodes();

#ifdef PRINTER
    {
        node n;
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

        /* initialize elements */
        assert(num_dpus == NR_DPUS);
        auto num_chunks = nodes->as_vec().num_chunks();
        auto elems_per_chunk = nodes->as_vec().elements_per_chunk();
        uint32_t total_elems = (151043 /* Comment */ + 135701 /* Post */);
        uint32_t elems_per_dpu = DIVCEIL(total_elems, NR_DPUS);
        auto elems_size_per_dpu = ELEM_SIZE * elems_per_dpu;
        bool dpu_overflow = elems_size_per_dpu > MRAM_INPUT_BUFFER_PARTITION;

        PRINT_TOP_RULE;
        PRINT("DPUs: %u", num_dpus);
        PRINT("Ranks: %u", num_ranks);
        PRINT("Chunks: %lu", num_chunks);
        PRINT("Elements per chunk: %u", elems_per_chunk);
        PRINT("Elements: %u", total_elems);
        PRINT("Elements per DPU: %u", elems_per_dpu);
        PRINT("Element Size: %lu", ELEM_SIZE);
        PRINT("Element Size per DPU: %lu (%lu MB)", elems_size_per_dpu, (elems_size_per_dpu / MB));

        if (dpu_overflow) {
            #if 0 /* TODO */
            PRINT_INFO(true, "Max. %d (%d MB) MRAM input buffer exceeded. Adjusting...", MRAM_INPUT_BUFFER_PARTITION, (MRAM_INPUT_BUFFER_PARTITION / MB));
            elems_per_dpu = (MRAM_INPUT_BUFFER_PARTITION / 2) / ELEM_SIZE;
            elems_size_per_dpu = ELEM_SIZE * elems_per_dpu;
            uint32_t dpu_exec_rounds = DIVCEIL(total_elems, (elems_per_dpu * num_dpus));

            PRINT("DPU execution rounds: %u", dpu_exec_rounds);
            PRINT("Elements per DPU: %u", elems_per_dpu);
            PRINT("Element Size per DPU: %lu (%lu MB)", elems_size_per_dpu, (elems_size_per_dpu / MB));

            mrnode* elems_buffer = (mrnode*) malloc (ELEM_SIZE * elems_per_dpu * num_dpus * dpu_exec_rounds);
            initialize_props(graph, elems_buffer);

            aggr_res local_dpu_res[NR_DPUS][dpu_exec_rounds];

            for (uint32_t round = 0; round < dpu_exec_rounds; round++) {
                PRINT("Execution Round %u...", (round + 1));

                if (round == (dpu_exec_rounds - 1)) {
                    /* make a balanced distribution of elements across DPUs */

                    uint32_t remaining_elems = total_elems - (round * NR_DPUS * elems_per_dpu);
                    uint32_t last_round_elems_per_dpu = DIVCEIL(remaining_elems, num_dpus);

                    PRINT("Remaining elements in last DPU execution round: %u", remaining_elems);
                    PRINT("Elements per DPU in last DPU execution round: %u", last_round_elems_per_dpu);

                    /* transfer chunks to DPU */
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
                        params[dpuid].elems = elems;
                        DPU_ASSERT(dpu_prepare_xfer(dpu, &params[dpuid]));
                    }
                    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "dpu_parameters", 0, sizeof(dpu_params), DPU_XFER_DEFAULT));

                }
                else {

                    /* transfer chunks to DPU */
                    PRINT("Transfer input data to DPUs...");
                    t.start("CPU to DPU xfer (input data)");
                    DPU_FOREACH(dpu_set, dpu, dpuid) {
                        DPU_ASSERT(dpu_prepare_xfer(dpu, &elems_buffer[(round * NR_DPUS + dpuid) * elems_per_dpu]));
                    }
                    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, ELEM_SIZE * elems_per_dpu, DPU_XFER_DEFAULT));
                    t.stop();

                    /* transfer DPU parameters to DPU */
                    PRINT("Transfer input parameters to DPUs...");
                    dpu_params param{elems_per_dpu};
                    DPU_ASSERT(dpu_copy_to(dpu_set, "dpu_parameters", 0, (const void*)&param, sizeof(dpu_params)));

                }

                /* launch the aggregate kernel */
                PRINT("Executing aggregate kernel on DPUs...");
                t.start("DPU exec (aggregation kernel)");
                DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
                t.stop();

                /* retrieve local aggregation results from DPUs */
                PRINT("Transfer local results from DPUs...");
                t.start("DPU to CPU transfer (local res)");
                DPU_FOREACH(dpu_set, dpu, dpuid) {
                    DPU_ASSERT(dpu_prepare_xfer(dpu, &local_dpu_res[dpuid][round]));
                }
                DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_results", 0, sizeof(aggr_res), DPU_XFER_DEFAULT));
                t.stop();

                // /* dump DPU logs */
                // PRINT("Dump DPU Logs...");
                // DPU_FOREACH(dpu_set, dpu, dpuid) {
                //     // std::cout << "DPU " << d << "\n";
                //     DPU_ASSERT(dpu_log_read(dpu, stdout));
                // }
            }

            PRINT("Collect local DPU aggregation results...");
            t.start("CPU exec (local aggregation res)");
            for (uint32_t d = 0; d < num_dpus; d++) {
                for (uint32_t r = 0; r < dpu_exec_rounds; r++) {
                    #if defined(COUNT) || defined(AVERAGE)
                    cnt += local_dpu_res[d][r].cnt;
                    #endif

                    #if defined(SUM) || defined(AVERAGE)
                    sum += local_dpu_res[d][r].sum;
                    #endif

                    #ifdef MINIMUM
                    if (min > local_dpu_res[d][r].min) {
                        min = local_dpu_res[d][r].min;
                    }
                    #endif

                    #ifdef MAXIMUM
                    if (max < local_dpu_res[d][r].max) {
                        max = local_dpu_res[d][r].max;
                    }
                    #endif
                }
            }
            #endif
        }
        else {

            mrnode* elems_buffer = (mrnode*) malloc (ELEM_SIZE * elems_per_dpu * NR_DPUS);
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
            dpu_params params[num_dpus];
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

            /* dump DPU logs */
            PRINT("Dump DPU Logs...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                // std::cout << "DPU " << d << "\n";
                DPU_ASSERT(dpu_log_read(dpu, stdout));
            }

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
            mrnode* global_part_buffer = (mrnode*) malloc (ELEM_SIZE * total_elems);
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

                /* transfer the partitions */
                for (uint32_t j = 0; j < partitions.size(); j += 2) {
                    uint32_t p = partitions[j];
                    uint32_t part_size = partitions[j + 1];
                    partition_sizes[i++] = part_size;
                    /* TODO: 1) scatter gather xfer, 2) skip data transpose for byte interleaving */
                    DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(uint32_t) * num_parts + ELEM_SIZE * part_offs, /* we store partition sizes at the beginning of MRAM */
                                           &global_part_buffer[global_prefix_sum[p][0]], ELEM_SIZE * part_size));
                    part_offs += part_size;
                }
                /* send the sizes of the partitions */
                DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME, 0, partition_sizes, sizeof(uint32_t) * num_parts));
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
            htable_entry* hash_tables[NR_DPUS];
            t.start("DPU to CPU transfer (hash tables of global partitions)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                hash_tables[dpuid] = (htable_entry*) malloc(HASH_TABLE_SIZE * num_partitions[dpuid]);
                /* TODO: 1) scatter gather xfer, 2) skip data transpose for byte interleaving */
                DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(uint32_t) * num_partitions[dpuid] + ELEM_SIZE * htable_offsets[dpuid],
                                         hash_tables[dpuid], sizeof(htable_entry) * num_partitions[dpuid] * NR_HASH_TABLE_ENTRIES));
            }
            t.stop();

            PRINT("Collect aggregation results of global partitions...");
            t.start("CPU exec (aggregation results of global partitions)");
            std::unordered_map<uint32_t, aggr_res> dpu_res;
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

            /* execute baseline aggregation on CPU */
            PRINT("Execute aggregates baselines on CPU...");
            /*
            // std::size_t threads = 4;
            std::size_t threads = std::thread::hardware_concurrency();
            aggr_res seq_cpu_res, par_cpu_res;
            t.start("Parallel CPU exec (aggregation kernel)");
            parallel_cpu_aggr(graph, par_cpu_res, threads);
            t.stop();
            */

            /* validate CPU and DPU results */
            PRINT("Validate CPU baseline results...");
            t.start("Sequential CPU exec (aggregation kernel)");
            std::unordered_map<uint32_t, aggr_res> seq_cpu_res;
            sequential_cpu_aggr(elems_buffer, total_elems, seq_cpu_res);
            t.stop();
            // validate_aggr(seq_cpu_res, par_cpu_res);

            PRINT("Validate DPU results...");
            validate_aggr(seq_cpu_res, dpu_res);
        }

        /* free DPUs */
        PRINT("Free DPUs...");
        DPU_ASSERT(dpu_free(dpu_set));

        PRINT_TOP_RULE;
        t.print();
        auto mark = std::to_string(NR_DPUS) + "DPUS_SF10";
        t.print_to_csv(CSV_FILE, mark);
    }
    catch (const dpu::DpuError &e) {
        std::cerr << e.what() << std::endl;
    }
}