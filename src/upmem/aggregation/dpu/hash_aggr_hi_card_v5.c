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

uint32_t prefix;
uint32_t* prefix_sum;
uint32_t* copy_count;

#ifdef SINGLE_HASH_TABLE
uint32_t* partition_sizes;
htable_entry* hash_table;
#elif defined PER_TASKLET_HASH_TABLE
uint32_t total_part_sizes;
uint32_t* partition_offsets;
#endif

BARRIER_INIT(aggr_barrier, NR_TASKLETS);
// VMUTEX_INIT(aggr_vmutex, NR_HASH_TABLE_ENTRIES, 16);
MUTEX_POOL_INIT(aggr_mutexpl, 32);
// MUTEX_INIT(aggr_mutex);


#ifdef SINGLE_HASH_TABLE

int aggregation() {
    unsigned int tasklet_id = me();

    if (tasklet_id == 0) {
        printf("Tasklet: %u\n", tasklet_id);
        mem_reset();

        /* get partition sizes from MRAM */
        uint32_t num_parts_aligned = (dpu_parameters.num_partitions % 2 == 0) ?
                                     (dpu_parameters.num_partitions) :
                                     (dpu_parameters.num_partitions + 1);

        partition_sizes = (uint32_t*) mem_alloc(num_parts_aligned * sizeof(uint32_t));
        mram_read((__mram_ptr void const*) DPU_MRAM_HEAP_POINTER, partition_sizes, num_parts_aligned * sizeof(uint32_t));

        /* allocate hash table buffer */
        hash_table = (htable_entry*) mem_alloc(NR_HASH_TABLE_ENTRIES * sizeof(htable_entry));
    }
    barrier_wait(&aggr_barrier);


    /* group and compute aggregation for each partition */
    uint32_t part_offs = 0;
    elem_t* mr_elems = (elem_t*) &((uint32_t*) DPU_MRAM_HEAP_POINTER)[dpu_parameters.max_num_partitions];
    elem_t* wr_elems = (elem_t*) mem_alloc(NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET * ELEM_SIZE);
    htable_entry* mr_htables = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions];

    for (uint32_t part = 0; part < dpu_parameters.num_partitions; part++) {
        /* reset all hash table entries */
        for (uint32_t idx = tasklet_id; idx < NR_HASH_TABLE_ENTRIES; idx += NR_TASKLETS) {
            hash_table[idx].key = (-1);
        }
        barrier_wait(&aggr_barrier);

        uint32_t part_size = partition_sizes[part];
        for (uint32_t i = tasklet_id * NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET; i < part_size; i += NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET * NR_TASKLETS) {
            mram_read((__mram_ptr void const*) &mr_elems[part_offs + i], wr_elems, NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET * ELEM_SIZE);

            uint32_t num_elems = ((i + NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET) < part_size) ?
                                 (NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET) :
                                 (part_size - i); /* last remaining elements less than NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET */

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
            uint32_t choffs = ch * NR_HASH_TABLE_CHUNK_ENTRIES;
            uint32_t mroffs = part * NR_HASH_TABLE_CHUNKS * NR_HASH_TABLE_CHUNK_ENTRIES + choffs;
            mram_write(&hash_table[choffs], (__mram_ptr void*) &mr_htables[mroffs], sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
        }
        barrier_wait(&aggr_barrier);
    }

    return 0;
}

#elif defined PER_TASKLET_HASH_TABLE

