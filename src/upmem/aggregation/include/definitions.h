#ifndef definitions_h_
#define definitions_h_

#define COUNT
#define SUM
#define AVERAGE
#define MINIMUM
#define MAXIMUM

#if defined(SUM) || defined(AVERAGE)
#define SUM_KEY AGGR_KEY
#endif
#ifdef MINIMUM
#define MIN_KEY AGGR_KEY
#endif
#ifdef MAXIMUM
#define MAX_KEY AGGR_KEY
#endif

#define KiB (1 << 10)
#define MiB (KiB << 10)

#define NR_TASKLETS 16
#define NR_DPUS 64
#define NR_CPU_THREADS 32

#define NR_ELEM_PROPS 4
#define GROUP_KEY 3
#define AGGR_KEY 2
#define GROUP_KEY_CARDINALITY 1048576

// #define SF0_1
#define SF1
// #define SF10

#define REP 1
#define ELEMS_PER_CHUNK 817

#define CSV_FILE "./res/res.csv"

typedef struct mrnode mrnode;
typedef struct aggr_res aggr_res;
typedef struct dpu_params dpu_params;
typedef struct htable_entry htable_entry;
typedef uint32_t prop_code_t;
typedef uint32_t aggr_val_t;

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

#define MRAM_SIZE (64 * MiB)
#define WRAM_SIZE (64 * KiB)
#define MAX_MRAM_WRAM_XFER_SIZE (2 * KiB)
#define MRAM_INPUT_BUFFER (48 * MiB)
#define WRAM_INPUT_BUFFER (48 * KiB)

#define ELEM_SIZE sizeof(mrnode)
#define HASH_TABLE_ENTRY_SIZE sizeof(htable_entry)

// #define HASH_BASED_HIGH_CARDINALITY_V1
// #define HASH_BASED_HIGH_CARDINALITY_V2
#define HASH_BASED_HIGH_CARDINALITY_V3
// #define HASH_BASED_LOW_CARDINALITY
// #define SORT_BASED_HIGH_CARDINALITY

#ifdef HASH_BASED_HIGH_CARDINALITY_V1
#define SINGLE_HISTOGRAM
// #define PER_TASKLET_HISTOGRAM

typedef enum kernel {
    partition_phase = 0,
    aggregation_phase = 1,
} kernel;

struct dpu_params {
    union {
        uint32_t num_elems;
        uint32_t num_partitions;
    };
    kernel phase;
};

#define NR_KERNELS 2
#define NR_PARTITIONS 64
#define HASH_AGGR_HI_CARD_V1_BIN "./dpu_bin/dpu_hash_aggr_hi_card_v1"

#define MRAM_INPUT_BUFFER_PARTITION (MRAM_INPUT_BUFFER / 2) /* reserve half the MRAM buffer to flush the partitioned elements */

#define NR_WR_ELEMS_PARTITION (((2 * KiB) / ELEM_SIZE) * 16) /* TODO: tune */
#define NR_WR_ELEMS_PER_TASKLET_PARTITION (NR_WR_ELEMS_PARTITION / NR_TASKLETS)

#define MRAM_INPUT_BUFFER_AGGREGATION (MRAM_INPUT_BUFFER / 2) /* reserve half the MRAM buffer to flush the hash table results */

#define NR_WR_ELEMS_AGGREGATION ((2 * KiB) / ELEM_SIZE / 16) /* TODO: tune */
// #define NR_WR_ELEMS_PER_TASKLET_AGGREGATION (NR_WR_ELEMS_AGGREGATION / NR_TASKLETS)
#define NR_WR_ELEMS_PER_TASKLET_AGGREGATION 5

