#ifndef aggregation_hpp_
#define aggregation_hpp_

#include "graph_db.hpp"
#include "thread_pool.hpp"

#include "definitions.h"
#include "hash.h"
#include "zipfian.h"

#include <dpu>
#include <dpu_log.h>


/*-------------------- Aggregation Functions --------------------*/

void hash_aggregation_hi_card_v1(graph_db_ptr &graph);
void hash_aggregation_hi_card_v2(graph_db_ptr &graph);
void hash_aggregation_hi_card_v3(graph_db_ptr &graph);
void hash_aggregation_hi_card_v4(graph_db_ptr &graph);
void hash_aggregation_hi_card_v5(graph_db_ptr &graph);
void hash_aggregation_low_card(graph_db_ptr &graph);
void sort_aggregation(graph_db_ptr &graph);


/*-------------------- Utility Functions --------------------*/

bool get_partition_func(struct sg_block_info* cpu_buffer, uint32_t d, uint32_t p, void* params);
bool get_hash_table_func(struct sg_block_info* cpu_buffer, uint32_t d, uint32_t h, void* params);
void* multidim_malloc(uint32_t rows, uint32_t* cols, uint32_t cell_size);
void validate_aggr(const std::unordered_map<uint32_t, aggr_res> &lhs, const std::unordered_map<uint32_t, aggr_res> &rhs);
void parallel_cpu_aggr(mrnode* elems_buf, uint32_t num_elems, std::unordered_map<uint32_t, aggr_res>* htables);
void sequential_cpu_aggr(mrnode* elems_buf, uint32_t num_elems, std::unordered_map<uint32_t, aggr_res> &hash_table);
void initialize_props(graph_db_ptr &graph, struct mrnode* nodes_ptr);
void parallel_grpkey_proj(mrnode* elems_buf, uint32_t num_elems, prop_code_t* grpkey_buf);
void parallel_elem_cpy(mrnode* elems_buf, uint32_t num_elems, mrnode* partitions_buf, uint32_t* prefix_sum, uint32_t* count);
void parallel_gpartition_cpy(elem_t* global_parts, elem_t* tmp_global_parts, uint32_t** local_part_sizes, uint32_t** global_prefix, uint32_t* tmp_global_prefix);


#ifdef MULTIVAL_DATA
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
#elif defined(KV_DATA)
struct aggr_task {
    using range = std::pair<std::size_t, std::size_t>;
    aggr_task(mrnode* elems_buf, std::unordered_map<uint32_t, aggr_res>* htable, std::size_t first, std::size_t last, std::size_t tid)
        : elems_buf_(elems_buf), htable_(htable), range_(first, last), tid_(tid) {}

