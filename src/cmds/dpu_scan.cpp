#include <dpu>
#include "graph_db.hpp"
#include <map>

using namespace dpu;

#define BUFFER_SIZE (1 << 16)

struct mr_node {
    uint8_t dummy_[40];       // transaction mgmt data
    uint64_t id_;
    uint64_t from_rship_list; // index in relationship list of first relationship
                              // where this node acts as from node
    uint64_t to_rship_list;   // index of relationship list of first relationship
                              // where this node acts as to node
    uint64_t property_list;   // index in property list
    uint32_t node_label;  

    mr_node() = default;
    mr_node(id_t id) : id_(id) {}
};

std::vector<dpu_set_t*> assigned_dpus;

struct mrchunk {
    struct mr_node data[817];
    struct mrchunk* next;
    char bitset[104];
    uint32_t first;
    char pad[56];
};


void assign_chunks_to_dpus(dpu_set_t &set, graph_db_ptr &gdb, uint32_t ndpus) {
    auto & nodes = gdb->get_nodes();
    int nr_chunks = nodes->as_vec().num_chunks();
    int chunk_size = nodes->as_vec().real_chunk_size();
    int chunks_per_dpu = nr_chunks / ndpus;

    dpu_set_t _dpu;
    auto iter = nodes->as_vec().chunk_list_begin();

    int dpu_id = 0;
    int assigned_chunks = 0;
    std::map<int,uint64_t> assigned_chunks_dpu;

    struct dpu_set_t dpu;

    int o = 0;
    while(iter != nodes->as_vec().chunk_list_end()) { 
        DPU_FOREACH(set, dpu, dpu_id) {
            if(assigned_chunks_dpu.find(dpu_id) == assigned_chunks_dpu.end()) {
                assigned_chunks_dpu[dpu_id] = 0;
            }
            
            //DPU_ASSERT(dpu_copy_to(dpu, "mr_chunk", dpu_offsets[dpu_id], chk_ptr, nodes->as_vec().real_chunk_size()));
            DPU_ASSERT(dpu_prepare_xfer(dpu, *iter));
            //DPU_ASSERT(dpu_launch(dpu, DPU_ASYNCHRONOUS));
            assigned_dpus.push_back(&dpu);
            assigned_chunks_dpu[dpu_id]++;
            //std::cout << "Chunk: " << assigned_chunks << " assigned to: " << dpu_id << std::endl;
            assigned_chunks++;
            iter++;    
            if(iter == nodes->as_vec().chunk_list_end()) break;
        }
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "mr_chunk", o * nodes->as_vec().real_chunk_size(), nodes->as_vec().real_chunk_size(), DPU_XFER_DEFAULT));
        //DPU_ASSERT(dpu_sync(set));
        o++;
    }

    DPU_FOREACH(set, dpu, dpu_id) { 
        DPU_ASSERT(dpu_prepare_xfer(dpu, &assigned_chunks_dpu[dpu_id]));
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "assigned_chunks", 0, sizeof(uint64_t), DPU_XFER_ASYNC));
    DPU_ASSERT(dpu_sync(set));

}

void dpu_scan(graph_db_ptr &gdb) {
    //auto system = DpuSet::allocate(1);
    //auto dpu = system.dpus()[0];
    struct dpu_set_t set, dpu;
    uint32_t nr_dpus;
    uint32_t nr_ranks;
    auto& nodes = gdb->get_nodes();

    {
        node n;
        std::cout << "chunk_size for nodes: " << nodes->as_vec().real_chunk_size() << "\n"
                  << "elements per chunk: " << nodes->as_vec().elements_per_chunk() << "\n"
                  << "number of chunks    : " << nodes->as_vec().num_chunks() << "\n"
                  << "size of a node      : " << sizeof(node) << "\n"
                  << "offset of id_       : " << n._offset() << std::endl; 
    }
    try {
        // TODO: 1. allocate DPUs 
        DPU_ASSERT(dpu_alloc(63, NULL, &set));
        DPU_ASSERT(dpu_get_nr_dpus(set, &nr_dpus));
        DPU_ASSERT(dpu_get_nr_ranks(set, &nr_ranks));
        std::cout << "#dpus = " << nr_dpus << std::endl;
        std::cout << "#ranks = " << nr_ranks << std::endl;
        DPU_ASSERT(dpu_load(set, "mram_scan", NULL));
        //dpu->load("mram_scan");

        // 2. upload chunks to MRAM
        assign_chunks_to_dpus(set, gdb, nr_dpus);
        
        std::cout << "Start dpu" << std::endl;
        auto start = std::chrono::steady_clock::now();
        // 3. execute the scan
        DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
        //DPU_ASSERT(dpu_sync(set));
        auto end = std::chrono::steady_clock::now();
        
        std::chrono::duration<double> diff = end - start;

        printf("printing log for dpu:\n");
        
        DPU_FOREACH(set, dpu) {
            dpu_log_read(dpu, stdout);
        }

        
        uint32_t found_results = 0;
        uint64_t tmp;
        DPU_FOREACH(set, dpu) {
            dpu_copy_from(dpu, "found_results", 0, &tmp, sizeof(uint64_t));
            found_results += tmp;
            tmp = 0;
        }
        
        std::cout << "MRAMScan results: " << found_results << std::endl;
        std::cout << "Scan executed in " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << " µsecs" << std::endl;
        DPU_ASSERT(dpu_free(set));

        auto start_scan = std::chrono::steady_clock::now();
        uint64_t resx[817];
        int resp = 0;
        int results = 0;
        for(auto & n : nodes->as_vec()) {
            if(n.node_label == 52) {
                resx[resp++] = n.id();
                results++;
            }
        }

        auto end_scan = std::chrono::steady_clock::now();

        std::cout << "NodeScan results: " << results << std::endl;
        std::chrono::duration<double> diff_scan = end_scan - start_scan;
        std::cout << "NodeScan executed in " << std::chrono::duration_cast<std::chrono::microseconds>(diff_scan).count() << " µsecs" << std::endl;
    }
    catch (const DpuError & e) {
        std::cerr << e.what() << std::endl;
    }
}

/* 

int offset = 0;
        std::cout << "DPU " << dpu_id++ << " :";
        // assign #chunks_per_dpu to _dpu
        //auto _chunks = dpu_id == ndpus ? (nr_chunks - assigned_chunks) : chunks_per_dpu;
        for(int i = 0; i < chunks_per_dpu; i++) {
            mr_node* chk_ptr = (mr_node*)*iter;
            dpu_copy_to(_dpu, "mr_chunk", offset, chk_ptr, nodes->as_vec().real_chunk_size()); 
            offset += 65536 * sizeof(mr_node);  
            iter++; 
            std::cout << " " << assigned_chunks++;
        }
        std::cout << std::endl;

*/