#define HASH_TABLE_SIZE (32 * KiB)
#define NR_HASH_TABLE_ENTRIES (HASH_TABLE_SIZE / HASH_TABLE_ENTRY_SIZE)
#define NR_HASH_TABLE_CHUNKS (HASH_TABLE_SIZE / MAX_MRAM_WRAM_XFER_SIZE)
#define NR_HASH_TABLE_CHUNK_ENTRIES (NR_HASH_TABLE_ENTRIES / NR_HASH_TABLE_CHUNKS)

#elif defined HASH_BASED_HIGH_CARDINALITY_V2

typedef enum kernel {
    partition_phase = 0,
    aggregation_phase = 1,
} kernel;

struct dpu_params {
    union {
        uint32_t num_elems;
        uint32_t num_partitions;
    };
    kernel phase;
};

#define NR_KERNELS 2
#define NR_PARTITIONS 1024
#define HASH_AGGR_HI_CARD_V2_BIN "./dpu_bin/dpu_hash_aggr_hi_card_v2"

#define MRAM_INPUT_BUFFER_PARTITION MRAM_INPUT_BUFFER /* reserve half the MRAM buffer to flush the partitioned elements */

#define NR_WR_ELEMS_PARTITION (((2 * KiB) / sizeof(prop_code_t)) * 16) /* TODO: tune */
#define NR_WR_ELEMS_PER_TASKLET_PARTITION (NR_WR_ELEMS_PARTITION / NR_TASKLETS)

#define MRAM_INPUT_BUFFER_AGGREGATION (MRAM_INPUT_BUFFER / 2) /* reserve half the MRAM buffer to flush the hash table results. TODO: tune */

#define NR_WR_ELEMS_AGGREGATION (((2 * KiB) / ELEM_SIZE) * 16) /* TODO: tune */
// #define NR_WR_ELEMS_PER_TASKLET_AGGREGATION (NR_WR_ELEMS_AGGREGATION / NR_TASKLETS)
#define NR_WR_ELEMS_PER_TASKLET_AGGREGATION 16

#define HASH_TABLE_SIZE (32 * KiB)
#define NR_HASH_TABLE_ENTRIES (HASH_TABLE_SIZE / HASH_TABLE_ENTRY_SIZE)
#define NR_HASH_TABLE_CHUNKS (HASH_TABLE_SIZE / MAX_MRAM_WRAM_XFER_SIZE)
#define NR_HASH_TABLE_CHUNK_ENTRIES (NR_HASH_TABLE_ENTRIES / NR_HASH_TABLE_CHUNKS)

#elif defined HASH_BASED_HIGH_CARDINALITY_V3

typedef enum kernel {
    partition_phase = 0,
    aggregation_phase = 1,
} kernel;

struct dpu_params {
    union {
        uint32_t num_elems;
        uint32_t num_partitions;
    };
    kernel phase;
};

#define NR_KERNELS 2
#define NR_PARTITIONS 1024
#define HASH_AGGR_HI_CARD_V3_BIN "./dpu_bin/dpu_hash_aggr_hi_card_v3"

#define MRAM_INPUT_BUFFER_PARTITION (MRAM_INPUT_BUFFER / 2) /* reserve half the MRAM buffer to flush the partitioned elements */

#define NR_WR_ELEMS_PARTITION (((2 * KiB) / ELEM_SIZE) * 16) /* TODO: tune */
#define NR_WR_ELEMS_PER_TASKLET_PARTITION (NR_WR_ELEMS_PARTITION / NR_TASKLETS)

#define MRAM_INPUT_BUFFER_AGGREGATION (MRAM_INPUT_BUFFER / 2) /* reserve half the MRAM buffer to flush the hash table results */

#define NR_WR_ELEMS_AGGREGATION ((2 * KiB) / ELEM_SIZE / 16) /* TODO: tune */
// #define NR_WR_ELEMS_PER_TASKLET_AGGREGATION (NR_WR_ELEMS_AGGREGATION / NR_TASKLETS)
#define NR_WR_ELEMS_PER_TASKLET_AGGREGATION 5

