#include <stdint.h>
#include <stdio.h>
#include <defs.h>
#include <mram.h>
#include <alloc.h>
#include <handshake.h>
#include <barrier.h>
#include <vmutex.h>
#include <mutex_pool.h>

#include "../include/definitions.h"
#include "../include/hash.h"


#ifndef HASH_BASED_LOW_CARDINALITY

int main() {
    return 0;
}

#else

__host dpu_params dpu_parameters;

htable_entry* hash_table;
uint32_t mr_sorted_elems_offs;
uint32_t* wr_range_partition_sizes;

BARRIER_INIT(aggr_barrier, NR_TASKLETS);
// VMUTEX_INIT(aggr_vmutex, NR_HASH_TABLE_ENTRIES, 16);
MUTEX_POOL_INIT(aggr_mutexpl, 8);
// MUTEX_INIT(aggr_mutex);


#ifdef PER_TASKLET_HASH_TABLE /* a separate hash table for each tasklet */

int aggregation() {
    unsigned int tasklet_id = me();

    if (tasklet_id == 0) {
        printf("Tasklet: %d\n", tasklet_id);
        mem_reset();
    }
    barrier_wait(&aggr_barrier);

    /* allocate hash table buffer and reset all hash table entries */
    htable_entry* htable = (htable_entry*) mem_alloc(HASH_TABLE_SIZE);

    /* reset all hash table entries */
    for (uint32_t idx = 0; idx < NR_HASH_TABLE_ENTRIES; idx++) {
        htable[idx].key = (-1);
    }

    /* group and compute aggregation */
    mrnode* mr_elems = (mrnode*) DPU_MRAM_HEAP_POINTER;
    mrnode* wr_elems = (mrnode*) mem_alloc(NR_WR_ELEMS_PER_TASKLET * ELEM_SIZE);
    for (uint32_t i = tasklet_id * NR_WR_ELEMS_PER_TASKLET; i < dpu_parameters.num_elems; i += NR_WR_ELEMS_PER_TASKLET * NR_TASKLETS) {
        mram_read((__mram_ptr void const*) &mr_elems[i], wr_elems, NR_WR_ELEMS_PER_TASKLET * ELEM_SIZE);

        uint32_t num_elems = ((i + NR_WR_ELEMS_PER_TASKLET) < dpu_parameters.num_elems) ?
                             NR_WR_ELEMS_PER_TASKLET :
                             dpu_parameters.num_elems - i;

        for (uint32_t j = 0; j < num_elems; j++) {
            uint32_t grp_key = wr_elems[j].properties[GROUP_KEY];
            // uint32_t idx = aggr_hash(grp_key) % NR_HASH_TABLE_ENTRIES;
            uint32_t idx = grp_key;

            uint32_t probe = 0;
            while (1) {
                if (htable[idx].key == grp_key) {
                    #if defined(COUNT) || defined(AVERAGE)
                    htable[idx].val.cnt++;
                    #endif

                    #if defined(SUM) || defined(AVERAGE)
                    htable[idx].val.sum += wr_elems[j].properties[SUM_KEY];
                    #endif

                    #ifdef MINIMUM
                    if (wr_elems[j].properties[MIN_KEY] < htable[idx].val.min) {
                        htable[idx].val.min = wr_elems[j].properties[MIN_KEY];
                    }
                    #endif

                    #ifdef MAXIMUM
                    if (wr_elems[j].properties[MAX_KEY] > htable[idx].val.max) {
                        htable[idx].val.max = wr_elems[j].properties[MAX_KEY];
                    }
                    #endif

                    break;
                }
                else if (htable[idx].key == (uint32_t)(-1)) {
                    htable[idx].key = grp_key;

                    #if defined(COUNT) || defined(AVERAGE)
                    htable[idx].val.cnt = 1;
                    #endif

                    #if defined(SUM) || defined(AVERAGE)
                    htable[idx].val.sum = wr_elems[j].properties[SUM_KEY];
                    #endif

                    #ifdef MINIMUM
                    htable[idx].val.min = (uint32_t)(-1);
                    if (wr_elems[j].properties[MIN_KEY] < htable[idx].val.min) {
                        htable[idx].val.min = wr_elems[j].properties[MIN_KEY];
                    }
                    #endif

                    #ifdef MAXIMUM
                    htable[idx].val.max = (uint32_t)0;
                    if (wr_elems[j].properties[MAX_KEY] > htable[idx].val.max) {
                        htable[idx].val.max = wr_elems[j].properties[MAX_KEY];
                    }
                    #endif

                    break;
                }
                else {
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

    /* copy hash table result for the current tasklet to MRAM */
    htable_entry* ptr = (htable_entry*) &mr_elems[dpu_parameters.htable_offset] + (tasklet_id * NR_HASH_TABLE_ENTRIES);
    mram_write(htable, (__mram_ptr void*) ptr, HASH_TABLE_SIZE);

    return 0;
}

#elif defined SINGLE_HASH_TABLE /* a single hash table shared by all tasklets */

int aggregation() {
    unsigned int tasklet_id = me();

    if (tasklet_id == 0) {
        printf("Tasklet: %d\n", tasklet_id);
        mem_reset();

        /* allocate hash table buffer */
        hash_table = (htable_entry*) mem_alloc(HASH_TABLE_SIZE);
    }
    barrier_wait(&aggr_barrier);

    /* reset all hash table entries */
    for (uint32_t idx = tasklet_id; idx < NR_HASH_TABLE_ENTRIES; idx += NR_TASKLETS) {
        hash_table[idx].key = (-1);
    }
    barrier_wait(&aggr_barrier);

    /* group and compute aggregation */
    mrnode* mr_elems = (mrnode*) DPU_MRAM_HEAP_POINTER;
    mrnode* wr_elems = (mrnode*) mem_alloc(NR_WR_ELEMS_PER_TASKLET * ELEM_SIZE);
    for (uint32_t i = tasklet_id * NR_WR_ELEMS_PER_TASKLET; i < dpu_parameters.num_elems; i += NR_WR_ELEMS_PER_TASKLET * NR_TASKLETS) {
        mram_read((__mram_ptr void const*) &mr_elems[i], wr_elems, NR_WR_ELEMS_PER_TASKLET * ELEM_SIZE);

        uint32_t num_elems = ((i + NR_WR_ELEMS_PER_TASKLET) < dpu_parameters.num_elems) ?
                                NR_WR_ELEMS_PER_TASKLET :
                                dpu_parameters.num_elems - i;

        for (uint32_t j = 0; j < num_elems; j++) {
            uint32_t grp_key = wr_elems[j].properties[GROUP_KEY];
            // uint32_t idx = aggr_hash(grp_key) % NR_HASH_TABLE_ENTRIES;
            uint32_t idx = grp_key;

            uint32_t probe = 0;
            while (1) {
                // vmutex_lock(&aggr_vmutex, idx);
                mutex_pool_lock(&aggr_mutexpl, idx);
                if (hash_table[idx].key == grp_key) {
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
                else if (hash_table[idx].key == (uint32_t)(-1)) {
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
                    hash_table[idx].val.max = (uint32_t)0;
                    if (wr_elems[j].properties[MAX_KEY] > hash_table[idx].val.max) {
                        hash_table[idx].val.max = wr_elems[j].properties[MAX_KEY];
                    }
                    #endif

                    // vmutex_unlock(&aggr_vmutex, idx);
                    mutex_pool_unlock(&aggr_mutexpl, idx);
                    break;
                }
                else {
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

    /* copy hash table result to MRAM */
    barrier_wait(&aggr_barrier);
    for (uint32_t ch = tasklet_id; ch < NR_HASH_TABLE_CHUNKS; ch += NR_TASKLETS) {
        htable_entry* src = hash_table + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
        htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.htable_offset] + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
        mram_write(src, (__mram_ptr void*) dest, sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
    }
    barrier_wait(&aggr_barrier);

    return 0;
}
#endif /* #ifdef PER_TASKLET_HASH_TABLE */

int (*kernels[NR_KERNELS])() = {aggregation};

int main() {
    return kernels[0]();
}

#endif /* #ifndef LOW_CARDINALITY */
