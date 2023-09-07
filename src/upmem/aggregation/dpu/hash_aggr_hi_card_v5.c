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


#ifndef HASH_BASED_HIGH_CARDINALITY_V5

int main() {
    return 0;
}

#else

__host dpu_params dpu_parameters;

uint32_t mr_sorted_elems_offs;
uint32_t* wr_range_partition_sizes;

/* partition parameters */
/* uint32_t mr_htable_offs; */
uint32_t* wr_partition_sizes;
uint32_t* prefix_sum;
uint32_t* copy_count;
uint32_t prefix;
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
        uint32_t num_parts_aligned = (dpu_parameters.num_partitions % 2 == 0) ?
                                     dpu_parameters.num_partitions :
                                     (dpu_parameters.num_partitions + 1);

        wr_partition_sizes = (uint32_t*) mem_alloc(num_parts_aligned * sizeof(uint32_t));
        mram_read((__mram_ptr void const*) DPU_MRAM_HEAP_POINTER, wr_partition_sizes, num_parts_aligned * sizeof(uint32_t));

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

    for (uint32_t part = 0; part < dpu_parameters.num_partitions; part++) {
        /* reset all hash table entries */
        for (uint32_t idx = tasklet_id; idx < NR_HASH_TABLE_ENTRIES; idx += NR_TASKLETS) {
            hash_table[idx].key = (-1);
        }
        barrier_wait(&aggr_barrier);

        uint32_t part_size = wr_partition_sizes[part];
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
            /* htable_entry* dest = (htable_entry*) &mr_elems[mr_htable_offs] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES); */
            htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES);
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

        /* allocate histogram */
        prefix = 0;
        prefix_sum = (uint32_t*) mem_alloc(NR_HISTOGRAM_ENTRIES * sizeof(uint32_t));
        copy_count = (uint32_t*) mem_alloc(NR_HISTOGRAM_ENTRIES * sizeof(uint32_t));
    }
    barrier_wait(&aggr_barrier);

    elem_t* mr_elems = (elem_t*) DPU_MRAM_HEAP_POINTER;
    elem_t* wr_elems = (elem_t*) mem_alloc(NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET * ELEM_SIZE);

    for (uint32_t batch = 0; batch < NR_HISTOGRAM_BATCHES; batch++) {
        /* reset histogram */
        for (uint32_t i = tasklet_id; i < NR_HISTOGRAM_ENTRIES; i += NR_TASKLETS) {
            prefix_sum[i] = 0;
        }
        for (uint32_t i = tasklet_id; i < NR_HISTOGRAM_ENTRIES; i += NR_TASKLETS) {
            copy_count[i] = 0;
        }
        barrier_wait(&aggr_barrier);

        /* read elements from MRAM and compute histogram for current batch */
        for (uint32_t i = tasklet_id * NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET; i < dpu_parameters.num_elems; i += NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET * NR_TASKLETS) {
            mram_read((__mram_ptr void const*) &mr_elems[i], wr_elems, NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET * ELEM_SIZE);

            uint32_t num_elems = ((i + NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET) < dpu_parameters.num_elems) ?
                                 NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET :
                                 dpu_parameters.num_elems - i; /* last remaining elements less than NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET */

            for (uint32_t j = 0; j < num_elems; j++) {
                prop_code_t grp_key = wr_elems[j].properties[GROUP_KEY];
                // uint32_t part = global_partition_hash(grp_key) % NR_PARTITIONS;
                uint32_t part = grp_key % NR_PARTITIONS;

                uint32_t quot = part / NR_HISTOGRAM_ENTRIES;
                uint32_t rem = part % NR_HISTOGRAM_ENTRIES;

                if (quot == batch) { /* the partition belongs to the current batch */
                    // vmutex_lock(&part_vmutex, idx);
                    mutex_pool_lock(&part_mutexpl, rem);
                    prefix_sum[rem]++;
                    // vmutex_unlock(&part_vmutex, idx);
                    mutex_pool_unlock(&part_mutexpl, rem);
                }
            }
        }
        barrier_wait(&aggr_barrier);

        /* compute prefix sums */
        if (tasklet_id == 0) {
            uint32_t tmp;
            for (uint32_t p = 0; p < NR_HISTOGRAM_ENTRIES; p++) {   
                tmp = prefix_sum[p];
                prefix_sum[p] = prefix;
                prefix += tmp;
            }
        }
        barrier_wait(&aggr_barrier);

#if 1 /* partition the data in MRAM */

        /* read elements again from MRAM and copy the elements belonging to the current to their MRAM partition buffers */
        for (uint32_t i = tasklet_id * NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET; i < dpu_parameters.num_elems; i += NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET * NR_TASKLETS) {
            mram_read((__mram_ptr void const*) &mr_elems[i], wr_elems, NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET * ELEM_SIZE);

            uint32_t num_elems = ((i + NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET) < dpu_parameters.num_elems) ?
                                 NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET :
                                 dpu_parameters.num_elems - i; /* last remaining elements less than NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET */

            for (uint32_t j = 0; j < num_elems; j++) {
                prop_code_t grp_key = wr_elems[j].properties[GROUP_KEY];
                // uint32_t part = glb_partition_hash(grp_key) % NR_PARTITIONS;
                uint32_t part = grp_key % NR_PARTITIONS;

                uint32_t quot = part / NR_HISTOGRAM_ENTRIES;
                uint32_t rem = part % NR_HISTOGRAM_ENTRIES;

                if (quot == batch) { /* the partition belongs to the current batch */
                    // vmutex_lock(&part_vmutex, idx);
                    mutex_pool_lock(&part_mutexpl, rem);
                    uint32_t mroffs = prefix_sum[rem] + copy_count[rem];
                    copy_count[rem]++;
                    // vmutex_unlock(&part_vmutex, idx);
                    mutex_pool_unlock(&part_mutexpl, rem);
                    mram_write(&wr_elems[j], (__mram_ptr void*) &mr_elems[dpu_parameters.max_num_elems + mroffs], ELEM_SIZE);
                }
            }
        }

        /* copy histogram of current batch to MRAM */
        uint32_t* mr_hist = (uint32_t*) &mr_elems[2 * dpu_parameters.max_num_elems];
        for (uint32_t ch = tasklet_id; ch < NR_HISTOGRAM_CHUNKS; ch += NR_TASKLETS) {
            uint32_t choffs = ch * NR_HISTOGRAM_CHUNK_ENTRIES;
            uint32_t mroffs = batch * NR_HISTOGRAM_ENTRIES + choffs;
            uint32_t num_entries = (ch == (NR_HISTOGRAM_CHUNKS - 1)) ?
                                   (NR_HISTOGRAM_ENTRIES - (ch * NR_HISTOGRAM_CHUNK_ENTRIES)) : /* remaining entries of the last chunk */
                                   NR_HISTOGRAM_CHUNK_ENTRIES;

            mram_write(&prefix_sum[choffs], (__mram_ptr void*) &mr_hist[mroffs], num_entries * sizeof(uint32_t));
        }
        barrier_wait(&aggr_barrier);

#endif
    }

    return 0;
}

