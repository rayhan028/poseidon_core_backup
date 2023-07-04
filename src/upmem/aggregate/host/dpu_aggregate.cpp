#include "graph_db.hpp"
#include "thread_pool.hpp"
#include <dpu>
#include <dpu_log.h>

#include "../common/timer.hpp"
#include "../common/common.h"

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
    uint64_t min = 18446744073709551615ul;
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

void validate_aggr(const struct aggr_res &lhs, const struct aggr_res &rhs) {
    bool equal = true;
    if (lhs.cnt != rhs.cnt) { // TODO: use bits of a config variable to represent which aggregates to validate
        equal = false;
        PRINT_ERROR("\"cnt\" is not equal");
    }
    if (lhs.sum != rhs.sum) {
        equal = false;
        PRINT_ERROR("\"sum\" is not equal");
    }
    if (lhs.avg != rhs.avg) {
        equal = false;
        PRINT_ERROR("\"avg\" is not equal");
    }
    if (lhs.min != rhs.min) {
        equal = false;
        PRINT_ERROR("\"min\" is not equal");
    }
    if (lhs.max != rhs.max) {
        equal = false;
        PRINT_ERROR("\"max\" is not equal");
    }

    if (equal) {
        PRINT_TOP_RULE;
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
        auto iter = graph->get_nodes()->range(first, last);
        while (iter) {
            auto &n = *iter;
            uint64_t nid = n.id();

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
        cpu_min_res[t] = 18446744073709551615ul;
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

void sequential_cpu_aggr(graph_db_ptr &graph, struct aggr_res &cpu_res) {
    uint64_t cnt = 0;
    uint64_t sum = 0;
    double avg = 0.0;
    uint64_t min = 18446744073709551615ul;
    uint64_t max = 0;

    auto &nodes = graph->get_nodes();
    auto iter = nodes->as_vec().chunk_list_begin();
    auto end =nodes->as_vec().chunk_list_end();
    auto elems_per_chunk = nodes->as_vec().elements_per_chunk();

    auto ch = 0;
    while (iter != end) {
        for (unsigned int i = 0; i < elems_per_chunk; i++) {
            // if ((*iter)->is_used(i)) { // TODO: implement on DPU program
                // uint64_t offs = ch * elems_per_chunk + i;
                auto &n = (*iter)->data_[i];
                uint64_t nid = n.id();
                // assert(nid == offs);

                #if defined(COUNT) || defined(AVERAGE)
                cnt++;
                #endif

                #if defined(SUM) || defined(AVERAGE)
                sum += nid;
                #endif

                #ifdef MINIMUM
                if (nid < min) {
                    min = nid;
                }
                #endif

                #ifdef MAXIMUM
                if (nid > max) {
                    max = nid;
                }
                #endif
            // }
        }
        iter++;
        ch++;
    }

    #if defined(COUNT) || defined(AVERAGE)
    cpu_res.cnt = cnt;
    #endif
    #if defined(SUM) || defined(AVERAGE)
    cpu_res.sum = sum;
    #endif
    #ifdef AVERAGE
    if (cnt > 0) {
        avg = sum / (double)cnt;
    }
    cpu_res.avg = avg;
    #endif
    #ifdef MINIMUM
    cpu_res.min = min;
    #endif
    #ifdef MAXIMUM
    cpu_res.max = max;
    #endif
}

struct mrchunk* chunks_from_mram_heap(dpu_set_t &dpu_set, std::size_t chunks_per_dpu, uint32_t num_dpus) {
    // retrieve chunks from MRAM
    uint32_t d = 0;
    dpu_set_t dpu;
    struct mrchunk* chunks = (struct mrchunk*) malloc(chunks_per_dpu * num_dpus * sizeof(mrchunk));
    DPU_FOREACH(dpu_set, dpu, d) {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &chunks[d * chunks_per_dpu]));
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, chunks_per_dpu * sizeof(mrchunk), DPU_XFER_DEFAULT));

    return chunks;
}

