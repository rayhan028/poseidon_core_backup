#include <random>
#include "aggregation.hpp"


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
    #ifdef SF0_1
    uint32_t post_code = 5827195; /* SF 0.1 */
    #elif defined SF1
    uint32_t post_code = 67111801; /* SF 1 */
    #elif defined SF10
    uint32_t post_code = 692931280; /* SF 10 */
    #endif

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
                if (lc == cmt_code /* Comment */ || lc == post_code /* Post */) {
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

                    #ifdef LOW_CARDINALITY

                    prop_code_t val = (*len >= 0 && *len < 40) ?
                                      0 :
                                      (*len >= 40 && *len < 80) ?
                                      1 :
                                      (*len >= 80 && *len < 160) ?
                                      2 : 3;
                    mrn.properties[prop++] = val % GROUP_KEY_CARDINALITY; /* Key: 3 */

                    #else /* #ifdef LOW_CARDINALITY */

                    #if 1
                    iter = node_desc.properties.find("id");
                    value = iter->second;
                    auto id = any_cast<uint64_t>(&value);
                    mrn.properties[prop++] = (prop_code_t)(*id % GROUP_KEY_CARDINALITY); /* Key: 3 */
                    #else
                    mrn.properties[prop++] = dist(gen);
                    #endif

                    #endif /* #ifdef LOW_CARDINALITY */

                    graph->commit_transaction();
                }
            // }
        }
        ch++;
        iter++;
    }
}
