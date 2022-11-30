#include <mram.h>
#include <stdint.h>
#include <stdio.h>
#include <defs.h>

#define CHUNK_SIZE 817
#define NUM_TASKS 8
#define WORK_SIZE (CHUNK_SIZE / NUM_TASKS)
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
    struct mr_node data[817];
    struct mrchunk* next;
    char bitset[104];
    uint32_t first;
    char pad[56];
};

__mram struct mrchunk mr_chunk[30]; 
__mram uint64_t assigned_chunks;
__mram_noinit uint32_t result[CHUNK_SIZE*10]; 
__mram uint64_t found_results = 0;

__mram uint32_t node_label;

int main() {
    int tasklet_id = me();

    int start = tasklet_id * WORK_SIZE;
    int end = tasklet_id == (NUM_TASKS-1) ? (CHUNK_SIZE-1) : start + WORK_SIZE; 
    printf("Task %d: from %d to %d\n", tasklet_id, start, end);
    
    printf("Chunks: %lu\n", assigned_chunks);    
    for(int i = 0; i < assigned_chunks; i++) {
        for(int j = start; j < end; j++) {
            //printf("Node Label: %d\n", mr_chunk[0].data[i].node_label);
            if(mr_chunk[i].data[j].node_label == 52) {
                //result[found_result] = mr_chunk[i][0].id_;
                found_results++;
            }
        }
    }
    
    //printf("Chunks: %lu\n", found_results);
    //my_var[0][0] = 24;

    return 0;
}
