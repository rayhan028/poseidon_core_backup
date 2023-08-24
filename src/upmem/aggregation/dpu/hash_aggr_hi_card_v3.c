#include <stdint.h>
#include <stdio.h>
#include <defs.h>
#include <mram.h>
#include <alloc.h>
#include <handshake.h>
#include <barrier.h>
#include <vmutex.h>
#include <mutex_pool.h>

#include "definitions.h"
#include "hash.h"


#ifndef HASH_BASED_HIGH_CARDINALITY_V3

int main() {
    return 0;
}

#else

__host dpu_params dpu_parameters;

uint32_t mr_sorted_elems_offs;
uint32_t* wr_range_partition_sizes;

/* partition parameters */
__host uint32_t dpu_partition_sizes[NR_PARTITIONS];
/* uint32_t mr_htable_offs; */
uint32_t* wr_partition_sizes;
uint32_t prefix_sum[NR_PARTITIONS];
uint32_t copy_count[NR_PARTITIONS];
// VMUTEX_INIT(part_vmutex, NR_PARTITIONS, 64);
MUTEX_POOL_INIT(part_mutexpl, 8);
// MUTEX_INIT(part_mutex);

/* aggregation parameters */
htable_entry* hash_table;
BARRIER_INIT(aggr_barrier, NR_TASKLETS);
// VMUTEX_INIT(aggr_vmutex, NR_HASH_TABLE_ENTRIES, 16);
MUTEX_POOL_INIT(aggr_mutexpl, 8);
// MUTEX_INIT(aggr_mutex);


