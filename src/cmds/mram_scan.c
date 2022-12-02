#include <mram.h>
#include <stdint.h>
#include <stdio.h>
#include <defs.h>
#include <mutex.h>

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#define ELEMENTS_PER_CHUNK 817
#define NUM_TASKS 24
#define WORK_SIZE (ELEMENTS_PER_CHUNK / NUM_TASKS)
#define CHUNKS_PER_DPU

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

struct mrchunk {
    struct mr_node data[ELEMENTS_PER_CHUNK];
    struct mrchunk* next;
    char bitset[104];
    uint32_t first;
    char pad[64];
};

__mram struct mrchunk mr_chunk[100]; 
__mram uint64_t assigned_chunks;
__host uint64_t found_results[NUM_TASKS];

__mram uint64_t node_label;

MUTEX_INIT(my_mutex);

int main() {
    int tasklet_id = me();
    int start = tasklet_id * WORK_SIZE;
    int end = tasklet_id == (NUM_TASKS-1) ? (ELEMENTS_PER_CHUNK-1) : start + WORK_SIZE; 
    
    found_results[tasklet_id] = 0;
    
    //printf("Task %d: from %d to %d\n", tasklet_id, start, end);
    
    //printf("Chunks: %lu\n", assigned_chunks);   
    for(int i = 0; i < assigned_chunks; i++) {
        for(int j = start; j < end+1; j++) {
            if(mr_chunk[i].data[j].node_label == 52) {
                //result[found_result] = mr_chunk[i][0].id_;
                found_results[tasklet_id] += 1;
                
            }
        }
    }
    
    //printf("Results: %lu\n", found_results);

    return 0;
}
