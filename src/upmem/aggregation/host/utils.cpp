#include <random>
#include "aggregation.hpp"


bool get_partition_func(struct sg_block_info* cpu_buffer, uint32_t d, uint32_t p, void* params) {
    sg_partition_xfer_ctx* sc_params = (sg_partition_xfer_ctx*) params;
    uint32_t* parts = sc_params->num_partitions;
    uint32_t** part_sizes = sc_params->partition_sizes;
    mrnode*** part_ptrs = sc_params->partition_ptrs;

    if (p >= parts[d]) { /* number of partitions on the DPU exceeded */
        return false;
    }

    cpu_buffer->addr = (uint8_t*) part_ptrs[d][p];
    cpu_buffer->length = part_sizes[d][p] * ELEM_SIZE;

    return true;
}

bool get_hash_table_func(struct sg_block_info* cpu_buffer, uint32_t d, uint32_t h, void* params) {
    sg_hash_table_xfer_ctx* sc_params = (sg_hash_table_xfer_ctx*) params;
    uint32_t* htables = sc_params->num_hash_tables;
    uint32_t** htable_sizes = sc_params->hash_table_sizes;
    htable_entry*** htable_ptrs = sc_params->hash_table_ptrs;

    if (h >= htables[d]) { /* number of hash tables on the DPU exceeded */
        return false;
    }

    cpu_buffer->addr = (uint8_t*) htable_ptrs[d][h];
    cpu_buffer->length = htable_sizes[d][h] * HASH_TABLE_ENTRY_SIZE;

    return true;
}

void* multidim_malloc(uint32_t rows, uint32_t* cols, uint32_t cell_size) {
    void** matrix = (void**) malloc(rows * sizeof(void*));
    for (uint32_t i = 0; i < rows; i++) {
        matrix[i] = malloc(cols[i] * cell_size);
    }

    return matrix;
}

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

            #if defined(COUNT) || defined(AVERAGE)
            if (lhs_res.cnt != rhs_res.cnt) {
                equal = false;
                PRINT_ERROR("\"cnt\" of group with key %u is not equal", lhs_key);
            }
            #endif

            #if defined(SUM) || defined(AVERAGE)
            if (lhs_res.sum != rhs_res.sum) {
                equal = false;
                PRINT_ERROR("\"sum\" of group with key %u is not equal", lhs_key);
            }
            #endif

            #ifdef AVERAGE
            if (lhs_res.avg != rhs_res.avg) {
                equal = false;
                PRINT_ERROR("\"avg\" of group with key %u is not equal", lhs_key);
            }
            #endif

            #ifdef MINIMUM
            if (lhs_res.min != rhs_res.min) {
                equal = false;
                PRINT_ERROR("\"min\" of group with key %u is not equal", lhs_key);
            }
            #endif

            #ifdef MAXIMUM
            if (lhs_res.max != rhs_res.max) {
                equal = false;
                PRINT_ERROR("\"max\" of group with key %u is not equal", lhs_key);
            }
            #endif
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

void parallel_grpkey_proj(mrnode* elems_buf, uint32_t num_elems, prop_code_t* grpkey_buf) {

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
            proj_task(elems_buf, grpkey_buf, start, end, tid)));
        start = end;
        end += (tid == (NR_CPU_THREADS - 2)) ?
               (num_elems - end) :
               elems_per_thread;
    }

    for (auto &f : res) {
        f.get();
    }
}

void parallel_elem_cpy(mrnode* elems_buf, uint32_t num_elems, mrnode* partitions_buf, uint32_t* prefix_sum, uint32_t* count) {

    uint32_t elems_per_thread = DIVCEIL(num_elems, NR_CPU_THREADS);
    PRINT("CPU threads: %d", NR_CPU_THREADS);
    PRINT("Total elements: %d", num_elems);
    PRINT("Elements per thread: %u", elems_per_thread);

    std::mutex mutexes[NR_PARTITIONS];
    std::vector<std::future<void>> res;
    res.reserve(NR_CPU_THREADS);
    thread_pool pool(NR_CPU_THREADS);
    uint32_t start = 0, end = elems_per_thread;
    for (uint32_t tid = 0; tid < NR_CPU_THREADS; tid++) {
        res.push_back(pool.submit(
            elemcpy_task(elems_buf, partitions_buf, prefix_sum, count, mutexes, start, end, tid)));
        start = end;
        end += (tid == (NR_CPU_THREADS - 2)) ?
               (num_elems - end) :
               elems_per_thread;
    }

    for (auto &f : res) {
        f.get();
    }
}

void parallel_gpartition_cpy(elem_t* global_parts, elem_t* tmp_global_parts, uint32_t** local_part_sizes, uint32_t** global_prefix, uint32_t* tmp_global_prefix) {

    uint32_t num_chunks = NR_DPUS;
    uint32_t chunks_per_thread_1 = num_chunks / NR_CPU_THREADS;
    uint32_t lap = num_chunks % NR_CPU_THREADS;
    uint32_t chunks_per_thread_2 = DIVCEIL(num_chunks, NR_CPU_THREADS);
    PRINT("CPU threads: %d", NR_CPU_THREADS);
    PRINT("Total local partition chunks: %d", num_chunks);
    PRINT("Local partition chunks per thread 1: %u", chunks_per_thread_1);
    PRINT("Local partition chunks per thread 2: %u", chunks_per_thread_2);

    std::vector<std::future<void>> res;
    res.reserve(NR_CPU_THREADS);
    thread_pool pool(NR_CPU_THREADS);
    uint32_t curr = 0;
    for (uint32_t tid = 0; tid < NR_CPU_THREADS; tid++) {
        uint32_t start = curr ;
        uint32_t end = start + chunks_per_thread_1;
        if (tid <  lap) {
            end = start + chunks_per_thread_2;
        }

        res.push_back(pool.submit(
            gpart_copy_task(global_parts, tmp_global_parts, local_part_sizes, global_prefix, tmp_global_prefix, start, end, tid)));

        curr = end;
    }

    for (auto &f : res) {
        f.get();
    }
}