int aggregation() {
    unsigned int tasklet_id = me();
    if (tasklet_id == 0) {
        printf("Tasklet: %u\n", tasklet_id);
        mem_reset();

        /* get partition sizes from MRAM */
        wr_partition_sizes = (uint32_t*) mem_alloc(dpu_parameters.num_partitions * sizeof(uint32_t));
        mram_read((__mram_ptr void const*) &((uint32_t*) DPU_MRAM_HEAP_POINTER)[0], wr_partition_sizes, dpu_parameters.num_partitions * sizeof(uint32_t)); /* TODO: increase data size for improved bandwidth utilization */

        /* mr_htable_offs = 0;
        for (uint32_t p = 0; p < dpu_parameters.num_partitions; p++) {
            mr_htable_offs += wr_partition_sizes[p];
        } */

        /* allocate hash table buffer */
        hash_table = (htable_entry*) mem_alloc(NR_HASH_TABLE_ENTRIES * sizeof(htable_entry));
    }
    barrier_wait(&aggr_barrier);


    /* group and compute aggregation for each partition */
    uint32_t part_offs = 0;
    elem_t* mr_elems = (elem_t*) &((uint32_t*) DPU_MRAM_HEAP_POINTER)[dpu_parameters.max_num_partitions];
    elem_t* wr_elems = (elem_t*) mem_alloc(NR_WR_ELEMS_PER_TASKLET_AGGREGATION * ELEM_SIZE);

    for (uint32_t partition = 0; partition < dpu_parameters.num_partitions; partition++) {
        /* reset all hash table entries */
        for (uint32_t idx = tasklet_id; idx < NR_HASH_TABLE_ENTRIES; idx += NR_TASKLETS) {
            hash_table[idx].key = (-1);
        }
        barrier_wait(&aggr_barrier);

        uint32_t part_size = wr_partition_sizes[partition];
        for (uint32_t i = tasklet_id * NR_WR_ELEMS_PER_TASKLET_AGGREGATION; i < part_size; i += NR_WR_ELEMS_PER_TASKLET_AGGREGATION * NR_TASKLETS) {
            mram_read((__mram_ptr void const*) &mr_elems[part_offs + i], wr_elems, NR_WR_ELEMS_PER_TASKLET_AGGREGATION * ELEM_SIZE);

            uint32_t num_elems = ((i + NR_WR_ELEMS_PER_TASKLET_AGGREGATION) < part_size) ?
                                 NR_WR_ELEMS_PER_TASKLET_AGGREGATION :
                                 part_size - i;

            for (uint32_t j = 0; j < num_elems; j++) {
                uint32_t grp_key = wr_elems[j].properties[GROUP_KEY];
                // uint32_t idx = aggr_hash(grp_key) % NR_HASH_TABLE_ENTRIES;
                uint32_t idx = grp_key % NR_HASH_TABLE_ENTRIES;

                uint32_t probe = 0;
                while (1) {
                    // vmutex_lock(&aggr_vmutex, idx);
                    mutex_pool_lock(&aggr_mutexpl, idx);

                    if (hash_table[idx].key == grp_key) { /* hash table entry hit */
                        #if defined(COUNT) || defined(AVERAGE)
                        hash_table[idx].val.cnt++;
                        #endif

                        #if defined(SUM) || defined(AVERAGE)
                        hash_table[idx].val.sum += wr_elems[j].properties[SUM_KEY];
                        #endif

                        #ifdef MINIMUM
                        if (wr_elems[j].properties[MIN_KEY] < hash_table[idx].val.min) {
                            hash_table[idx].val.min = wr_elems[j].properties[MIN_KEY];
                        }
                        #endif

                        #ifdef MAXIMUM
                        if (wr_elems[j].properties[MAX_KEY] > hash_table[idx].val.max) {
                            hash_table[idx].val.max = wr_elems[j].properties[MAX_KEY];
                        }
                        #endif

                        // vmutex_unlock(&aggr_vmutex, idx);
                        mutex_pool_unlock(&aggr_mutexpl, idx);
                        break;
                    }
                    else if (hash_table[idx].key == (uint32_t)(-1)) { /* unused hash table entry */
                        hash_table[idx].key = grp_key;

                        #if defined(COUNT) || defined(AVERAGE)
                        hash_table[idx].val.cnt = 1;
                        #endif

                        #if defined(SUM) || defined(AVERAGE)
                        hash_table[idx].val.sum = wr_elems[j].properties[SUM_KEY];
                        #endif

                        #ifdef MINIMUM
                        hash_table[idx].val.min = (uint32_t)(-1);
                        if (wr_elems[j].properties[MIN_KEY] < hash_table[idx].val.min) {
                            hash_table[idx].val.min = wr_elems[j].properties[MIN_KEY];
                        }
                        #endif

                        #ifdef MAXIMUM
                        hash_table[idx].val.max = 0;
                        if (wr_elems[j].properties[MAX_KEY] > hash_table[idx].val.max) {
                            hash_table[idx].val.max = wr_elems[j].properties[MAX_KEY];
                        }
                        #endif

                        // vmutex_unlock(&aggr_vmutex, idx);
                        mutex_pool_unlock(&aggr_mutexpl, idx);
                        break;
                    }
                    else { /* hash table entry collision */
                        // vmutex_unlock(&aggr_vmutex, idx);
                        mutex_pool_unlock(&aggr_mutexpl, idx);

                        /* linear probing */
                        if (++probe > NR_HASH_TABLE_ENTRIES) {
                            printf("Hash table size exceeded\n");
                            return -1;
                        }

                        if (++idx >= NR_HASH_TABLE_ENTRIES) {
                            idx = 0;
                        }
                    }
                }
            }
        }
        part_offs += part_size;

        /* copy hash table result for the current partition to MRAM */
        barrier_wait(&aggr_barrier);
        for (uint32_t ch = tasklet_id; ch < NR_HASH_TABLE_CHUNKS; ch += NR_TASKLETS) {
            htable_entry* src = hash_table + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
            /* htable_entry* dest = (htable_entry*) &mr_elems[mr_htable_offs] + ((partition * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES); */
            htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions] + ((partition * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES);
            mram_write(src, (__mram_ptr void*) dest, sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
        }
        barrier_wait(&aggr_barrier);
    }

    return 0;
}

int partition() {
    unsigned int tasklet_id = me();

    if (tasklet_id == 0) {
        printf("Tasklet: %u\n", tasklet_id);
        mem_reset();
    }
    barrier_wait(&aggr_barrier);

    for (uint32_t i = tasklet_id; i < NR_PARTITIONS; i += NR_TASKLETS) {
        dpu_partition_sizes[i] = 0;
    }
    for (uint32_t i = tasklet_id; i < NR_PARTITIONS; i += NR_TASKLETS) {
        copy_count[i] = 0;
    }
    barrier_wait(&aggr_barrier);

    elem_t* mr_elems = (elem_t*) DPU_MRAM_HEAP_POINTER;
    elem_t* wr_elems = (elem_t*) mem_alloc(NR_WR_ELEMS_PER_TASKLET_PARTITION * ELEM_SIZE);

    /* read elements from MRAM and compute histogram */
    for (uint32_t i = tasklet_id * NR_WR_ELEMS_PER_TASKLET_PARTITION; i < dpu_parameters.num_elems; i += NR_WR_ELEMS_PER_TASKLET_PARTITION * NR_TASKLETS) {
        mram_read((__mram_ptr void const*) &mr_elems[i], wr_elems, NR_WR_ELEMS_PER_TASKLET_PARTITION * ELEM_SIZE);

        uint32_t num_elems = ((i + NR_WR_ELEMS_PER_TASKLET_PARTITION) < dpu_parameters.num_elems) ?
                             NR_WR_ELEMS_PER_TASKLET_PARTITION :
                             dpu_parameters.num_elems - i; /* last remaining elements less than NR_WR_ELEMS_PER_TASKLET_PARTITION */

        for (uint32_t j = 0; j < num_elems; j++) {
            prop_code_t grp_key = wr_elems[j].properties[GROUP_KEY];
            // uint32_t partition = global_partition_hash(grp_key) % NR_PARTITIONS;
            uint32_t partition = grp_key % NR_PARTITIONS;

            // vmutex_lock(&part_vmutex, idx);
            mutex_pool_lock(&part_mutexpl, partition);
            dpu_partition_sizes[partition]++;
            // vmutex_unlock(&part_vmutex, idx);
            mutex_pool_unlock(&part_mutexpl, partition);
        }
    }
    barrier_wait(&aggr_barrier);

#if 1 /* partition the data in MRAM */

    /* compute prefix sums */
    if (tasklet_id == 0) {
        uint32_t prefix = 0;
        for (uint32_t p = 0; p < NR_PARTITIONS; p++) {   
            prefix_sum[p] = prefix;
            prefix += dpu_partition_sizes[p];
        }
    }
    barrier_wait(&aggr_barrier);

    /* if (tasklet_id == 0) {
        prefix_sum[0] = 0;
    }

    for (uint32_t p = tasklet_id; p < NR_PARTITIONS; p += NR_TASKLETS) {
        if (tasklet_id != 0) {
            handshake_wait_for(tasklet_id - 1);
        }
        prefix_sum[p + 1] = prefix_sum[p] + dpu_partition_sizes[p];
        if (tasklet_id < NR_TASKLETS - 1) {
            handshake_notify();
        }
        barrier_wait(&aggr_barrier);
    } */

    /* read elements for the second time from MRAM and copy them to their MRAM partition buffers */
    for (uint32_t i = tasklet_id * NR_WR_ELEMS_PER_TASKLET_PARTITION; i < dpu_parameters.num_elems; i += NR_WR_ELEMS_PER_TASKLET_PARTITION * NR_TASKLETS) {
        mram_read((__mram_ptr void const*) &mr_elems[i], wr_elems, NR_WR_ELEMS_PER_TASKLET_PARTITION * ELEM_SIZE);

        uint32_t num_elems = ((i + NR_WR_ELEMS_PER_TASKLET_PARTITION) < dpu_parameters.num_elems) ?
                             NR_WR_ELEMS_PER_TASKLET_PARTITION :
                             dpu_parameters.num_elems - i; /* last remaining elements less than NR_WR_ELEMS_PER_TASKLET_PARTITION */

        for (uint32_t j = 0; j < num_elems; j++) {
            prop_code_t grp_key = wr_elems[j].properties[GROUP_KEY];
            // uint32_t partition = glb_partition_hash(grp_key) % NR_PARTITIONS;
            uint32_t partition = grp_key % NR_PARTITIONS;

            // vmutex_lock(&part_vmutex, idx);
            mutex_pool_lock(&part_mutexpl, partition);
            uint32_t mroffs = prefix_sum[partition] + copy_count[partition];
            copy_count[partition]++;
            // vmutex_unlock(&part_vmutex, idx);
            mutex_pool_unlock(&part_mutexpl, partition);
            mram_write(&wr_elems[j], (__mram_ptr void*) &mr_elems[dpu_parameters.max_num_elems + mroffs], ELEM_SIZE);
        }
    }
    barrier_wait(&aggr_barrier);
#endif

    return 0;
}

int (*kernels[NR_KERNELS])() = {partition, aggregation};

int main() {
    return kernels[dpu_parameters.phase]();
}

#endif /* #ifndef HASH_BASED_HIGH_CARDINALITY_V3 */
