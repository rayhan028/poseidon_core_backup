#ifndef common_h_
#define common_h_

#define COUNT
#define SUM
#define AVERAGE
#define MINIMUM
#define MAXIMUM

#define HIGH_CARDINALITY
// #define LOW_CARDINALITY
// #define LOW_CARD_1
// #define LOW_CARD_2

#define KB (1 << 10)
#define MB (KB << 10)

#define NR_TASKLETS 16
#define NR_DPUS 64
#define NR_CPU_THREADS 32

#define REP 1
#define DPU_BIN "./dpu_aggregate"
#define CSV_FILE "./res/res.csv"

typedef struct mrnode mrnode;
typedef struct dpu_params dpu_params;
typedef struct aggr_res aggr_res;
typedef struct htable_entry htable_entry;
typedef uint32_t prop_code_t;
typedef uint32_t aggr_val_t;

#define NR_ELEM_PROPS 4
#define GROUP_KEY 3
#define AGGR_KEY 2
#define NR_PARTITIONS 64
#define NR_GROUPS 4
#define NR_KERNERLS 2

#define ELEMS_PER_CHUNK 817

#if defined(SUM) || defined(AVERAGE)
    #define SUM_KEY AGGR_KEY
#endif

#ifdef MINIMUM
    #define MIN_KEY AGGR_KEY
#endif

#ifdef MAXIMUM
    #define MAX_KEY AGGR_KEY
#endif

#if 0
struct mrnode {
    uint8_t dummy_[40];       // transaction mgmt data
    uint64_t id_;
    uint64_t from_rship_list; // index in relationship list of first relationship
                              // where this node acts as from node
    uint64_t to_rship_list;   // index of relationship list of first relationship
                              // where this node acts as to node
    uint64_t property_list;   // index in property list
    uint32_t node_label;
    prop_code_t properties[NR_ELEM_PROPS];
};
#endif

struct mrnode {
    uint64_t id_;
    prop_code_t properties[NR_ELEM_PROPS];
    uint32_t node_label;
};

struct mrchunk {
    struct mrnode data[ELEMS_PER_CHUNK];
    struct mrchunk* next;
    char bitset[104];
    uint32_t first;
    char pad[58];
};

typedef enum kernel {
    partition_phase = 0,
    aggregation_phase = 1,
} kernel;

#ifdef HIGH_CARDINALITY
struct dpu_params {
    union {
        uint32_t num_elems;
        uint32_t num_partitions;
    };
    kernel phase;
};
#elif defined LOW_CARDINALITY
struct dpu_params {
    uint32_t num_elems;
    uint32_t htable_offset;
};
#endif

struct aggr_res {
#if defined(COUNT) || defined(AVERAGE)
    aggr_val_t cnt;
#endif

#if defined(SUM) || defined(AVERAGE)
    aggr_val_t sum;
#endif

#ifdef MINIMUM
    aggr_val_t min;
#endif

#ifdef MAXIMUM
    aggr_val_t max;
#endif

#ifdef AVERAGE
    double avg;
#endif
};

struct htable_entry {
    prop_code_t key;
    aggr_res val;
};

#define DIVCEIL(n, d) (((n) - 1) / (d) + 1)
#define ROUNDUP(n, d) ((n / d) * d + d)

#define MRAM_SIZE (64 * MB)
#define WRAM_SIZE (64 * KB)
#define MAX_MRAM_WRAM_XFER_SIZE (2 * KB)
#define MRAM_INPUT_BUFFER (48 * MB)
#define WRAM_INPUT_BUFFER (48 * KB)

#define ELEM_SIZE sizeof(mrnode)
#define HASH_TABLE_ENTRY_SIZE sizeof(htable_entry)

#ifdef HIGH_CARDINALITY

#define MRAM_INPUT_BUFFER_PARTITION (MRAM_INPUT_BUFFER / 2) /* reserve half the MRAM buffer to flush the partitioned elements */

#define NR_WR_ELEMS_PARTITION (((2 * KB) / ELEM_SIZE) * 16) /* TODO: tune */
#define NR_WR_ELEMS_PER_TASKLET_PARTITION (NR_WR_ELEMS_PARTITION / NR_TASKLETS)

#define MRAM_INPUT_BUFFER_AGGREGATION (MRAM_INPUT_BUFFER / 2) /* reserve half the MRAM buffer to flush the hash table results */

#define NR_WR_ELEMS_AGGREGATION ((2 * KB) / ELEM_SIZE / 16) /* TODO: tune */
// #define NR_WR_ELEMS_PER_TASKLET_AGGREGATION (NR_WR_ELEMS_AGGREGATION / NR_TASKLETS)
#define NR_WR_ELEMS_PER_TASKLET_AGGREGATION 5

#define HASH_TABLE_SIZE (32 * KB)
#define NR_HASH_TABLE_ENTRIES (HASH_TABLE_SIZE / HASH_TABLE_ENTRY_SIZE)
#define NR_HASH_TABLE_CHUNKS (HASH_TABLE_SIZE / MAX_MRAM_WRAM_XFER_SIZE)
#define NR_HASH_TABLE_CHUNK_ENTRIES (NR_HASH_TABLE_ENTRIES / NR_HASH_TABLE_CHUNKS)

#elif defined LOW_CARDINALITY

#define NR_WR_ELEMS_PER_TASKLET 16
#define MRAM_INPUT_BUFFER_SIZE (MRAM_INPUT_BUFFER - MR_HASH_TABLES_SIZE)

#ifdef LOW_CARD_1
#define HASH_TABLE_SIZE (32 * 64) /* max. hash table size: (32 * 64) */
#define NR_HASH_TABLE_ENTRIES (HASH_TABLE_SIZE / HASH_TABLE_ENTRY_SIZE)
#define MR_HASH_TABLES_SIZE (NR_TASKLETS * HASH_TABLE_SIZE)
#elif defined LOW_CARD_2
#define HASH_TABLE_SIZE (32 * KB)
#define NR_HASH_TABLE_ENTRIES (HASH_TABLE_SIZE / HASH_TABLE_ENTRY_SIZE)
#define NR_HASH_TABLE_CHUNKS (HASH_TABLE_SIZE / MAX_MRAM_WRAM_XFER_SIZE)
#define NR_HASH_TABLE_CHUNK_ENTRIES (NR_HASH_TABLE_ENTRIES / NR_HASH_TABLE_CHUNKS)
#define MR_HASH_TABLES_SIZE HASH_TABLE_SIZE
#endif

#endif

// #define PRINTER
#define PRINT_ERROR(fmt, ...)       fprintf(stderr, "\033[0;31mERROR:\033[0m   " fmt "\n", ##__VA_ARGS__)
#define PRINT_WARNING(fmt, ...)     fprintf(stderr, "\033[0;35mWARNING:\033[0m " fmt "\n", ##__VA_ARGS__)
#define PRINT_INFO(cond, fmt, ...)  if(cond) printf("\033[0;32mINFO:\033[0m    " fmt "\n", ##__VA_ARGS__)
#define PRINT(fmt, ...)             printf(fmt "\n", ##__VA_ARGS__)
#define PRINT_TOP_RULE              printf("\033[0;33m\n====================\033[0m\n");

#endif