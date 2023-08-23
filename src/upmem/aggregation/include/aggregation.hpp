#ifndef aggregation_hpp_
#define aggregation_hpp_

#include "graph_db.hpp"
#include "thread_pool.hpp"

#include "definitions.h"
#include "hash.h"

#include <dpu>
#include <dpu_log.h>


/*-------------------- Aggregation Functions --------------------*/

void hash_aggregation_hi_card_v1(graph_db_ptr &graph);
void hash_aggregation_hi_card_v2(graph_db_ptr &graph);
void hash_aggregation_hi_card_v3(graph_db_ptr &graph);
void hash_aggregation_hi_card_v4(graph_db_ptr &graph);
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