    static void aggregate(struct mrnode* elems_buf, std::unordered_map<uint32_t, aggr_res>* htable, uint32_t first, uint32_t last, uint32_t tid) {

        for (uint32_t e = first; e < last; e++) {
            mrnode &elem = elems_buf[e];
            uint32_t group_key = elem.key;
            uint32_t aggr_value = elem.val;
            // uint32_t hash = aggr_hash(group_key) % NR_HASH_TABLE_ENTRIES;
            uint32_t hash = group_key;

            auto iter = htable->find(hash);
            if (iter != htable->end()) {
                auto &aggr = iter->second;

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
#endif /* #ifdef MULTIVAL_DATA */

struct proj_task {
    using range = std::pair<std::size_t, std::size_t>;
    proj_task(mrnode* elems_buf, prop_code_t* grpkey_buf, std::size_t first, std::size_t last, std::size_t tid)
        : elems_buf_(elems_buf), grpkey_buf_(grpkey_buf), range_(first, last), tid_(tid) {}

    static void project(struct mrnode* elems_buf, prop_code_t* grpkey_buf, uint32_t first, uint32_t last, uint32_t tid) {

        // std::cout << "CPU thread: " <<  tid << "\n";
        for (uint32_t e = first; e < last; e++) {
#ifdef MULTIVAL_DATA
            grpkey_buf[e] = elems_buf[e].properties[GROUP_KEY];
#elif defined(KV_DATA)
            grpkey_buf[e] = elems_buf[e].key;
#endif /* #ifdef MULTIVAL_DATA */
        }
    }

    void operator()() {
        project(elems_buf_, grpkey_buf_, range_.first, range_.second, tid_);
    }

    mrnode* elems_buf_;
    prop_code_t* grpkey_buf_;
    range range_;
    std::size_t tid_;
};

struct elemcpy_task {
    using range = std::pair<std::size_t, std::size_t>;
    elemcpy_task(mrnode* elems_buf, mrnode* partitions_buf, uint32_t* prefix_sum, uint32_t* count, std::mutex* mutexes, std::size_t first, std::size_t last, std::size_t tid)
        : elems_buf_(elems_buf), partitions_buf_(partitions_buf), prefix_sum_(prefix_sum), count_(count), mutexes_(mutexes), range_(first, last), tid_(tid) {}

    static void copy_elems(struct mrnode* elems_buf, mrnode* partitions_buf, uint32_t* prefix_sum, uint32_t* count, std::mutex* mutexes, uint32_t first, uint32_t last, uint32_t tid) {

        // std::cout << "CPU thread: " <<  tid << "\n";
        for (uint32_t e = first; e < last; e++) {
#ifdef MULTIVAL_DATA
            prop_code_t grp_key = elems_buf[e].properties[GROUP_KEY];
#elif defined(KV_DATA)
            prop_code_t grp_key = elems_buf[e].key;
#endif /* #ifdef MULTIVAL_DATA */
            uint32_t partition = grp_key % NR_PARTITIONS;

            mutexes[partition].lock();
            uint32_t offs = prefix_sum[partition] + count[partition];
            count[partition]++;
            memcpy(&partitions_buf[offs], &elems_buf[e], ELEM_SIZE);
            mutexes[partition].unlock();
        }
    }

    void operator()() {
        copy_elems(elems_buf_, partitions_buf_, prefix_sum_, count_, mutexes_, range_.first, range_.second, tid_);
    }

    mrnode* elems_buf_;
    mrnode* partitions_buf_;
    uint32_t* prefix_sum_;
    uint32_t* count_;
    std::mutex* mutexes_;
    range range_;
    std::size_t tid_;
};

struct gpart_copy_task {
    using range = std::pair<std::size_t, std::size_t>;
    gpart_copy_task(elem_t* global_parts, elem_t* tmp_global_parts, uint32_t** local_part_sizes, uint32_t** global_prefix, uint32_t* tmp_global_prefix, std::size_t first, std::size_t last, std::size_t tid)
        : global_parts_(global_parts), tmp_global_parts_(tmp_global_parts), local_part_sizes_(local_part_sizes), global_prefix_sum_(global_prefix), tmp_global_prefix_sum_(tmp_global_prefix), range_(first, last), tid_(tid) {}

    static void copy_parts(elem_t* global_parts, elem_t* tmp_global_parts, uint32_t** local_part_sizes, uint32_t** global_prefix, uint32_t* tmp_global_prefix, std::size_t first, std::size_t last, std::size_t tid) {

        // std::cout << "CPU thread: " <<  tid << "\n";
        for (uint32_t d = first; d < last; d++) {
            elem_t* local_partitions = &tmp_global_parts[tmp_global_prefix[d]];

            uint32_t part_offs = 0;
            for (auto p = 0; p < NR_PARTITIONS; p++) {
                uint32_t part_size = local_part_sizes[d][p];
                memcpy(&global_parts[global_prefix[p][d]], &local_partitions[part_offs], part_size * ELEM_SIZE);
                part_offs += part_size;
            }
        }
    }

    void operator()() {
        copy_parts(global_parts_, tmp_global_parts_, local_part_sizes_, global_prefix_sum_, tmp_global_prefix_sum_, range_.first, range_.second, tid_);
    }

    elem_t* global_parts_;
    elem_t* tmp_global_parts_;
    uint32_t** local_part_sizes_;
    uint32_t** global_prefix_sum_;
    uint32_t* tmp_global_prefix_sum_;
    range range_;
    std::size_t tid_;
};

using time_val = std::chrono::_V2::steady_clock::time_point;

struct event {
    std::vector<time_val> starts;
    std::vector<time_val> stops;
    std::string name;
};

struct timer {
    void start(const std::string &name) {
        if (timing || !cur_event.empty()) {
            fprintf(stderr, "\033[0;35mWARNING:\033[0m    There is an ongoing timing\n");
        }
        else if (events.find(name) != events.end()) {
            auto &e = events[name];
            e.starts.push_back({std::chrono::steady_clock::now()});
            timing = true;
            cur_event = name;
        }
        else {
            event e;
            e.name = name;
            e.starts.push_back({std::chrono::steady_clock::now()});
            events.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(e));
            timing = true;
            cur_event = name;

            events_seq.push_back(name);
        }
    }

    void stop() {
        if (!timing) {
            fprintf(stderr, "\033[0;35mWARNING:\033[0m    Timer was not started\n");
        }
        else {
            auto &e = events[cur_event];
            e.stops.push_back({std::chrono::steady_clock::now()});
            timing = false;
            cur_event = "";
        }
    }

    void print() {
        for (auto &n : events_seq) {
            auto &e = events.find(n)->second;
            assert(e.starts.size() == e.stops.size());
            uint64_t total = 0;
            for (std::size_t t = 0; t < e.starts.size(); t++) {
                auto d = std::chrono::duration_cast<std::chrono::microseconds>(e.stops[t] - e.starts[t]).count();
                total += d;
            }
            printf("\033[0;32mINFO:\033[0m    %s: %f ms\n", e.name.c_str(), (total / (e.starts.size() * 1000.0)));
        }
    }

    void print_to_csv(const std::string &f, const std::string &mark, bool append = true) {
        std::ofstream ofs;
        if (append) {
            ofs.open(f, std::ios_base::app);
        }
        else {
            ofs.open(f);
        }

        if (!ofs) {
            fprintf(stderr, "\033[0;35mWARNING:\033[0m    Cannot open CSV file\n");
        }
        else {
            auto iter = events.begin();
            for (std::size_t t = 0; t < iter->second.starts.size(); t++) {
                /* assuming all events were measured the same number of times */
                for (auto &n : events_seq) {
                    auto &e = events.find(n)->second;
                    assert(e.starts.size() == e.stops.size());
                    auto d = std::chrono::duration_cast<std::chrono::microseconds>(e.stops[t] - e.starts[t]).count();
                    ofs << (d / 1000.0) << ",";
                }
                ofs << mark << "\n";
            }
            ofs.flush();

            // ofs << mark;
            // for (auto &[n, e] : events) {
            //     assert(e.starts.size() == e.stops.size());
            //     uint64_t total = 0;
            //     for (std::size_t t = 0; t < e.starts.size(); t++) {
            //         auto d = std::chrono::duration_cast<std::chrono::microseconds>(e.stops[t] - e.starts[t]).count();
            //         total += d;
            //     }
            //     ofs << "," << (total / (e.starts.size() * 1000.0));
            // }
            // ofs << "\n";
            // ofs.flush();
        }

        ofs.close();
    }

private:
    std::map<std::string, event> events;
    std::vector<std::string> events_seq;
    bool timing = false;
    std::string cur_event;
};

#endif