int aggregation() {
    unsigned int tasklet_id = me();

    if (tasklet_id == 0) {
        printf("Tasklet: %u\n", tasklet_id);
        mem_reset();

        /* compute offsets of partitions in MRAM */
        uint32_t tmp;
        uint32_t offs = 0;
        uint32_t num_parts_aligned = (dpu_parameters.num_partitions % 2 == 0) ?
                                     (dpu_parameters.num_partitions) :
                                     (dpu_parameters.num_partitions + 1);

        partition_offsets = (uint32_t*) mem_alloc(num_parts_aligned * sizeof(uint32_t));
        mram_read((__mram_ptr void const*) DPU_MRAM_HEAP_POINTER, partition_offsets, num_parts_aligned * sizeof(uint32_t));
        for (uint32_t p = 0; p < dpu_parameters.num_partitions; p++) {   
            tmp = partition_offsets[p];
            partition_offsets[p] = offs;
            offs += tmp;
        }
        total_part_sizes = offs;
    }
    barrier_wait(&aggr_barrier);

    /* allocate hash table buffer */
    htable_entry* hash_table = (htable_entry*) mem_alloc(NR_HASH_TABLE_ENTRIES * sizeof(htable_entry)); /* 64 hash table entries per tasklet */

    /* group and compute aggregation for each partition */
    elem_t* mr_elems = (elem_t*) &((uint32_t*) DPU_MRAM_HEAP_POINTER)[dpu_parameters.max_num_partitions];
    elem_t* wr_elems = (elem_t*) mem_alloc(NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET * ELEM_SIZE);
    htable_entry* mr_htables = (htable_entry*) &mr_elems[dpu_parameters.size_of_max_num_partitions];

    for (uint32_t part = tasklet_id; part < dpu_parameters.num_partitions; part += NR_TASKLETS) {
        /* reset all hash table entries */
        for (uint32_t idx = 0; idx < NR_HASH_TABLE_ENTRIES; idx++) {
            hash_table[idx].key = (-1);
        }

        uint32_t part_offs = partition_offsets[part];
        uint32_t part_size = (part == (dpu_parameters.num_partitions - 1)) ?
                             (total_part_sizes - part_offs) :
                             (partition_offsets[part + 1] - part_offs);

        for (uint32_t i = 0; i < part_size; i += NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET) {
            mram_read((__mram_ptr void const*) &mr_elems[part_offs + i], wr_elems, NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET * ELEM_SIZE);

            uint32_t num_elems = ((i + NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET) < part_size) ?
                                 NR_WRAM_AGGREGATION_CACHE_ELEMS_PER_TASKLET :
                                 part_size - i;

            for (uint32_t j = 0; j < num_elems; j++) {
                uint32_t grp_key = wr_elems[j].properties[GROUP_KEY];
                // uint32_t idx = aggr_hash(grp_key) % NR_HASH_TABLE_ENTRIES;
                uint32_t idx = grp_key % NR_HASH_TABLE_ENTRIES;

                uint32_t probe = 0;
                while (1) {

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

                        break;
                    }
                    else { /* hash table entry collision */

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

        /* copy hash table result for the current partition to MRAM */
        for (uint32_t ch = 0; ch < NR_HASH_TABLE_CHUNKS; ch++) {
            uint32_t choffs = ch * NR_HASH_TABLE_CHUNK_ENTRIES;
            uint32_t mroffs = part * NR_HASH_TABLE_CHUNKS * NR_HASH_TABLE_CHUNK_ENTRIES + choffs;
            mram_write(&hash_table[choffs], (__mram_ptr void*) &mr_htables[mroffs], sizeof(htable_entry) * NR_HASH_TABLE_CHUNK_ENTRIES);
        }
    }

    return 0;
}

#endif /* #ifdef SINGLE_HASH_TABLE */

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
    uint32_t* mr_hist = (uint32_t*) &mr_elems[2 * dpu_parameters.max_num_elems];

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
                    // vmutex_lock(&aggr_vmutex, idx);
                    mutex_pool_lock(&aggr_mutexpl, rem);
                    prefix_sum[rem]++;
                    // vmutex_unlock(&aggr_vmutex, idx);
                    mutex_pool_unlock(&aggr_mutexpl, rem);
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
                    // vmutex_lock(&aggr_vmutex, idx);
                    mutex_pool_lock(&aggr_mutexpl, rem);
                    uint32_t mroffs = prefix_sum[rem] + copy_count[rem];
                    copy_count[rem]++;
                    // vmutex_unlock(&aggr_vmutex, idx);
                    mutex_pool_unlock(&aggr_mutexpl, rem);
                    mram_write(&wr_elems[j], (__mram_ptr void*) &mr_elems[dpu_parameters.max_num_elems + mroffs], ELEM_SIZE);
                }
            }
        }

        /* copy histogram of current batch to MRAM */
        for (uint32_t ch = tasklet_id; ch < NR_HISTOGRAM_CHUNKS; ch += NR_TASKLETS) {
            uint32_t choffs = ch * NR_HISTOGRAM_CHUNK_ENTRIES;
            uint32_t mroffs = batch * NR_HISTOGRAM_ENTRIES + choffs;
            uint32_t num_entries = (ch == (NR_HISTOGRAM_CHUNKS - 1)) ?
                                   (NR_HISTOGRAM_ENTRIES - (ch * NR_HISTOGRAM_CHUNK_ENTRIES)) : /* remaining entries of the last chunk */
                                   (NR_HISTOGRAM_CHUNK_ENTRIES);

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

    elem_t* mr_elems_out = (elem_t*) DPU_MRAM_HEAP_POINTER; /* we use the beginning of MRAM as a cache */
    elem_t* mr_elems_in = &((elem_t*) DPU_MRAM_HEAP_POINTER)[dpu_parameters.sg_offset];
    elem_t* wr_elems = (elem_t*) mem_alloc(NR_WRAM_PARTITION_CACHE_ELEMS_PER_TASKLET * ELEM_SIZE);

    /* copy partitions of the current SG transfer batch to the beginning of MRAM */
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

int (*kernels[NR_KERNELS])() = {partition, aggregation, sg_batch_xfer};

int main() {
    return kernels[dpu_parameters.phase]();
}

#endif /* #ifndef HASH_BASED_HIGH_CARDINALITY_V5 */