int sg_batch_xfer() {
    unsigned int tasklet_id = me();

    if (tasklet_id == 0) {
        printf("Tasklet: %u\n", tasklet_id);
        mem_reset();
    }
    barrier_wait(&aggr_barrier);

    elem_t* mr_elems_out = (elem_t*) DPU_MRAM_HEAP_POINTER;
    elem_t* mr_elems_in = &((elem_t*) DPU_MRAM_HEAP_POINTER)[dpu_parameters.sg_offset];
    elem_t* wr_elems = (elem_t*) mem_alloc(NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET * ELEM_SIZE);

    for (uint32_t i = (dpu_parameters.sg_batch_beg_offs + tasklet_id * NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET);
                  i < dpu_parameters.sg_batch_end_offs;
                  i += NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET * NR_TASKLETS) {

        mram_read((__mram_ptr void const*) &mr_elems_in[i], wr_elems, NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET * ELEM_SIZE);

        uint32_t num_elems = ((i + NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET) < dpu_parameters.sg_batch_end_offs) ?
                             (NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET) :
                             dpu_parameters.sg_batch_end_offs - i; /* last remaining elements less than NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET */

        mram_write(wr_elems, (__mram_ptr void*) &mr_elems_out[i - dpu_parameters.sg_batch_beg_offs], num_elems * ELEM_SIZE);
    }

    return 0;
}

