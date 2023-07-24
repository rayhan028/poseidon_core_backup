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
#define MRAM_INPUT_BUFFER (48 * MB)

#define NR_TASKLETS 16
#define NR_DPUS 2
#define MAX_THREADS 32

#define REP 1
#define DPU_BIN "./dpu_aggregate"
#define CSV_FILE "./res/res.csv"

typedef struct mrnode mrnode;
typedef struct dpu_params dpu_params;
typedef struct aggr_res aggr_res;
typedef uint64_t prop_code_t;
typedef uint64_t aggr_val_t;

#define NR_NODE_PROPS 8
#define ELEMS_PER_CHUNK 817

struct mrnode {
    uint8_t dummy_[40];       // transaction mgmt data
    uint64_t id_;
    uint64_t from_rship_list; // index in relationship list of first relationship
                              // where this node acts as from node
    uint64_t to_rship_list;   // index of relationship list of first relationship
                              // where this node acts as to node
    uint64_t property_list;   // index in property list
    uint32_t node_label;
    prop_code_t properties[NR_NODE_PROPS];
};

struct mrchunk {
    struct mrnode data[ELEMS_PER_CHUNK];
    struct mrchunk* next;
    char bitset[104];
    uint32_t first;
    char pad[58];
};

struct dpu_params {
    uint64_t elems;
    // uint64_t elems_per_chunk;
};

struct aggr_res {
#if defined(COUNT) || defined(AVERAGE)
    uint64_t cnt;
#endif

#if defined(SUM) || defined(AVERAGE)
    uint64_t sum;
#endif

#ifdef MINIMUM
    uint64_t min;
#endif

#ifdef MAXIMUM
    uint64_t max;
#endif

#ifdef AVERAGE
    double avg;
#endif
};

// #define MAX_CHUNKS_PER_DPU (MRAM_SIZE / sizeof(struct mrchunk))
#define MAX_CHUNKS_PER_DPU 401

#define DIVCEIL(n, d) (((n) - 1) / (d) + 1)
#define ROUNDUP(n, d) ((n / d) * d + d)

#define ELEM_SIZE sizeof(mrnode)
#define NUM_WR_ELEMS (16 * 14)
#define NUM_WR_ELEMS_PER_TASKLET (NUM_WR_ELEMS / NR_TASKLETS)

// #define PRINTER
#define PRINT_ERROR(fmt, ...)       fprintf(stderr, "\033[0;31mERROR:\033[0m   " fmt "\n", ##__VA_ARGS__)
#define PRINT_WARNING(fmt, ...)     fprintf(stderr, "\033[0;35mWARNING:\033[0m " fmt "\n", ##__VA_ARGS__)
#define PRINT_INFO(cond, fmt, ...)  if(cond) printf("\033[0;32mINFO:\033[0m    " fmt "\n", ##__VA_ARGS__)
#define PRINT(fmt, ...)             printf(fmt "\n", ##__VA_ARGS__)
#define PRINT_TOP_RULE              printf("\033[0;33m\n====================\033[0m\n");
#define LOG_MSG(msg)               std::cout << msg

#define NUM_KERNELS 1

#endif