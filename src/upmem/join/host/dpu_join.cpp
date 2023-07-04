#include "graph_db.hpp"
#include "thread_pool.hpp"
#include <dpu>
#include <dpu_log.h>

#include <random>

#include "../common/timer.hpp"
#include "../common/common.h"

struct tuple {
    tuple(uint64_t k, uint64_t id) : key(k), tid(id) {}

    bool operator==(const tuple &other) const {
        return key == other.key && tid == other.tid;
    }

    friend std::ostream& operator<<(std::ostream& os, const tuple& t);
    friend struct comparator;

private:
    uint64_t key;
    uint64_t tid;
};

std::ostream& operator<<(std::ostream& os, const tuple& t) {
    os << t.key << " : " << t.tid;
    return os;
}

struct comparator {
    bool operator()(tuple &lhs, tuple &rhs) {
        return lhs.key < rhs.key;
    }
};

struct relation {
    relation(std::size_t s) : size(s) {}

    std::vector<tuple>& get_tuples() { // TODO: cannot pass const pointer to dpu_prepare_xfer
        return tuples;
    }

    void print_relation(bool sorted = false) {
        if (sorted) {
            auto tmp = tuples;
            struct comparator cmp;
            std::sort(tmp.begin(), tmp.end(), cmp);
            for (auto &tuple : tmp) {
                std::cout << tuple << ", ";
            }
            std::cout << "\n";
        }
        else {
            for (auto &tuple : tuples) {
                std::cout << tuple << ", ";
            }
            std::cout << "\n";
        }
    }

    friend void populate_relations(relation &inner, relation &outer);

private:
    std::size_t size;
    std::vector<tuple> tuples;
};

void populate_relations(relation &inner, relation &outer) {
    assert(outer.size >= inner.size);
    inner.tuples.reserve(inner.size);

    std::random_device rd;
    std::mt19937::result_type seed = rd() ^ (
        (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() +
        (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

    std::mt19937 gen(seed);
    std::uniform_int_distribution<uint64_t> dist_r(7000, 7000 + inner.size);
    std::uniform_int_distribution<uint64_t> dist_s(8000, 8000 + outer.size);

    for (std::size_t i = 0; i < inner.size; i++) {
        uint64_t rval = dist_r(gen);
        // std::cout << i << " : " << rval;
        inner.tuples.emplace_back(i, rval);
    }

    std::vector<uint64_t> ref_keys(inner.size);
    std::iota(ref_keys.begin(), ref_keys.end(), 0);
    outer.tuples.reserve(outer.size);
    for (std::size_t i = 0; i < outer.size; i++) {
        if ((i % inner.size) == 0) {
            std::shuffle(ref_keys.begin(), ref_keys.end(), gen);
        }
        uint64_t rval = dist_s(gen);
        outer.tuples.emplace_back(ref_keys[i % inner.size], rval);
    }

    // std::shuffle(inner.tuples.begin(), inner.tuples.end(), gen);
    // std::shuffle(outer.tuples.begin(), outer.tuples.end(), gen);
}

void print_partitioned_tuples(const tuple* tuples, uint64_t* prefix, uint64_t num_tuples) {
    printf("Partitioned tuples on CPU------ \n");
}

void dpu_join() {
    // std::size_t cardinality = DPU_TUPLES * NR_DPUS; // 4128766;
    std::size_t cardinality = DPU_TUPLES; // 4128766;
    struct relation r{cardinality};
    struct relation s{cardinality};
    populate_relations(r, s); // TODO: save the relations in .tbl files for reuse and consistent measurements
    // r.print_relation();
    // s.print_relation();
    // s.print_relation(true);

    try {

        timer t;

        struct dpu_set_t dpu_set, dpu;
        uint32_t num_ranks, num_dpus;
        uint32_t dpuid = 0;
        uint64_t prefix;

        /* allocate DPUs and load binary */
        DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &dpu_set));
        DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &num_dpus));
        DPU_ASSERT(dpu_get_nr_ranks(dpu_set, &num_ranks));
        DPU_ASSERT(dpu_load(dpu_set, DPU_BIN, NULL));
        assert(num_dpus == NR_DPUS);

        auto &inner_tuples = r.get_tuples();
        auto tuples_r = inner_tuples.data();
        auto size_r = inner_tuples.size();
        uint32_t r_tuples_per_dpu = DIVCEIL(size_r, NR_DPUS);

        auto &outer_tuples = s.get_tuples();
        auto tuples_s = outer_tuples.data();
        auto size_s = outer_tuples.size();
        uint32_t s_tuples_per_dpu = DIVCEIL(size_s, NR_DPUS);

        PRINT_TOP_RULE;
        PRINT("DPUs: %u", NR_DPUS);
        PRINT("Ranks: %u", num_ranks);
        PRINT("Inner Tuples: %lu", size_r);
        PRINT("Inner tuples per DPU: %u", r_tuples_per_dpu);
        PRINT("Outer Tuples: %lu", size_s);
        PRINT("Outer tuples per DPU: %u", s_tuples_per_dpu);

        for (auto r = 0; r < REP; r++) {

            /********** Partitioning of the the inner relation R **********/

            /* transfer tuples to DPUs */
            /* TODO: when total tuples do not fit in DPUs 
                (nodes_per_dpu < MAX_TUPLES_PER_DPU) */
            PRINT("Transfer R...");
            t.start("CPU to DPU xfer (R)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &tuples_r[dpuid * r_tuples_per_dpu]));
                /* TODO: memory range of tuples_r likely exceeded in last DPU */
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME,
                                     0, sizeof(tuple) * r_tuples_per_dpu, DPU_XFER_DEFAULT));
            t.stop();

            /* transfer input parameters for partitioning */
            PRINT("Transfer parameters (partitioning)...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t tuples = (dpuid == (NR_DPUS - 1)) ?
                                  (size_r - dpuid * r_tuples_per_dpu) :
                                  r_tuples_per_dpu;
                // std::cout << "DPU: " << dpuid << " Tuples: " << tuples << "\n";
                struct dpu_params dpu_parameters{{tuples}, partition_phase};
                DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_parameters));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "parameters",
                                     0, sizeof(dpu_params), DPU_XFER_DEFAULT));

            /* launch the "partition" kernel */
            PRINT("Partition R...");
            t.start("DPU exec (R partitioning)");
            DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
            t.stop();

