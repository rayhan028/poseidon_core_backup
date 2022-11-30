#include <mram.h>
#include <stdint.h>
#include <stdio.h>
#include <defs.h>

struct mr_node {
    uint8_t dummy_[40];       // transaction mgmt data
    uint64_t id_;
    uint64_t from_rship_list; // index in relationship list of first relationship
                              // where this node acts as from node
    uint64_t to_rship_list;   // index of relationship list of first relationship
                              // where this node acts as to node
    uint64_t property_list;   // index in property list
    uint32_t node_label;  
};

#define CHUNK_SIZE 817
#define NUM_TASKS 24
#define WORK_SIZE (CHUNK_SIZE / NUM_TASKS)
#define CHUNKS_PER_DPU

__mram uint32_t node_label;
__mram uint8_t assigned_chunks;
__mram struct mr_node mr_chunk[CHUNK_SIZE][10]; 
__mram_noinit uint32_t result[CHUNK_SIZE*10]; 
__mram_noinit uint32_t found_results[NUM_TASKS];
__mram_noinit uint8_t init = 0;
int main() {
    int tasklet_id = me();
    if(!init) {
        found_results[tasklet_id] = 0;
        init = 1;
    }

    int start = tasklet_id * WORK_SIZE;
    int end = tasklet_id == (NUM_TASKS-1) ? (CHUNK_SIZE-1) : start + WORK_SIZE; 
    //printf("Task %d: from %d to %d\n", tasklet_id, start, end);
    
    for(int i = start; i < end; i++) {
        if(mr_chunk[i][0].node_label == 52) {
            //result[found_result] = mr_chunk[i][0].id_;
            found_results[tasklet_id] = 0;
        }
    }
    
    //my_var[0][0] = 24;

    return 0;
}
