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
    auto end = nodes->as_vec().chunk_list_end();
    auto elems_per_chunk = nodes->as_vec().elements_per_chunk();
    auto last_offs = nodes->as_vec().last_used();

    uint32_t ch = 0;
    uint64_t offs = 0;
    while (iter != end) {
        for (unsigned int i = 0; i < elems_per_chunk; i++) {
            offs = ch * elems_per_chunk + i;
            // if ((*iter)->is_used(i)) { // TODO: implement on DPU program
                if (offs > last_offs) {
                    break;
                }

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

void validate_chunks(graph_db_ptr &graph, struct mrchunk* chunks) {

    auto &nodes = graph->get_nodes();
    auto iter_beg = nodes->as_vec().chunk_list_begin();
    auto iter_end = nodes->as_vec().chunk_list_end();
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
                mrnode n1, n2;
                memcpy(&n1, &chunk.data[j], sizeof(mrnode));
                memcpy(&n2, &chunks[i].data[j], sizeof(mrnode));
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

void initialize_props(graph_db_ptr &graph, struct mrnode* nodes_ptr) {

    auto &nodes = graph->get_nodes();
    auto iter = nodes->as_vec().chunk_list_begin();
    auto end = nodes->as_vec().chunk_list_end();
    auto elems_per_chunk = nodes->as_vec().elements_per_chunk();
    auto last_offs = nodes->as_vec().last_used();

    uint32_t ch = 0;
    uint32_t offs = 0;
    bool equal = true;
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
                struct mrnode &mrn = nodes_ptr[offs];
                memcpy(&mrn, &n, sizeof(node));

                {
                    auto prop = 0;
                    graph->begin_transaction();
                    auto node_desc = graph->get_node_description(nid);
                    for (auto &[key, value] : node_desc.properties) {
                        // std::cout << key << " : " << value << "\n";
                        const std::type_info &ti = value.type();
                        if (ti == typeid(std::string)) {
                            auto str = any_cast<std::string>(&value);
                            auto c = graph->get_code(*str);
                            mrn.properties[prop++] = c;
                        }
                        else if (ti == typeid(uint64_t)) {
                            auto val = any_cast<uint64_t>(&value);
                            mrn.properties[prop++] = *val;
                        }
                        else if (ti == typeid(boost::posix_time::ptime)) {
                            auto dt = any_cast<boost::posix_time::ptime>(&value);
                            boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
                            uint64_t secs = (uint64_t)(*dt - epoch).total_seconds();
                            mrn.properties[prop++] = secs;
                        }
                        else if (ti == typeid(int)) {
                            auto val = any_cast<int>(&value);
                            mrn.properties[prop++] = *val;
                        }
                        else if (ti == typeid(double)) {
                            auto val = any_cast<double>(&value);
                            double d = *val;
                            memcpy(&mrn.properties[prop++], &d, sizeof(double));
                        }
                        else {
                            mrn.properties[prop++] = UNKNOWN_CODE;
                        }
                    }
                    graph->commit_transaction();
                }

                bool f1 = nid != mrn.id_;
                bool f2 = n.from_rship_list != mrn.from_rship_list;
                bool f3 = n.to_rship_list != mrn.to_rship_list;
                bool f4 = n.property_list != mrn.property_list;
                bool f5 = n.node_label != mrn.node_label;
                if (f1 || f2 || f3 || f4 || f5) {
                    equal = false;
                    #ifdef PRINTER
                    PRINT("Mismatch in Node: %lu : %lu", nid, mrn.id_);
                    PRINT_INFO(true, "Printing node %lu : %lu", nid, mrn.id_);
                    PRINT("id_: %lu : %lu", n, mrn.id_);
                    PRINT("from_rship_list: %lu : %lu", n.from_rship_list, mrn.from_rship_list);
                    PRINT("to_rship_list: %lu : %lu", n.to_rship_list, mrn.to_rship_list);
                    PRINT("property_list: %lu : %lu", n.property_list, mrn.property_list);
                    PRINT("node_label: %u : %u", n.node_label, mrn.node_label);
                    #endif
                }
            // }
        }
        ch++;
        iter++;
    }

#ifdef PRINTER
    if (equal) {
        PRINT_TOP_RULE;
        PRINT_INFO(true, "Chunks are equal");
    }
    else {
        PRINT_ERROR("Chunks are not equal");
    }
#endif
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
        uint64_t total_elems = nodes->as_vec().last_used() + 1;
        uint32_t elems_per_dpu = DIVCEIL(total_elems, num_dpus);
        auto elems_size_per_dpu = sizeof(mrnode) * elems_per_dpu;
        bool dpu_overflow = elems_size_per_dpu > MRAM_INPUT_BUFFER;

        PRINT_TOP_RULE;
        PRINT("DPUs: %u", num_dpus);
        PRINT("Ranks: %u", num_ranks);
        PRINT("Chunks: %lu", num_chunks);
        PRINT("Elements per chunk: %u", elems_per_chunk);
        PRINT("Elements: %lu", total_elems);
        PRINT("Elements per DPU: %u", elems_per_dpu);
        PRINT("Element Size: %lu", sizeof(mrnode));
        PRINT("Element Size per DPU: %lu (%lu MB)", elems_size_per_dpu, (elems_size_per_dpu / MB));

        struct aggr_res dpu_res;
        uint64_t cnt = 0;
        uint64_t sum = 0;
        double avg = 0.0;
        uint64_t min = 18446744073709551615ul;
        uint64_t max = 0;

        if (dpu_overflow) {
            PRINT_INFO(true, "Max. %d (%d MB) MRAM input buffer exceeded. Adjusting...", MRAM_INPUT_BUFFER, (MRAM_INPUT_BUFFER / MB));
            elems_per_dpu = MRAM_INPUT_BUFFER / sizeof(mrnode);
            elems_size_per_dpu = sizeof(mrnode) * elems_per_dpu;
            uint32_t dpu_exec_rounds = DIVCEIL(total_elems, (elems_per_dpu * num_dpus));

            PRINT("DPU execution rounds: %u", dpu_exec_rounds);
            PRINT("Elements per DPU: %u", elems_per_dpu);
            PRINT("Element Size per DPU: %lu (%lu MB)", elems_size_per_dpu, (elems_size_per_dpu / MB));

            mrnode* elems_buffer = (mrnode*) malloc (sizeof(mrnode) * elems_per_dpu * num_dpus * dpu_exec_rounds);
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
                    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, sizeof(mrnode) * elems_per_dpu, DPU_XFER_DEFAULT));
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
                    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, sizeof(mrnode) * elems_per_dpu, DPU_XFER_DEFAULT));
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
        }
        else {

            mrnode* elems_buffer = (mrnode*) malloc (sizeof(mrnode) * elems_per_dpu * num_dpus);
            initialize_props(graph, elems_buffer);

            /* transfer chunks to DPU */
            PRINT("Transfer input data to DPUs...");
            t.start("CPU to DPU xfer (input data)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &elems_buffer[dpuid * elems_per_dpu]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, sizeof(mrnode) * elems_per_dpu, DPU_XFER_DEFAULT));
            t.stop();

            /* transfer DPU parameters to DPU */
            PRINT("Transfer input parameters to DPUs...");
            dpu_params params[num_dpus];
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t elems = (dpuid == (NR_DPUS - 1)) ?
                                (total_elems - dpuid * elems_per_dpu) :
                                elems_per_dpu;
                params[dpuid].elems = elems;
                DPU_ASSERT(dpu_prepare_xfer(dpu, &params[dpuid]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "dpu_parameters", 0, sizeof(dpu_params), DPU_XFER_DEFAULT));

            /* launch the aggregate kernel */
            PRINT("Executing aggregate kernel on DPUs...");
            t.start("DPU exec (aggregation kernel)");
            DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
            t.stop();

            /* retrieve local aggregation results from DPUs */
            PRINT("Transfer local results from DPUs...");
            aggr_res local_dpu_res[num_dpus];
            t.start("DPU to CPU transfer (local res)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &local_dpu_res[dpuid]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "dpu_results", 0, sizeof(aggr_res), DPU_XFER_DEFAULT));
            t.stop();

            // /* dump DPU logs */
            // PRINT("Dump DPU Logs...");
            // DPU_FOREACH(dpu_set, dpu, dpuid) {
            //     // std::cout << "DPU " << d << "\n";
            //     DPU_ASSERT(dpu_log_read(dpu, stdout));
            // }

            PRINT("Collect local DPU aggregation results...");
            t.start("CPU exec (local aggregation res)");
            for (uint32_t d = 0; d < num_dpus; d++) {
                #if defined(COUNT) || defined(AVERAGE)
                cnt += local_dpu_res[d].cnt;
                #endif

                #if defined(SUM) || defined(AVERAGE)
                sum += local_dpu_res[d].sum;
                #endif

                #ifdef MINIMUM
                if (min > local_dpu_res[d].min) {
                    min = local_dpu_res[d].min;
                }
                #endif

                #ifdef MAXIMUM
                if (max < local_dpu_res[d].max) {
                    max = local_dpu_res[d].max;
                }
                #endif
            }
        }

        #if defined(COUNT) || defined(AVERAGE) /* TODO: optimize */
        dpu_res.cnt = cnt;
        #endif
        #if defined(SUM) || defined(AVERAGE)
        dpu_res.sum = sum;
        #endif
        #ifdef AVERAGE
        if (cnt > 0) {
            avg = sum / (double)cnt;
        }
        dpu_res.avg = avg;
        #endif
        #ifdef MINIMUM
        dpu_res.min = min;
        #endif
        #ifdef MAXIMUM
        dpu_res.max = max;
        #endif
        t.stop();

        /* execute baseline aggregation on CPU */
        PRINT("Execute aggregates baselines on CPU...");
        // std::size_t threads = 4;
        std::size_t threads = std::thread::hardware_concurrency();
        aggr_res seq_cpu_res, par_cpu_res;
        t.start("Parallel CPU exec (aggregation kernel)");
        parallel_cpu_aggr(graph, par_cpu_res, threads);
        t.stop();

        /* validate CPU and DPU results */
        PRINT("Validate CPU baseline results...");
        t.start("Sequential CPU exec (aggregation kernel)");
        sequential_cpu_aggr(graph, seq_cpu_res);
        t.stop();
        validate_aggr(seq_cpu_res, par_cpu_res);

        PRINT("Validate DPU results...");
        validate_aggr(seq_cpu_res, dpu_res);

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