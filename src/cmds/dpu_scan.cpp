#include <dpu>
#include "graph_db.hpp"

using namespace dpu;

#define BUFFER_SIZE (1 << 16)

struct mr_node {
    uint8_t dummy_[40];       // transaction mgmt data
    id_t id_;
    uint64_t from_rship_list; // index in relationship list of first relationship
                              // where this node acts as from node
    uint64_t to_rship_list;   // index of relationship list of first relationship
                              // where this node acts as to node
    uint64_t property_list;   // index in property list
    uint32_t node_label;  
};

void dpu_scan(graph_db_ptr &gdb) {
    struct dpu_set_t set, dpu;
    uint32_t nr_dpus;
    auto& nodes = gdb->get_nodes();

    {
        node n;
        std::cout << "chunk_size for nodes: " << nodes->as_vec.()real_chunk_size() << "\n"
                  << "size of a node      : " << sizeof(node) << "\n"
                  << "offset of id_       : " << n._offset() << std::endl; 
    }
    try {
        // TODO: 1. allocate DPUs
        DPU_ASSERT(dpu_alloc(DPU_ALLOCATE_ALL, NULL, &set));
        DPU_ASSERT(dpu_get_nr_dpus(set, &nr_dpus));
        std::cout << "#dpus = " << nr_dpus << std::endl;
        DPU_ASSERT(dpu_load(set, "mram_scan", NULL));

        // 2. upload chunks to MRAM
        for (auto iter = nodes->as_vec().chunk_list_begin(); iter != nodes->as_vec().chunk_list_end(); iter++) {
            auto chk_ptr = iter->get();
            // copy nodes.real_chunk_size() bytes from chk_ptr to MRAM
            // TODO!!! dpu.copy("buffer", chk_ptr, nodes.real_chunk_size());
        } 

        auto start = std::chrono::steady_clock::now();

        // 3. execute the scan
        DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

        auto end = std::chrono::steady_clock::now();

        std::chrono::duration<double> diff = end - start;
        std::cout << "Scan executed in " << diff.count() << " secs" << std::endl;
        DPU_ASSERT(dpu_free(set));
    }
    catch (const DpuError & e) {
        std::cerr << e.what() << std::endl;
    }
}