// struct mrchunk* chunks_from_mram_host(dpu_set_t &dpu_set, std::size_t chunks_per_dpu, uint32_t num_dpus) {
//     // retrieve chunks from MRAM
//     uint32_t d = 0;
//     dpu_set_t dpu;
//     struct mrchunk* chunks = (struct mrchunk*) malloc(chunks_per_dpu * num_dpus * sizeof(mrchunk));
//     DPU_FOREACH(dpu_set, dpu, d) {
//         DPU_ASSERT(dpu_prepare_xfer(dpu, &chunks[d * chunks_per_dpu]));
//     }
//     DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, chunks_per_dpu * sizeof(mrchunk), DPU_XFER_DEFAULT));

//     return chunks;
// }

void validate_chunks(graph_db_ptr &graph, struct mrchunk* chunks) {

    auto &nodes = graph->get_nodes();
    auto iter_beg = nodes->as_vec().chunk_list_begin();
    auto iter_end =nodes->as_vec().chunk_list_end();
    auto elems_per_chunk = nodes->as_vec().elements_per_chunk();

    uint32_t i = 0;
    bool equal = true;
    auto iter = iter_beg;
    while (iter != iter_end) {
        struct mrchunk chunk;
        memcpy(&chunk, *iter, sizeof(mrchunk));
        for (std::size_t j = 0; j < elems_per_chunk; j++) {
        // for (std::size_t j = 0; j < 7; j++) {
            // if ((*iter)->is_used((i * elems_per_chunk) + j)) {
            if ((*iter)->is_used(j)) {
                struct mr_node n1, n2;
                memcpy(&n1, &chunk.data[j], sizeof(mr_node));
                memcpy(&n2, &chunks[i].data[j], sizeof(mr_node));
                bool f1 = n1.id_ != n2.id_;
                bool f2 = n1.from_rship_list != n2.from_rship_list;
                bool f3 = n1.to_rship_list != n2.to_rship_list;
                bool f4 = n1.property_list != n2.property_list;
                bool f5 = n1.node_label != n2.node_label;
                // TODO: n1.dummy_ == n2.dummy_
                if (f1 || f2 || f3 || f4 || f5) {
                    equal = false;
                    #ifdef PRINTER
                    PRINT("Mismatch in Node: %lu", n2.id_);
                    #endif
                }
                #ifdef PRINTER
                PRINT_INFO(true, "Printing node %lu---", n2.id_);
                PRINT("id_: %lu", n2.id_);
                PRINT("from_rship_list: %lu", n2.from_rship_list);
                PRINT("to_rship_list: %lu", n2.to_rship_list);
                PRINT("property_list: %lu", n2.property_list);
                PRINT("node_label: %u", n2.node_label);
                #endif
            }
        }
        i++;
        iter++;
    }

    if (equal) {
        PRINT_TOP_RULE;
        PRINT_INFO(true, "Chunks are equal");
    }
    else {
        PRINT_ERROR("Chunks are not equal");
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
        struct dpu_set_t dpu_set, dpu;
        uint32_t num_ranks, num_dpus;
        uint32_t dpuid = 0;
        timer t;

        // allocate DPUs and load binary 
        DPU_ASSERT(dpu_alloc(num_of_dpus, NULL, &dpu_set));
        DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &num_dpus));
        DPU_ASSERT(dpu_get_nr_ranks(dpu_set, &num_ranks));
        DPU_ASSERT(dpu_load(dpu_set, dpu_binary.c_str(), NULL));

        auto real_chunk_size = nodes->as_vec().real_chunk_size();
        auto num_chunks = nodes->as_vec().num_chunks();
        uint32_t chunks_per_dpu = DIVCEIL(num_chunks, num_dpus); // TODO handle corner case in last dpu

        PRINT_TOP_RULE;
        PRINT("DPUs: %u", num_dpus);
        PRINT("Ranks: %u", num_ranks);
        PRINT("Chunks: %lu", num_chunks);
        PRINT("Chunks per DPU: %u", chunks_per_dpu);

