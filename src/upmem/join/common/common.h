#ifndef common_h_
#define common_h_

#define NR_TASKLETS 16
#define NR_DPUS 2
#define MAX_THREADS 32
#define NR_PARTITIONS 2

#define REP 1
#define DPU_BIN "./dpu_join"
#define CSV_FILE "./res/res.csv"

#define MAX_WR_TUPLES 128
#define WR_TUPLES (MAX_WR_TUPLES / 2)

#define DPU_TUPLES 256
#define NUM_KERNELS 3

#define DIVCEIL(n, d) (((n) - 1) / (d) + 1)
#define ROUNDUP(n, d) ((n / d) * d + d)

#define BUCKET_SIZE 128

/* Balkesen */
/* #define RADIX_HASH(V)  ((V>>7)^(V>>13)^(V>>21)^V) */
#define HASH_BIT_MODULO(K, MASK, NBITS) (((K) & MASK) >> NBITS)
#define NUM_RADIX_BITS 4

typedef struct mrtuple mrtuple;
typedef struct dpu_params dpu_params;
typedef enum kernel {
    partition_phase = 0,
    join_phase = 1,
} kernel;

struct mrtuple {
    uint64_t key;
    uint64_t tid;
};

struct dpu_params {
    union {
        uint64_t num_tuples;
        uint64_t num_partitions;
    };
    // uint64_t num_tuples;
    kernel phase;    
};


#define TUPLE_SIZE sizeof(mrtuple)
#define MRAM_SIZE (64 << 20)
#define MAX_MRAM_TUPLES (MRAM_SIZE / TUPLE_SIZE)

// #define PRINTER
#define ANSI_COLOR_RED              "\x1b[31m"
#define ANSI_COLOR_GREEN            "\x1b[32m"
#define ANSI_COLOR_RESET            "\x1b[0m"
#define PRINT_ERROR(fmt, ...)       fprintf(stderr, "\033[0;31mERROR:\033[0m   " fmt "\n", ##__VA_ARGS__)
#define PRINT_WARNING(fmt, ...)     fprintf(stderr, "\033[0;35mWARNING:\033[0m " fmt "\n", ##__VA_ARGS__)
#define PRINT_INFO(cond, fmt, ...)  if(cond) printf("\033[0;32mINFO:\033[0m    " fmt "\n", ##__VA_ARGS__)
#define PRINT(fmt, ...)             printf(fmt "\n", ##__VA_ARGS__)
#define PRINT_TOP_RULE              printf("\033[0;33m\n====================\033[0m\n");

#ifndef NEXT_POW_2
/** 
 *  compute the next number, greater than or equal to 32-bit unsigned v.
 *  taken from "bit twiddling hacks":
 *  http://graphics.stanford.edu/~seander/bithacks.html
 */
#define NEXT_POW_2(V)                           \
    do {                                        \
        V--;                                    \
        V |= V >> 1;                            \
        V |= V >> 2;                            \
        V |= V >> 4;                            \
        V |= V >> 8;                            \
        V |= V >> 16;                           \
        V++;                                    \
    } while(0)
#endif

#endif