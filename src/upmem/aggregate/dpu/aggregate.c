#include <stdint.h>
#include <stdio.h>
#include <defs.h>
#include <mram.h>
#include <alloc.h>
#include <handshake.h>
#include <barrier.h>
#include <vmutex.h>
#include <mutex_pool.h>

#include "../common/common.h"
#include "../common/hash.h"


__host dpu_params dpu_parameters;
__host aggr_res dpu_results;
__host uint32_t dpu_partition_sizes[NR_PARTITIONS];

/* partition parameters */
uint32_t histogram[NR_TASKLETS][NR_PARTITIONS];
uint32_t prefix_sum[NR_TASKLETS + 1][NR_PARTITIONS];
/* TODO: uint32_t prefix_sum[NR_PARTITIONS][NR_TASKLETS + 1]; */
uint32_t copy_count[NR_TASKLETS][NR_PARTITIONS];

/* aggregation parameters */
htable_entry* hash_table;
uint32_t* wr_partition_sizes;
uint32_t mr_htable_offs;

#if defined(COUNT) || defined(AVERAGE)
aggr_val_t dpu_cnt_res[NR_TASKLETS];
#endif

#if defined(SUM) || defined(AVERAGE)
aggr_val_t dpu_sum_res[NR_TASKLETS];
#endif

#ifdef AVERAGE
double dpu_avg_res[NR_TASKLETS];
#endif

#ifdef MINIMUM
aggr_val_t dpu_min_res[NR_TASKLETS];
#endif

#ifdef MAXIMUM
aggr_val_t dpu_max_res[NR_TASKLETS];
#endif

BARRIER_INIT(aggr_barrier, NR_TASKLETS);
// VMUTEX_INIT(aggr_vmutex, NR_HASH_TABLE_ENTRIES, 16);
MUTEX_POOL_INIT(aggr_mutexpl, 8);
// MUTEX_INIT(aggr_mutex);


#ifdef HIGH_CARDINALITY /* partition data such that hash table for each partition fits in WRAM */