#if 0
            /* retrieve results from DPUs */
            PRINT("Retrieving transferred DPU input data...");
            tuple* dpu_inner_tuples = (tuple*) malloc (sizeof(tuple) * r_tuples_per_dpu * NR_DPUS);
            t.start("DPU to CPU xfer (non-partitioned)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_inner_tuples[dpuid * r_tuples_per_dpu]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME,
                                     0, sizeof(tuple) * r_tuples_per_dpu, DPU_XFER_DEFAULT));
            t.stop();

            {
                bool equal = true;
                for (std::size_t i = 0; i < (r_tuples_per_dpu * NR_DPUS); i++) {
                    // std::cout << dpu_inner_tuples[i] << ", ";
                    if (!(dpu_inner_tuples[i] == inner_tuples[i])) {
                        equal = false;
                        std::cout << dpu_inner_tuples[i] << " not equal to " << inner_tuples[i] << "\n";
                    }
                }
                std::cout << "\n";
                if (equal) {
                    PRINT_INFO(true, "Input data xfer valid (R)");
                }
                else {
                    PRINT_ERROR("Input data xfer invalid (R)");
                }
            }
#endif

            /* retrieve local histograms from DPUs and compute global prefix sum */
            PRINT("Compute global prefix sum for R...");
            uint32_t r_dpu_partition_sizes[NR_DPUS][NR_PARTITIONS];
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &r_dpu_partition_sizes[dpuid]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "partition_sizes",
                                     0, sizeof(uint32_t) * NR_PARTITIONS, DPU_XFER_DEFAULT));
            uint64_t r_prefix_sum[NR_DPUS][NR_PARTITIONS];
            prefix = 0;
            for (auto p = 0; p < NR_PARTITIONS; p++) {
                for (auto d = 0; d < NR_DPUS; d++) { /* TODO: optimize loop */
                    r_prefix_sum[d][p] = prefix;
                    prefix += r_dpu_partition_sizes[d][p];
                }
            }

            /* retrieve partitioned tuples from DPUs */
            PRINT("Retrieve sub-partitions of R...");
            tuple* buffer_r = (tuple*) malloc (sizeof(tuple) * r_tuples_per_dpu * NR_DPUS);
            uint32_t r_mr_offs[NR_DPUS] = {0};
            t.start("DPU to CPU xfer (R sub-partitions)");
            for (auto p = 0; p < NR_PARTITIONS; p++) {
                DPU_FOREACH(dpu_set, dpu, dpuid) {
                    // printf("Partition: %u, DPU: %u, offs: %u\n", p, dpuid, r_mr_offs[dpuid]);
                    DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(tuple) * (r_tuples_per_dpu + r_mr_offs[dpuid]),
                                             &buffer_r[r_prefix_sum[dpuid][p]], sizeof(tuple) * r_dpu_partition_sizes[dpuid][p]));
                    r_mr_offs[dpuid] += r_dpu_partition_sizes[dpuid][p];
                }
            }
            t.stop();
            print_partitioned_tuples(buffer_r, r_prefix_sum[0], size_r);

            /* dump DPU logs */
            PRINT("Display DPU Logs...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                std::cout << "DPU " << dpuid << "\n";
                DPU_ASSERT(dpu_log_read(dpu, stdout));
            }

