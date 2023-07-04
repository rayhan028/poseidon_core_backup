#ifndef common_h_
#define common_h_

#define COUNT
#define SUM
#define AVERAGE
#define MINIMUM
#define MAXIMUM

#define KB (1 << 10)
#define MB (KB << 10)
#define MRAM_SIZE (64 * MB)

#define ELEMENTS_PER_CHUNK 817
#define NUM_TASKS 19
#define CACHE_SIZE (ELEMENTS_PER_CHUNK / NR_TASKLETS) // 43 nodes per tasklet
#define CACHE_SIZE_BYTES (CACHE_SIZE * sizeof(struct mr_node)) // 3440 bytes

struct mr_node {
    uint8_t dummy_[40];       // transaction mgmt data
    uint64_t id_;
    uint64_t from_rship_list; // index in relationship list of first relationship
                              // where this node acts as from node
    uint64_t to_rship_list;   // index of relationship list of first relationship
                              // where this node acts as to node
    uint64_t property_list;   // index in property list
    uint32_t node_label;  
};

struct mrchunk {
    struct mr_node data[ELEMENTS_PER_CHUNK];
    struct mrchunk* next;
    char bitset[104];
    uint32_t first;
    char pad[58];
};

// struct dpu_aggr_res {
//     uint32_t max;
// };

struct dpu_params {
    uint64_t chunks;
    // uint64_t elems_per_chunk;
};

struct aggr_res {
    uint64_t cnt;
    uint64_t sum;
    uint64_t min;
    uint64_t max;
    double avg;
};

// #define MAX_CHUNKS_PER_DPU (MRAM_SIZE / sizeof(struct mrchunk))
#define MAX_CHUNKS_PER_DPU 401

#define DIVCEIL(n, d) (((n) - 1) / (d) + 1)
#define ROUNDUP(n, d) ((n / d) * d + d)

// #define CACHE_SIZE_BYTES 2560
// #define G_CACHE_SIZE_BYTES ROUNDUP(CACHE_SIZE_BYTES, sizeof(mr_node))
// #define CACHE_SIZE (G_CACHE_SIZE_BYTES / sizeof(mr_node)) // 32

// #define PRINTER
#define PRINT_ERROR(fmt, ...)       fprintf(stderr, "\033[0;31mERROR:\033[0m   " fmt "\n", ##__VA_ARGS__)
#define PRINT_WARNING(fmt, ...)     fprintf(stderr, "\033[0;35mWARNING:\033[0m " fmt "\n", ##__VA_ARGS__)
#define PRINT_INFO(cond, fmt, ...)  if(cond) printf("\033[0;32mINFO:\033[0m    " fmt "\n", ##__VA_ARGS__)
#define PRINT(fmt, ...)             printf(fmt "\n", ##__VA_ARGS__)
#define PRINT_TOP_RULE              printf("\033[0;33m\n====================\033[0m\n");
#define LOG_MSG(msg)               std::cout << msg

#define NUM_KERNELS 1

#endif