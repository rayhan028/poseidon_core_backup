#include <mram.h>
#include <stdint.h>
#include <stdio.h>
#include <defs.h>
#include <mutex.h>
#include <barrier.h>
#include <limits.h>
#include "../common/defs.h"

__mram_noinit struct mrchunk mr_chunk[1000]; 
__mram uint64_t assigned_chunks;
__mram uint64_t node_label;

__mram_noinit uint64_t found_results[NR_TASKLETS];
//__mram_noinit uint64_t results[NUM_TASKS][100000];

int main() {
    int tasklet_id = me();

    int start = tasklet_id * WORK_SIZE;
    int end = tasklet_id == (NR_TASKLETS-1) ? (ELEMENTS_PER_CHUNK-1) : start + WORK_SIZE; 
    
    found_results[tasklet_id] = 0;

    //printf("Task %d: from %d to %d\n", tasklet_id, start, end);

    //printf("Chunks: %lu\n", assigned_chunks);   
    //__dma_aligned uint8_t *buffer = (uint8_t*)mem_alloc(25*80);

    
    for(int i = 0; i < assigned_chunks; i++) {
        for(int j = start; j <= end; j++) {
            //mram_read(mr_chunk[i].data, buffer, 25*80);
            //struct mr_node ** nodes = (struct mr_node **)buffer;
            //if(BITTEST(mr_chunk[i].bitset, j))
            if(mr_chunk[i].data[j].node_label == 52) {
                //BITSET(result_vec[tasklet_id], mr_chunk[i].data[j].id_);
                found_results[tasklet_id] += 1;
            }
            
        }
    }

    //printf("Results: %lu\n", found_results[me()]);
    
    return 0;
}
