#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

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