#if 0
            /********** TODO: partitions are not of equal sizes **********/
            /********** TODO: not all partitions start at offset 0 on MRAM heap **********/
            /* send partitions of inner relation to DPU */
            PRINT("Transfer R partitions...");
            uint32_t r_parts_per_dpu = DIVCEIL(NR_PARTITIONS, NR_DPUS);
            /* TODO: when "r_parts_per_dpu do not fit in the DPU all at once" */
            t.start("CPU to DPU xfer (R partitions)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                /* at least one partition has to fit in a DPU (worst case) 
                   of course we are assigning several partitions to a DPU */
                uint32_t start_part = dpuid * r_parts_per_dpu;
                uint32_t end_part = start_part + r_parts_per_dpu;
                uint32_t part_size = 0;
                for (uint32_t p = start_part; p < end_part; p++) {
                    part_size += (p == (NR_PARTITIONS - 1)) ?
                                 (size_r - r_prefix_sum[0][p])
                                 : (r_prefix_sum[0][p + 1] - r_prefix_sum[0][p]);
                }
                DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME,
                    0, &buffer_r[r_prefix_sum[0][start_part]], sizeof(tuple) * part_size));
            }
            t.stop();

            /* transfer input parameters for join */
            PRINT("Transfer parameters (join) (R)...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t partitions = (dpuid == (NR_DPUS - 1)) ?
                                      (NR_PARTITIONS - dpuid * r_parts_per_dpu) :
                                      r_parts_per_dpu;
                struct dpu_params dpu_join_parameters{{partitions}, join_phase};
                DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_join_parameters));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "parameters",
                                     0, sizeof(dpu_params), DPU_XFER_DEFAULT));

            /* launch the "join" kernel */
            PRINT("Join R and S (R)...");
            t.start("DPU exec (join)");
            DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
            t.stop();

            /* retrieve sent partition data from DPU */
            PRINT("Retrieving transferred partitioned DPU input data...");
            tuple* r_join_input_buff = (tuple*) malloc (sizeof(tuple) * r_tuples_per_dpu * NR_DPUS);
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t start_part = dpuid * r_parts_per_dpu;
                uint32_t end_part = start_part + r_parts_per_dpu;
                uint32_t part_size = 0;
                for (uint32_t p = start_part; p < end_part; p++) {
                    part_size += (p == (NR_PARTITIONS - 1)) ?
                                 (size_r - r_prefix_sum[0][p])
                                 : (r_prefix_sum[0][p + 1] - r_prefix_sum[0][p]);
                }
                DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, 0, &r_join_input_buff[r_prefix_sum[0][start_part]], sizeof(tuple) * part_size));
            }

            {
                bool equal = true;
                for (std::size_t i = 0; i < (r_tuples_per_dpu * NR_DPUS); i++) {
                    // std::cout << r_join_input_buff[i] << "\n";
                    if (!(r_join_input_buff[i] == buffer_r[i])) {
                        equal = false;
                        std::cout << r_join_input_buff[i] << " not equal to " << buffer_r[i] << "\n";
                    }
                }

                if (equal) {
                    PRINT_INFO(true, "Partitioned input data xfer valid (R)");
                }
                else {
                    PRINT_ERROR("Partitioned input data xfer invalid (R)");
                }
                print_partitioned_tuples(r_join_input_buff, r_prefix_sum[0], size_r);
            }

            // dump DPU logs
            PRINT("Displaying DPU Logs...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                std::cout << "DPU " << dpuid << "\n";
                DPU_ASSERT(dpu_log_read(dpu, stdout));
            }