int split_aggregation() {
    unsigned int tasklet_id = me();
    if (tasklet_id == 0) {
        printf("Tasklet: %u\n", tasklet_id);
        mem_reset();

        /* get partition sizes from MRAM */
        uint32_t num_parts_aligned = (dpu_parameters.num_partitions % 2 == 0) ?
                                     dpu_parameters.num_partitions :
                                     (dpu_parameters.num_partitions + 1);

        wr_partition_sizes = (uint32_t*) mem_alloc(num_parts_aligned * sizeof(uint32_t));
        mram_read((__mram_ptr void const*) DPU_MRAM_HEAP_POINTER, wr_partition_sizes, num_parts_aligned * sizeof(uint32_t));

        /* mr_htable_offs = 0;
        for (uint32_t p = 0; p < dpu_parameters.num_partitions; p++) {
            mr_htable_offs += wr_partition_sizes[p];
        } */

        /* allocate hash table buffer */
        for (uint32_t h = 0; h < NR_HASH_TABLES; h++) {
            hash_tables[h] = (htable_entry*) mem_alloc(NR_HASH_TABLE_ENTRIES * sizeof(htable_entry));
        }
    }
    barrier_wait(&aggr_barrier);


    /* group and compute aggregation for each partition */
    uint32_t part_offs;
    elem_t* mr_elems = (elem_t*) &((uint32_t*) DPU_MRAM_HEAP_POINTER)[dpu_parameters.max_num_partitions];
    elem_t* wr_elems = (elem_t*) mem_alloc(NR_WR_ELEMS_PER_TASKLET_AGGREGATION * ELEM_SIZE);

    uint32_t fourth_parts_pivot = dpu_parameters.num_partitions / 2;
    uint32_t second_parts_pivot = fourth_parts_pivot / 2;
    uint32_t first_parts_pivot = second_parts_pivot / 2;
    uint32_t third_parts_pivot = second_parts_pivot + first_parts_pivot;
    uint32_t fifth_parts_pivot = fourth_parts_pivot + first_parts_pivot;
    uint32_t sixth_parts_pivot = fifth_parts_pivot + first_parts_pivot;
    uint32_t seventh_parts_pivot = sixth_parts_pivot + first_parts_pivot;

    if (tasklet_id >= 0 && tasklet_id < 2) {
        /* Tasklet ids 0 to 1 process first set of partitions */
        uint32_t htable_idx = 0;
        htable_entry* hash_table = hash_tables[htable_idx]; /* use the first hash table */

        part_offs = 0;
        for (uint32_t part = 0; part < first_parts_pivot; part++) {
            /* reset all hash table entries */
            for (uint32_t idx = tasklet_id; idx < NR_HASH_TABLE_ENTRIES; idx += (NR_TASKLETS / NR_HASH_TABLES)) {
                hash_table[idx].key = (-1);
            }
            barrier_wait(&aggr_barrier_1);

            uint32_t part_size = wr_partition_sizes[part];
            for (uint32_t i = tasklet_id * NR_WR_ELEMS_PER_TASKLET_AGGREGATION; i < part_size; i += NR_WR_ELEMS_PER_TASKLET_AGGREGATION * (NR_TASKLETS / NR_HASH_TABLES)) {
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
                        mutex_pool_lock(&aggr_mutexpl_1, idx);

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
                            mutex_pool_unlock(&aggr_mutexpl_1, idx);
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
                            mutex_pool_unlock(&aggr_mutexpl_1, idx);
                            break;
                        }
                        else { /* hash table entry collision */
                            // vmutex_unlock(&aggr_vmutex, idx);
                            mutex_pool_unlock(&aggr_mutexpl_1, idx);

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
            barrier_wait(&aggr_barrier_1);
            for (uint32_t ch = tasklet_id; ch < NR_HASH_TABLE_CHUNKS; ch += (NR_TASKLETS / NR_HASH_TABLES)) {
                htable_entry* src = hash_table + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
                /* htable_entry* dest = (htable_entry*) &mr_elems[mr_htable_offs] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES); */
                htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES);
                mram_write(src, (__mram_ptr void*) dest, sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
            }
            barrier_wait(&aggr_barrier_1);
        }
    }
    else if (tasklet_id >= 2 && tasklet_id < 4) {
        /* Tasklet ids 2 to 3 process second set of partitions */
        uint32_t htable_idx = 1;
        htable_entry* hash_table = hash_tables[htable_idx]; /* use the second hash table */

        part_offs = 0;
        for (uint32_t part = 0; part < first_parts_pivot; part++) {
            part_offs += wr_partition_sizes[part];
        }

        for (uint32_t part = first_parts_pivot; part < second_parts_pivot; part++) {
            /* reset all hash table entries */
            for (uint32_t idx = (tasklet_id - 2); idx < NR_HASH_TABLE_ENTRIES; idx += (NR_TASKLETS / NR_HASH_TABLES)) {
                hash_table[idx].key = (-1);
            }
            barrier_wait(&aggr_barrier_2);

            uint32_t part_size = wr_partition_sizes[part];
            for (uint32_t i = (tasklet_id - 2) * NR_WR_ELEMS_PER_TASKLET_AGGREGATION; i < part_size; i += NR_WR_ELEMS_PER_TASKLET_AGGREGATION * (NR_TASKLETS / NR_HASH_TABLES)) {
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
                        mutex_pool_lock(&aggr_mutexpl_2, idx);

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
                            mutex_pool_unlock(&aggr_mutexpl_2, idx);
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
                            mutex_pool_unlock(&aggr_mutexpl_2, idx);
                            break;
                        }
                        else { /* hash table entry collision */
                            // vmutex_unlock(&aggr_vmutex, idx);
                            mutex_pool_unlock(&aggr_mutexpl_2, idx);

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
            barrier_wait(&aggr_barrier_2);
            for (uint32_t ch = (tasklet_id - 2); ch < NR_HASH_TABLE_CHUNKS; ch += (NR_TASKLETS / NR_HASH_TABLES)) {
                htable_entry* src = hash_table + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
                /* htable_entry* dest = (htable_entry*) &mr_elems[mr_htable_offs] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES); */
                htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES);
                mram_write(src, (__mram_ptr void*) dest, sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
            }
            barrier_wait(&aggr_barrier_2);
        }
    }
    else if (tasklet_id >= 4 && tasklet_id < 6) {
        /* Tasklet ids 4 to 5 process third set of partitions */
        uint32_t htable_idx = 2;
        htable_entry* hash_table = hash_tables[htable_idx]; /* use the third hash table */

        part_offs = 0;
        for (uint32_t part = 0; part < second_parts_pivot; part++) {
            part_offs += wr_partition_sizes[part];
        }

        for (uint32_t part = second_parts_pivot; part < third_parts_pivot; part++) {
            /* reset all hash table entries */
            for (uint32_t idx = (tasklet_id - 4); idx < NR_HASH_TABLE_ENTRIES; idx += (NR_TASKLETS / NR_HASH_TABLES)) {
                hash_table[idx].key = (-1);
            }
            barrier_wait(&aggr_barrier_3);

            uint32_t part_size = wr_partition_sizes[part];
            for (uint32_t i = (tasklet_id - 4) * NR_WR_ELEMS_PER_TASKLET_AGGREGATION; i < part_size; i += NR_WR_ELEMS_PER_TASKLET_AGGREGATION * (NR_TASKLETS / NR_HASH_TABLES)) {
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
                        mutex_pool_lock(&aggr_mutexpl_3, idx);

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
                            mutex_pool_unlock(&aggr_mutexpl_3, idx);
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
                            mutex_pool_unlock(&aggr_mutexpl_3, idx);
                            break;
                        }
                        else { /* hash table entry collision */
                            // vmutex_unlock(&aggr_vmutex, idx);
                            mutex_pool_unlock(&aggr_mutexpl_3, idx);

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
            barrier_wait(&aggr_barrier_3);
            for (uint32_t ch = (tasklet_id - 4); ch < NR_HASH_TABLE_CHUNKS; ch += (NR_TASKLETS / NR_HASH_TABLES)) {
                htable_entry* src = hash_table + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
                /* htable_entry* dest = (htable_entry*) &mr_elems[mr_htable_offs] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES); */
                htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES);
                mram_write(src, (__mram_ptr void*) dest, sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
            }
            barrier_wait(&aggr_barrier_3);
        }
    }
    else if (tasklet_id >= 6 && tasklet_id < 8) {
        /* Tasklet ids 6 to 7 process fourth set of partitions */
        uint32_t htable_idx = 3;
        htable_entry* hash_table = hash_tables[htable_idx]; /* use the fourth hash table */

        part_offs = 0;
        for (uint32_t part = 0; part < third_parts_pivot; part++) {
            part_offs += wr_partition_sizes[part];
        }

        for (uint32_t part = third_parts_pivot; part < fourth_parts_pivot; part++) {
            /* reset all hash table entries */
            for (uint32_t idx = (tasklet_id - 6); idx < NR_HASH_TABLE_ENTRIES; idx += (NR_TASKLETS / NR_HASH_TABLES)) {
                hash_table[idx].key = (-1);
            }
            barrier_wait(&aggr_barrier_4);

            uint32_t part_size = wr_partition_sizes[part];
            for (uint32_t i = (tasklet_id - 6) * NR_WR_ELEMS_PER_TASKLET_AGGREGATION; i < part_size; i += NR_WR_ELEMS_PER_TASKLET_AGGREGATION * (NR_TASKLETS / NR_HASH_TABLES)) {
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
                        mutex_pool_lock(&aggr_mutexpl_4, idx);

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
                            mutex_pool_unlock(&aggr_mutexpl_4, idx);
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
                            mutex_pool_unlock(&aggr_mutexpl_4, idx);
                            break;
                        }
                        else { /* hash table entry collision */
                            // vmutex_unlock(&aggr_vmutex, idx);
                            mutex_pool_unlock(&aggr_mutexpl_4, idx);

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
            barrier_wait(&aggr_barrier_4);
            for (uint32_t ch = (tasklet_id - 6); ch < NR_HASH_TABLE_CHUNKS; ch += (NR_TASKLETS / NR_HASH_TABLES)) {
                htable_entry* src = hash_table + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
                /* htable_entry* dest = (htable_entry*) &mr_elems[mr_htable_offs] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES); */
                htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES);
                mram_write(src, (__mram_ptr void*) dest, sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
            }
            barrier_wait(&aggr_barrier_4);
        }
    }
    else if (tasklet_id >= 8 && tasklet_id < 10) {
        /* Tasklet ids 8 to 9 process fifth set of partitions */
        uint32_t htable_idx = 4;
        htable_entry* hash_table = hash_tables[htable_idx]; /* use the fifth hash table */

        part_offs = 0;
        for (uint32_t part = 0; part < fourth_parts_pivot; part++) {
            part_offs += wr_partition_sizes[part];
        }

        for (uint32_t part = fourth_parts_pivot; part < fifth_parts_pivot; part++) {
            /* reset all hash table entries */
            for (uint32_t idx = (tasklet_id - 8); idx < NR_HASH_TABLE_ENTRIES; idx += (NR_TASKLETS / NR_HASH_TABLES)) {
                hash_table[idx].key = (-1);
            }
            barrier_wait(&aggr_barrier_5);

            uint32_t part_size = wr_partition_sizes[part];
            for (uint32_t i = (tasklet_id - 8) * NR_WR_ELEMS_PER_TASKLET_AGGREGATION; i < part_size; i += NR_WR_ELEMS_PER_TASKLET_AGGREGATION * (NR_TASKLETS / NR_HASH_TABLES)) {
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
                        mutex_pool_lock(&aggr_mutexpl_5, idx);

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
                            mutex_pool_unlock(&aggr_mutexpl_5, idx);
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
                            mutex_pool_unlock(&aggr_mutexpl_5, idx);
                            break;
                        }
                        else { /* hash table entry collision */
                            // vmutex_unlock(&aggr_vmutex, idx);
                            mutex_pool_unlock(&aggr_mutexpl_5, idx);

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
            barrier_wait(&aggr_barrier_5);
            for (uint32_t ch = (tasklet_id - 8); ch < NR_HASH_TABLE_CHUNKS; ch += (NR_TASKLETS / NR_HASH_TABLES)) {
                htable_entry* src = hash_table + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
                /* htable_entry* dest = (htable_entry*) &mr_elems[mr_htable_offs] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES); */
                htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES);
                mram_write(src, (__mram_ptr void*) dest, sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
            }
            barrier_wait(&aggr_barrier_5);
        }
    }
    else if (tasklet_id >= 10 && tasklet_id < 12) {
        /* Tasklet ids 10 to 11 process sixth set of partitions */
        uint32_t htable_idx = 5;
        htable_entry* hash_table = hash_tables[htable_idx]; /* use the sixth hash table */

        part_offs = 0;
        for (uint32_t part = 0; part < fifth_parts_pivot; part++) {
            part_offs += wr_partition_sizes[part];
        }

        for (uint32_t part = fifth_parts_pivot; part < sixth_parts_pivot; part++) {
            /* reset all hash table entries */
            for (uint32_t idx = (tasklet_id - 10); idx < NR_HASH_TABLE_ENTRIES; idx += (NR_TASKLETS / NR_HASH_TABLES)) {
                hash_table[idx].key = (-1);
            }
            barrier_wait(&aggr_barrier_6);

            uint32_t part_size = wr_partition_sizes[part];
            for (uint32_t i = (tasklet_id - 10) * NR_WR_ELEMS_PER_TASKLET_AGGREGATION; i < part_size; i += NR_WR_ELEMS_PER_TASKLET_AGGREGATION * (NR_TASKLETS / NR_HASH_TABLES)) {
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
                        mutex_pool_lock(&aggr_mutexpl_6, idx);

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
                            mutex_pool_unlock(&aggr_mutexpl_6, idx);
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
                            mutex_pool_unlock(&aggr_mutexpl_6, idx);
                            break;
                        }
                        else { /* hash table entry collision */
                            // vmutex_unlock(&aggr_vmutex, idx);
                            mutex_pool_unlock(&aggr_mutexpl_6, idx);

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
            barrier_wait(&aggr_barrier_6);
            for (uint32_t ch = (tasklet_id - 10); ch < NR_HASH_TABLE_CHUNKS; ch += (NR_TASKLETS / NR_HASH_TABLES)) {
                htable_entry* src = hash_table + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
                /* htable_entry* dest = (htable_entry*) &mr_elems[mr_htable_offs] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES); */
                htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES);
                mram_write(src, (__mram_ptr void*) dest, sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
            }
            barrier_wait(&aggr_barrier_6);
        }
    }
    else if (tasklet_id >= 12 && tasklet_id < 14) {
        /* Tasklet ids 12 to 13 process seventh set of partitions */
        uint32_t htable_idx = 6;
        htable_entry* hash_table = hash_tables[htable_idx]; /* use the seventh hash table */

        part_offs = 0;
        for (uint32_t part = 0; part < sixth_parts_pivot; part++) {
            part_offs += wr_partition_sizes[part];
        }

        for (uint32_t part = sixth_parts_pivot; part < seventh_parts_pivot; part++) {
            /* reset all hash table entries */
            for (uint32_t idx = (tasklet_id - 12); idx < NR_HASH_TABLE_ENTRIES; idx += (NR_TASKLETS / NR_HASH_TABLES)) {
                hash_table[idx].key = (-1);
            }
            barrier_wait(&aggr_barrier_7);

            uint32_t part_size = wr_partition_sizes[part];
            for (uint32_t i = (tasklet_id - 12) * NR_WR_ELEMS_PER_TASKLET_AGGREGATION; i < part_size; i += NR_WR_ELEMS_PER_TASKLET_AGGREGATION * (NR_TASKLETS / NR_HASH_TABLES)) {
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
                        mutex_pool_lock(&aggr_mutexpl_7, idx);

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
                            mutex_pool_unlock(&aggr_mutexpl_7, idx);
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
                            mutex_pool_unlock(&aggr_mutexpl_7, idx);
                            break;
                        }
                        else { /* hash table entry collision */
                            // vmutex_unlock(&aggr_vmutex, idx);
                            mutex_pool_unlock(&aggr_mutexpl_7, idx);

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
            barrier_wait(&aggr_barrier_7);
            for (uint32_t ch = (tasklet_id - 12); ch < NR_HASH_TABLE_CHUNKS; ch += (NR_TASKLETS / NR_HASH_TABLES)) {
                htable_entry* src = hash_table + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
                /* htable_entry* dest = (htable_entry*) &mr_elems[mr_htable_offs] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES); */
                htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES);
                mram_write(src, (__mram_ptr void*) dest, sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
            }
            barrier_wait(&aggr_barrier_7);
        }
    }
    else  {
        /* Tasklet ids 14 to 15 process eight set of partitions */
        uint32_t htable_idx = 7;
        htable_entry* hash_table = hash_tables[htable_idx]; /* use the eight hash table */

        part_offs = 0;
        for (uint32_t part = 0; part < seventh_parts_pivot; part++) {
            part_offs += wr_partition_sizes[part];
        }

        for (uint32_t part = seventh_parts_pivot; part < dpu_parameters.num_partitions; part++) {
            /* reset all hash table entries */
            for (uint32_t idx = (tasklet_id - 14); idx < NR_HASH_TABLE_ENTRIES; idx += (NR_TASKLETS / NR_HASH_TABLES)) {
                hash_table[idx].key = (-1);
            }
            barrier_wait(&aggr_barrier_8);

            uint32_t part_size = wr_partition_sizes[part];
            for (uint32_t i = (tasklet_id - 14) * NR_WR_ELEMS_PER_TASKLET_AGGREGATION; i < part_size; i += NR_WR_ELEMS_PER_TASKLET_AGGREGATION * (NR_TASKLETS / NR_HASH_TABLES)) {
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
                        mutex_pool_lock(&aggr_mutexpl_8, idx);

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
                            mutex_pool_unlock(&aggr_mutexpl_8, idx);
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
                            mutex_pool_unlock(&aggr_mutexpl_8, idx);
                            break;
                        }
                        else { /* hash table entry collision */
                            // vmutex_unlock(&aggr_vmutex, idx);
                            mutex_pool_unlock(&aggr_mutexpl_8, idx);

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
            barrier_wait(&aggr_barrier_8);
            for (uint32_t ch = (tasklet_id - 14); ch < NR_HASH_TABLE_CHUNKS; ch += (NR_TASKLETS / NR_HASH_TABLES)) {
                htable_entry* src = hash_table + (ch * NR_HASH_TABLE_CHUNK_ENTRIES);
                /* htable_entry* dest = (htable_entry*) &mr_elems[mr_htable_offs] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES); */
                htable_entry* dest = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions] + ((part * NR_HASH_TABLE_CHUNKS + ch) * NR_HASH_TABLE_CHUNK_ENTRIES);
                mram_write(src, (__mram_ptr void*) dest, sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
            }
            barrier_wait(&aggr_barrier_8);
        }
    }

    return 0;
}

int (*kernels[NR_KERNELS])() = {partition, aggregation, sg_batch_xfer};

int main() {
    return kernels[dpu_parameters.phase]();
}

#endif /* #ifndef HASH_BASED_HIGH_CARDINALITY_V5 */