#if 1
        // transfer node chunks to MRAM
        auto iter_beg = nodes->as_vec().chunk_list_begin();
        auto iter_end =nodes->as_vec().chunk_list_end();
        auto iter = iter_beg;

        LOG_MSG("Loading input data to DPUs...\n");
        uint64_t dpu_chunk_cnt = 0;
        uint64_t total_sent_chunks = 0;
        std::map<uint32_t, uint32_t> dpuid_chunks;
        t.start("Loading input data to DPU");
        while (iter != iter_end && dpu_chunk_cnt < MAX_CHUNKS_PER_DPU) { // TODO: when total chunks do not fit in DPUs
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, *iter));
                if (dpuid_chunks.find(dpuid) == dpuid_chunks.end()) {
                    dpuid_chunks[dpuid] = 0;
                }
                dpuid_chunks[dpuid]++;
                total_sent_chunks++;
                iter++;
                if (iter == iter_end) {
                    break;                
                }
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "mr_chunks", dpu_chunk_cnt * real_chunk_size, real_chunk_size, DPU_XFER_DEFAULT));
            dpu_chunk_cnt++;
        }
        t.stop();
        LOG_MSG("Input data loaded\n");

        bool all_chunks_processed = (iter == iter_end) && (total_sent_chunks == num_chunks); // continue after this execution round
        LOG_MSG((all_chunks_processed ? "All chunks processed\n" : "Unprocessed chunks remaining\n"));

        std::cout << "Chunks sent per DPU: " << dpu_chunk_cnt << "\n";
        std::cout << "Total chunks sent : " << total_sent_chunks << "\n";
        std::cout << "Remaining chunks : " << (num_chunks - total_sent_chunks) << "\n";

        // transfer input parameters
        LOG_MSG("Loading input parameters to DPUs...\n");
        t.start("Loading DPU input parameters");
        dpuid = 0;
        struct dpu_params params[num_dpus];
        std::cout << "params[i].chunks:\n";
        for (unsigned int i = 0; i < num_dpus; i++) {
            std::cout << params[i].chunks << (i == (num_dpus-1) ? " " : ", ");
        }
        std::cout << "\n";
        DPU_FOREACH(dpu_set, dpu, dpuid) {
            std::cout << "dpuid:" << dpuid << "\n";
            params[dpuid].chunks = dpuid_chunks[dpuid];
            DPU_ASSERT(dpu_prepare_xfer(dpu, &params[dpuid]));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "dpu_args", 0, sizeof(dpu_params), DPU_XFER_DEFAULT));
        t.stop();
        LOG_MSG("Input parameters loaded\n");
        std::cout << "params[i].chunks:\n";
        for (unsigned int i = 0; i < num_dpus; i++) {
            std::cout << params[i].chunks << (i == (num_dpus - 1) ? " " : ", ");
        }
        std::cout << "\n";

#else
        // transfer node chunks to MRAM
        auto iter_beg = nodes->as_vec().chunk_list_begin();
        auto iter_end =nodes->as_vec().chunk_list_end();
        auto iter = iter_beg;

        std::cout << "Loading input data to DPUs...\n";
        DPU_FOREACH(dpu_set, dpu) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, *iter));

            uint32_t c = chunks_per_dpu;
            while (c > 0 && iter != iter_end) {
                c--;
                iter++;
            }
            if (iter == iter_end) {
                break;                
            }
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, chunks_per_dpu * sizeof(mrchunk), DPU_XFER_DEFAULT));
        std::cout << "Input data loaded\n";

        // retrieve chunks from MRAM
        struct mrchunk* chunks = chunks_from_mram_heap(dpu_set, chunks_per_dpu, num_dpus);

        std::cout << "Validating transferred chunks...\n";
        validate_chunks(graph, chunks);
        std::cout << "Chunks validation complete\n";

        #ifdef PRINTER
        // print_dpu_chunks(dpu_set, chunks_per_dpu, num_dpus);
        #endif

#endif
        // launch the aggregate kernel
        LOG_MSG("Executing program on DPUs...\n");
        t.start("DPU execution");
        DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
        t.stop();
        LOG_MSG("Program execution complete\n");

        // retrieve results from DPUs
        LOG_MSG("Retrieving DPU results...\n");
        uint64_t cnt_vals[num_dpus];
        uint64_t sum_vals[num_dpus];
        double avg_vals[num_dpus];
        uint64_t min_vals[num_dpus];
        uint64_t max_vals[num_dpus];

        t.start("DPU to CPU transfer");
        dpuid = 0;