#endif

            /********** Partitioning of the the outer relation S **********/

            PRINT("Transfer S...");
            t.start("CPU to DPU xfer (S)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &tuples_s[dpuid * s_tuples_per_dpu]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME,
                                     0, sizeof(tuple) * s_tuples_per_dpu, DPU_XFER_DEFAULT));
            t.stop();

            /* transfer input parameters for partitioning */
            PRINT("Transfer parameters (partitioning)...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t tuples = (dpuid == (NR_DPUS - 1)) ?
                                  (size_s - dpuid * s_tuples_per_dpu) :
                                  s_tuples_per_dpu;
                // std::cout << "DPU: " << dpuid << " Tuples: " << tuples << "\n";
                struct dpu_params dpu_parameters{{tuples}, partition_phase};
                DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_parameters));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "parameters",
                                     0, sizeof(dpu_params), DPU_XFER_DEFAULT));

            /* launch the "partition" kernel */
            PRINT("Partition S...");
            t.start("DPU exec (S partitioning)");
            DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
            t.stop();

#if 0
            /* retrieve results from DPUs */
            PRINT("Retrieving transferred DPU input data...");
            tuple* dpu_outer_tuples = (tuple*) malloc (sizeof(tuple) * r_tuples_per_dpu * NR_DPUS);
            t.start("DPU to CPU xfer (non-partitioned)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_outer_tuples[dpuid * r_tuples_per_dpu]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME,
                                     0, sizeof(tuple) * r_tuples_per_dpu, DPU_XFER_DEFAULT));
            t.stop();

            {
                bool equal = true;
                for (std::size_t i = 0; i < (r_tuples_per_dpu * NR_DPUS); i++) {
                    // std::cout << dpu_outer_tuples[i] << ", ";
                    if (!(dpu_outer_tuples[i] == outer_tuples[i])) {
                        equal = false;
                        std::cout << dpu_outer_tuples[i] << " not equal to " << outer_tuples[i] << "\n";
                    }
                }
                std::cout << "\n";
                if (equal) {
                    PRINT_INFO(true, "Input data xfer valid (S)");
                }
                else {
                    PRINT_ERROR("Input data xfer invalid (S)");
                }
            }
#endif

            /* retrieve local histograms from DPUs and compute global prefix sum */
            PRINT("Compute global prefix sum for S...");
            uint32_t s_dpu_partition_sizes[NR_DPUS][NR_PARTITIONS];
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                DPU_ASSERT(dpu_prepare_xfer(dpu, &s_dpu_partition_sizes[dpuid]));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "partition_sizes",
                                     0, sizeof(uint32_t) * NR_PARTITIONS, DPU_XFER_DEFAULT));
            uint64_t s_prefix_sum[NR_DPUS][NR_PARTITIONS];
            prefix = 0;
            for (auto p = 0; p < NR_PARTITIONS; p++) {
                for (auto d = 0; d < NR_DPUS; d++) { /* TODO: optimize loop */
                    s_prefix_sum[d][p] = prefix;
                    prefix += s_dpu_partition_sizes[d][p];
                }
            }

            /* retrieve partitioned tuples from DPUs */
            PRINT("Retrieve sub-partitions of S...");
            tuple* buffer_s = (tuple*) malloc (sizeof(tuple) * s_tuples_per_dpu * NR_DPUS);
            uint32_t s_mr_offs[NR_DPUS] = {0};
            t.start("DPU to CPU xfer (S sub-partitions)");
            for (auto p = 0; p < NR_PARTITIONS; p++) {
                DPU_FOREACH(dpu_set, dpu, dpuid) {
                    // printf("Partition: %u, DPU: %u, offs: %u\n", p, dpuid, s_mr_offs[dpuid]);
                    DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(tuple) * (s_tuples_per_dpu + s_mr_offs[dpuid]),
                                             &buffer_s[s_prefix_sum[dpuid][p]], sizeof(tuple) * s_dpu_partition_sizes[dpuid][p]));
                    s_mr_offs[dpuid] += s_dpu_partition_sizes[dpuid][p];
                }
            }
            t.stop();
            print_partitioned_tuples(buffer_s, s_prefix_sum[0], size_s);

            /* dump DPU logs */
            PRINT("Display DPU Logs...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                std::cout << "DPU " << dpuid << "\n";
                DPU_ASSERT(dpu_log_read(dpu, stdout));
            }

