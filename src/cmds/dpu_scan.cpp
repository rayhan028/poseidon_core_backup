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

void assign_chunks_to_dpus(dpu_set_t &set, graph_db_ptr &gdb, uint32_t ndpus) {
    auto & nodes = gdb->get_nodes();
    int nr_chunks = nodes->as_vec().num_chunks();
    int chunk_size = nodes->as_vec().real_chunk_size();
    int chunks_per_dpu = nr_chunks / ndpus;

    dpu_set_t _dpu;
    auto iter = nodes->as_vec().chunk_list_begin();

    int dpu_id = 0;
    int assigned_chunks = 0;
    std::map<int,int> dpu_offsets;
    
    struct dpu_set_t dpu;

    while(iter != nodes->as_vec().chunk_list_end()) { 
        dpu_id = 0;
        DPU_FOREACH(set, dpu) {
            if(dpu_offsets.find(dpu_id) == dpu_offsets.end()) {
                dpu_offsets[dpu_id] = 0;
            }
            mr_node* chk_ptr = (mr_node*)*iter;
            DPU_ASSERT(dpu_copy_to(dpu, "mr_chunk", dpu_offsets[dpu_id], chk_ptr, nodes->as_vec().real_chunk_size()));
            DPU_ASSERT(dpu_launch(dpu, DPU_ASYNCHRONOUS));
            assigned_dpus.push_back(&dpu);
            std::cout << "Chunk: " << assigned_chunks << " assigned to: " << dpu_id  << " at offset " << dpu_offsets[dpu_id] << std::endl;
            dpu_offsets[dpu_id] += nodes->as_vec().real_chunk_size() * sizeof(mr_node);
            assigned_chunks++;
            iter++;    
            dpu_id++;
            if(iter == nodes->as_vec().chunk_list_end()) break;
        }
    }
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
                  << "number of chunks    : " << nodes->as_vec().num_chunks() << "\n"
                  << "size of a node      : " << sizeof(node) << "\n"
                  << "offset of id_       : " << n._offset() << std::endl; 
    }
    try {
        // TODO: 1. allocate DPUs 
        DPU_ASSERT(dpu_alloc(DPU_ALLOCATE_ALL, "regionMode=perf", &set));
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
        for(auto & d : assigned_dpus) {
            dpu_sync(*d);
        }
        // 3. execute the scan
        //DPU_FOREACH(set, dpu) {
            //DPU_ASSERT(dpu_launch(dpu, DPU_SYNCHRONOUS));
        //}
        auto end = std::chrono::steady_clock::now();
        
        std::chrono::duration<double> diff = end - start;

        printf("printing log for dpu:\n");
        
        DPU_FOREACH(set, dpu) {
            //DPU_ASSERT(dpu_log_read(dpu, stdout));
        }

        uint64_t result[65536];
        mr_node test[65536];
        //dpu_copy_from(set, "result", 0, &result, sizeof(uint64_t) * 65536);
        
        DPU_FOREACH(set, dpu) {
            //DPU_ASSERT(dpu_copy_from(dpu, "result", 0, (uint64_t *)&result, sizeof(result)));
        }

        std::cout << "Scan executed in " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << " µsecs" << std::endl;
        DPU_ASSERT(dpu_free(set));

        auto start_scan = std::chrono::steady_clock::now();
        uint64_t resx[65536];
        int resp = 0;
        for(auto & n : nodes->as_vec()) {
            if(n.node_label == 52) {
                resx[resp++] = n.id();
            }
        }
        auto end_scan = std::chrono::steady_clock::now();
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