#include <stdint.h>
#include <stdio.h>
#include <defs.h>
#include <mram.h>
#include <alloc.h>
#include <handshake.h>
#include <barrier.h>

#include "../common/common.h"

__mram_noinit struct dpu_params dpu_args; // TODO: place in WRAM
__mram_noinit struct mrchunk mr_chunks[MAX_CHUNKS_PER_DPU];

#if defined(COUNT) || defined(AVERAGE)
__mram_noinit uint64_t dpu_cnt_res[NR_TASKLETS];
__mram_noinit uint64_t dpu_cnt_val; // TODO: place in WRAM
#endif

#if defined(SUM) || defined(AVERAGE)
__mram_noinit uint64_t dpu_sum_res[NR_TASKLETS];
__mram_noinit uint64_t dpu_sum_val;
#endif

#ifdef AVERAGE
__mram_noinit double dpu_avg_res[NR_TASKLETS];
__mram_noinit double dpu_avg_val;
#endif

#ifdef MINIMUM
__mram_noinit uint64_t dpu_min_res[NR_TASKLETS];
__mram_noinit uint64_t dpu_min_val;
#endif

#ifdef MAXIMUM
__mram_noinit uint64_t dpu_max_res[NR_TASKLETS];
__mram_noinit uint64_t dpu_max_val;
#endif


// __host struct dpu_arg dpu_args;
// __host struct dpu_aggr_res DPU_RESULTS[NR_TASKLETS];

// __mram_noinit struct mrchunk dpu_chunks[MAX_CHUNKS_PER_DPU];

BARRIER_INIT(aggr_barrier, NR_TASKLETS);

void compute_aggr() {
    uint64_t cnt = 0;
    uint64_t sum = 0;
    double avg = 0.0;
    uint64_t min = 18446744073709551615ul;
    uint64_t max = 0;

    for (unsigned int i = 0; i < NR_TASKLETS; i++) {
        #if defined(COUNT) || defined(AVERAGE)
        cnt += dpu_cnt_res[i];
        #endif

        #if defined(SUM) || defined(AVERAGE)
        sum += dpu_sum_res[i];
        #endif

        #ifdef MINIMUM
        if (min > dpu_min_res[i]) {
            min = dpu_min_res[i];
        }
        #endif

        #ifdef MAXIMUM
        if (max < dpu_max_res[i]) {
            max = dpu_max_res[i];
        }
        #endif
    }

    #if defined(COUNT) || defined(AVERAGE)
    dpu_cnt_val = cnt;
    #endif

    #if defined(SUM) || defined(AVERAGE)
    dpu_sum_val = sum;
    #endif

    #ifdef AVERAGE
    if (dpu_cnt_val > 0) {
        avg = dpu_sum_val / (double)dpu_cnt_val;
    }
    dpu_avg_val = avg;
    #endif

    #ifdef MINIMUM
    dpu_min_val = min;
    #endif

    #ifdef MAXIMUM
    dpu_max_val = max;
    #endif
}

int aggr_kernel() {
    unsigned int tasklet_id = me();
    // printf("Tasklet: %d\n", tasklet_id);

    // struct mrchunk * ch = (struct mrchunk *)&mr_chunk_buf[0];
    // printf("node id_: %lu\n", ch->data[1].id_);

    // initialize tasklet result buffers
    #if defined(COUNT) || defined(AVERAGE)
    dpu_cnt_res[tasklet_id] = 0;
    #endif

    #if defined(SUM) || defined(AVERAGE)
    dpu_sum_res[tasklet_id] = 0;
    #endif

    #ifdef AVERAGE
    dpu_avg_res[tasklet_id] = 0.0;
    #endif

    #ifdef MINIMUM
    dpu_min_res[tasklet_id] = 18446744073709551615ul;
    #endif

    #ifdef MAXIMUM
    dpu_max_res[tasklet_id] = 0;
    #endif

    for (unsigned int ch = 0; ch < dpu_args.chunks; ch++) {
        for (unsigned int i = tasklet_id; i < ELEMENTS_PER_CHUNK; i += NR_TASKLETS) {
            #if defined(COUNT) || defined(AVERAGE)
            dpu_cnt_res[tasklet_id]++;
            #endif

            #if defined(SUM) || defined(AVERAGE)
            dpu_sum_res[tasklet_id] += mr_chunks[ch].data[i].id_;
            #endif

            #ifdef MINIMUM
            if (dpu_min_res[tasklet_id] > mr_chunks[ch].data[i].id_) {
                dpu_min_res[tasklet_id] = mr_chunks[ch].data[i].id_;
            }
            #endif

            #ifdef MAXIMUM
            if (dpu_max_res[tasklet_id] < mr_chunks[ch].data[i].id_) {
                dpu_max_res[tasklet_id] = mr_chunks[ch].data[i].id_;
            }
            #endif
        }
    }

    barrier_wait(&aggr_barrier);
    if (tasklet_id == 0) { // first tasklet collects the DPU local results
        compute_aggr();
        printf("Processed chunks: %lu\n", dpu_args.chunks);
    }

// #ifdef PRINTER
//     printf("Tasklet: %d\n", tasklet_id);
//     printf("CACHE_SIZE_BYTES: %d\n", CACHE_SIZE_BYTES);
//     printf("NR_TASKLETS: %d\n", NR_TASKLETS);
// #endif
//     if (tasklet_id == 0) {
//         mem_reset();
//     }

//     barrier_wait(&aggr_barrier);
//     struct dpu_aggr_res* result = &DPU_RESULTS[tasklet_id];
//     uint32_t num_chunks = dpu_args.chunks;
//     // uint32_t elems_per_chunk = dpu_args.elems_per_chunk;
//     uint32_t chunks_size_bytes = num_chunks * sizeof(struct mrchunk);

//     uint32_t tasklet_beg_addr = tasklet_id * CACHE_SIZE_BYTES;
//     printf("Tasklet: %u, tasklet_beg_addr: %u\n", tasklet_id, tasklet_beg_addr);

//     uint32_t chunks_beg_addr = (uint32_t)DPU_MRAM_HEAP_POINTER;

//     uint8_t* cache = (uint8_t*) mem_alloc(CACHE_SIZE_BYTES);

//     barrier_wait(&aggr_barrier);

//     printf("chunks_size_bytes: %d\n", chunks_size_bytes);

//     for (uint32_t c = 0; c < num_chunks; c++) {
//         uint32_t chunk_addr = (uint32_t)(DPU_MRAM_HEAP_POINTER + c * sizeof(struct mrchunk));

//         uint32_t start_node = tasklet_id * CACHE_SIZE;
//         for (uint32_t n = start_node; n < ELEMENTS_PER_CHUNK; n += (CACHE_SIZE * NR_TASKLETS)) {
//             printf("tasklet_id = %u, start_node = %u\n", tasklet_id, start_node);
//             // printf("tasklet_id = %u, byte_index = %u\n", tasklet_id, byte_index);
//         }
//     }


/*

817 / 19 = 43 nodes per tasklet


43 * 80 = 3440 bytes per cache

*/
    return 0;
}

int (*kernels[NUM_KERNELS])() = {aggr_kernel};

int main() {
    return kernels[0]();
}