#ifdef MULTIVAL_DATA

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

            #if defined(COUNT) || defined(AVERAGE)
            aggr.cnt = 1;
            #endif

            #if defined(SUM) || defined(AVERAGE)
            aggr.sum = elem.properties[SUM_KEY];
            #endif

            #ifdef MINIMUM
            aggr.min = (uint32_t)(-1);
            if (elem.properties[MIN_KEY] < aggr.min) {
                aggr.min = elem.properties[MIN_KEY];
            }
            #endif

            #ifdef MAXIMUM
            aggr.max = 0;
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

void initialize_props(graph_db_ptr &graph, elem_t* nodes_ptr) {

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

#ifdef UNIFORM_DISTRIBUTION
    std::random_device rd;
    std::mt19937::result_type seed = rd() ^ (
        (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() +
        (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

    std::mt19937 uniform_gen(seed);
    std::uniform_int_distribution<prop_code_t> uniform_dist(0, (GROUP_KEY_CARDINALITY - 1)); /* [x, y] */
#elif defined(ZIPFIAN_DISTRIBUTION)
    std::default_random_engine zipf_gen;
    zipfian_int_distribution<prop_code_t> zipf_dist(0, (GROUP_KEY_CARDINALITY - 1), 0.7); /* [x, y] */
#endif

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

                    #ifdef HASH_BASED_LOW_CARDINALITY

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
                    #elif defined(UNIFORM_DISTRIBUTION)
                    prop_code_t uniform_val = uniform_dist(uniform_gen);
                    mrn.properties[prop++] = uniform_val;
                    #elif defined(ZIPFIAN_DISTRIBUTION)
                    prop_code_t zipf_val = zipf_dist(zipf_gen);
                    mrn.properties[prop++] = zipf_val;
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

#elif defined(KV_DATA)

void sequential_cpu_aggr(mrnode* elems_buf, uint32_t num_elems, std::unordered_map<uint32_t, aggr_res> &hash_table) {

    for (uint32_t e = 0; e < num_elems; e++) {
        mrnode &elem = elems_buf[e];
        uint32_t group_key = elem.key;
        uint32_t aggr_value = elem.val;
        // uint32_t hash = aggr_hash(group_key) % NR_HASH_TABLE_ENTRIES;
        uint32_t hash = group_key;

        if (hash_table.find(hash) != hash_table.end()) {
            auto &aggr = hash_table[hash];

            #if defined(COUNT) || defined(AVERAGE)
            aggr.cnt++;
            #endif

            #if defined(SUM) || defined(AVERAGE)
            aggr.sum += aggr_value;
            #endif

            #ifdef MINIMUM
            if (aggr_value < aggr.min) {
                aggr.min = aggr_value;
            }
            #endif

            #ifdef MAXIMUM
            if (aggr_value > aggr.max) {
                aggr.max = aggr_value;
            }
            #endif
        }
        else {
            aggr_res aggr;

            #if defined(COUNT) || defined(AVERAGE)
            aggr.cnt = 1;
            #endif

            #if defined(SUM) || defined(AVERAGE)
            aggr.sum = aggr_value;
            #endif

            #ifdef MINIMUM
            aggr.min = (uint32_t)(-1);
            if (aggr_value < aggr.min) {
                aggr.min = aggr_value;
            }
            #endif

            #ifdef MAXIMUM
            aggr.max = 0;
            if (aggr_value > aggr.max) {
                aggr.max = aggr_value;
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

    std::mt19937 uniform_gen(seed);
    std::uniform_int_distribution<prop_code_t> uniform_dist(0, (GROUP_KEY_CARDINALITY - 1)); /* [x, y] */

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
                    // mrn.id_ = nid;
                    // mrn.node_label = lc;

                    graph->begin_transaction();

                    auto prop = 0;
                    auto node_desc = graph->get_node_description(nid);
                    auto iter = node_desc.properties.find("creationDate");
                    auto value = iter->second;
                    auto dt = any_cast<boost::posix_time::ptime>(&value);
                    auto dtstr = to_iso_extended_string(dt->date());
                    auto yrstr = dtstr.substr(0, dtstr.find("-"));
                    // mrn.properties[prop++] = (prop_code_t)std::stoi(yrstr); /* Key: 0 */

                    // if (lc == cmt_code) { /* Key: 1 */
                    //     mrn.properties[prop++] = (prop_code_t)1;
                    // }
                    // else if (lc == post_code) {
                    //     mrn.properties[prop++] = (prop_code_t)0;
                    // }

                    iter = node_desc.properties.find("length");
                    value = iter->second;
                    auto len = any_cast<int>(&value);

                    prop_code_t length = (prop_code_t)(*len);
                    mrn.val = length + 1; /* Key: 2 */

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
                    prop_code_t group_key = (prop_code_t)(*id % GROUP_KEY_CARDINALITY);
                    mrn.key = group_key + 1; /* Key: 3 */
                    #else
                    mrn.properties[prop++] = uniform_dist(uniform_gen);
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

#endif /* #ifdef MULTIVAL_DATA */