#if 0
            /********** TODO: partitions are not of equal sizes **********/
            /********** TODO: not all partitions start at offset 0 on MRAM heap **********/
            /* send partitions of inner relation to DPU */
            /* send partitions of outer relation to DPU */
            PRINT("Transfer S partitions...");
            uint32_t s_parts_per_dpu = DIVCEIL(NR_PARTITIONS, NR_DPUS);
            /* TODO: when "s_parts_per_dpu do not fit in the DPU all at once" */
            t.start("CPU to DPU xfer (S partitions)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                /* at least one partition has to fit in a DPU (worst case) 
                   of course we are assigning several partitions to a DPU */
                uint32_t start_part = dpuid * s_parts_per_dpu;
                uint32_t end_part = start_part + s_parts_per_dpu;
                uint32_t part_size = 0;
                for (uint32_t p = start_part; p < end_part; p++) {
                    part_size += (p == (NR_PARTITIONS - 1)) ?
                                 (size_s - s_prefix_sum[0][p])
                                 : (s_prefix_sum[0][p + 1] - s_prefix_sum[0][p]);
                }
                DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME,
                    0, &buffer_s[s_prefix_sum[0][start_part]], sizeof(tuple) * part_size));
            }
            t.stop();

            /* transfer input parameters for join */
            PRINT("Transfer parameters (join) (S)...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t partitions = (dpuid == (NR_DPUS - 1)) ?
                                      (NR_PARTITIONS - dpuid * s_parts_per_dpu) :
                                      s_parts_per_dpu;
                struct dpu_params dpu_join_parameters{{partitions}, join_phase};
                DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_join_parameters));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "parameters",
                                     0, sizeof(dpu_params), DPU_XFER_DEFAULT));

            /* launch the "join" kernel */
            PRINT("Join R and S (S)...");
            t.start("DPU exec (join)");
            DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
            t.stop();

            /* retrieve sent partition data from DPU */
            PRINT("Retrieving transferred partitioned DPU input data...");
            tuple* s_join_input_buff = (tuple*) malloc (sizeof(tuple) * s_tuples_per_dpu * NR_DPUS);
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t start_part = dpuid * s_parts_per_dpu;
                uint32_t end_part = start_part + s_parts_per_dpu;
                uint32_t part_size = 0;
                for (uint32_t p = start_part; p < end_part; p++) {
                    part_size += (p == (NR_PARTITIONS - 1)) ?
                                 (size_s - s_prefix_sum[0][p])
                                 : (s_prefix_sum[0][p + 1] - s_prefix_sum[0][p]);
                }
                DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME,
                    0, &s_join_input_buff[s_prefix_sum[0][start_part]], sizeof(tuple) * part_size));
            }

            {
                bool equal = true;
                for (std::size_t i = 0; i < (r_tuples_per_dpu * NR_DPUS); i++) {
                    // std::cout << s_join_input_buff[i] << "\n";
                    if (!(s_join_input_buff[i] == buffer_s[i])) {
                        equal = false;
                        std::cout << s_join_input_buff[i] << " not equal to " << buffer_s[i] << "\n";
                    }
                }

                if (equal) {
                    PRINT_INFO(true, "Partitioned input data xfer valid (S)");
                }
                else {
                    PRINT_ERROR("Partitioned input data xfer invalid (S)");
                }
                print_partitioned_tuples(s_join_input_buff, s_prefix_sum[0], size_s);
            }

            // dump DPU logs
            PRINT("Displaying DPU Logs...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                std::cout << "DPU " << dpuid << "\n";
                DPU_ASSERT(dpu_log_read(dpu, stdout));
            }
