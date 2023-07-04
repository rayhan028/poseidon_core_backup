#include <stdint.h>
#include <stdio.h>
#include <defs.h>
#include <mram.h>
#include <alloc.h>
#include <handshake.h>
#include <barrier.h>

#include <string.h>
#include <stdalign.h>

#include "../common/common.h"

#define sss 64


__host struct dpu_params parameters;

/* partition parameters */
__host uint32_t partition_sizes[NR_PARTITIONS];

uint32_t prefix_sum[NR_TASKLETS + 1][NR_PARTITIONS];
uint32_t histogram[NR_TASKLETS][NR_PARTITIONS];
uint32_t copy_count[NR_TASKLETS][NR_PARTITIONS];

/* join parameters */


BARRIER_INIT(join_barrier, NR_TASKLETS);

int join() {
    unsigned int tasklet_id = me();
    // printf("Tasklet: %u, Stack: %u\n", tasklet_id, check_stack());
    uint32_t matches = 0;

    if (tasklet_id == 0) {
        mem_reset();
    }
    barrier_wait(&join_barrier);

    uint32_t alloc_size;
    uint32_t num_parts = parameters.num_partitions;

    /* get the partition sizes and compute the partition offsets in MRAM */
    alloc_size = 2 * (num_parts + 1) * sizeof(uint64_t);
    uint64_t* part_offs = (uint64_t*) mem_alloc(alloc_size);
    mram_read((__mram_ptr void const*) DPU_MRAM_HEAP_POINTER, part_offs, alloc_size);

    uint32_t prev_offs = 0;
    for (uint32_t i = 0; i < (2 * (num_parts + 1)); i++) {
        /* TODO: to be done by one tasklet on a global array */
        /* TODO: transfer the partition offsets alongside the
            partition sizes from the CPU */
        uint32_t tmp_offs = part_offs[i];
        part_offs[i] = prev_offs;
        prev_offs += tmp_offs;
    }

    alloc_size = WR_TUPLES * TUPLE_SIZE;
    /* TODO: have separate WR_TUPLES_PART and WR_TUPLES_JOIN */
    mrtuple* tuples_wr = (mrtuple*) mem_alloc(alloc_size);
    mrtuple* tmp_tuples_wr = (mrtuple*) mem_alloc(alloc_size);

    /* each tasklet processes a partition */
    for (uint32_t i = tasklet_id; i < num_parts; i += NR_TASKLETS) {
        /* get the partition from MRAM and build the hash table */
        uint32_t rtidx = 0, stidx = 0;
        const uint32_t MASK = (BUCKET_SIZE - 1) << (NUM_RADIX_BITS);
        
        uint32_t offs, sz;
        uint32_t roffs = part_offs[2 * i];
        uint32_t soffs = part_offs[2 * i + 1];

        /* read the R partition */
        uint32_t rsize = (uint32_t)(soffs - roffs);
        uint32_t ssize = (uint32_t)(part_offs[2 * (i + 1)] - soffs);
        /* uint8_t next[rsize]; */
        uint8_t next[BUCKET_SIZE]; /* TODO: if mem_alloc would be better */
        uint32_t bsize = rsize;
        NEXT_POW_2(bsize);
        /* uint32_t bucket[bsize]; */
        uint32_t bucket[BUCKET_SIZE]; /* TODO: if mem_alloc would be better */
        memset(bucket, 0, sizeof(bucket));

        mrtuple* tuples = (mrtuple*) DPU_MRAM_HEAP_POINTER + num_parts + roffs; /* mrtuples == 2 * uint64_t */

        offs = roffs;
        sz = rsize; /* remaining partition size to be read */
        // tuples = tuples + offs;
        while ((offs - roffs) < rsize) {
            uint32_t rsize_wr = sz > WR_TUPLES ? WR_TUPLES : sz;
            /* TODO: have separate WR_TUPLES_PART and WR_TUPLES_JOIN */
            printf("Tasklet: %u, num_partitions slots: %u, offs: %u, sz: %u, rsize_wr: %u\n",
                    tasklet_id, num_parts, offs, sz, rsize_wr);
            offs += rsize_wr;
            sz -= rsize_wr;

            alloc_size = rsize_wr * TUPLE_SIZE;
            mram_read((__mram_ptr void const*) tuples, tuples_wr, alloc_size);

            for (unsigned int k = 0; k < rsize_wr; k++) {
                printf("%lu : %lu\t", tuples_wr[k].key, tuples_wr[k].tid);
                uint32_t part = HASH_BIT_MODULO(tuples_wr[k].key, MASK, NUM_RADIX_BITS);
                next[rtidx] = bucket[part];
                bucket[part] = ++i;
                rtidx++;
            }
            printf("\n");

            tuples = tuples + rsize_wr;
        }

        offs = soffs;
        sz = ssize; /* remaining partition size to be read */
        while ((offs - soffs) < ssize) {
            uint32_t ssize_wr = sz > WR_TUPLES ? WR_TUPLES : sz;
            /* TODO: have separate WR_TUPLES_PART and WR_TUPLES_JOIN */
            printf("Tasklet: %u, num_partitions slots: %u, offs: %u, sz: %u, ssize_wr: %u\n",
                    tasklet_id, num_parts, offs, sz, ssize_wr);
            offs += ssize_wr;
            sz -= ssize_wr;

            alloc_size = ssize_wr * TUPLE_SIZE;
            mram_read((__mram_ptr void const*) tuples, tuples_wr, alloc_size);

            for (unsigned int k = 0; k < ssize_wr; k++) {
                printf("%lu : %lu\t", tuples_wr[k].key, tuples_wr[k].tid);
                uint32_t part = HASH_BIT_MODULO(tuples_wr[k].key, MASK, NUM_RADIX_BITS);

                for (uint32_t hit = bucket[part]; hit > 0; hit = next[hit - 1]) {

                    uint32_t tmp_rtidx = 0;
                    mrtuple* tmp_tuples = (mrtuple*) DPU_MRAM_HEAP_POINTER + num_parts + roffs; /* mrtuples == 2 * uint64_t */

                    uint32_t tmp_offs = roffs;
                    uint32_t tmp_sz = rsize; /* remaining partition size to be read */
                    // tuples = tuples + offs;
                    while ((tmp_offs - roffs) < rsize) {
                        uint32_t rsize_wr = tmp_sz > WR_TUPLES ? WR_TUPLES : tmp_sz;
                        /* TODO: have separate WR_TUPLES_PART and WR_TUPLES_JOIN */
                        // printf("Tasklet: %u, num_partitions slots: %u, tmp_offs: %u, tmp_sz: %u, rsize_wr: %u\n",
                        //         tasklet_id, num_parts, tmp_offs, tmp_sz, rsize_wr);
                        tmp_offs += rsize_wr;
                        tmp_sz -= rsize_wr;

                        alloc_size = rsize_wr * TUPLE_SIZE;
                        mram_read((__mram_ptr void const*) tmp_tuples, tmp_tuples_wr, alloc_size);

                        for (unsigned int m = 0; m < rsize_wr; m++) {
                            // printf("%lu : %lu\t", tmp_tuples_wr[m].key, tmp_tuples_wr[m].tid);
                            
                            if (tmp_rtidx == (hit - 1)) {

                                if (tuples_wr[k].key == tmp_tuples_wr[m].key) {
                                    matches++;
                                }
                            }
                            tmp_rtidx++;
                        }
                        // printf("\n");

                        tmp_tuples = tmp_tuples + rsize_wr;
                    }
                }

                stidx++;
            }
            printf("\n");

            tuples = tuples + ssize_wr;
        }
    }
    printf("Taskelt: %u, Matches: %u\n", tasklet_id, matches);

    return 0;
}