#define HASH_TABLE_SIZE (32 * KiB)
#define NR_HASH_TABLE_ENTRIES (HASH_TABLE_SIZE / HASH_TABLE_ENTRY_SIZE)
#define NR_HASH_TABLE_CHUNKS (HASH_TABLE_SIZE / MAX_MRAM_WRAM_XFER_SIZE)
#define NR_HASH_TABLE_CHUNK_ENTRIES (NR_HASH_TABLE_ENTRIES / NR_HASH_TABLE_CHUNKS)

#elif defined HASH_BASED_LOW_CARDINALITY
// #define PER_TASKLET_HASH_TABLE
#define SINGLE_HASH_TABLE

struct dpu_params {
    uint32_t num_elems;
    uint32_t htable_offset;
};

#define NR_KERNELS 1
#define HASH_AGGR_LOW_CARD_BIN "./dpu_bin/dpu_hash_aggr_low_card"

#define NR_WR_ELEMS_PER_TASKLET 16
#define MRAM_INPUT_BUFFER_SIZE (MRAM_INPUT_BUFFER - MR_HASH_TABLES_SIZE)

#ifdef PER_TASKLET_HASH_TABLE
#define HASH_TABLE_SIZE (32 * 64) /* max. hash table size: (32 * 64) */
#define NR_HASH_TABLE_ENTRIES (HASH_TABLE_SIZE / HASH_TABLE_ENTRY_SIZE)
#define MR_HASH_TABLES_SIZE (NR_TASKLETS * HASH_TABLE_SIZE)
#elif defined SINGLE_HASH_TABLE
#define HASH_TABLE_SIZE (32 * KiB)
#define NR_HASH_TABLE_ENTRIES (HASH_TABLE_SIZE / HASH_TABLE_ENTRY_SIZE)
#define NR_HASH_TABLE_CHUNKS (HASH_TABLE_SIZE / MAX_MRAM_WRAM_XFER_SIZE)
#define NR_HASH_TABLE_CHUNK_ENTRIES (NR_HASH_TABLE_ENTRIES / NR_HASH_TABLE_CHUNKS)
#define MR_HASH_TABLES_SIZE HASH_TABLE_SIZE
#endif /* PER_TASKLET_HASH_TABLE */

#elif defined SORT_BASED_HIGH_CARDINALITY

typedef enum kernel {
    sort_phase = 0,
    aggregation_phase = 1,
} kernel;

struct dpu_params {
    union {
        uint32_t num_elems;
        uint32_t num_partitions;
    };
    kernel phase;
};

#define NR_KERNELS 2
#define NR_PARTITIONS 1024
#define SORT_AGGR_BIN "./dpu_bin/dpu_sort_aggr"

#define MRAM_INPUT_BUFFER_SORT MRAM_INPUT_BUFFER /* reserve half the MRAM buffer to flush the sorted */

#define NR_WR_ELEMS_SORT (((2 * KiB) / ELEM_SIZE) * 16) /* TODO: tune */
#define NR_WR_ELEMS_PER_TASKLET_SORT (NR_WR_ELEMS_SORT / NR_TASKLETS)

#endif /* #ifdef HASH_BASED_HIGH_CARDINALITY_V1 */

// #define PRINTER
#define PRINT_ERROR(fmt, ...)       fprintf(stderr, "\033[0;31mERROR:\033[0m   " fmt "\n", ##__VA_ARGS__)
#define PRINT_WARNING(fmt, ...)     fprintf(stderr, "\033[0;35mWARNING:\033[0m " fmt "\n", ##__VA_ARGS__)
#define PRINT_INFO(cond, fmt, ...)  if(cond) printf("\033[0;32mINFO:\033[0m    " fmt "\n", ##__VA_ARGS__)
#define PRINT(fmt, ...)             printf(fmt "\n", ##__VA_ARGS__)
#define PRINT_TOP_RULE              printf("\033[0;33m\n====================\033[0m\n");

#endif