#endif

            /********** Joining R and S **********/

            /* TODO: make a join task queue for the partitions of inner and outer relations */
            /* send partitions of inner and outer relations to DPU */
            PRINT("Transfer R and S partitions...");
            uint32_t d = 0;
            uint32_t assigned_part_size[NR_DPUS] = {0};
            std::map<uint32_t, std::vector<uint32_t>> dpu_to_parts;
            for (uint32_t p = 0; p < NR_PARTITIONS; p++) {
                uint32_t r_part_size = (p == (NR_PARTITIONS - 1)) ?
                                       (size_r - r_prefix_sum[0][p]) :
                                       (r_prefix_sum[0][p + 1] - r_prefix_sum[0][p]);
                uint32_t s_part_size = (p == (NR_PARTITIONS - 1)) ?
                                       (size_s - s_prefix_sum[0][p]) :
                                       (s_prefix_sum[0][p + 1] - s_prefix_sum[0][p]);
                while ((assigned_part_size[d] + r_part_size + s_part_size) >= MAX_MRAM_TUPLES) {
                    d++;
                    d %= NR_DPUS; /* we assign DPUs in a round-robin fashion */
                    /* TODO: avoid looping infinitely when all partitions cannot fit in DPU*/
                }
                if (dpu_to_parts.find(d) == dpu_to_parts.end()) {
                    dpu_to_parts[d];
                }
                dpu_to_parts[d].push_back(p);
                dpu_to_parts[d].push_back(r_part_size);
                dpu_to_parts[d].push_back(s_part_size);
                assigned_part_size[d] += r_part_size;
                assigned_part_size[d] += s_part_size;
                d++;
                d %= NR_DPUS; /* we assign DPUs in a round-robin fashion */
            }

            t.start("CPU to DPU xfer (R and S partitions)");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t j = 0;
                uint32_t r_offs = 0;
                uint32_t s_offs = 0;
                auto &parts = dpu_to_parts[dpuid];
                uint64_t num_parts = parts.size() / 3;
                uint64_t part_sizes[2 * num_parts]; /* we store the R and S partition sizes at the start of DPU_MRAM_HEAP_POINTER_NAME */
                r_offs += num_parts; /* sizeof(tuple) = 2 *sizeof(uint64_t) */
                s_offs += num_parts; /* sizeof(tuple) = 2 *sizeof(uint64_t) */

                /* copy R and S partitions */
                for (std::size_t i = 0; i < parts.size(); i += 3) {
                    uint32_t p = parts[i];
                    uint32_t r_part_size = parts[i + 1];
                    uint32_t s_part_size = parts[i + 2];
                    part_sizes[j++] = r_part_size;
                    part_sizes[j++] = s_part_size;
                    DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(tuple) * r_offs,
                                            &buffer_r[r_prefix_sum[0][p]], sizeof(tuple) * r_part_size));
                    s_offs += r_part_size;
                    DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME, sizeof(tuple) * s_offs,
                                            &buffer_s[s_prefix_sum[0][p]], sizeof(tuple) * s_part_size));
                    // std::cout << "r_offs: " << r_offs << " r_part_size: " << r_part_size << " r_prefix_sum: "
                    //         << r_prefix_sum[0][p] << " Partition: " << p << " DPU: " << dpuid << "\n"; 
                    // std::cout << "s_offs: " << s_offs << " s_part_size: " << s_part_size << " s_prefix_sum: "
                    //         << s_prefix_sum[0][p] << " Partition: " << p << " DPU: " << dpuid << "\n"; 
                    r_offs += s_part_size;
                }
                /* copy the partition sizes */
                DPU_ASSERT(dpu_copy_to(dpu, DPU_MRAM_HEAP_POINTER_NAME,
                                       0, part_sizes, sizeof(tuple) * num_parts));
                // std::cout << "r_offs: " << r_offs << "\n";
                // std::cout << "s_offs: " << s_offs << "\n";
            }
            t.stop();

            /* transfer input parameters for join */
            PRINT("Transfer parameters (join)...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                uint32_t partitions = dpu_to_parts[dpuid].size() / 3;
                // std::cout << "DPU: " << dpuid << " Partitions: " << partitions << "\n";
                struct dpu_params dpu_parameters{{partitions}, join_phase}; /* we need an array for this outside this scope */
                DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_parameters));
            }
            DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "parameters",
                                     0, sizeof(dpu_params), DPU_XFER_DEFAULT));

            /* launch the "partition" kernel */
            PRINT("Join R and S partitions...");
            t.start("DPU exec (Joining R and S partitions)");
            DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
            t.stop();

            /* dump DPU logs */
            PRINT("Display DPU Logs...");
            DPU_FOREACH(dpu_set, dpu, dpuid) {
                std::cout << "DPU " << dpuid << "\n";
                DPU_ASSERT(dpu_log_read(dpu, stdout));
            }
        }

        // free DPUs
        PRINT("Free DPUs...");
        DPU_ASSERT(dpu_free(dpu_set));

        PRINT_TOP_RULE;
        t.print();
        t.print_to_csv(CSV_FILE, std::to_string(NR_DPUS));
    }
    catch (const dpu::DpuError &e) {
        std::cerr << e.what() << std::endl;
    }
}