int partition() {
    unsigned int tasklet_id = me();

    if (tasklet_id == 0) {
        // printf("Tasklet: %u\n", tasklet_id);
        mem_reset();
    }
    // uint32_t* prf_sum = &prefix_sum[tasklet_id][0];
    // for (unsigned int i = 0; i < NR_PARTITIONS; i++) {
    //     prf_sum[i] = 0;
    // }
    for (unsigned int i = 0; i < NR_PARTITIONS; i++) {
        histogram[tasklet_id][i] = 0;
    }
    for (unsigned int i = 0; i < NR_PARTITIONS; i++) {
        copy_count[tasklet_id][i] = 0;
    }

    unsigned int wrtuples = parameters.num_tuples > WR_TUPLES
                          ? WR_TUPLES : parameters.num_tuples;
    struct mrtuple* tuples = (struct mrtuple*) DPU_MRAM_HEAP_POINTER;
    struct mrtuple* incache = (struct mrtuple*) mem_alloc(wrtuples * TUPLE_SIZE);

    // compute histogram
    unsigned int tasklet_start = tasklet_id * wrtuples;
    for (unsigned int i = tasklet_start; i < parameters.num_tuples; i += wrtuples * NR_TASKLETS) {
        mram_read((__mram_ptr void const*) &tuples[i], incache, wrtuples * TUPLE_SIZE);  // TODO: read MAX_WR_TUPLES

        for (unsigned int t = 0; t < wrtuples; t++) {
            unsigned int partition = incache[t].key % NR_PARTITIONS; // TODO: radix
            histogram[tasklet_id][partition]++;
        }
    }

    // first partition
    if (tasklet_id == 0) {
        prefix_sum[tasklet_id][0] = 0;
    }
    else {
        handshake_wait_for(tasklet_id - 1);
    }
    if (tasklet_id < NR_TASKLETS - 1) {
        prefix_sum[tasklet_id + 1][0] = prefix_sum[tasklet_id][0] + histogram[tasklet_id][0];
        handshake_notify();
    }
    else {
        // end of first partition
        prefix_sum[tasklet_id + 1][0] = prefix_sum[tasklet_id][0] + histogram[tasklet_id][0];
    }

    // remaining partitions
    for (unsigned int i = 1; i < NR_PARTITIONS; i++) {
        barrier_wait(&join_barrier);
        if (tasklet_id == 0) {
            // beginning of current partition = end of previous partition
            prefix_sum[0][i] = prefix_sum[NR_TASKLETS][i - 1];
        }
        else {
            handshake_wait_for(tasklet_id - 1);
        }
        if (tasklet_id < NR_TASKLETS - 1) {
            prefix_sum[tasklet_id + 1][i] = prefix_sum[tasklet_id][i] + histogram[tasklet_id][i];
            handshake_notify();
        }
        if (tasklet_id == NR_TASKLETS - 1) {
            // end of current partition
            prefix_sum[tasklet_id + 1][i] = prefix_sum[tasklet_id][i] + histogram[tasklet_id][i];
        }
    }

    // copy partitions
    // struct mrtuple* outcache = (struct mrtuple*) mem_alloc(WR_TUPLES * TUPLE_SIZE); // TODO: coalesce tuples starting from the largest partition
    for (unsigned int i = tasklet_start; i < parameters.num_tuples; i += wrtuples * NR_TASKLETS) {
        mram_read((__mram_ptr void const*) &tuples[i], incache, wrtuples * TUPLE_SIZE);
        // printf("Tasklet %d --- %lu : %lu\n", tasklet_id, incache[0].key, incache[0].tid);

        for (unsigned int t = 0; t < wrtuples; t++) {
            unsigned int partition = incache[t].key % NR_PARTITIONS; // TODO: radix
            unsigned int offs = prefix_sum[tasklet_id][partition] + copy_count[tasklet_id][partition];
            copy_count[tasklet_id][partition]++;
            mram_write(&incache[t], (__mram_ptr void*) &tuples[parameters.num_tuples + offs], TUPLE_SIZE);
        }
    }

    // get partition sizes
    if (tasklet_id == NR_TASKLETS - 1) {
        partition_sizes[0] = prefix_sum[NR_TASKLETS][0];
        for (unsigned int i = 1; i < NR_PARTITIONS; i++) {
            partition_sizes[i] = prefix_sum[NR_TASKLETS][i] - prefix_sum[NR_TASKLETS][i - 1];
        }
    }

    return 0;
}

int (*kernels[NUM_KERNELS])() = {partition, join};

int main() {
    return kernels[parameters.phase]();
}