#if defined(COUNT) || defined(AVERAGE)
        DPU_FOREACH(dpu_set, dpu, dpuid) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &cnt_vals[dpuid]));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_cnt_val", 0, sizeof(uint64_t), DPU_XFER_DEFAULT));
#endif

#if defined(SUM) || defined(AVERAGE)
        dpuid = 0;
        DPU_FOREACH(dpu_set, dpu, dpuid) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &sum_vals[dpuid]));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_sum_val", 0, sizeof(uint64_t), DPU_XFER_DEFAULT));
#endif

#ifdef AVERAGE
        dpuid = 0;
        DPU_FOREACH(dpu_set, dpu, dpuid) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &avg_vals[dpuid]));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_avg_val", 0, sizeof(double), DPU_XFER_DEFAULT));
#endif

#ifdef MINIMUM
        dpuid = 0;
        DPU_FOREACH(dpu_set, dpu, dpuid) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &min_vals[dpuid]));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_min_val", 0, sizeof(uint64_t), DPU_XFER_DEFAULT));
#endif

#ifdef MAXIMUM
        dpuid = 0;
        DPU_FOREACH(dpu_set, dpu, dpuid) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &max_vals[dpuid]));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_max_val", 0, sizeof(uint64_t), DPU_XFER_DEFAULT));
#endif
        t.stop();
        PRINT("DPU results retrieved");

        // dump DPU logs
        PRINT_TOP_RULE;
        PRINT("Displaying DPU Logs...");
        dpuid = 0;
        t.start("Print DPU");
        DPU_FOREACH(dpu_set, dpu, dpuid) {
            // std::cout << "DPU " << d << "\n";
            DPU_ASSERT(dpu_log_read(dpu, stdout));
        }
        t.stop();
        PRINT("DPU logs display complete");

        PRINT_TOP_RULE;
        PRINT("Collecting local DPU results...");
        struct aggr_res dpu_res;
        t.start("Collect local DPU results");
        collect_res(dpu_res, num_of_dpus, cnt_vals, sum_vals, avg_vals, min_vals, max_vals);
        t.stop();
        PRINT("Results collection complete");

        PRINT("Executing aggregates on CPU...");
        // std::size_t threads = 4;
        std::size_t threads = std::thread::hardware_concurrency();
        struct aggr_res seq_cpu_res, par_cpu_res;
        t.start("CPU execution");
        parallel_cpu_aggr(graph, par_cpu_res, threads);
        t.stop();
        PRINT("CPU execution complete");

        LOG_MSG("Validating CPU results...\n");
        sequential_cpu_aggr(graph, seq_cpu_res);
        t.start("CPU Results Validation");
        validate_aggr(seq_cpu_res, par_cpu_res);
        t.stop();
        LOG_MSG("Results validation complete\n");

        LOG_MSG("Validating DPU results...\n");
        t.start("DPU Results Validation");
        validate_aggr(seq_cpu_res, dpu_res);
        t.stop();
        LOG_MSG("Results validation complete\n");

        #ifdef PRINTER
        PRINT("\"cnt\", \"sum\", \"avg\", \"min\", \"max\" : %lu, %lu, %f, %lu, %lu",
        seq_cpu_res.cnt, seq_cpu_res.sum, seq_cpu_res.avg, seq_cpu_res.min, seq_cpu_res.max);
        PRINT("\"cnt\", \"sum\", \"avg\", \"min\", \"max\" : %lu, %lu, %f, %lu, %lu",
        par_cpu_res.cnt, par_cpu_res.sum, par_cpu_res.avg, par_cpu_res.min, par_cpu_res.max);
        PRINT("\"cnt\", \"sum\", \"avg\", \"min\", \"max\" : %lu, %lu, %f, %lu, %lu",
        dpu_res.cnt, dpu_res.sum, dpu_res.avg, dpu_res.min, dpu_res.max);
        #endif

        LOG_MSG("Freeing allocated DPUs...\n");
        t.start("dpu_free");
        DPU_ASSERT(dpu_free(dpu_set));
        t.stop();
        LOG_MSG("DPU free complete\n");

        PRINT_TOP_RULE;
        t.print();
    }
    catch (const dpu::DpuError &e) {
        std::cerr << e.what() << std::endl;
    }

}