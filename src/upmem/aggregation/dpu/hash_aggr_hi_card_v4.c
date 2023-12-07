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


#ifndef HASH_BASED_HIGH_CARDINALITY_V4

int main() {
    return 0;
}

#else

__host dpu_params dpu_parameters;
__host uint32_t dpu_num_hash_tables;

htable_entry* hash_table;
uint32_t local_max = 0;
uint32_t local_min = (-1);

BARRIER_INIT(aggr_barrier, NR_TASKLETS);
// VMUTEX_INIT(aggr_vmutex, NR_HASH_TABLE_ENTRIES, 16);
MUTEX_POOL_INIT(aggr_mutexpl, 8);
// MUTEX_INIT(aggr_mutex);

int aggregation() {
    unsigned int tasklet_id = me();

    if (tasklet_id == 0) {
        printf("Tasklet: %d\n", tasklet_id);
        mem_reset();

        /* allocate hash table buffer */
        hash_table = (htable_entry*) mem_alloc(HASH_TABLE_SIZE);
    }
    barrier_wait(&aggr_barrier);

    uint32_t max_key = 0;
    uint32_t min_key = (-1);
    mrnode* mr_elems = (mrnode*) DPU_MRAM_HEAP_POINTER;
    mrnode* wr_elems = (mrnode*) mem_alloc(NR_WR_ELEMS_PER_TASKLET * ELEM_SIZE);

    /* obtain DPU-local maximum and minimum grouping keys */
    for (uint32_t i = tasklet_id * NR_WR_ELEMS_PER_TASKLET; i < dpu_parameters.num_elems; i += NR_WR_ELEMS_PER_TASKLET * NR_TASKLETS) {
        mram_read((__mram_ptr void const*) &mr_elems[i], wr_elems, NR_WR_ELEMS_PER_TASKLET * ELEM_SIZE);

        uint32_t num_elems = ((i + NR_WR_ELEMS_PER_TASKLET) < dpu_parameters.num_elems) ?
                                NR_WR_ELEMS_PER_TASKLET :
                                dpu_parameters.num_elems - i;

        for (uint32_t j = 0; j < num_elems; j++) {
            uint32_t grp_key = wr_elems[j].properties[GROUP_KEY];
            // uint32_t idx = aggr_hash(grp_key) % NR_HASH_TABLE_ENTRIES;
            // uint32_t idx = grp_key;

            if (grp_key > max_key) {
                max_key = grp_key;
            }
            if (grp_key < min_key) {
                min_key = grp_key;
            }

        }
    }
    barrier_wait(&aggr_barrier);

    // vmutex_lock(&aggr_vmutex, 0);
    mutex_pool_lock(&aggr_mutexpl, 0);
    if (max_key > local_max) {
        local_max = max_key;
    }
    if (min_key < local_min) {
        local_min = min_key;
    }
    // vmutex_unlock(&aggr_vmutex, 0);
    mutex_pool_unlock(&aggr_mutexpl, 0);
    barrier_wait(&aggr_barrier);

    uint32_t max_num_unique_keys = local_max - local_min + 1;
    uint32_t max_num_htables = DIVCEIL(max_num_unique_keys, NR_HASH_TABLE_ENTRIES); /* TODO: sparse grouping keys */
    if (tasklet_id == 0) {
        dpu_num_hash_tables = max_num_htables;
    }

    for (uint32_t batch = 0; batch < max_num_htables; batch++) {
        /* reset all hash table entries */
        for (uint32_t idx = tasklet_id; idx < NR_HASH_TABLE_ENTRIES; idx += NR_TASKLETS) {
            hash_table[idx].key = (-1);
        }
        barrier_wait(&aggr_barrier);

        /* compute aggregation for grouping keys of the current batch */
        for (uint32_t i = tasklet_id * NR_WR_ELEMS_PER_TASKLET; i < dpu_parameters.num_elems; i += NR_WR_ELEMS_PER_TASKLET * NR_TASKLETS) {
            mram_read((__mram_ptr void const*) &mr_elems[i], wr_elems, NR_WR_ELEMS_PER_TASKLET * ELEM_SIZE);

            uint32_t num_elems = ((i + NR_WR_ELEMS_PER_TASKLET) < dpu_parameters.num_elems) ?
                                 NR_WR_ELEMS_PER_TASKLET :
                                 (dpu_parameters.num_elems - i);

            for (uint32_t j = 0; j < num_elems; j++) {
                uint32_t grp_key = wr_elems[j].properties[GROUP_KEY];
                // uint32_t idx = aggr_hash(grp_key) % NR_HASH_TABLE_ENTRIES;
                uint32_t b = (grp_key - local_min) / NR_HASH_TABLE_ENTRIES;

                if (b == batch) { /* compute aggregation for element if the grouping key belongs to the current batch */
#ifdef SIMPLE_HASH
                    uint32_t idx = grp_key % NR_HASH_TABLE_ENTRIES;
#elif defined(TABULATION_HASH)
                    uint32_t idx = ((NR_HASH_TABLE_ENTRIES - 1) & join_hash(grp_key));
#endif

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
        }

        /* copy hash table result to MRAM */
        barrier_wait(&aggr_barrier);
        for (uint32_t ch = tasklet_id; ch < NR_HASH_TABLE_CHUNKS; ch += NR_TASKLETS) {
            htable_entry* src = hash_table + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
            htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.htable_offset] + (batch * NR_HASH_TABLE_ENTRIES) + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
            mram_write(src, (__mram_ptr void*) dest, sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
        }
        barrier_wait(&aggr_barrier);
    }

    return 0;
}

int (*kernels[NR_KERNELS])() = {aggregation};

int main() {
    return kernels[0]();
}

#endif /* #ifndef HASH_BASED_HIGH_CARDINALITY_V4 */