int aggregation() {
    unsigned int tasklet_id = me();
    if (tasklet_id == 0) {
        printf("Tasklet: %u\n", tasklet_id);
        mem_reset();

        /* get partition sizes from MRAM */
        wr_partition_sizes = (uint32_t*) mem_alloc(dpu_parameters.num_partitions * sizeof(uint32_t));
        mram_read((__mram_ptr void const*) &((uint32_t*) DPU_MRAM_HEAP_POINTER)[0], wr_partition_sizes, dpu_parameters.num_partitions * sizeof(uint32_t)); /* TODO: increase data size for improved bandwidth utilization */

        mr_htable_offs = 0;
        for (uint32_t p = 0; p < dpu_parameters.num_partitions; p++) {
            mr_htable_offs += wr_partition_sizes[p];
        }

        /* allocate hash table buffer */
        hash_table = (htable_entry*) mem_alloc(NR_HASH_TABLE_ENTRIES * sizeof(htable_entry));
    }
    barrier_wait(&aggr_barrier);

    /* group and compute aggregation for each partition */
    uint32_t part_offs = 0;
    uint32_t num_parts_aligned = (dpu_parameters.num_partitions % 2 == 0) ?
                                 dpu_parameters.num_partitions :
                                 (dpu_parameters.num_partitions + 1);

    mrnode* mr_elems = (mrnode*) &((uint32_t*) DPU_MRAM_HEAP_POINTER)[num_parts_aligned];
    mrnode* wr_elems = (mrnode*) mem_alloc(NR_WR_ELEMS_PER_TASKLET_AGGREGATION * ELEM_SIZE);

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
                uint32_t idx = grp_key;

                while (1) {
                    vmutex_lock(&aggr_vmutex, idx);

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

                        vmutex_unlock(&aggr_vmutex, idx);
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

                        vmutex_unlock(&aggr_vmutex, idx);
                        break;
                    }
                    else { /* hash table entry collision */
                        vmutex_unlock(&aggr_vmutex, idx);

                        /* linear probing */
                        idx++;
                        if (idx >= NR_HASH_TABLE_ENTRIES) {
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
            htable_entry* dest = (htable_entry*) &mr_elems[mr_htable_offs] + ((partition * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES);
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

    for (uint32_t i = 0; i < NR_PARTITIONS; i++) {
        copy_count[tasklet_id][i] = 0;
    }
    for (uint32_t i = 0; i < NR_PARTITIONS; i++) {
        histogram[tasklet_id][i] = 0;
    }

    mrnode* mr_elems = (mrnode*) DPU_MRAM_HEAP_POINTER;
    mrnode* wr_elems = (mrnode*) mem_alloc(NR_WR_ELEMS_PER_TASKLET_PARTITION * ELEM_SIZE);

    /* read elements from MRAM and compute histogram */
    for (uint32_t i = tasklet_id * NR_WR_ELEMS_PER_TASKLET_PARTITION; i < dpu_parameters.num_elems; i += NR_WR_ELEMS_PER_TASKLET_PARTITION * NR_TASKLETS) {
        mram_read((__mram_ptr void const*) &mr_elems[i], wr_elems, NR_WR_ELEMS_PER_TASKLET_PARTITION * ELEM_SIZE);

        uint32_t num_elems = ((i + NR_WR_ELEMS_PER_TASKLET_PARTITION) < dpu_parameters.num_elems) ?
                             NR_WR_ELEMS_PER_TASKLET_PARTITION :
                             dpu_parameters.num_elems - i; /* last remaining elements less than NR_WR_ELEMS_PER_TASKLET_PARTITION */

        for (uint32_t j = 0; j < num_elems; j++) {
            prop_code_t grp_key = wr_elems[j].properties[GROUP_KEY];
            // uint32_t partition = global_partition_hash(grp_key) % NR_PARTITIONS;
            uint32_t partition = grp_key;
            histogram[tasklet_id][partition]++;
        }
    }

    /* compute prefix sums for first partition */
    uint32_t p = 0;
    if (tasklet_id == 0) {
        prefix_sum[tasklet_id][p] = 0;
    }
    else {
        handshake_wait_for(tasklet_id - 1);
    }

    if (tasklet_id < NR_TASKLETS - 1) {
        prefix_sum[tasklet_id + 1][p] = prefix_sum[tasklet_id][p] + histogram[tasklet_id][p];
        handshake_notify();
    }
    else {
        /* end of first partition */
        prefix_sum[tasklet_id + 1][p] = prefix_sum[tasklet_id][p] + histogram[tasklet_id][p];
        dpu_partition_sizes[p] = prefix_sum[tasklet_id + 1][p];
    }

    /* compute prefix sums for remaining partitions */
    p++;
    while (p < NR_PARTITIONS) {
        /* we need the last tasklet to have finished with the previous partition */
        barrier_wait(&aggr_barrier);

        if (tasklet_id == 0) {
            /* beginning of current partition = end of previous partition */
            prefix_sum[tasklet_id][p] = prefix_sum[NR_TASKLETS][p - 1];
        }
        else {
            handshake_wait_for(tasklet_id - 1);
        }

        if (tasklet_id < NR_TASKLETS - 1) {
            prefix_sum[tasklet_id + 1][p] = prefix_sum[tasklet_id][p] + histogram[tasklet_id][p];
            handshake_notify();
        }
        else {
            /* end of current partition */
            prefix_sum[tasklet_id + 1][p] = prefix_sum[tasklet_id][p] + histogram[tasklet_id][p];
            dpu_partition_sizes[p] = prefix_sum[tasklet_id + 1][p] - prefix_sum[tasklet_id + 1][p - 1];
        }

        p++;
    }

    /* read elements for the second time from MRAM and copy them to their MRAM partition buffers */
    for (uint32_t i = tasklet_id * NR_WR_ELEMS_PER_TASKLET_PARTITION; i < dpu_parameters.num_elems; i += NR_WR_ELEMS_PER_TASKLET_PARTITION * NR_TASKLETS) {
        mram_read((__mram_ptr void const*) &mr_elems[i], wr_elems, NR_WR_ELEMS_PER_TASKLET_PARTITION * ELEM_SIZE);

        uint32_t num_elems = ((i + NR_WR_ELEMS_PER_TASKLET_PARTITION) < dpu_parameters.num_elems) ?
                             NR_WR_ELEMS_PER_TASKLET_PARTITION :
                             dpu_parameters.num_elems - i; /* last remaining elements less than NR_WR_ELEMS_PER_TASKLET_PARTITION */

        for (uint32_t j = 0; j < num_elems; j++) {
            prop_code_t grp_key = wr_elems[j].properties[GROUP_KEY];
            // uint32_t partition = glb_partition_hash(grp_key) % NR_PARTITIONS;
            uint32_t partition = grp_key;
            uint32_t mroffs = prefix_sum[tasklet_id][partition] + copy_count[tasklet_id][partition];
            copy_count[tasklet_id][partition]++;
            mram_write(&wr_elems[j], (__mram_ptr void*) &mr_elems[dpu_parameters.num_elems + mroffs], ELEM_SIZE);
        }
    }
    barrier_wait(&aggr_barrier);

    return 0;
}

int (*kernels[NR_KERNERLS])() = {partition, aggregation};

int main() {
    return kernels[dpu_parameters.phase]();
}

#elif defined LOW_CARDINALITY /* hash table for entire data fits in WRAM */


#ifdef LOW_CARD_1 /* a separate hash table for each tasklet */

int aggregation() {
    unsigned int tasklet_id = me();

    if (tasklet_id == 0) {
        // printf("Tasklet: %d\n", tasklet_id);
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
                    idx++;
                    if (idx >= NR_HASH_TABLE_ENTRIES) {
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

#elif defined LOW_CARD_2 /* a single hash table shared by all tasklets */

int aggregation() {
    unsigned int tasklet_id = me();

    if (tasklet_id == 0) {
        // printf("Tasklet: %d\n", tasklet_id);
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
                    idx++;
                    if (idx >= NR_HASH_TABLE_ENTRIES) {
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
#endif

int (*kernels[NR_KERNERLS])() = {aggregation};

int main() {
    return kernels[0]();
}

#endif